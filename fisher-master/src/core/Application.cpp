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
Job* Application::job() noexcept
{
    Application::instance->m_jobMutex.lock();
    Job* job = Application::instance->m_job;
    Application::instance->m_jobMutex.unlock();
    return job;
}

// static 
void Application::setJob(Job* job, void* userdata)
{
    if (Application::instance->m_window == nullptr || Application::instance->m_renderer == nullptr)
    {
        throw std::runtime_error("need bind window and renderer");
    }

    Job* oldJob = Application::job();
    if (oldJob != nullptr)
    {
        oldJob->onHide();
        oldJob->m_innerHide();
    }

    Application::instance->m_jobMutex.lock();
    Application::instance->m_job = job;
    Application::instance->m_jobMutex.unlock();

    if (job == nullptr)
    {
        return;
    }

    if (job->m_state == Job::State::nil)
    {
        job->onCreate(userdata);
    }
    job->onShow();
    job->m_innerShow();
}

Application::Application(int argc, char** argv):
    m_window(nullptr),
    m_renderer(nullptr),
    m_job(nullptr)
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
    if (Application::job() == nullptr)
        return EXIT_FAILURE;

    SDL_Event ev;
    Uint32 ticks = SDL_GetTicks();
    while (isRunning())
    {
        ticks = SDL_GetTicks();
        while (SDL_PollEvent(&ev) > 0)
        {
            Application::job()->onEvent(ev);
            if (ev.type == SDL_QUIT)
            {
                Application::quit();
            }
        }
        Application::job()->onDraw();
        int delay = 1000 / 60 + ticks - SDL_GetTicks();
        if (delay > 0)
            SDL_Delay(delay);
    }
    
    return EXIT_SUCCESS;
}

}; // namespace Fisher