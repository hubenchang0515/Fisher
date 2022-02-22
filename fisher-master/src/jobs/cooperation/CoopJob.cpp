#include <CoopJob.h>
#include <thread>

namespace Fisher
{

CoopRobot::CoopRobot()
{
    
}

CoopRobot::~CoopRobot()
{
    if (m_data != nullptr)
        free(m_data);

    if (m_texture != nullptr)
        delete m_texture;
}

void CoopRobot::onCreate(void* userdata)
{
    m_coopSocket = std::move(*static_cast<Socket*>(userdata));
    std::thread netThread(&CoopRobot::net, this);
    netThread.detach();
}

void CoopRobot::onDraw()
{
    Application::renderer()->setTarget();
    Application::renderer()->clear();

    m_mutex.lock();
    if (m_data != nullptr)
    {
        m_texture->update(m_data, m_stride);
        Application::renderer()->copy(m_texture);
    }
    Application::renderer()->present();
    m_mutex.unlock();
}

void CoopRobot::onEvent(SDL_Event& ev)
{
    #define CMD_SIZE 128
    char cmd[CMD_SIZE];
    if (ev.type == SDL_QUIT)
        m_coopSocket.shutdown();

    bool hasCmd = false;

    if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT)
    {
        memset(cmd, 0, CMD_SIZE);
        sprintf(cmd, "type:pressed x:%d y:%d", ev.button.x, ev.button.y);
        hasCmd = true;
    }

    if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT)
    {
        memset(cmd, 0, CMD_SIZE);
        sprintf(cmd, "type:released x:%d y:%d", ev.button.x, ev.button.y);
        hasCmd = true;
    }

    if (ev.type == SDL_MOUSEMOTION && ev.motion.state == SDL_PRESSED)
    {
        memset(cmd, 0, CMD_SIZE);
        sprintf(cmd, "type:motion x:%d y:%d", ev.motion.x, ev.motion.y);
        hasCmd = true;
    }

    if (hasCmd)
    {
        uint32_t size = strlen(cmd) + 1;
        m_coopSocket.write(size);
        m_coopSocket.write(cmd, size);
    }
}

void CoopRobot::net()
{
    while (Application::isRunning())
    {
        uint32_t stride;
        uint32_t width;
        uint32_t height;
        m_coopSocket.read(&stride);
        m_coopSocket.read(&width);
        m_coopSocket.read(&height);

        if (stride == 0 || width == 0 || height == 0)
            continue;

        if (stride > INT16_MAX || width > INT16_MAX || height > INT16_MAX)
            continue;

        LOG("%u %u %u", stride, width, height);

        if (m_width != static_cast<int>(width) || m_height != static_cast<int>(height))
        {
            m_mutex.lock();
            if (m_texture != nullptr) 
            {
                delete m_texture;
            }
            m_width = static_cast<int>(width);
            m_height = static_cast<int>(height);
            m_texture = new Texture(Application::renderer(), m_width, m_height, SDL_PIXELFORMAT_RGBA32);
            Application::window()->resize(m_width, m_height);
            m_mutex.unlock();
        }

        if (m_stride != static_cast<int>(stride) || m_height != static_cast<int>(height))
        {
            m_mutex.lock();
            if (m_data != nullptr)
            {
                free(m_data);
            }

            m_stride = static_cast<int>(stride);
            m_height = static_cast<int>(height);
            m_data = malloc(m_stride * m_height);
            m_mutex.unlock();
        }

        void* data = malloc(m_stride * m_height);

        if (!m_coopSocket.read(data, m_stride * m_height))
        {
            Application::quit();
            break;
        }

        m_mutex.lock();
        memcpy(m_data, data, m_stride * m_height);
        m_mutex.unlock();

        free(data);
    }
}

}; // namespace Fisher