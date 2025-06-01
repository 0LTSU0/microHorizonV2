#include <RoadLoader.h>
#include <chrono>
#include <cmath>

void RoadLoader::run()
{
    // While app is running, check every 5s if current input position has moved too far from previously loaded map center
    while (m_sharedata->appIsRunning)
    {
        if (mapDataLoadNeeded() && m_sharedata->roadLoaderState != sharedData::RoadLoaderState::LOADING_MAP)
        {
            { //scope for freeing the mutex after setting load state
                std::lock_guard<std::mutex> guard(m_sharedata->roadLoaderStateMutex);
                m_sharedata->roadLoaderState = sharedData::RoadLoaderState::LOADING_MAP;
            }
            bool res = loadMapData();
            if (!res) { // loadMapData() returned false -> we didn't have input position to perform the load with OR no roads were found around current position
                std::lock_guard<std::mutex> guard(m_sharedata->roadLoaderStateMutex);
                m_sharedata->roadLoaderState = sharedData::RoadLoaderState::NOT_INITIALIZED;
            }
            else {
                std::lock_guard<std::mutex> guard(m_sharedata->roadLoaderStateMutex);
                m_sharedata->roadLoaderState = sharedData::RoadLoaderState::IDLE;
            }
        }
        else 
        {
            Tracer::log("No need to perform map load", traceLevel::DEBUG);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

bool RoadLoader::loadMapData()
{
    // Map data should not get triggered if this is the case, but just to make sure we don't try to load with invalid position
    sharedData::inputPosition currPos = m_sharedata->lastProcessedPosition;
    if (!currPos.isValidObs())
    {
        return false;
    }
    
    // Set bounding box around latest input position
    m_loadBoundingBoxCenterLat = currPos.lat;
    m_loadBoundingBoxCenterLon = currPos.lon;
    setBoudningBox(currPos.lat, currPos.lon);
    
    // Start map load
    m_tmpRoadNetwork.clear();
    index_type index;
    location_handler_type location_handler{ index };
    osmium::io::Reader reader(m_mapPath, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way);
    osmium::apply(reader, location_handler, *this);
    m_sharedata->initialMapLoadDone = true;
    std::lock_guard<std::mutex> guard(m_sharedata->mapDataMutex);
    m_sharedata->mapData = m_tmpRoadNetwork;
    Tracer::log("Map data load finished!", traceLevel::INFO);
    return m_tmpRoadNetwork.size() > 0;
}

bool RoadLoader::mapDataLoadNeeded()
{
    sharedData::inputPosition lastProcPos = m_sharedata->lastProcessedPosition;
    if (!lastProcPos.isValidObs())
    {
        Tracer::log("mapDataLoadNeeded() cannot deduce wheter map load is needed or not because last known position doesn't seem to be valid", traceLevel::WARNING);
        return false;
    }
    
    // if no map load has been performed, we need it for sure
    if (!m_sharedata->initialMapLoadDone || m_sharedata->roadLoaderState == sharedData::RoadLoaderState::NOT_INITIALIZED)
    {
        Tracer::log("Map data load needed: no load performed yet", traceLevel::INFO);
        return true;
    }

    // otherwise check if position has gone too far from previous map center
    float moveTreshold = m_loadRadius / 3;
    
    if (std::fabs(lastProcPos.lat - m_loadBoundingBoxCenterLat) > moveTreshold ||
        std::fabs(lastProcPos.lon - m_loadBoundingBoxCenterLon) > moveTreshold)
    {
        Tracer::log("Map data load needed: position has moved too far from loaded map center", traceLevel::INFO);
        return true;
    }

    return false;
}

void RoadLoader::setBoudningBox(double lat, double lon)
{
	m_loadBoundingBox = osmium::Box(
        osmium::Location(lon - m_loadRadius, lat - m_loadRadius),
        osmium::Location(lon + m_loadRadius, lat + m_loadRadius)
    );
}

void RoadLoader::way(const osmium::Way& way)
{
    // This function is called for each road that osmium parses from the map data
    const char* highway_tag = way.tags()["highway"];
    if (highway_tag && std::string(highway_tag) != "path") { // TODO: check if paths need to be included.. https://wiki.openstreetmap.org/wiki/Key:highway#Paths -> probably not
        // Check if any of the way's nodes are within the bounding box, if yes make roadInfo obj and add to m_tmpRoadNetwork
        for (const auto& node : way.nodes()) {
            if (node.location().is_undefined())
            {
                Tracer::log("Node " + std::to_string(node.ref()) + " has no valid location", traceLevel::DEBUG);
                continue;
            }
            if (m_loadBoundingBox.contains(node.location())) {
                Tracer::log("Inside rect --- Road ID: " + std::to_string(way.id()) + " (highway=" + highway_tag + ")", traceLevel::DEBUG);
                std::vector<osmium::Location> road_nodes;
                for (const auto& node : way.nodes()) {
                    if (node.location().is_undefined()) {
                        // in case this way had some undefined node location
                        continue;
                    }
                    road_nodes.push_back(node.location());
                }

                sharedData::RoadInfo roadInfo{
                     way.id(),
                     {way.tags().get_value_by_key("name", "Unnamed Road"), highway_tag, way.tags().get_value_by_key("maxspeed", "N/A")},
                     road_nodes
                };
                m_tmpRoadNetwork.push_back(roadInfo);
                break; //break out from current road
            }
        }
    }
}