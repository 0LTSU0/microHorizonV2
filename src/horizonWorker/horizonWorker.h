#pragma once

#include <SharedData.h>
#include <tracer.h>

class horizonWorker {
public:
    horizonWorker(std::shared_ptr<sharedData::SharedData> data)
        : m_shareData(data) {}
    ~horizonWorker() {
        Tracer::log("horizonWorker destructor called", traceLevel::DEBUG); //temp to see if all components quit properly
    }

    void run();

private:
    bool generateHorizonGraph(const sharedData::inputPosition&);
    void generateSubPathsForRoad(sharedData::pathStruct& path, bool, int);
    bool extendMPP(sharedData::pathStruct&, const bool);
    float getCurrentRoadSegmentLen(sharedData::pathStruct&);
    
    float m_currentMPPLenght = 0.0;
    std::vector<int> m_roadIdsOnMPP;
    std::shared_ptr<sharedData::SharedData> m_shareData;
};