#include <stdexcept>
#include <Window.h>
#include <Application.h>
#include <thread>

#define CHECK_MAIN_THREAD() do { if(std::this_thread::get_id() != Application::mainThreadId()) WAR("control window out main thread"); }while(0)

namespace Fisher
{

Window::Window(const string& title, int width, int height) noexcept:
    m_title(title),
    m_width(width),
    m_height(height)
{
    m_window = SDL_CreateWindow(
        title.c_str(), 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        width, 
        height, 
        SDL_WINDOW_SHOWN
    );

    if (m_window == nullptr)
    {
        ERR("%s", SDL_GetError());
    }
}

Window::~Window() noexcept
{
    if (m_window != nullptr)
        SDL_DestroyWindow(m_window);
}

string Window::title() const noexcept
{
    return m_title;
}

void Window::setTitle(const string& title) noexcept
{
    CHECK_MAIN_THREAD();
    SDL_SetWindowTitle(m_window, title.c_str());
}

int Window::width() const noexcept
{
    return m_width;
}

int Window::height() const noexcept
{
    return m_height;
}

void Window::setWidth(int width) noexcept
{
    CHECK_MAIN_THREAD();
    SDL_SetWindowSize(m_window, width, m_height);
    m_width = width;
}

void Window::setHeight(int height) noexcept
{
    CHECK_MAIN_THREAD();
    SDL_SetWindowSize(m_window, m_width, height);
    m_height = height;
}

void Window::resize(int width, int height) noexcept
{
    CHECK_MAIN_THREAD();
    SDL_SetWindowSize(m_window, width, height);
    m_width = width;
    m_height = height;
}

}; // namespace Fisher
