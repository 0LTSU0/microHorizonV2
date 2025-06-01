#include <fancyFrontEnd.h>
#include <chrono>
#include <iostream>
#include <thread>

void fancyFrontEndWorker::run() {
	std::chrono::milliseconds nextRenderTs = std::chrono::milliseconds(0);
	auto targetSleepInterval = std::chrono::milliseconds(static_cast<int>(std::round(m_updateIntervalS * 1000)));
	auto renderStartTS = std::chrono::milliseconds(0);
	auto renderEndTS = std::chrono::milliseconds(0);
	
	//SFML doesn't play nice with threads... The window needs to be created here instead of e.g. contructor
	//or else we get bunch of OpenGL errors because the context is somehow active in another thread or smth
	//ALSO: if we sleep in this thread OR set framerate limit to something low like 1, the window interaction
	//becomes very laggy -> set fps to 60 but don't do any of the rendering stuff if next render time not hit
	m_window.create(sf::VideoMode({ 1600, 900 }), "MicroHorizon FrontEnd");
	m_window.setFramerateLimit(60);
	
	while (m_shareData->appIsRunning)
	{
		// if window is closed -> shut down the entire app
		while (const std::optional event = m_window.pollEvent())
		{
			if (event->is<sf::Event::Closed>()) {
				m_window.close();
				m_shareData->appIsRunning = false;
			}
		}
		
		auto currentTS = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		if (currentTS < nextRenderTs){
			// TODO: figure out how to prevent not having to render the window on every iteration. If I don't do anything here,
			// then the fps limit does not work and this loop runs fast as fuck taking up cpu. If I call m_window.display() then
			// the image is flickering because of some stupid crap about opengl flipping buffers... Anyway for now need to render
			// the window on every iteration even though the content only updates every m_updateIntervalS.
			drawCurrentWindow();
			continue;
		}
		
		renderStartTS = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		if (m_shareData->outputHorizonDataAvailable || true) //REMOVE TMP TRUE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			
			prepareCurrentWindow();
			drawCurrentWindow();
		}

		// next render timestamp is "current epoch" + "target interval" - "what was spent doing current render"
		renderEndTS = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		nextRenderTs = renderEndTS + targetSleepInterval - (renderEndTS - renderStartTS);
	}
}


void fancyFrontEndWorker::prepareCurrentWindow() {
	m_windowContent.roads.clear(); //clear old content form the content vector
	m_windowContent.texts.clear();

	// draw scene
	renderedRoad r;
	r.roadID = 12345;
	r.roadPoints.push_back(sf::Vertex{ sf::Vector2f(10.f, 10.f), sf::Color::Blue });
	r.roadPoints.push_back(sf::Vertex{ sf::Vector2f(150.f, 150.f), sf::Color::Blue });
	r.roadPoints.push_back(sf::Vertex{ sf::Vector2f(150.f, 200.f), sf::Color::Blue });
	m_windowContent.roads.push_back(r);
}

void fancyFrontEndWorker::drawCurrentWindow() {
	m_window.clear();

	// loop all roads and draw them
	for (auto& r : m_windowContent.roads)
	{
		m_window.draw(r.roadPoints.data(), r.roadPoints.size(), sf::PrimitiveType::LineStrip);
	}
	std::vector<sf::Vertex> test;
	test.push_back(sf::Vertex{ sf::Vector2f(30.f, 300.f), sf::Color::Green });
	test.push_back(sf::Vertex{ sf::Vector2f(30.f, 600.f), sf::Color::Green });
	test.push_back(sf::Vertex{ sf::Vector2f(300.f, 300.f), sf::Color::Green });
	m_window.draw(test.data(), test.size(), sf::PrimitiveType::LineStrip);
	m_window.display();
}
