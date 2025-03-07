#ifndef RTSP_CAPTURE_H
#define RTSP_CAPTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif

#include <vector>
#include <mutex>

class RtspCapture {
public:
	RtspCapture();
	~RtspCapture();
	void init();
	bool open(const char* url);
    int geth264(char *frame);
	bool close();

    bool reconnect();

	int getWidth() { return width; }
	int getHeight() { return height; }

    void setReconnect(bool status) { is_enable_reconnect = status; }
    void setReconnectParams(int maxAttempts, int intervalMs)
    {
        max_reconnect_attempts = maxAttempts;
        reconnect_interval = intervalMs;
    }

private:
	int width;
	int height;

	int video_stream_index;
	AVRational time_base;
	AVFormatContext* av_format_ctx;
    AVFormatContext* av_output_format_ctx;
	AVPacket* av_packet;
    
    int reconnect_attempts;     // 当前重连尝试次数
    int max_reconnect_attempts; // 最大重连次数
    int reconnect_interval;     // 重连间隔（毫秒）
    bool reconnecting;          // 是否正在重连
    bool is_enable_reconnect;
    bool is_connected;

    std::mutex buffer_mutex;

    const char *re_url;
};

#endif