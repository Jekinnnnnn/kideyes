#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libswscale/swscale.h"
#include "include/libavutil/frame.h"
#include "include/libavutil/imgutils.h"
#ifdef __cplusplus
};
#endif

#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <Windows.h>

namespace frameutil
{
	auto freeAVCodecContext = [](AVCodecContext* codecCtx)
	{
		if (codecCtx == nullptr) return;
		avcodec_close(codecCtx);
	};
	using AVCodecContextType = std::unique_ptr<AVCodecContext, decltype(freeAVCodecContext)>;

	AVCodecContextType AllocateAVCodecContext(const AVCodec* codec)
	{
		return AVCodecContextType(avcodec_alloc_context3(codec), freeAVCodecContext);
	}

	auto freeFormatContext = [](AVFormatContext* formatCtx)
	{
		if (formatCtx == nullptr) return;
		avformat_close_input(&formatCtx);
	};

	using AVFormatContextType = std::unique_ptr <AVFormatContext, decltype(freeFormatContext)>;
	struct ImageNameArgs
	{
		int isKeyFrame;	// 1 -> keyframe, 0-> not
		char frameType;	// I B P
		int frameIdx;
	};
	// open video stream by ffmpeg
	class VideoStream
	{
	public:
		explicit VideoStream(std::string const& filePath, std::string const& fileName);
		virtual ~VideoStream();
		void ProcessFrameByFrame();
	private:
		void SaveFrameToBmp(uint8_t* data, int width, int height, int bpp, ImageNameArgs nameArgs);
	private:
		AVFormatContextType m_formatContext;
		AVCodecContextType m_decoderContext;

		int m_videoStreamIndex{ -1 };
		AVCodec* m_decoder{ nullptr };

		std::string m_inputFilePath;
		std::string m_inputFileName;
	};

	auto freeAVFrame = [](AVFrame* frame)
	{
		if (frame == nullptr) return;
		av_free(frame);
	};
	using AVFrameType = std::unique_ptr<AVFrame, decltype(freeAVFrame)>;
	AVFrameType AllocateAVFrame()
	{
		return AVFrameType(av_frame_alloc(), freeAVFrame);
	}

	auto freeSwsContext = [](SwsContext* swsCtx)
	{
		if (swsCtx == nullptr) return;
		sws_freeContext(swsCtx);
	};

	using SwsContextType = std::unique_ptr<SwsContext, decltype(freeSwsContext)>;
	SwsContextType AllocateSwsContext(
			int srcW,
			int srcH, 
			enum AVPixelFormat srcFormat,
			int dstW, 
			int dstH,
			enum AVPixelFormat dstFormat,
			int flags, 
			SwsFilter* srcFilter,
			SwsFilter* dstFilter,
			const double* param
		)
	{
		return SwsContextType(sws_getContext(srcW,srcH,srcFormat,dstW,dstH,dstFormat,flags, srcFilter, dstFilter, param), freeSwsContext);
	}

	auto freeAVPacket = [](AVPacket* packet)
	{
		if (packet == nullptr) return;
		av_packet_unref(packet);
	};

	using AVPacketType = std::unique_ptr<AVPacket, decltype(freeAVPacket)>;
	AVPacketType AllocateAVPacket()
	{
		return AVPacketType(av_packet_alloc(), freeAVPacket);
	}

	auto freeAVMemory = [](void* ptr)
	{
		if (ptr == nullptr) return;
		av_free(ptr);
	};
	using AVMemoryType = std::unique_ptr<void, decltype(freeAVMemory)>;
	AVMemoryType AllocateMemory(size_t size)
	{
		return AVMemoryType(av_malloc(size), freeAVMemory);
	}
} // namespace frameutil