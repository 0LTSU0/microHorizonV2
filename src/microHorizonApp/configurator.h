#pragma once

#include "json.hpp"
#include <posInputWorker.h> //for POSMode enum
#include "tracer.h"

using nlohmann::json;


struct cmdArgs
{
	std::string configPath; // Path for json config file
	int appTimeOut = 0; // Time after which app should quit
};

cmdArgs parseCMDArgs(int argc, char* argv[]);

enum frontEndMode {
	NONE,
	FANCY,
	RASPI
};

class MHConfigurator
{
public:
	bool loadConfig(std::string&);
	std::string getMapPath();
	POSMode getPosMode();
	int getUDPPort();
	float getLoadRadius();
	bool getWriteDebugDumps();
	float getFEUpdateFreq();
	frontEndMode getFEMode();
	std::string getFontPath();

private:
	std::string c_mapPath = "";
	POSMode c_posMode = POSMode::UDP;
	int c_udpPort = 0;
	float c_loadRadius = 0.5; //radius in degrees (latlon) that is loaded around requested coordinates by OSMProcessor
	bool c_writeDebugDumps = false;
	float c_frontEndUpdateInterval = 1; //in seconds
	frontEndMode c_fronEndMode = frontEndMode::NONE;
	std::string c_feFontPath = "";

	bool loadUDPSpecificConfs(json& conf);
	bool loadGPSSpecificConfs(json& conf);
};