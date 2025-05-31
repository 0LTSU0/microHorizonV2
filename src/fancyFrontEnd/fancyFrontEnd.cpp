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
			m_window.display(); //I guess this needs to be called for fps limit to work
			continue;
		}
		
		renderStartTS = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		if (m_shareData->outputHorizonDataAvailable)
		{
			drawCurrentHorizon();
		}

		m_window.display();

		// next render timestamp is "current epoch" + "target interval" - "what was spent doing current render"
		renderEndTS = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		nextRenderTs = renderEndTS + targetSleepInterval - (renderEndTS - renderStartTS);
	}
}


void fancyFrontEndWorker::drawCurrentHorizon() {
	// draw scene
}
