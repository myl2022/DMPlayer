#ifndef RENDER_H
#define RENDER_H
/**
 *
 * @class Render 渲染类
 *
 * @brief 视频数据渲染类. 负责将 ffmpeg 解码的 yuv 数据渲染并显示到 SDL 窗口
 *
 */

#include <QDebug>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class QWidget;

class Render
{
public:
    Render();
    ~Render();

    /**
     * @brief set_window 设置播放窗口
     * @param window 窗口指针
     * @return 成功/失败
     */
    bool set_window(void *window);

    /**
     * @brief initialize 初始化
     * @return 成功/失败
     */
    bool initialize();

    void initialize_audio();

    /**
     * @brief render 开始渲染
     * @return 成功/失败
     */
    bool start_render(unsigned char *buffer1, int size1, unsigned char *buffer2, int size2, unsigned char *buffer3, int size3);

    /**
     * @brief stop_render 停止渲染
     * @return 成功/失败
     */
    bool stop_render();

private:

    /**
     * @brief create_texture 创建纹理
     * @return 成功/失败
     */
    bool create_texture();


private:

    /**
     * @brief m_window 窗口指针
     */
    SDL_Window *m_window;

    /**
     * @brief m_renderer 渲染器
     */
    SDL_Renderer *m_renderer;

    /**
     * @brief m_texture 纹理
     */
    SDL_Texture *m_texture;

};

#endif // RENDER_H
