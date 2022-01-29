#ifndef FISHER_APPLICATION_H
#define FISHER_APPLICATION_H

#include <SDL2/SDL.h>
#include <functional>

#include <Window.h>
#include <Renderer.h>
#include <Robot.h>

namespace Fisher
{

class Application
{
    friend Window;
    friend Renderer;

public:
    Application(int argc=0, char** argv=nullptr);
    ~Application() noexcept;

    void bind(Window* window, Renderer* renderer) noexcept;
    int exec() noexcept;

    static bool isRunning() noexcept;
    static void quit() noexcept;
    static int argc() noexcept;
    static const char* arg(int n) noexcept;
    static Window* window() noexcept;
    static Renderer* renderer() noexcept;
    static void setRobot(Robot* worker, void* userdata=nullptr);

private:
    int m_argc;
    char** m_argv;
    bool m_running;
    SDL_mutex* m_runningMutex;
    Window* m_window;
    Renderer* m_renderer;
    Robot* m_worker;

    static Application* instance;
    static int drawThread(void* userdata) noexcept;
    static int eventThread(void* userdata) noexcept;
    static int otherThread(void* userdata) noexcept;

}; // class Application

}; // namespace Fisher

#endif // FISHER_APPLICATION_H