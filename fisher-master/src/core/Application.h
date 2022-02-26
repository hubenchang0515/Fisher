#ifndef FISHER_APPLICATION_H
#define FISHER_APPLICATION_H

#include <SDL2/SDL.h>
#include <functional>
#include <mutex>

#include <Window.h>
#include <Renderer.h>
#include <Job.h>
#include <base.h>

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

    static std::thread::id mainThreadId() noexcept;
    static bool isRunning() noexcept;
    static void quit() noexcept;
    static int argc() noexcept;
    static const char* arg(int n) noexcept;
    static Window* window() noexcept;
    static Renderer* renderer() noexcept;

    static Job* job() noexcept;
    static void setJob(Job* job, void* userdata=nullptr);

private:
    int m_argc;
    char** m_argv;

    std::thread::id m_mainThreadId;

    bool m_running;
    std::mutex m_runningMutex;

    Window* m_window;
    Renderer* m_renderer;

    Job* m_job;
    std::mutex m_jobMutex;

    static Application* instance;

}; // class Application

}; // namespace Fisher

#endif // FISHER_APPLICATION_H