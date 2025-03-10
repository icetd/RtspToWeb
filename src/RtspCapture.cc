#include "RtspCapture.h"
#include "log.h"
#include <thread>
#include <malloc.h>
#include <iostream>

RtspCapture::RtspCapture()
{
}

RtspCapture::~RtspCapture()
{
    close();
}

void RtspCapture::init()
{
    is_connected = false;
    reconnect_attempts = 0;
    max_reconnect_attempts = 3;
    reconnect_interval = 1000;
    reconnecting = false;
	video_stream_index = -1;
	av_format_ctx = nullptr;
	av_packet = nullptr;
	avformat_network_init();
	av_log_set_level(AV_LOG_FATAL);
	LOG(INFO, "libCapture init success.");
}

bool RtspCapture::open(const char* url)
{
    re_url = url;
	av_format_ctx = avformat_alloc_context();
	if (!av_format_ctx) {
		LOG(ERROR, "Couldn't create AVFormatContext");
		return false;
	}

	/* options rtsp*/
	AVDictionary* opts = nullptr;
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);
	av_dict_set(&opts, "buffer_size", "1024000", 0);
	av_dict_set(&opts, "stimeout", "1000000", 0);
	av_dict_set(&opts, "max_delay", "1000000", 0);

	if (avformat_open_input(&av_format_ctx, url, NULL, &opts) != 0) {
		LOG(ERROR, "Could't open video url");
		return false;
	}
	avformat_find_stream_info(av_format_ctx, NULL);
	int totalMs = av_format_ctx->duration / (AV_TIME_BASE / 1000);
	LOG(INFO, "total time : %dms", totalMs);

	/** @brief echo format info */
	av_dump_format(av_format_ctx, NULL, NULL, false);

	AVCodecParameters* av_codec_params;
	AVStream* av_stream;

	for (uint32_t i = 0; i < av_format_ctx->nb_streams; ++i) {
		av_stream = av_format_ctx->streams[i];
		av_codec_params = av_format_ctx->streams[i]->codecpar;

		if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
			width = av_codec_params->width;
			height = av_codec_params->height;
			time_base = av_format_ctx->streams[i]->time_base;
			break;
		}
	}

	if (video_stream_index == -1) {
		LOG(ERROR, "Couldn't find valid video stream from url");
		return false;
	}

	av_packet = av_packet_alloc();
	if (!av_packet) {
		LOG(ERROR, "Couldn't allocate AVPacket");
		return false;
	}
	LOG(INFO, "libCapture open url success.");

    is_connected = true;
	return true;
}

bool RtspCapture::reconnect() 
{
    if (reconnecting) {
        LOG(INFO, "Already reconnecting...");
        return false;
    }

    reconnecting = true;
    close();

    while (reconnect_attempts < max_reconnect_attempts && is_enable_reconnect) {
        LOG(INFO, "Attempting to reconnect... (%d/%d)", reconnect_attempts + 1, max_reconnect_attempts);
        if (open(re_url)) { 
            reconnect_attempts = 0; // 重置重连次数
            reconnecting = false;
            LOG(INFO, "Reconnection successful.");
            return true;
        }

        reconnect_attempts++;
        std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_interval)); // 线程休眠
    }

    reconnecting = false;
    LOG(ERROR, "Failed to reconnect after %d attempts.", max_reconnect_attempts);
    return false;
}

int RtspCapture::geth264(char *frame)
{
    int size;
    char errStr[256] = {0};
    is_enable_reconnect = true;

    while (is_connected) {
        int ret = av_read_frame(av_format_ctx, av_packet);
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                LOG(INFO, "End of stream reached");
                if (!reconnect()) {
                    return false;
                }
                continue;
            }
            // 判断是否为连接错误
            if (ret == AVERROR(EIO) || ret == AVERROR(ECONNRESET)) {
                LOG(ERROR, "Network error detected, attempting to reconnect...");
                if (!reconnect()) {
                    return false;
                }
                continue; 
            }

            av_strerror(ret, errStr, sizeof(errStr));
            LOG(ERROR, "av_read_frame failed: %s", errStr);
            return false;
        }

        if (av_packet->stream_index != video_stream_index) {
            av_packet_unref(av_packet);
            continue;
        }

        std::lock_guard<std::mutex> lock(buffer_mutex);
        memcpy(frame, av_packet->data, av_packet->size);
        size = av_packet->size;
        av_packet_unref(av_packet);
        return size;
    }

    return size;
}

bool RtspCapture::close()
{
	if (av_format_ctx) {
		avformat_close_input(&av_format_ctx);
		avformat_free_context(av_format_ctx);
	}
	if (av_packet) {
		av_free(av_packet);
	}

    av_packet = nullptr;
    is_connected = false;
	return true;
}