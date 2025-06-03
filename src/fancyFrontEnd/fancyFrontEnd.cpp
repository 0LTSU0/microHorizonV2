#include <fancyFrontEnd.h>
#include <chrono>
#include <iostream>
#include <thread>

#include <FEHelpers.h>

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

		prepareCurrentWindow();
		drawCurrentWindow();

		// next render timestamp is "current epoch" + "target interval" - "what was spent doing current render"
		renderEndTS = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch());
		nextRenderTs = renderEndTS + targetSleepInterval - (renderEndTS - renderStartTS);
	}
}


void fancyFrontEndWorker::prepareCurrentWindow() {
	m_windowContent.roads.clear(); //clear old content form the content vector
	m_windowContent.texts.clear();
	const auto* hpptr = &m_shareData->horizonPositon;
	m_screenCenter = { m_window.getSize().x / 2 , m_window.getSize().y / 2 };

	// write some text stuff
	sf::Text mapLoaderState(m_font);
	switch (m_shareData->roadLoaderState) {
	case sharedData::RoadLoaderState::NOT_INITIALIZED:
		mapLoaderState.setString("Map load status: Not Initialized");
		break;
	case sharedData::RoadLoaderState::IDLE:
		mapLoaderState.setString("Map load status: Idle");
		break;
	case sharedData::RoadLoaderState::LOADING_MAP:
		mapLoaderState.setString("Map load status: Map loading in progress");
		break;
	default:
		break;
	}
	m_windowContent.texts.push_back(mapLoaderState);
	sf::Text currentPos(m_font);
	currentPos.setString("Current position: " + std::to_string(hpptr->inputPos.lat) + "," + std::to_string(hpptr->inputPos.lon));
	m_windowContent.texts.push_back(currentPos);
	sf::Text currentRoadName(m_font);
	currentRoadName.setString("Current road: " + hpptr->path.road.attributes.name);
	m_windowContent.texts.push_back(currentRoadName);
	sf::Text currentRoadInfo(m_font);
	currentRoadInfo.setString("Current attributes: Speed Limit - " + hpptr->path.road.attributes.speedLimit + " | Road type - " + hpptr->path.road.attributes.highway_type);
	m_windowContent.texts.push_back(currentRoadInfo);

	// draw roads (only if there is some data available according to horizon generator
	std::pair<float, float> transformedPts;
	if (m_shareData->outputHorizonDataAvailable) {
		renderedRoad r;
		r.roadID = hpptr->path.road.id;
		for (const auto& node : hpptr->path.road.nodes)
		{
			transformedPts = latLonTo2d(node.lat(), node.lon(), hpptr->inputPos.lat, hpptr->inputPos.lon);
			r.roadPoints.push_back(sf::Vertex{ sf::Vector2f(transformedPts.first + m_screenCenter.x, transformedPts.second + m_screenCenter.y), sf::Color::Green });
		}
		m_windowContent.roads.push_back(r);
		for (const auto& path : hpptr->path.childPaths)
		{
			renderedRoad rs;
			for (const auto& node : path.road.nodes)
			{
				transformedPts = latLonTo2d(node.lat(), node.lon(), hpptr->inputPos.lat, hpptr->inputPos.lon);
				rs.roadPoints.push_back(sf::Vertex{ sf::Vector2f(transformedPts.first + m_screenCenter.x, transformedPts.second + m_screenCenter.y), sf::Color::White });
			}
			m_windowContent.roads.push_back(rs);
		}
	}
	

}


void fancyFrontEndWorker::drawCurrentWindow() {
	m_window.clear();

	// assuming math in prepareCurrentWindow is mathing then car is at the center of our screen
	sf::CircleShape carMarker;
	carMarker.setRadius(5);
	carMarker.setPosition(static_cast<sf::Vector2f>(m_screenCenter));
	carMarker.setOrigin({ 5, 5 });
	carMarker.setFillColor(sf::Color::Red);
	m_window.draw(carMarker);
	
	// loop all roads and draw them TODO: scale so thay actually show up in the output
	for (auto& r : m_windowContent.roads)
	{
		m_window.draw(r.roadPoints.data(), r.roadPoints.size(), sf::PrimitiveType::LineStrip);
	}
	
	// loop all texts and write them
	for (size_t i = 0; i < m_windowContent.texts.size(); i++) {
		auto& t = m_windowContent.texts[i];
		t.setFillColor(sf::Color::White);
		t.setCharacterSize(20);
		t.setPosition({ 0, static_cast<float>(i) * 20.f });
		m_window.draw(t);
	}

	m_window.display();
}
