#include "video.h"
#include <QDebug>

#include "videodecodec.h"
#include "render.h"

Render g_render; // 渲染器
VideoDecodec g_video_decodec; // 解码器

extern double g_play_time;
extern double g_video_duration;

Video::Video()
{
//    g_render.initialize();
    qDebug() << "initial video success!";
    connect(&g_video_decodec, SIGNAL(finished()), this, SIGNAL(finished()));
}

void Video::set_video_window(void *window)
{
    g_render.set_window(window);
}

bool Video::set_video_file(const QString filename)
{
    Q_UNUSED(filename);
    g_video_decodec.set_input_file(filename);
    return true;
}

bool Video::initialize()
{
        // 步骤1: 视频文件解封装, 从视频文件中获取视频流信息
        // 步骤2: 获取视频流索引号, 获取解码器, 打开解码器, 读取视频的长和宽
        g_video_decodec.initialize();

        // 主要是重新创建纹理
        g_render.initialize();

        return true;
}

bool Video::video_play(bool play)
{
    Q_UNUSED(play);

    g_video_decodec.play();
    return true;
}

bool Video::video_stop()
{
    g_video_decodec.stop();
    return true;
}

bool Video::video_forward()
{
    return true;
}

bool Video::video_reback()
{
    return true;
}

bool Video::set_volume(int volume)
{
    Q_UNUSED(volume);

    return true;
}

bool Video::volume_up()
{
    return g_video_decodec.volume_up();
}

bool Video::volume_down()
{
    return g_video_decodec.volume_down();
}

double Video::play_time()
{
    return g_play_time;
}

double Video::duration()
{
    return g_video_duration;
}

void Video::forward()
{

}

void Video::backward()
{

}
