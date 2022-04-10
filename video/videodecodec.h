#ifndef VIDEODECODEC_H
#define VIDEODECODEC_H

/**
 * @brief The VideoDecodec class
 *  视频解码类. 负责对视频文件解封装, 视频像素数据解码
 */

#include <QString>
#include <QThread>
#include <QMutex>

//class AVFrame;
//class AVFormatContext;
//class AVCodecContext;
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavdevice/avdevice.h>
    #include <libavutil/time.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/log.h>
    #include <libswresample/swresample.h>
}

typedef enum VIDEO_STATUS_T
{
    VIDEO_UNPLAY,	// 未播放
    VIDEO_STOP,		// 停止
    VIDEO_PLAY,		// 播放
    VIDEO_PAUSE		// 暂停
}videoStatu;

class PlayThread;
class VideoDecodec : public QObject
{
    Q_OBJECT

public:
    VideoDecodec();
    ~VideoDecodec();

    /**
     * @brief set_input_file  设置播放的视频文件
     * @param file 视频文件名
     * @return 成功/失败
     */
    bool set_input_file(QString file);

    /**
     * @brief play 从视频文件中读取视频像素数据解码成 YUV420P 数据并显示
     */
    void play();

    /**
     * @brief stop 停止播放
     */
    void stop();

    /**
    * @brief video_duration 获取视频时长
    * @return 视频时长
    */
   long long video_duration();

   /**
    * @brief volume_up 音量增加
    * @return 成功/失败
    */
   bool volume_up();

   /**
    * @brief volume_down 音量减小
    * @return 成功/失败
    */
   bool volume_down();

   /**
    * @brief video_width 视频宽度
    * @return 宽度值
    */
   int video_width();

   /**
    * @brief video_height 视频高度
    * @return 高度值
    */
   int video_height();

   /**
    * @brief initialize 初始化
    * @return 成功/失败
    */
   bool initialize();

    /**
     * @brief close_input 关闭输入视频文件
     */
    void close_input();

    /**
     * @brief forward 快进
     */
    void forward();

    /**
     * @brief backward 后退
     */
    void backward();

private:

    /**
     * @brief unpack 视频文件解封装
     * @return 成功/失败
     */
    bool unpack();

    /**
     * @brief get_stream_index 获取音视频流索引号
     * @return 成功/失败
     */
    bool get_stream_index();

    /**
     * @brief create_video_task 创建播放任务
     * @return 成功/失败
     */
    bool create_video_task();

Q_SIGNALS:

    /**
     * @brief video_status_change_signal 停止播放信号
     */
    void video_status_change_signal(videoStatu);

private Q_SLOTS:

    void slot_close_task();

private:

    /**
     * @brief m_input_file 输入视频文件全路径名称
     */
    QString m_input_file;

    /**
     * @brief m_formatCtx 视频格式上下文信息
     */
    AVFormatContext *m_formatCtx;

    /**
     * @brief m_codec 解码器上下文
     */
    AVCodecContext *m_codec_ctx;

    /**
     * @brief m_frame 一帧视频像素数据
     */
    AVFrame *m_frame;

    /**
     * @brief video_index 视频流索引号
     */
    int m_video_index;

    /**
     * @brief m_audio_index 音频流索引号
     */
    int m_audio_index;

    /**
     * @brief m_video_status 视频状态: 1: 播放 0: 暂停
     */
    videoStatu m_video_status;

    /**
    * @brief m_task_thread 负责播放视频的线程
    */
   PlayThread *m_task_thread;

   /**
    * @brief m_has_task 是否已存在播放任务
    */
   bool m_has_task;
};

/**
 * @brief The PlayThread class  播放视频线程类
 */
class PlayThread : public QThread
{
    Q_OBJECT

public:
    PlayThread(AVFormatContext *formatCtx, int videoStreamIndex, int audioStreamIndex,  QObject *parent = nullptr);
    ~PlayThread();

private:

    /**
     * @brief initialize_audio 初始化音频
     * @return 成功/失败
     */
    bool initialize_audio();

private Q_SLOTS:

    /**
     * @brief slot_video_status_change 状态变化-槽函数
     */
    void slot_video_status_change(videoStatu);

protected:

    virtual void run() override;

private:

    /**
     * @brief m_format_ctx 音视频格式上下文信息
     */
    AVFormatContext *m_format_ctx;

    /**
     * @brief m_codec_ctx 视频解码器上下文信息
     */
    AVCodecContext *m_video_codec_ctx;

    /**
     * @brief m_video_codec 视频解码器
     */
    AVCodec *m_video_codec;

    /**
     * @brief m_audio_codec_ctx 音频解码器上下文信息
     */
    AVCodecContext *m_audio_codec_ctx;

    /**
     * @brief m_audio_codec 音频解码器
     */
    AVCodec *m_audio_codec;

    /**
     * @brief m_video_index 视频流索引号
     */
    int m_video_index;

    /**
     * @brief m_audio_index 音频流索引号
     */
    int m_audio_index;

    /**
     * @brief m_audio_out_buffer 音频输出缓冲区
     */
    uint8_t *m_audio_out_buffer;

    /**
     * @brief m_audio_out_buffer_size 音频输出缓冲区大小
     */
    int m_audio_out_buffer_size;

    /**
     * @brief m_audio_convert_ctx 音频重采样结构
     */
    struct SwrContext *m_audio_convert_ctx;

    /**
     * @brief m_video_status 停止播放标志
     */
    videoStatu m_video_status;

    /**
     * @brief m_mutex 线程状态锁
     */
    QMutex m_mutex;
};

#endif // VIDEODECODEC_H
