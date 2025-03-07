#include <iostream>
#include "VideoServer.h"
#include "log.h"
int main()
{
    initLogger(ERROR);
    VideoServer video_server;
    video_server.run();
    return 0;
}