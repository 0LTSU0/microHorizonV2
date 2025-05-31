#pragma once

#include <SharedData.h>
#include <tracer.h>
#include <SFML/Graphics.hpp>

class fancyFrontEndWorker {
public:
    fancyFrontEndWorker(std::shared_ptr<sharedData::SharedData> data, float updateInterval)
        : m_shareData(data), m_updateIntervalS(updateInterval){
    }
    ~fancyFrontEndWorker() {
        Tracer::log("fancyFrontEndWorker destructor called", traceLevel::DEBUG); //temp to see if all components quit properly
    }

    void run();

private:
    std::shared_ptr<sharedData::SharedData> m_shareData;
    float m_updateIntervalS;
};