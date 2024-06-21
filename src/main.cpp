#include <iostream>
#include <signal.h>
#include <thread>

#include "CommonUtils/GeneralLogger.h"

void sigHandler(int signalNumber)
{
    GPCRIT("Signal Caught! {}", signalNumber);

    CommonUtils::GeneralLogger::traceLogger->dump_backtrace();
    exit(1);
}


int main()
{
    // Catch seg faults for logging purposes, then
    // initialize the logger.  this should alway
    // be done as soon as possible!
    signal(SIGSEGV, sigHandler);
    CommonUtils::GeneralLogger logObject;
    logObject.init("StarterProj");
    

    // Now let's actually do something with our program!
    GPINFO("Hello Starter Project!");

    return 0;
}