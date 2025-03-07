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
    VideoServer();
    ~VideoServer();
    void run(int thread_num);
private:
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
