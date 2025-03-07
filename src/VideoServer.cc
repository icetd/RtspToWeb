#include "VideoServer.h"
#include "Logger.h"
#include <nlohmann/json.hpp>
#include <iostream>

using nlohmann::json;

VideoServer::VideoServer() :
    m_loop(new EventLoop())
{
    m_server = std::make_unique<WebSocketServer>(m_loop, InetAddress(9000));
}

VideoServer::~VideoServer()
{
}

void VideoServer::clientConnectCallback(const TcpConnectionPtr &conn)
{
    std::cout << "client :" << conn->fd() << " connect" << std::endl;
}

void VideoServer::clientCloseCallback(const TcpConnectionPtr &conn)
{
    std::cout << "client :" << conn->fd() << " closed" << std::endl;
    auto cap = m_mediaChannls.find(conn->fd());
    if (cap == m_mediaChannls.end())
        return;
    
    auto index = m_taskIndexes.find(conn->fd());
    if (index == m_taskIndexes.end())   
        return;
    
    conn->getLoop()->cancel(index->second);
}

void VideoServer::messageCallback(const Buffer *input, const TcpConnectionPtr &conn) 
{
    const std::string start_back = "start";
    const std::string stop_back = "stop";
    const std::string err_back_json = "check json example: {\"cmd\": \"start\",\"url\": \"rtsp://192.168.2.105:8555/unicast\"}";
    const std::string err_back_rtsp = "check json rtsp url is exist";
 
    if (input->readableBytes() == 0) {
        WebSocketServer::getInstance()->send(err_back_json.c_str(), err_back_json.size(), WSCodeType::WSCodeText, conn);
        return;
    }

    json j = json::parse(std::string(input->peek(), input->readableBytes()));
    std::string cmd, url;
   
    if (j.contains("cmd")) {
        cmd = j.at("cmd").get<std::string>();
    } else {
        WebSocketServer::getInstance()->send(err_back_json.c_str(), err_back_json.size(), WSCodeType::WSCodeText, conn);
        return;
    } 
    
    if (j.contains("url")) {
        url = j.at("url").get<std::string>();
    } else {
        WebSocketServer::getInstance()->send(err_back_json.c_str(), err_back_json.size(), WSCodeType::WSCodeText, conn);
        return;
    } 

    if (cmd == "start") {
        m_mediaChannls.insert(std::make_pair(conn->fd(), std::make_unique<RtspCapture>()));
        m_mediaChannls.find(conn->fd())->second->init();
        bool re = m_mediaChannls.find(conn->fd())->second->open(url.c_str());
        if (re) {
            WebSocketServer::getInstance()->send(start_back.c_str(), start_back.size(), WSCodeType::WSCodeText, conn);
            int64_t taskId = conn->getLoop()->runEvery(0.005, [this, conn] { taskSendH264(conn); });
            m_taskIndexes.insert(std::make_pair(conn->fd(), taskId));
        } else {
            WebSocketServer::getInstance()->send(err_back_rtsp.c_str(), err_back_rtsp.size(), WSCodeType::WSCodeText, conn);
        }
    } else if (cmd == "stop") {
        auto cap = m_mediaChannls.find(conn->fd());
        if (cap == m_mediaChannls.end())
            return;
        
        auto index = m_taskIndexes.find(conn->fd());
        if (index == m_taskIndexes.end())   
            return;
        
        conn->getLoop()->cancel(index->second);
        usleep(10000);
        WebSocketServer::getInstance()->send(stop_back.c_str(), stop_back.size(), WSCodeType::WSCodeText, conn);
    }
}

void VideoServer::run(int thread_num)
{
    m_server->setClientConnectCallback([this](const TcpConnectionPtr &conn) { clientConnectCallback(conn); });
    m_server->setClientCloseCallback([this](const TcpConnectionPtr &conn) { clientCloseCallback(conn); });
    m_server->setHttpCallback([this](const Buffer *input, const TcpConnectionPtr &conn) { messageCallback(input, conn); });
    m_server->start(thread_num);
    m_loop->loop();
}


void VideoServer::taskSendH264(const TcpConnectionPtr &conn)
{   
    if (!m_mediaChannls.find(conn->fd())->second)
        return;

    char *frame = (char*) malloc(sizeof(char) * m_mediaChannls.find(conn->fd())->second->getWidth() * m_mediaChannls.find(conn->fd())->second->getWidth());
    int len = m_mediaChannls.find(conn->fd())->second->geth264(frame);
    if (len > 0)
        WebSocketServer::getInstance()->send(frame, len, WSCodeType::WSCodeBinary, conn);
    free(frame);
}
