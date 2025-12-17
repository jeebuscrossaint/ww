#include "ww.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// Forward declarations
extern void set_error(const char *msg);
struct image_data_t {
    uint8_t *data;
    int width;
    int height;
    int channels;
};

// Video decoder state
struct video_decoder_t {
    AVFormatContext *format_ctx;
    AVCodecContext *codec_ctx;
    struct SwsContext *sws_ctx;
    int video_stream_idx;
    
    int width;
    int height;
    int target_width;
    int target_height;
    
    AVFrame *frame;
    AVPacket *packet;
    
    double fps;
    double frame_duration;
    int64_t start_time;
    
    bool loop;
    bool eof;
    
    pthread_mutex_t lock;
};

// Create video decoder
extern "C" video_decoder_t* ww_video_create(const char *path, int target_width, int target_height, bool loop) {
    if (!path) {
        set_error("NULL path provided");
        return nullptr;
    }
    
    video_decoder_t *decoder = (video_decoder_t*)calloc(1, sizeof(video_decoder_t));
    if (!decoder) {
        set_error("Out of memory");
        return nullptr;
    }
    
    decoder->target_width = target_width;
    decoder->target_height = target_height;
    decoder->loop = loop;
    decoder->video_stream_idx = -1;
    pthread_mutex_init(&decoder->lock, nullptr);
    
    // Open input file
    if (avformat_open_input(&decoder->format_ctx, path, nullptr, nullptr) < 0) {
        set_error("Failed to open video file");
        free(decoder);
        return nullptr;
    }
    
    // Retrieve stream information
    if (avformat_find_stream_info(decoder->format_ctx, nullptr) < 0) {
        set_error("Failed to find stream info");
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    // Find video stream
    for (unsigned int i = 0; i < decoder->format_ctx->nb_streams; i++) {
        if (decoder->format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            decoder->video_stream_idx = i;
            break;
        }
    }
    
    if (decoder->video_stream_idx == -1) {
        set_error("No video stream found");
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    // Get codec parameters
    AVCodecParameters *codecpar = decoder->format_ctx->streams[decoder->video_stream_idx]->codecpar;
    
    // Find decoder
    const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        set_error("Codec not found");
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    // Allocate codec context
    decoder->codec_ctx = avcodec_alloc_context3(codec);
    if (!decoder->codec_ctx) {
        set_error("Failed to allocate codec context");
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    // Copy codec parameters to context
    if (avcodec_parameters_to_context(decoder->codec_ctx, codecpar) < 0) {
        set_error("Failed to copy codec parameters");
        avcodec_free_context(&decoder->codec_ctx);
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    // Open codec
    if (avcodec_open2(decoder->codec_ctx, codec, nullptr) < 0) {
        set_error("Failed to open codec");
        avcodec_free_context(&decoder->codec_ctx);
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    decoder->width = decoder->codec_ctx->width;
    decoder->height = decoder->codec_ctx->height;
    
    // Get FPS
    AVRational frame_rate = decoder->format_ctx->streams[decoder->video_stream_idx]->avg_frame_rate;
    if (frame_rate.num && frame_rate.den) {
        decoder->fps = (double)frame_rate.num / (double)frame_rate.den;
        decoder->frame_duration = 1.0 / decoder->fps;
    } else {
        decoder->fps = 30.0;
        decoder->frame_duration = 1.0 / 30.0;
    }
    
    // Allocate frame and packet
    decoder->frame = av_frame_alloc();
    decoder->packet = av_packet_alloc();
    
    if (!decoder->frame || !decoder->packet) {
        set_error("Failed to allocate frame/packet");
        if (decoder->frame) av_frame_free(&decoder->frame);
        if (decoder->packet) av_packet_free(&decoder->packet);
        avcodec_free_context(&decoder->codec_ctx);
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    // Initialize software scaler
    decoder->sws_ctx = sws_getContext(
        decoder->width, decoder->height, decoder->codec_ctx->pix_fmt,
        target_width, target_height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
    
    if (!decoder->sws_ctx) {
        set_error("Failed to initialize scaler");
        av_frame_free(&decoder->frame);
        av_packet_free(&decoder->packet);
        avcodec_free_context(&decoder->codec_ctx);
        avformat_close_input(&decoder->format_ctx);
        free(decoder);
        return nullptr;
    }
    
    return decoder;
}

// Decode next frame
extern "C" image_data_t* ww_video_next_frame(video_decoder_t *decoder) {
    if (!decoder) {
        set_error("NULL decoder");
        return nullptr;
    }
    
    pthread_mutex_lock(&decoder->lock);
    
    // Read frames until we get a video frame
    while (true) {
        int ret = av_read_frame(decoder->format_ctx, decoder->packet);
        
        if (ret < 0) {
            if (ret == AVERROR_EOF) {
                // End of file
                if (decoder->loop) {
                    // Seek back to start
                    av_seek_frame(decoder->format_ctx, decoder->video_stream_idx, 0, AVSEEK_FLAG_BACKWARD);
                    avcodec_flush_buffers(decoder->codec_ctx);
                    continue;
                } else {
                    decoder->eof = true;
                    pthread_mutex_unlock(&decoder->lock);
                    return nullptr;
                }
            } else {
                set_error("Error reading frame");
                pthread_mutex_unlock(&decoder->lock);
                return nullptr;
            }
        }
        
        // Check if packet is from video stream
        if (decoder->packet->stream_index != decoder->video_stream_idx) {
            av_packet_unref(decoder->packet);
            continue;
        }
        
        // Send packet to decoder
        ret = avcodec_send_packet(decoder->codec_ctx, decoder->packet);
        av_packet_unref(decoder->packet);
        
        if (ret < 0) {
            set_error("Error sending packet to decoder");
            pthread_mutex_unlock(&decoder->lock);
            return nullptr;
        }
        
        // Receive decoded frame
        ret = avcodec_receive_frame(decoder->codec_ctx, decoder->frame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            set_error("Error receiving frame from decoder");
            pthread_mutex_unlock(&decoder->lock);
            return nullptr;
        }
        
        // Successfully decoded a frame
        break;
    }
    
    // Allocate output image
    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) {
        set_error("Out of memory");
        pthread_mutex_unlock(&decoder->lock);
        return nullptr;
    }
    
    img->width = decoder->target_width;
    img->height = decoder->target_height;
    img->channels = 4;
    img->data = (uint8_t*)malloc(decoder->target_width * decoder->target_height * 4);
    
    if (!img->data) {
        set_error("Out of memory");
        free(img);
        pthread_mutex_unlock(&decoder->lock);
        return nullptr;
    }
    
    // Convert frame to RGBA
    uint8_t *dst_data[4] = { img->data, nullptr, nullptr, nullptr };
    int dst_linesize[4] = { decoder->target_width * 4, 0, 0, 0 };
    
    sws_scale(
        decoder->sws_ctx,
        decoder->frame->data, decoder->frame->linesize,
        0, decoder->height,
        dst_data, dst_linesize
    );
    
    av_frame_unref(decoder->frame);
    pthread_mutex_unlock(&decoder->lock);
    
    return img;
}

// Get frame duration in seconds
extern "C" double ww_video_get_frame_duration(video_decoder_t *decoder) {
    if (!decoder) {
        return 1.0 / 30.0; // Default 30 FPS
    }
    return decoder->frame_duration;
}

// Check if video reached EOF
extern "C" bool ww_video_is_eof(video_decoder_t *decoder) {
    if (!decoder) {
        return true;
    }
    return decoder->eof;
}

// Seek to start
extern "C" void ww_video_seek_start(video_decoder_t *decoder) {
    if (!decoder) {
        return;
    }
    
    pthread_mutex_lock(&decoder->lock);
    av_seek_frame(decoder->format_ctx, decoder->video_stream_idx, 0, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(decoder->codec_ctx);
    decoder->eof = false;
    pthread_mutex_unlock(&decoder->lock);
}

// Destroy video decoder
extern "C" void ww_video_destroy(video_decoder_t *decoder) {
    if (!decoder) {
        return;
    }
    
    pthread_mutex_lock(&decoder->lock);
    
    if (decoder->sws_ctx) {
        sws_freeContext(decoder->sws_ctx);
    }
    
    if (decoder->frame) {
        av_frame_free(&decoder->frame);
    }
    
    if (decoder->packet) {
        av_packet_free(&decoder->packet);
    }
    
    if (decoder->codec_ctx) {
        avcodec_free_context(&decoder->codec_ctx);
    }
    
    if (decoder->format_ctx) {
        avformat_close_input(&decoder->format_ctx);
    }
    
    pthread_mutex_unlock(&decoder->lock);
    pthread_mutex_destroy(&decoder->lock);
    
    free(decoder);
}