#include <CoopJob.h>
#include <thread>
#include <jpeglib.h>

namespace Fisher
{

CoopRobot::CoopRobot()
{
    
}

CoopRobot::~CoopRobot()
{
    if (m_data != nullptr)
        free(m_data);
}

void CoopRobot::onCreate(void* userdata)
{
    m_coopSocket = std::move(*static_cast<Socket*>(userdata));
    std::thread netThread(&CoopRobot::net, this);
    netThread.detach();
}

void CoopRobot::onDraw()
{
    if (m_width == 0 || m_height == 0)
        return;

    if (m_width != Application::window()->width() || m_height != Application::window()->height())
    {
        Application::window()->resize(m_width, m_height);
        m_texture.realloc(Application::renderer(), m_width, m_height);
    }

    m_mutex.lock();
    if (m_data != nullptr)
    {
        Application::renderer()->setTarget(&m_texture);
        m_texture.update(m_data, m_width * 3); // RGB
        Application::renderer()->setTarget();
        Application::renderer()->copy(&m_texture);
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
        int32_t width = 0;
        int32_t height = 0;
        int32_t size = 0;
        if (!m_coopSocket.read(&width) || !m_coopSocket.read(&height) || !m_coopSocket.read(&size))
        {
            Application::quit();
            break;
        }

        if (width <= 0 || height <= 0 || size < 0)
            continue;

        if (width > INT16_MAX || height > INT16_MAX)
            continue;

        if (m_width != static_cast<int>(width) || m_height != static_cast<int>(height))
        {
            m_mutex.lock();
            m_width = static_cast<int>(width);
            m_height = static_cast<int>(height);
            m_mutex.unlock();
        }

        uint8_t* jpeg = reinterpret_cast<uint8_t*>(malloc(size));

        if (!m_coopSocket.read(jpeg, size))
        {
            Application::quit();
            break;
        }

        uint8_t* data = nullptr;
        size_t outSize;
        uncompress(jpeg, size, reinterpret_cast<void**>(&data), &outSize);

        if (m_size != outSize)
        {
            m_mutex.lock();
            if (m_data != nullptr)
                free(m_data);

            m_data = malloc(outSize);
            m_mutex.unlock();
        }

        m_mutex.lock();
        memcpy(m_data, data, outSize);
        m_mutex.unlock();

        free(jpeg);
        free(data);
    }
}

bool CoopRobot::uncompress(const void* jpeg, size_t inSize, void** rgb, size_t* outSize)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err=jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    jpeg_mem_src(&cinfo, (const unsigned char*)(jpeg), inSize);
    
    jpeg_read_header(&cinfo, false);
    jpeg_calc_output_dimensions(&cinfo);

    size_t rowSize = cinfo.output_components * cinfo.output_width;
    *outSize = rowSize * cinfo.output_height;
    *rgb = malloc(*outSize);

    JSAMPARRAY buffer = cinfo.mem->alloc_sarray((j_common_ptr)(&cinfo), JPOOL_IMAGE, rowSize, 1);

    jpeg_start_decompress(&cinfo);
    for(uint8_t* ptr = reinterpret_cast<uint8_t*>(*rgb); cinfo.output_scanline < cinfo.output_height; ptr += rowSize)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(ptr, buffer[0], rowSize);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return true;
}

}; // namespace Fisher
