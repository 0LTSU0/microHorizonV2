#include <iostream>
#include <memory>

#include <SharedData.h>
#include <tracer.h>
#include <configurator.h>
#include <posInputWorker.h>



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

	std::shared_ptr<sharedData::SharedData> sharedData = std::make_shared<sharedData::SharedData>();
	
	posInputWorker w_posInput(sharedData, appConfigurator.getPosMode(), appConfigurator.getUDPPort());

	return 0;
}