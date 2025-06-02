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
    void generateSubPathsForRoad(sharedData::RoadInfo&);
    
    std::shared_ptr<sharedData::SharedData> m_shareData;
};