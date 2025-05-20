#include "configurator.h"
#include <fstream>
#include <filesystem>

cmdArgs parseCMDArgs(int argc, char* argv[])
{
	cmdArgs args;
	std::string currArg;
	std::string searchStr;
	for (int i = 0; i < argc; i++)
	{
		currArg = argv[i];

		searchStr = "cfg="; // path to json config file
		auto findPos = currArg.find(searchStr);
		if (findPos != std::string::npos)
		{
			args.configPath = currArg.substr(findPos + searchStr.length());
		}
		searchStr = "timeout="; // timeout for how long the app should run for (currently needed because app has no method for shutting down)
		findPos = currArg.find(searchStr);
		if (findPos != std::string::npos)
		{
			args.appTimeOut = std::atoi(currArg.substr(findPos + searchStr.length()).c_str());
		}
		
		searchStr = "someOtherArg"; // TODO: other args if necessary
	}
	return args;
}

bool MHConfigurator::loadConfig(std::string &confPath)
{
	Tracer::log("loadConfig()", traceLevel::DEBUG);
	
	std::ifstream ifs;
	if (confPath.empty())
	{
		Tracer::log("loadConfig(): No config path given as cmd argument -> trying to read from current working dir", traceLevel::INFO);
		std::filesystem::path pth = std::filesystem::current_path();
		pth = pth / "config.json";
		ifs.open(pth);
	}
	else 
	{
		Tracer::log("loadConfig(): Trying to read config file from: " + confPath, traceLevel::INFO);
		ifs.open(confPath);
	}
	
	if (!ifs.is_open())
	{
		Tracer::log("Failed to open config file", traceLevel::ERROR);
		return false;
	}
	json conf = json::parse(ifs);
	if (!conf.contains("mapPath") || !conf.contains("posMode")) //TODO: use json schema
	{
		return false;
	}

	// map path
	c_mapPath = conf["mapPath"].get<std::string>();

	//loadRadius (this is optional, defaults to tiny 0.5 degrees)
	if (conf.contains("loadRadius"))
	{
		c_loadRadius = conf["loadRadius"].get<float>();
	}

	// positioning mode
	if (conf["posMode"].get<std::string>() == "UDP")
	{
		c_posMode = POSMode::UDP;
		if (!loadUDPSpecificConfs(conf))
		{
			return false;
		}
	}
	else if (conf["posMode"].get<std::string>() == "GPS")
	{
		c_posMode = POSMode::GPS_RECEIVER;
		if (!loadGPSSpecificConfs(conf))
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	// writeDebugDumps. When on, e.g. position matcher writes some debuug jsons with candidates for debugging. Default False
	if (conf.contains("writeDebugDumps"))
	{
		c_writeDebugDumps = conf["writeDebugDumps"].get<bool>();
	}

	return true;
}

std::string MHConfigurator::getMapPath()
{
	return c_mapPath;
}

float MHConfigurator::getLoadRadius()
{
	return c_loadRadius;
}

POSMode MHConfigurator::getPosMode()
{
	return c_posMode;
}

int MHConfigurator::getUDPPort()
{
	return c_udpPort;
}

bool MHConfigurator::getWriteDebugDumps()
{
	return c_writeDebugDumps;
}

bool MHConfigurator::loadUDPSpecificConfs(json& conf)
{
	if (!conf.contains("UDPPosPort"))
	{
		return false;
	}
	try {
		c_udpPort = conf["UDPPosPort"].get<int>();
	}
	catch (const json::type_error& e)
	{
		return false;
	}
	
	return true;
}

bool MHConfigurator::loadGPSSpecificConfs(json& conf)
{
	return false; //todo when implementing gps receiver support
}
