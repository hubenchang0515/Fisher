#include <CoopRobot.h>

namespace Fisher
{

CoopRobot::CoopRobot()
{
    auto renderer = Application::renderer();
    m_texture = new Texture(renderer, m_width, m_height);
    m_data = malloc(m_width * m_height * 3);
}

CoopRobot::~CoopRobot()
{
    if (m_data != nullptr)
        free(m_data);

    if (m_texture != nullptr)
        delete m_texture;

    if (m_mutex != nullptr)
        SDL_DestroyMutex(m_mutex);

    if (m_coopSocket != -1)
        close(m_coopSocket);
}

void CoopRobot::setSocket(int fd)
{
    m_coopSocket = fd;
}


void CoopRobot::init(void* userdata)
{
    m_coopSocket = *static_cast<int*>(userdata);
    Application::window()->setWidth(m_width);
    Application::window()->setHeight(m_height);
}

void CoopRobot::draw()
{
    Application::renderer()->setTarget();
    Application::renderer()->clear();
    if (m_data != nullptr)
    {
        m_texture->update(m_data, m_width * 3);
        Application::renderer()->copy(m_texture);
    }
    Application::renderer()->present();
}

void CoopRobot::event(SDL_Event& ev)
{
    #define CMD_SIZE 128
    char cmd[CMD_SIZE];
    if (ev.type == SDL_QUIT)
        shutdown(m_coopSocket, SHUT_RDWR);

    if (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button == SDL_BUTTON_LEFT)
    {
        memset(cmd, 0, CMD_SIZE);
        sprintf(cmd, "type:pressed x:%d y:%d", ev.button.x, ev.button.y);
        write(m_coopSocket, cmd, CMD_SIZE);
    }

    if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT)
    {
        memset(cmd, 0, CMD_SIZE);
        sprintf(cmd, "type:released x:%d y:%d", ev.button.x, ev.button.y);
        write(m_coopSocket, cmd, CMD_SIZE);
    }

    if (ev.type == SDL_MOUSEMOTION && ev.motion.state == SDL_PRESSED)
    {
        memset(cmd, 0, CMD_SIZE);
        sprintf(cmd, "type:motion x:%d y:%d", ev.motion.x, ev.motion.y);
        write(m_coopSocket, cmd, CMD_SIZE);
    }
}

void CoopRobot::other()
{
    uint8_t* data = (uint8_t*)malloc(m_width * m_height * 3);

    ssize_t get = 0;
    do
    {
        ssize_t len = read(m_coopSocket, data + get, m_width * m_height * 3 - get);
        if (len <= 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", strerror(errno));
            break;
        }
        get += len;
    }while(get < m_width * m_height * 3);

    SDL_LockMutex(m_mutex);
    memcpy(m_data, data, m_width * m_height * 3);
    SDL_UnlockMutex(m_mutex);

    free(data);
}

}; // namespace Fisher