#pragma once
#include <memory>

#include "SharedData.h"
#include <tracer.h>

enum POSMode
{
    UDP,
    GPS_RECEIVER
};

class posInputWorker {
public:
    posInputWorker(std::shared_ptr<sharedData::SharedData> data, POSMode posMode, int udpPort)
        : m_sharedata(data), 
          m_posMode(posMode),
          m_udpPosPrt(udpPort) { }
    ~posInputWorker() { 
        Tracer::log("posInputWorker destructor called", traceLevel::DEBUG); //temp to see if all components quit properly
        closeUDPSocket(); 
    }
    
    void run(); // Open UDP port or GNSS receiver reader and start receiving input data

private:
    std::shared_ptr<sharedData::SharedData> m_sharedata;
    POSMode m_posMode;
    int m_udpPosPrt = 12345;
    int m_sockfd = -1;

    bool startUDPPosLoop();
    bool openUDPSocket();
    void closeUDPSocket();
    void parseUDPMsg(std::string& rcvData);
    bool startGNSSPosLoop(); //TODO
};