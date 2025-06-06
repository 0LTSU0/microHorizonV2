#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include <SharedData.h>
#include <tracer.h>
#include <configurator.h>
#include <posInputWorker.h>
#include <RoadLoader.h>
#include <horizonWorker.h>
#include <fancyFrontEnd.h>

int main(int argc, char* argv[])
{
	Tracer::log("Starting microHorizonApp...", traceLevel::INFO);
	
	MHConfigurator appConfigurator;
	cmdArgs args = parseCMDArgs(argc, argv);
	if (!appConfigurator.loadConfig(args.configPath))
	{
		Tracer::log("loadConfig() failed, cannot start application", traceLevel::ERROR);
		return -1;
	}

	// Data structure to be shared betweewn threads
	std::shared_ptr<sharedData::SharedData> sharedData = std::make_shared<sharedData::SharedData>();
	
	// Create and start threads
	posInputWorker w_posInput(sharedData, appConfigurator.getPosMode(), appConfigurator.getUDPPort());
	std::thread posInputThread(&posInputWorker::run, &w_posInput);
	RoadLoader w_roadLoader(sharedData, appConfigurator.getLoadRadius(), appConfigurator.getMapPath());
	std::thread roadLoaderThread(&RoadLoader::run, &w_roadLoader);
	horizonWorker w_horizonGen(sharedData);
	std::thread horizonGenThread(&horizonWorker::run, &w_horizonGen);
	fancyFrontEndWorker w_fancyFrontend(sharedData, appConfigurator.getFEUpdateFreq(), appConfigurator.getFontPath());
	std::thread frontEndThread;
	switch (appConfigurator.getFEMode())
	{
	case frontEndMode::FANCY:
		frontEndThread = std::thread(&fancyFrontEndWorker::run, &w_fancyFrontend);
		break;
	case frontEndMode::RASPI:
		break;
	default:
		break;
	}

	//TODO: implement some method for quitting the app
	Tracer::log("Sleep in main thread for seconds: " + std::to_string(args.appTimeOut), traceLevel::DEBUG);
	std::this_thread::sleep_for(std::chrono::seconds(args.appTimeOut));
	
	//Shut down threads
	Tracer::log("Setting sharedData->appIsRunning=false and waiting for workers to quit", traceLevel::INFO);
	sharedData->appIsRunning = false;
	posInputThread.join();
	roadLoaderThread.join();
	horizonGenThread.join();
	if (frontEndThread.joinable()) {
		frontEndThread.join();
	}

	return 0;
}