#include "videodecodec.h"
#include "render.h"

extern "C"
{
    #include <SDL/SDL.h>
}

#include <QDebug>

extern Render g_render; // 渲染器

int g_video_width;
int g_video_height;
double g_play_time = 0.00;
double g_video_duration = 0x00;
static int volume = (SDL_MIX_MAXVOLUME / 2);

VideoDecodec::VideoDecodec()
    :m_input_file(QString()),
    m_formatCtx(nullptr),
    m_codec_ctx(nullptr),
    m_frame(nullptr),
    m_video_index(-1),
    m_audio_index(-1),
    m_video_status(VIDEO_STOP),
    m_task_thread(nullptr),
    m_has_task(false)
{
    // 注册所有封装格式
    av_register_all();
}

VideoDecodec::~VideoDecodec()
{
    close_input();
}

bool VideoDecodec::set_input_file(QString file)
{
    if (file.isEmpty() || file == m_input_file)
        return false;

    qDebug() << "set input video file successed!\n";
    m_input_file = file;

    return true;
}

bool VideoDecodec::unpack()
{
    if (m_input_file.isEmpty())
        return false;

    m_formatCtx = avformat_alloc_context();

    int ret = avformat_open_input(&m_formatCtx, m_input_file.toLatin1().data(), NULL, NULL);
    if ( NULL != ret )
    {
        av_log(NULL, AV_LOG_WARNING, "avformat open input file failed!");
        return false;
    }

    ret = avformat_find_stream_info(m_formatCtx, NULL);
    if ( 0 > ret )
    {
        av_log(NULL, AV_LOG_ERROR, "avformat find stream info failed!");
        return false;
    }

    qDebug() << "open input video file successed!\n";
    return true;
}

bool VideoDecodec::get_stream_index()
{
    if (m_input_file.isEmpty() || m_formatCtx == nullptr)
        return false;

    // 搜索视频流索引号
    for (int index = 0; index < m_formatCtx->nb_streams; ++index)
    {
        if (m_formatCtx->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            m_video_index = index;
        }
        else if (m_formatCtx->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            m_audio_index = index;
        }
    }

    // 未找到视频流索引号
    if ( -1 == m_video_index )
    {
        av_log(NULL, AV_LOG_ERROR, "didn't find video stream\n");
        return false;
    }

    // 未找到音频流索引号
    if ( - 1 == m_audio_index )
    {
        qDebug() << "didn't find audio stream\n";
    }

    // 读取视频长宽
    g_video_width = m_formatCtx->streams[m_video_index]->codec->width;
    g_video_height = m_formatCtx->streams[m_video_index]->codec->height;

    g_video_duration = m_formatCtx->streams[m_video_index]->duration*av_q2d(m_formatCtx->streams[m_video_index]->time_base) * 1000;
    qDebug() << "get streams index successed!\n";
    return true;
}

bool VideoDecodec::create_video_task()
{
    if (nullptr == m_formatCtx )
        return false;

    // 关闭上一个播放线程
    if (m_task_thread != nullptr)
    {
        // 停止播放
        stop();

        while(m_task_thread != nullptr);
    }

    // 为新的播放任务创建新线程
    m_task_thread = new PlayThread(m_formatCtx, m_video_index, m_audio_index);
    m_video_status = VIDEO_UNPLAY;

    connect(m_task_thread, SIGNAL(finished()), this, SLOT(slot_close_task()));
    connect(this, SIGNAL(video_status_change_signal(videoStatu)), m_task_thread, SLOT(slot_video_status_change(videoStatu)));

    m_has_task = true;

    qDebug() << "create PlayThread successed!\n";
    return true;
}

void VideoDecodec::slot_close_task()
{
    if (m_task_thread->isFinished())
    {
        qDebug() << "PlayThread finished!" << endl;
        m_task_thread->deleteLater();
        m_task_thread = nullptr;

        m_has_task = false;
        m_video_status = VIDEO_UNPLAY;
    }

    emit finished();
}

void VideoDecodec::play()
{
    // 不存在播放任务, 则创建播放任务
    if (!m_has_task)	{
        create_video_task();

        av_seek_frame( m_formatCtx, m_audio_index, 0, AVSEEK_FLAG_BACKWARD );
        av_seek_frame( m_formatCtx, m_video_index, 0, AVSEEK_FLAG_BACKWARD );
    }

    // 已停止
    if (m_video_status == VIDEO_STOP)	return;

    if(m_video_status == VIDEO_PLAY)
    {
        // 播放
        qDebug() << "video status changed: VIDEO_PLAY ------------> VIDEO_PAUSE" << endl;
        m_video_status = VIDEO_PAUSE;
    }
    else if (m_video_status == VIDEO_PAUSE) {
        // 暂停
        qDebug() << "video status changed: VIDEO_PLAY <------------ VIDEO_PAUSE" << endl;
        m_video_status = VIDEO_PLAY;
    }
    else if (m_video_status == VIDEO_UNPLAY) {
        //开始
        qDebug() << "video status changed: VIDEO_UNPLAY ==========> VIDEO_PLAY" << endl;
        m_task_thread->start();
        m_video_status = VIDEO_PLAY;
    }
    emit video_status_change_signal(m_video_status);
}

void VideoDecodec::stop()
{
    if ( m_video_status == VIDEO_UNPLAY || m_video_status == VIDEO_STOP)	return;

    qDebug() << "video status changed: VIDEO_STOP" << endl;
    m_video_status = VIDEO_STOP;
    emit video_status_change_signal(m_video_status);
}

long long VideoDecodec::video_duration()
{
    if (nullptr == m_formatCtx)
        return  -1;

    return m_formatCtx->duration;
}

bool VideoDecodec::volume_up()
{
    if (volume >= SDL_MIX_MAXVOLUME)	return false;

    volume += 10;

    // 最大值
    if (volume > SDL_MIX_MAXVOLUME) {
        volume = SDL_MIX_MAXVOLUME;
    }
    qDebug() << "current volume: " << volume << endl;

    return true;
}

bool VideoDecodec::volume_down()
{
    if (volume <= 0)	return false;

    volume -= 10;

    // 最小值
    if (volume < 0) {
        volume = 0;
    }

    qDebug() << "current volume: " << volume << endl;

    return true;
}

int VideoDecodec::video_width()
{
    if (nullptr == m_codec_ctx)
        return -1;

    return m_codec_ctx->width;
}

int VideoDecodec::video_height()
{
    if (nullptr == m_codec_ctx)
        return -1;

    return m_codec_ctx->height;
}

bool VideoDecodec::initialize()
{
    bool ret = unpack();
    ret = get_stream_index();

    return ret;
}

void VideoDecodec::close_input()
{
    if (m_formatCtx)
    {
        qDebug() << "avformat close input\n";
        avformat_close_input(&m_formatCtx);
    }
}

void VideoDecodec::forward()
{
    int64_t current_dts = 0;
    int64_t VideoDts = current_dts + (int64_t)(10 / av_q2d(m_formatCtx->streams[m_video_index]->time_base));
    av_seek_frame(m_formatCtx, m_video_index, VideoDts, AVSEEK_FLAG_FRAME);
}

void VideoDecodec::backward()
{
    int64_t current_dts = 0;
    int64_t VideoDts = current_dts - (int64_t)(10 / av_q2d(m_formatCtx->streams[m_video_index]->time_base));
    av_seek_frame(m_formatCtx, m_video_index, VideoDts, AVSEEK_FLAG_FRAME|AVSEEK_FLAG_BACKWARD);
}

static Uint8* audio_chunk;
static Uint32 audio_len;  //音频剩余长度
static Uint8* audio_pos;  //静态控制音频播放位置

// udata就是我们给到SDL的指针，stream是我们要把声音数据写入的缓冲区指针，len是缓冲区的大小。
void fill_audio(void* udata,Uint8* stream,int len)
{
    SDL_memset(stream,0,len);
    if(audio_len == 0)
    return ;
    len = (len>audio_len?len:audio_len);   //尽可能为最大音频量

    SDL_MixAudio(stream,audio_pos,len,volume); //这里的音量设置为函数要求，不影响硬件音量

    audio_pos +=len;//音频播放位置
    audio_len -=len;//剩余音频长度
}

PlayThread::PlayThread(AVFormatContext *formatCtx, int videoStreamIndex, int audioStreamIndex, QObject *parent)
    :QThread(parent),
      m_format_ctx(formatCtx),
      m_video_codec_ctx(nullptr),
      m_video_codec(nullptr),
      m_audio_codec_ctx(nullptr),
      m_audio_codec(nullptr),
      m_video_index(videoStreamIndex),
      m_audio_index(audioStreamIndex),
      m_audio_out_buffer(nullptr),
      m_audio_out_buffer_size(0),
      m_audio_convert_ctx(nullptr),
      m_video_status(VIDEO_UNPLAY)
{
    // 根据视频流解码器上下文中解码器 id 查找视频解码器
    m_video_codec_ctx = m_format_ctx->streams[m_video_index]->codec;
    m_video_codec = avcodec_find_decoder(m_video_codec_ctx->codec_id);
    if (m_video_codec == nullptr) {
        qDebug() << "can not find video codec.\n";
        return;
    }

    // 打开音频解码器
    if (avcodec_open2(m_video_codec_ctx, m_video_codec, nullptr) < 0) {
        qDebug() << "open video codec failed!\n";
    }

    // 查找音频解码器
    m_audio_codec_ctx = m_format_ctx->streams[m_audio_index]->codec;
    m_audio_codec = avcodec_find_decoder(m_audio_codec_ctx->codec_id);
    if (m_audio_codec == nullptr) {
        qDebug() << "can not find audio codec.\n";
        return;
    }

    // 打开音频解码器
    if (avcodec_open2(m_audio_codec_ctx, m_audio_codec, nullptr) < 0) {
        qDebug() << "open audio codec failed!\n";
        return;
    }

}

PlayThread::~PlayThread()
{

}

bool PlayThread::initialize_audio()
{
    if (m_audio_codec_ctx == nullptr)
        return false;

    // 音频
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO; // 声道格式
    AVSampleFormat out_sample_format = AV_SAMPLE_FMT_S16; // 采样格式
    int out_nb_sample = m_audio_codec_ctx->frame_size; // 格式大小 /*有的是视频格式数据头为非标准格式，从frame_size中得不到正确的数据大小，只能解码一帧数据后才可以获得*/
    int out_sample_rate = 44100; // 采样率
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout); // 根据声道格式返回声道个数
    m_audio_out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_sample, out_sample_format, 1); // 获取输出缓冲区大小

    // 分配音频输出缓存内存
    m_audio_out_buffer = (uint8_t*)av_malloc(192000*2);
    memset(m_audio_out_buffer, 0, 192000*2);

    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = out_sample_rate; // 采样率
    wanted_spec.format = AUDIO_S16SYS; // 告诉 SDL 我们将要给的格式
    wanted_spec.channels = out_channels;
    wanted_spec.silence = 0; // 音量
    wanted_spec.samples = out_nb_sample; // 格式大小
    wanted_spec.callback = fill_audio; // 回调函数
    wanted_spec.userdata = m_audio_codec;

    if (SDL_OpenAudio(&wanted_spec, NULL) < 0) {
        qDebug() << "open audio failed!\n";
        return false;
    }

    // 根据声道数返回默认输入声道格式
    int64_t in_channel_layout = av_get_default_channel_layout(m_audio_codec_ctx->channels);

    // 音频格式转换准备
    m_audio_convert_ctx = swr_alloc();
    m_audio_convert_ctx= swr_alloc_set_opts(m_audio_convert_ctx, out_channel_layout, out_sample_format, out_sample_rate,
                                                           in_channel_layout, m_audio_codec_ctx->sample_fmt, m_audio_codec_ctx->sample_rate, 0, NULL);
    swr_init(m_audio_convert_ctx); // 初始化

    return true;
}

void PlayThread::slot_video_status_change(videoStatu status)
{
    m_mutex.lock();
    qDebug() << "PlayThread video status changed: " << status << endl;
    m_video_status = status;
    m_mutex.unlock();
}

void PlayThread::run()
{
    // 一帧压缩数据包 (H264数据包)
    AVPacket *packet = (AVPacket*)malloc(sizeof(AVPacket));

    // 释放旧的视频像素数据
//    if ( nullptr == m_frame )
//        av_frame_free( &m_frame );

    // 像素数据包 (YUV420P数据包)
    AVFrame *m_video_frame = av_frame_alloc();
    AVFrame *m_audio_frame = av_frame_alloc();
    int got_picture_ptr = 0;

    qDebug() << "PlayThread run starting" << endl;

    if (!initialize_audio()) {
        qDebug() << "initialize audio failed!\n";
        return;
    }

    // 开始播放音频
    SDL_PauseAudio(0);

    AVRational time_base = m_format_ctx->streams[m_video_index]->time_base;
    g_play_time = 0.00;

    // 读取一帧数据
    while ( true )
    {
        m_mutex.lock();
        if (m_video_status == VIDEO_PLAY )
        {
            if (av_read_frame(m_format_ctx, packet) < 0 ) {
                    m_mutex.unlock();
                    break;
                }
        }
        else if (m_video_status == VIDEO_STOP) {
            m_mutex.unlock();
            break;
        }
        else if (m_video_status == VIDEO_PAUSE) {
            m_mutex.unlock();
            continue;
        }
        m_mutex.unlock();

        if (nullptr == packet)	return;

        if (packet->stream_index == m_video_index)
        {
            // 解码一帧视频数据
            if (0 > avcodec_decode_video2(m_video_codec_ctx, m_video_frame, &got_picture_ptr, packet) )
            {
                    qDebug() << "decode frame failed!" << endl;
                    av_log(nullptr, AV_LOG_ERROR, "decode frame failed!\n");
                    break;
            }

            qDebug() << "now time: " << g_play_time << endl;
            g_play_time = m_video_frame->pts*av_q2d(time_base);

            if (got_picture_ptr)
            {
                g_render.start_render( m_video_frame->data[0], m_video_frame->linesize[0],
                                                     m_video_frame->data[1], m_video_frame->linesize[1],
                                                     m_video_frame->data[2], m_video_frame->linesize[2] );
            }
        }
        else if (packet->stream_index == m_audio_index)
        {
            // 音频数据
            if (0 > avcodec_decode_audio4(m_audio_codec_ctx, m_audio_frame, &got_picture_ptr, packet))
            {
                qDebug() << "decode audio failed!\n";
                break;
            }

            if (got_picture_ptr)
            {
                swr_convert(m_audio_convert_ctx, &m_audio_out_buffer, 192000, (const uint8_t**)m_audio_frame->data, m_audio_frame->nb_samples);
            }

            // 延时 1ms
            while(audio_len>0)
                SDL_Delay(1);

            // 指向音频数据
            audio_chunk = (Uint8 *)m_audio_out_buffer;

            // 音频长度
            audio_len = m_audio_out_buffer_size;

            // 当前播放位置
            audio_pos = audio_chunk;
        }
        else
        {
             // 其他数据流
            continue;
        }

        got_picture_ptr = 0;
    }

    // 停止播放音频
    SDL_PauseAudio(1);
    SDL_CloseAudio();

    qDebug() << "PlayThread run ending" << endl;

    // 释放压缩数据缓冲区
    av_free_packet(packet);
    qDebug() << "free AVPacket memery\n";

    // 释放重采样数据空间
    swr_free(&m_audio_convert_ctx);
    qDebug() << "free SwrContext memery\n";

    // 释放音频输出缓冲区
    av_free(m_audio_out_buffer);

    // 释放视频解码数据缓冲区
    av_frame_free(&m_video_frame);

    // 释放音频解码数据缓冲区
    av_frame_free(&m_audio_frame);

    // 关闭视频解码器
    avcodec_close(m_video_codec_ctx);

    // 关闭音频解码器
    avcodec_close(m_audio_codec_ctx);
}
