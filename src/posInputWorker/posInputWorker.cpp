#include <posInputWorker.h>

#ifdef  _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif //  _WIN32

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

void posInputWorker::closeUDPSocket() // This should be called from destructor
{
	if (m_sockfd)
	{
#ifdef _WIN32
		closesocket(m_sockfd);
		WSACleanup();
#else
		close(sockfd);
#endif
	}
	m_sockfd = 0;
}

bool posInputWorker::openUDPSocket()
{
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	m_sockfd = static_cast<int>(socket(AF_INET, SOCK_DGRAM, 0));
	if (m_sockfd < 0) {
		Tracer::log("Socket creation failed", traceLevel::ERROR);
		return false;
	}

#ifdef _WIN32 // Enable 5s timeout for socket as otherwise the positioning thread will never be able to quit unless it receives data
	DWORD timeoutMs = 5000;
	setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));
#else
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif

	sockaddr_in servaddr{};
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(m_udpPosPrt);

	if (bind(m_sockfd, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		Tracer::log("Socket creation failed", traceLevel::ERROR);
		return false;
	}

	return true;
}

bool posInputWorker::startUDPPosLoop()
{
	if (!openUDPSocket())
	{
		return false;
	}

	char buffer[1024];
	sockaddr_in clientaddr{};
#ifdef _WIN32
	int len = sizeof(clientaddr);
#else
	socklen_t len = sizeof(clientaddr);
#endif

	std::string rcvStr;
	while (m_sharedata->appIsRunning) {
		int n = static_cast<int>(recvfrom(m_sockfd, buffer, sizeof(buffer), 0,
			(sockaddr*)&clientaddr, &len));
		if (n > 0) {
			rcvStr = std::string(buffer, n);
			Tracer::log("UDP thread received data: " + rcvStr, traceLevel::DEBUG);
			parseUDPMsg(rcvStr);
		}
	}

	closeUDPSocket();
	return true;
}

void posInputWorker::parseUDPMsg(std::string& rcvData)
{
	std::stringstream ss(rcvData);
	std::string token;
	sharedData::inputPosition newPos;
	if (std::getline(ss, token, ',')) {
		newPos.lat = std::stod(token);
	}
	if (std::getline(ss, token, ',')) {
		newPos.lon = std::stod(token);
	}
	if (std::getline(ss, token, ',')) {
		newPos.speed = std::stod(token);
	}
	if (std::getline(ss, token, ',')) {
		newPos.heading = std::stod(token);
	}

	if (newPos.isValidObs())
	{
		std::lock_guard<std::mutex> guard(m_sharedata->inputPosMutex);
		m_sharedata->incomingPositions.push(newPos);
	}
}

bool posInputWorker::startGNSSPosLoop()
{
	return false; //TODO
}