#include <CoopRobot.h>

namespace Fisher
{

CoopRobot::CoopRobot()
{
    m_buffer = new NetBuffer();
}

CoopRobot::~CoopRobot()
{
    if (m_buffer != nullptr)
        delete m_buffer;

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
}

void CoopRobot::draw()
{
    Application::renderer()->setTarget();
    Application::renderer()->clear();

    if (m_data != nullptr)
    {
        m_texture->update(m_data, m_stride);
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
        m_buffer->clear();
        m_buffer->write(size);
        m_buffer->write(cmd);
        write(m_coopSocket, m_buffer->data(), m_buffer->size());

        m_buffer->seek();
        m_buffer->read(&size);
        m_buffer->read(cmd, size);
    }
}

void CoopRobot::other()
{
    // uint32_t stride
    // uint32_t width 
    // uint32_t height 
    // void* data
    constexpr size_t headerSize = sizeof(uint32_t) * 3;
    uint8_t header[headerSize]; 

    ssize_t len = recv(m_coopSocket, header, headerSize, MSG_WAITALL);
    if (len < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", strerror(errno));
        return;
    }

    m_buffer->clear();
    m_buffer->write(header, headerSize);

    uint32_t stride;
    uint32_t width;
    uint32_t height;
    m_buffer->seek();
    m_buffer->read(&stride);
    m_buffer->read(&width);
    m_buffer->read(&height);

    if (m_width != static_cast<int>(width) || m_height != static_cast<int>(height))
    {
        if (m_texture != nullptr) 
        {
            delete m_texture;
        }
        m_width = static_cast<int>(width);
        m_height = static_cast<int>(height);
        m_texture = new Texture(Application::renderer(), m_width, m_height, SDL_PIXELFORMAT_RGBA32);
        Application::window()->setWidth(m_width);
        Application::window()->setHeight(m_height);
    }

    if (m_stride != static_cast<int>(stride) || m_height != static_cast<int>(height))
    {
        if (m_data != nullptr)
        {
            free(m_data);
        }

        m_stride = static_cast<int>(stride);
        m_height = static_cast<int>(height);
        m_data = malloc(m_stride * m_height);
    }

    uint8_t* data = (uint8_t*)malloc(m_stride * m_height);

    ssize_t get = 0;
    do
    {
        ssize_t len = read(m_coopSocket, data + get, m_stride * m_height - get);
        if (len == 0)
        {
            SDL_Log("client disconnected");
            Application::quit();
            return;
        }
        else if (len < 0)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", strerror(errno));
            break;
        }
        get += len;
    }while(get < m_stride * m_height);

    SDL_LockMutex(m_mutex);
    memcpy(m_data, data, m_stride * m_height);
    SDL_UnlockMutex(m_mutex);

    free(data);
}

}; // namespace Fisher