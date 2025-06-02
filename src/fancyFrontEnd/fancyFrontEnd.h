#pragma once

#include <SharedData.h>
#include <tracer.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <array>

// roads in the rendered output. roadID is the OSM id and roadLines are the lines that make up the road.
// We cannot directly use the latlon coordinates because those will look distorted. Instead of transforming
// them in the horizon generator thread, this thread will handle trasforming them into good loking "format"
struct renderedRoad {
    int roadID;
    std::vector<sf::Vertex> roadPoints;
};

struct windowCont {
    std::vector<renderedRoad> roads;
    std::vector<sf::Text> texts;
};

class fancyFrontEndWorker {
public:
    fancyFrontEndWorker(std::shared_ptr<sharedData::SharedData> data, float updateInterval, std::string fontPath)
        : m_shareData(data), m_updateIntervalS(updateInterval), m_fontPath(fontPath){
        if (!m_font.openFromFile(fontPath)) {
            Tracer::log("FancyFrontEnd failed to open font from: " + fontPath, traceLevel::ERROR);
        }
    }
    ~fancyFrontEndWorker() {
        Tracer::log("fancyFrontEndWorker destructor called", traceLevel::DEBUG); //temp to see if all components quit properly
        if (m_window.isOpen()) {
            m_window.close();
        }
    }

    void run();

private:
    void prepareCurrentWindow();
    void drawCurrentWindow();
    void drawSubpathsOfARoad(sharedData::RoadInfo&);
    
    sf::Vector2u m_screenCenter;
    std::shared_ptr<sharedData::SharedData> m_shareData;
    float m_updateIntervalS;
    std::string m_fontPath;
    sf::RenderWindow m_window;
    sf::Font m_font;
    windowCont m_windowContent;
};