#ifndef VIDEO_SERVER_H
#define VIDEO_SERVER_H

#include "EventLoop.h"
#include "WebSocketServer.h"
#include "RtspCapture.h"
#include <memory>

using namespace inet;

class VideoServer
{
public:
    typedef struct
    {
        int port;
        int thread_num;
        int log_level;
        std::string log_basename;
    } Config_t;
    
    VideoServer();
    ~VideoServer();
    void run();
private:
    Config_t m_config;
    EventLoop *m_loop;
    std::unique_ptr<WebSocketServer> m_server;
    std::unordered_map<int, std::unique_ptr<RtspCapture>> m_mediaChannls;
    std::unordered_map<int, int64_t> m_taskIndexes;

    void taskSendH264(const TcpConnectionPtr &conn);
    void messageCallback(const Buffer *input, const TcpConnectionPtr &conn);
    void clientConnectCallback(const TcpConnectionPtr &conn);
    void clientCloseCallback(const TcpConnectionPtr &conn);
};

#endif
