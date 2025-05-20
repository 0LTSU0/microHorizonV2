#pragma once
#include <string>

#undef ERROR //Libosmium defines some "ERROR" which conflicts with traceLevel here -> undef

enum class traceLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Tracer {    
public:
    enum traceMode {
        CONSOLE,
        TRACEFILE //to be implemented
    };
    
    static void log(const std::string& message, traceLevel);
    static void changeTracingMode(Tracer::traceMode mode);

private:
    static Tracer::traceMode tracerMode;
};