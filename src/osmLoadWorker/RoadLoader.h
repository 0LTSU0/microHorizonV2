#pragma once

#include <SharedData.h>
#include <tracer.h>

//note to self, getting locations for nodes is kinda complicated, good example for that https://github.com/osmcode/libosmium/blob/cf81df16ddd2fbee6eede4e84978130419eef759/examples/osmium_road_length.cpp#L3
using index_type = osmium::index::map::FlexMem<osmium::unsigned_object_id_type, osmium::Location>; //not sure what this does
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>; //but this is needed to get valid locations on nodes

class RoadLoader : public osmium::handler::Handler {
public:
    RoadLoader(std::shared_ptr<sharedData::SharedData> data, float loadRadius, std::string mapPath) 
        : m_loadRadius(loadRadius),
          m_sharedata(data),
          m_mapPath(mapPath) {}
    ~RoadLoader()
    {
        Tracer::log("RoadLoader destructor called", traceLevel::DEBUG); //temp to see if all components quit properly
    }
    
    void way(const osmium::Way& way); // see ThirdParty/libosmium-2.20.0/include/osmium/handler.hpp
    
    void setBoudningBox(double lat, double lon); // Create bounding box around current location with size defined in m_loadRadius

    void run(); // entry point to this "thread". Monitors when input position is nearing endge of currently loaded area and triggers new load

private:
    bool mapDataLoadNeeded();
    bool loadMapData();
    
    osmium::Box m_loadBoundingBox;
    float m_loadBoundingBoxCenterLat = 0.0;
    float m_loadBoundingBoxCenterLon = 0.0;
    float m_loadRadius;
    std::string m_mapPath;
    std::vector<sharedData::RoadInfo> m_tmpRoadNetwork; // While loading is ongoing, roads in bounding box are put here and finally pushed to shared data
    std::shared_ptr<sharedData::SharedData> m_sharedata;
};