#include <Application.h>
#include <SDL2/SDL.h>
#include <stdexcept>

namespace Fisher
{

// static
Application* Application::instance = nullptr;

// static
bool Application::isRunning() noexcept
{
    Application::instance->m_runningMutex.lock();
    bool is = Application::instance->m_running;
    Application::instance->m_runningMutex.unlock();
    return is;
}

// static
void Application::quit() noexcept
{
    Application::instance->m_runningMutex.lock();
    Application::instance->m_running = false;
    Application::instance->m_runningMutex.unlock();
}

// static
int Application::argc() noexcept
{
    return Application::instance->m_argc;
}

// static
const char* Application::arg(int n) noexcept
{
    return Application::instance->m_argv[n];
}

// static 
Window* Application::window() noexcept
{
    return Application::instance->m_window;
}

// static 
Renderer* Application::renderer() noexcept
{
    return Application::instance->m_renderer;
}

// static 
void Application::setJob(Job* job, void* userdata)
{
    if (Application::instance->m_window == nullptr || Application::instance->m_renderer == nullptr)
    {
        throw std::runtime_error("need bind window and renderer");
    }

    if (Application::instance->m_job != nullptr)
    {
        Application::instance->m_job->onHide();
        Application::instance->m_job->m_innerHide();
    }

    Application::instance->m_job = job;
    if (Application::instance->m_job == nullptr)
    {
        return;
    }

    if (Application::instance->m_job->m_state == Job::State::nil)
    {
        Application::instance->m_job->onCreate(userdata);
    }
    Application::instance->m_job->onShow();
    Application::instance->m_job->m_innerShow();
}

Application::Application(int argc, char** argv)
{
    if (Application::instance != nullptr)
    {
        throw std::runtime_error("application should be singleton");
        return;
    }

    SDL_Init(SDL_INIT_EVERYTHING);

    m_argc = argc;
    m_argv = argv;
    m_running = true;

    Application::instance = this;
}

Application::~Application() noexcept
{
    SDL_Quit();
}

void Application::bind(Window* window, Renderer* renderer) noexcept
{
    m_window = window;
    m_renderer = renderer;
}

int Application::exec() noexcept
{
    if (Application::instance->m_job == nullptr)
        return EXIT_FAILURE;

    SDL_Event ev;
    Uint32 ticks = SDL_GetTicks();
    while (isRunning())
    {
        ticks = SDL_GetTicks();
        while (SDL_PollEvent(&ev) > 0)
        {
            Application::instance->m_job->onEvent(ev);
        }
        Application::instance->m_job->onDraw();
        int delay = 1000 / 60 + ticks - SDL_GetTicks();
        if (delay > 0)
            SDL_Delay(delay);
    }
    
    return EXIT_SUCCESS;
}

}; // namespace Fisher