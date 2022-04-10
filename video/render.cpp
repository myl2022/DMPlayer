#include "render.h"
#include <QtWidgets/QWidget>
extern "C"
{
    #include <SDL/SDL.h>
}

extern int g_video_width;
extern int g_video_height;

Render::Render()
    :m_window(nullptr)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}

Render::~Render()
{
    stop_render();
}

bool Render::set_window(void *window)
{
    if ( nullptr == window && m_window != nullptr )
        return false;

    // 创建 SDL 窗口
//    m_window = SDL_CreateWindow("SDL Demo",
//            SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,//这两个是窗口的位置 x y（不在乎）
//            800,600,//窗口的宽度和高度
//            SDL_WINDOW_SHOWN //确保在创建窗口时显示该窗口
//        );
    m_window = SDL_CreateWindowFrom(window);
    if (nullptr == m_window)
    {
        SDL_DestroyWindow(m_window);
        SDL_Log("create SDL window failed!\n");
        return false;
    }

    // 创建渲染器
    if (nullptr == m_window)
        return false;

    m_renderer = SDL_CreateRenderer(m_window, -1, 0);

    return true;
}

bool Render::initialize()
{
    return create_texture();
}

void Render::initialize_audio()
{

}

bool Render::start_render(unsigned char *buffer1, int size1, unsigned char *buffer2, int size2, unsigned char *buffer3, int size3)
{
    bool ret = SDL_UpdateYUVTexture(m_texture, NULL, buffer1, size1, buffer2, size2, buffer3, size3 );
    if (ret != 0)
    {
        SDL_Log("update texture failed!\n");
        return false;
    }

//    SDL_Rect rect;
//    rect.x = 0;
//    rect.y = 0;
//    rect.w = g_video_width;
//    rect.h = g_video_height;

//    SDL_Rect dstRect;
//    dstRect.x = 0;
//    dstRect.y = 0;
//    dstRect.w = g_video_width;
//    dstRect.h = g_video_height;

    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_texture, /*&rect, &dstRect*/ NULL, NULL);
    SDL_RenderPresent(m_renderer);
//    SDL_Delay(40);

    return true;
}

bool Render::stop_render()
{
    qDebug() << "stop render" << endl;
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_Quit();

    return true;
}

bool Render::create_texture()
{
    if (nullptr == m_window || nullptr == m_renderer )
        return false;

    Uint32 pixformat = SDL_PIXELFORMAT_IYUV;
    m_texture = SDL_CreateTexture(m_renderer, pixformat, SDL_TEXTUREACCESS_STREAMING, g_video_width, g_video_height);
    if (m_texture == NULL)
    {
        qDebug() << "create texture failed!\n";
        return false;
    }

    return true;
}
