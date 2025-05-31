#include <horizonWorker.h>
#include <posMatcherImpl.h>
#include <chrono>

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
	m_shareData->horizonPositon.currentRoad = currentRoad;
	m_shareData->horizonDataLock.unlock();

	return true;
}

