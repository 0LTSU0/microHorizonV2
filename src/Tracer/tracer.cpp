#include "tracer.h"
#include <iostream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <sstream>


std::mutex tracerMutex;

//init static members
Tracer::traceMode Tracer::tracerMode = traceMode::CONSOLE; //Init to console, shall be set to file at the beginning of app startup if needed using changeTracingMode()

void toConsole(std::string string)
{
    std::cout << string << std::endl;
}

void Tracer::log(const std::string& message, traceLevel level) {
    std::lock_guard<std::mutex> lock(tracerMutex);
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%H:%M:%S") << "." << std::setw(3) << std::setfill('0') << ms.count();
    switch (level)
    {
    case traceLevel::DEBUG:
        oss << " [DEBUG] " << message;
        break;
    case traceLevel::INFO:
        oss << " [INFO] " << message;
        break;
    case traceLevel::WARNING:
        oss << " [WARNING] " << message;
        break;
    case traceLevel::ERROR:
        oss << " [ERROR] " << message;
        break;
    }
        
    switch (Tracer::tracerMode) {
    case traceMode::CONSOLE:
        toConsole(oss.str());
        break;
    case traceMode::TRACEFILE:
        std::cout << "Tracing to file is not yet implemented! " << oss.str() << std::endl;
        break;
    default:
        break;
    }
}

void Tracer::changeTracingMode(Tracer::traceMode mode)
{
    std::lock_guard<std::mutex> lock(tracerMutex);
    tracerMode = mode;
    //TODO if set to file, should open the file etc.
}