#include <Application.h>
#include <SDL2/SDL.h>

namespace Fisher
{

// static
Application* Application::instance = nullptr;

// static
bool Application::isRunning() noexcept
{
    SDL_LockMutex(Application::instance->m_runningMutex);
    bool is = Application::instance->m_running;
    SDL_UnlockMutex(Application::instance->m_runningMutex);
    return is;
}

// static
void Application::quit() noexcept
{
    SDL_LockMutex(Application::instance->m_runningMutex);
    Application::instance->m_running = false;
    SDL_UnlockMutex(Application::instance->m_runningMutex);
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
void Application::setRobot(Robot* worker, void* userdata)
{
    if (Application::instance->m_window == nullptr || Application::instance->m_renderer == nullptr)
    {
        throw std::runtime_error("need bind window and renderer");
    }
    Application::instance->m_worker = worker;
    Application::instance->m_worker->init(userdata);
}

// static
int Application::drawThread(void* userdata) noexcept
{
    (void)(userdata);
    if (Application::instance->m_worker == nullptr)
        return EXIT_FAILURE;

    Uint32 ticks = SDL_GetTicks();
    while (isRunning())
    {
        ticks = SDL_GetTicks();
        Application::instance->m_worker->draw();
        int delay = 1000 / 60 + ticks - SDL_GetTicks();
        if (delay > 0)
            SDL_Delay(delay);
    }
    
    return EXIT_SUCCESS;
}

// static
int Application::eventThread(void* userdata) noexcept
{
    (void)(userdata);
    if (Application::instance->m_worker == nullptr)
        return EXIT_FAILURE;
    SDL_Event ev;
    while (isRunning())
    {
        if (SDL_WaitEventTimeout(&ev, 100) > 0)
        {
            Application::instance->m_worker->event(ev);
            if (ev.type == SDL_QUIT)
                Application::quit();
        }
    }
    return EXIT_SUCCESS;
}

// static 
int Application::otherThread(void* userdata) noexcept
{
    (void)(userdata);
    if (Application::instance->m_worker == nullptr)
        return EXIT_FAILURE;
    while (isRunning())
    {
        Application::instance->m_worker->other();
    }
    return EXIT_SUCCESS;
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
    m_runningMutex = SDL_CreateMutex();
    if (m_runningMutex == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
        return;
    }

    Application::instance = this;
}

Application::~Application() noexcept
{
    if (m_runningMutex != nullptr)
    {
        SDL_DestroyMutex(m_runningMutex);
    }

    SDL_Quit();
}

void Application::bind(Window* window, Renderer* renderer) noexcept
{
    m_window = window;
    m_renderer = renderer;
}

int Application::exec() noexcept
{
    SDL_Thread* drawID = SDL_CreateThread(Application::drawThread, "main", nullptr);
    if (drawID == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }

    SDL_Thread* eventID = SDL_CreateThread(Application::eventThread, "event", nullptr);
    if (eventID == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }

    SDL_Thread* otherID = SDL_CreateThread(Application::otherThread, "io", nullptr);
    if (otherID == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }
    
    SDL_WaitThread(drawID, nullptr);
    SDL_WaitThread(eventID, nullptr);
    SDL_WaitThread(otherID, nullptr);
    return 0;
}

}; // namespace Fisher