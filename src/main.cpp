#include <iostream>
#include <signal.h>
#include <thread>

#include "CommonUtils/GeneralLogger.h"

void sigHandler(int signalNumber)
{
    GPCRIT("Signal Caught! {}", signalNumber);

    CommonUtils::GeneralLogger::traceLogger->dump_backtrace();
}


int main()
{
    // Catch seg faults for logging purposes
    // and initialize logger.  this should alway
    // be done as soon as possible!
    signal(SIGSEGV, sigHandler);
    CommonUtils::GeneralLogger logObject;
    logObject.init("StarterProj");
    
    GPTRACE("Trace Test!");
    GPINFO("Hello Starter Project!");
    return 0;
}