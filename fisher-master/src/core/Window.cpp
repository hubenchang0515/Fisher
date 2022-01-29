#include <stdexcept>
#include <Window.h>

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
        SDL_LogError(SDL_LOG_CATEGORY_VIDEO, "%s", SDL_GetError());
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
    SDL_SetWindowSize(m_window, width, m_height);
    m_width = width;
}

void Window::setHeight(int height) noexcept
{
    SDL_SetWindowSize(m_window, m_width, height);
    m_height = height;
}

}; // namespace Fisher
