#ifndef FISHER_WINDOW_H
#define FISHER_WINDOW_H

#include <string>
#include <SDL2/SDL.h>
#include <base.h>

namespace Fisher
{

using std::string;

class Renderer;

class Window
{
    friend Renderer;
public:
    Window(const string& title, int width, int height) noexcept;
    ~Window() noexcept;

    string title() const noexcept;
    void setTitle(const string& title) noexcept;

    int width() const noexcept;
    int height() const noexcept;

    void setWidth(int width) noexcept;
    void setHeight(int height) noexcept;
    void resize(int width, int height) noexcept;

private:
    string m_title;
    int m_width;
    int m_height;
    SDL_Window* m_window;
}; // class Window

}; // namespace Fisher


#endif // FISHER_WINDOW_H