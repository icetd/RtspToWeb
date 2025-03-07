#include <iostream>
#include "VideoServer.h"
#include "Logger.h"
#include "log.h"
int main()
{
    initLogger(INFO);
    Logger::setLogLevel(Logger::LogLevel::ERROR);
    Logger::setOutput(DefaultOutput);
    VideoServer video_server;
    video_server.run(10);
    return 0;
}