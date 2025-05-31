#include <fancyFrontEnd.h>
#include <chrono>
#include <iostream>
#include <thread>

void fancyFrontEndWorker::run() {
	auto loopStart = std::chrono::system_clock::now();
	auto targetSleepInterval = std::chrono::milliseconds(static_cast<int>(std::round(m_updateIntervalS * 1000)));
	std::chrono::milliseconds loopDur = std::chrono::milliseconds(0);
	std::chrono::milliseconds sleepTime = std::chrono::milliseconds(1000);
	
	while (m_shareData->appIsRunning)
	{
		loopStart = std::chrono::system_clock::now();
		if (m_shareData->outputHorizonDataAvailable)
		{
			// do things
		}

		loopDur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - loopStart);
		sleepTime = targetSleepInterval - loopDur;
		
		if (sleepTime > std::chrono::milliseconds(0)) {
			std::this_thread::sleep_for(sleepTime);
		}
	}
}