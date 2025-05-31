#pragma once

#include <SharedData.h>
#include <tracer.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class fancyFrontEndWorker {
public:
    fancyFrontEndWorker(std::shared_ptr<sharedData::SharedData> data, float updateInterval)
        : m_shareData(data), m_updateIntervalS(updateInterval){
    }
    ~fancyFrontEndWorker() {
        Tracer::log("fancyFrontEndWorker destructor called", traceLevel::DEBUG); //temp to see if all components quit properly
        if (m_window.isOpen()) {
            m_window.close();
        }
    }

    void run();

private:
    void drawCurrentHorizon();
    
    std::shared_ptr<sharedData::SharedData> m_shareData;
    float m_updateIntervalS;
    sf::RenderWindow m_window;

};