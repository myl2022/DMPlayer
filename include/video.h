#ifndef VIDEO_H
#define VIDEO_H

#include "video_global.h"
#include <QtWidgets/QWidget>
#include <QString>
#include <QObject>

class VIDEOSHARED_EXPORT Video : public QObject
{
    Q_OBJECT
public:
    Video();

    /**
     * @brief set_video_window 设置视频播放控件
     * @param window 视频播放控件的指针
     */
    void set_video_window(void *window);

    /**
     * @brief set_video_file 设置播放视频文件
     * @param filename 视频文件名称, 带全路径的名称
     * @return 视频文件是否有效.  1: 有效   0: 无效
     */
     bool set_video_file(const QString filename);

     /**
      * @brief initialize 初始化视频文件播放环境
      * @return 成功/失败
      */
     bool initialize();

     /**
      * @brief video_play 播放/暂停视频操作
      * @param play 1: 播放 0: 暂停
      * @return 播放/暂停操作成功或失败.   1: 成功 0: 失败
      */
     bool video_play(bool play);

     /**
      * @brief video_stop 停止播放操作
      * @return 停止播放操作成功/失败.  1: 成功  0: 失败
      */
     bool video_stop();

     /**
      * @brief video_forward 视频快进操作
      * @return 视频快进操作成功/失败.   1: 成功  0: 失败
      */
     bool video_forward();

     /**
      * @brief video_reback 视频后退操作
      * @return 视频后退操作成功/失败.   1: 成功  0: 失败
      */
     bool video_reback();

     /**
      * @brief set_volume 设置视频播放音量操作
      * @param volume 音量数值
      * @return 设置视频播放音量操作成功/失败.  1: 成功   0: 失败
      */
     bool set_volume(int volume);

     /**
      * @brief volume_up 音量增加
      * @return 成功/失败
      */
     bool volume_up();

     /**
      * @brief volume_down 音量减小
      * @return	成功/失败
      */
     bool volume_down();

     /**
      * @brief play_time 当前播放进度
      * @return 时间
      */
     double play_time();

     /**
      * @brief duration 视频时长
      * @return 时长
      */
     double duration();

     /**
      * @brief forward 快进
      */
     void forward();

     /**
      * @brief backward 后退
      */
     void backward();

signals:

     /**
      * @brief      播放完成
      */
     void finished();
};

#endif // VIDEO_H
