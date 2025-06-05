#include <horizonWorker.h>
#include <posMatcherImpl.h>
#include <chrono>
#include <osmium/geom/haversine.hpp>

void horizonWorker::run()
{
	while (m_shareData->appIsRunning)
	{
		if (m_shareData->incomingPositions.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		
		// get oldest position from incoming pos queue
		sharedData::inputPosition currPos = m_shareData->incomingPositions.front();
		m_shareData->incomingPositions.pop();
	
		std::string debugStr = "horizonWorker processing input pos: " + std::to_string(currPos.lat) + "," + std::to_string(currPos.lat) + " - Positions remaining in queue: " + std::to_string(m_shareData->incomingPositions.size());
		Tracer::log(debugStr, traceLevel::INFO);

		if (currPos.isValidObs())
		{
			// if current position is valid set as "last known position". Map loader thread uses this to perform map load
			m_shareData->lastProcessedPosition = currPos;
		}
		if (!m_shareData->initialMapLoadDone)
		{
			// if no map data has been loaded yet there is no point in tryting to generate horizon
			continue;
		} 
		
		generateHorizonGraph(currPos);
	}
	
}

bool horizonWorker::generateHorizonGraph(const sharedData::inputPosition& pos)
{
	auto currentRoad = matchPosition(pos, m_shareData);
	if (!isRoadInfoValid(currentRoad))
	{
		return false;
		m_shareData->outputHorizonDataAvailable = false;
	}

	m_shareData->outputHorizonDataAvailable = true;
	m_shareData->horizonDataLock.lock();
	m_shareData->horizonPositon.inputPos = pos;
	m_shareData->horizonPositon.path.road = currentRoad;
	m_shareData->horizonDataLock.unlock();

	generateSubPathsForRoad(m_shareData->horizonPositon.path, true, -1);
	bool targetDistReached = extendMPP(m_shareData->horizonPositon.path);

	return true;
}

// TODO improve logic here. Now priority is that 
// try to stay on road with same name. If not possible
// take the highest road class. If many share the same
// take the one that has least trun from current path
sharedData::pathStruct& chooseMostProbableTransition(sharedData::pathStruct& currentPath)
{
	for (int i = 0; i < currentPath.childPaths.size(); i++) {
		if (!currentPath.childPaths[i].ignoreInMPPGeneration) {
			currentPath.childPaths[i].isPartOfMPP = true;
			return currentPath.childPaths[i];
		}
	}
	return currentPath.childPaths[0];
}

bool horizonWorker::extendMPP(sharedData::pathStruct& startPath)
{
	constexpr float targetDist = 1000; // temp, to be configurable
	sharedData::pathStruct currentPath = startPath;
	m_roadIdsOnMPP.push_back(startPath.road.id);
	float MPPLength = 0.0;
	while (true)
	{
		if (currentPath.childPaths.size() == 0) {
			break; //there are no children, we cannot continue
		}
		MPPLength = MPPLength + getCurrentRoadSegmentLen(currentPath);
		if (MPPLength > targetDist) {
			break;
		}
		currentPath = chooseMostProbableTransition(currentPath);
		generateSubPathsForRoad(currentPath, false, m_roadIdsOnMPP.back());
	}
	
	return MPPLength > targetDist;
}

// TODO: we should probably instead of doing this keep track of some kind of "node to way" dict
// when performing map load because looping through EVERYTHING is quite slow
void horizonWorker::generateSubPathsForRoad(sharedData::pathStruct& path, bool pathIsWhereVehicleIs, int previousExtensionId)
{
	m_shareData->horizonDataLock.lock();
	m_shareData->mapDataMutex.lock();
	path.childPaths.clear();
	int parentNodeI = 0;
	for (auto& node : path.road.nodes)
	{
		for (auto& road : m_shareData->mapData)
		{
			bool roadIsSubRoad = false;
			bool firstNodeIsCommon = true;
			for (auto& subnode : road.nodes)
			{
				if (node == subnode && path.road.id != road.id)
				{
					roadIsSubRoad = true;
					break;
				}
				firstNodeIsCommon = false;
			}
			if (roadIsSubRoad)
			{
				auto child = sharedData::pathStruct(road, {});
				
				// if we are generating subpaths for the path where our vehicle is currently located on
				// the subpath(s) that are located "behid" of vehicle/travel direction needs to be ignored
				// in mpp generation
				if (pathIsWhereVehicleIs && parentNodeI == 0 && path.road.direction == sharedData::SAME_VEHICLE_TRAVEL ||
					pathIsWhereVehicleIs && parentNodeI == path.road.nodes.size() - 1 && path.road.direction == sharedData::OPPOSITE_VEHICLE_TRAVEL)
				{
					child.ignoreInMPPGeneration = true;
					path.childPaths.push_back(child);
					continue;
				}
				// also if the "child" is oneway street which is modelled in direction where the last point is on our parent
				// road, then it means we cannot drive there -> needs to be ignored
				else if (road.attributes.oneWay && !firstNodeIsCommon) {
					continue;
				}
				// otherwise, if this connected road is the previous piece of predicted path, it needs to be ignored and also should not be pushed to childPaths
				if (road.id == previousExtensionId)
				{
					continue;
				}

				path.childPaths.push_back(child);
			}
		}
		parentNodeI++;
	}
	m_shareData->mapDataMutex.unlock();
	m_shareData->horizonDataLock.unlock();
}

float horizonWorker::getCurrentRoadSegmentLen(sharedData::pathStruct& path)
{
	float totalDistance = 0.0;
	for (size_t i = 1; i < path.road.nodes.size(); ++i) {
		totalDistance += osmium::geom::haversine::distance(
			path.road.nodes[i - 1], path.road.nodes[i]
		);
	}
	return totalDistance;
}
