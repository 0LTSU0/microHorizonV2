#include <posInputWorker.h>


void posInputWorker::run()
{
	switch (m_posMode) {
	case POSMode::UDP:
		startUDPPosLoop();
		break;
	case POSMode::GPS_RECEIVER:
		startGNSSPosLoop();
		break;
	default:
		Tracer::log("FAILED TO START posInputWorker: run() called with unexpected position mode", traceLevel::ERROR);
		break;
	}
}

bool startUDPPosLoop()
{
	return true;
}