// process frame by ffmpeg
#include "frame_util.hpp"

using namespace frameutil;
using namespace std;

// TODO videoStream mgr manage files

frameutil::VideoStream::VideoStream(string const& filePath, string const& fileName)
	:m_formatContext(nullptr, freeFormatContext),m_decoderContext(nullptr,freeAVCodecContext)
{
	AVFormatContext* formatCtx = nullptr;
	string videopath = filePath + "/" + fileName;
	cout << "file path : " << videopath << endl;
	if (avformat_open_input(&formatCtx, videopath.c_str(), nullptr, nullptr) >= 0)
	{
		if (avformat_find_stream_info(formatCtx, nullptr) >= 0)
		{
			// open file successful
			// find video stream index
			int videoStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
			if (videoStreamIndex >= 0)
			{
				// as media has mutil stream
				AVStream* videoStream = formatCtx->streams[videoStreamIndex];
				AVCodec* decoder = avcodec_find_decoder(videoStream->codecpar->codec_id);
				if (decoder != nullptr)
				{
					auto tmpCodecCtx = AllocateAVCodecContext(decoder);
					if (avcodec_parameters_to_context(tmpCodecCtx.get(), videoStream->codecpar) >= 0)
					{
						if (avcodec_open2(tmpCodecCtx.get(), decoder, nullptr) >= 0)
						{
							m_decoder = decoder;
							m_videoStreamIndex = videoStreamIndex;
							m_formatContext.reset(formatCtx);
							m_decoderContext.reset(tmpCodecCtx.release());
						}
					} 
				}
			}
		}
	}
	else
	{
		avformat_close_input(&formatCtx);
	}
}

frameutil::VideoStream::~VideoStream()
{
	m_decoder = nullptr;
	m_videoStreamIndex = -1;
}

void frameutil::VideoStream::SaveFrameToBmp(uint8_t* data, int width, int height, int bpp, ImageNameArgs nameArgs)
{
	string nameSeparator = "_";
	string extraName = nameSeparator + to_string(nameArgs.frameIdx) + nameSeparator + to_string(nameArgs.frameType) + nameSeparator + to_string(nameArgs.isKeyFrame);
	string outputPath = "E:/Project/githubRepo/kideyes/tmp/sceneshot";
	string outputName = "";
	string output = outputPath + "/" + outputName + extraName + ".bmp";
	ofstream fout(output.c_str(), ofstream::out | ofstream::binary);
	if (!fout) 
	{
		cout << "open fout fail filename : " << output << endl;
		return;
	}

	BITMAPFILEHEADER bmpHeader;
	bmpHeader.bfType = 0x4d42;
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmpHeader.bfSize = bmpHeader.bfOffBits + width * height * bpp / 8;

	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = bpp;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage = (width * bpp + 31) / 32 * 4 * height;
	bmpInfo.bmiHeader.biXPelsPerMeter = 100;
	bmpInfo.bmiHeader.biYPelsPerMeter = 100;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	fout.write(reinterpret_cast<const char*>(&bmpHeader), sizeof(BITMAPFILEHEADER));
	fout.write(reinterpret_cast<const char*>(&bmpInfo.bmiHeader), bmpInfo.bmiHeader.biSize);
	fout.write(reinterpret_cast<const char*>(data), width * height * bpp / 8);
	fout.close();
}

void frameutil::VideoStream::ProcessFrameByFrame()
{
	if (m_decoderContext == nullptr)
	{
		cout << "codec context not init" << endl;
		return;
	}
	auto videoFrame = AllocateAVFrame();
	if (videoFrame.get() == nullptr)
	{
		cout << "allocate video frame err" << endl;
		return;
	}
	auto pictureFrame = AllocateAVFrame();
	if (pictureFrame.get() == nullptr)
	{
		cout << "allocate video frame err" << endl;
		return;
	}
	size_t pictureByte = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_decoderContext.get()->width, m_decoderContext.get()->height, 1);
	auto pictureBuff = AllocateMemory(pictureByte * sizeof(uint8_t));
	av_image_fill_arrays(pictureFrame->data, pictureFrame->linesize, reinterpret_cast<const uint8_t *>(pictureBuff.get()), AV_PIX_FMT_RGB24, m_decoderContext.get()->width, m_decoderContext.get()->height, 1);

	auto swsCtx = AllocateSwsContext(m_decoderContext.get()->width, m_decoderContext.get()->height, m_decoderContext.get()->pix_fmt,
		m_decoderContext.get()->width, m_decoderContext.get()->height, AV_PIX_FMT_BGR24,
		SWS_BICUBIC, nullptr, nullptr, nullptr);
	auto packet = AllocateAVPacket();
	int pictureCnt = 1;
	while (av_read_frame(m_formatContext.get(), packet.get()) >= 0)
	{
		if (packet.get()->stream_index != m_videoStreamIndex) continue;
		// send raw data to decode
		//if (!packet.get()->flags) continue;
		avcodec_send_packet(m_decoderContext.get(), packet.get());
		// send decode data to struct
		if (avcodec_receive_frame(m_decoderContext.get(), videoFrame.get()) != 0) continue;
		// convert
		sws_scale(swsCtx.get(),
			videoFrame.get()->data, videoFrame.get()->linesize,
			0, m_decoderContext.get()->height,
			pictureFrame.get()->data, pictureFrame.get()->linesize);
		ImageNameArgs args{ packet.get()->flags, av_get_picture_type_char(videoFrame.get()->pict_type), pictureCnt++ };

		frameutil::VideoStream::SaveFrameToBmp(pictureFrame.get()->data[0], videoFrame.get()->width, videoFrame.get()->height, 24, args);
	}
}

int main()
{
	string inputPath = "E:/Project/githubRepo/kideyes/hls/hls";
	//string inputName = "test.mp4";
	string inputName = "20190805-0.ts";

	VideoStream tsFrameToBmp(inputPath, inputName);
	tsFrameToBmp.ProcessFrameByFrame();
	system("pause");
	return 0;
}
