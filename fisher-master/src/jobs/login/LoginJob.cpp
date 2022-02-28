#include <LoginJob.h>
#include <thread>

namespace Fisher
{

LoginRobot::LoginRobot()
{
    auto ipList = getIpList();
    auto port = listen();
    auto content = info(ipList, port);
    genQrCode(content);
}

LoginRobot::~LoginRobot()
{

}

// 设置登录后跳转到目标
void LoginRobot::setTarget(Job* target)
{
    m_target = target;
}

// 获取本机的全部IP地址
std::vector<std::string> LoginRobot::getIpList() noexcept
{
    std::vector<std::string> ipList;

    char hostName[256];
    gethostname(hostName, 256);

    struct hostent* hostEntry = gethostbyname(hostName);
    for(int i = 0; hostEntry->h_addr_list[i]; i++) 
    {
        std::string ip = inet_ntoa(*(struct in_addr*)(hostEntry->h_addr_list[i]));
        ipList.emplace_back(ip);
        LOG("IP: %s\n", ip.c_str());
    }

    return ipList;
}

// 监听并返回端口号
int LoginRobot::listen() noexcept
{
    m_listenSocket = Socket();
    m_listenSocket.bind();
    m_listenSocket.listen();
    int port = m_listenSocket.port();
    LOG("PORT: %d", port);
    return port;
}

// 生成连接信息，用于生成二维码
std::string LoginRobot::info(std::vector<std::string> ipList, int port) noexcept
{
    std::string content;
    for (auto& ip : ipList)
    {
        content += "IP: " + ip + "\n";
    }
    content += "PORT: " + std::to_string(port) + "\n";
    return content;
}

// 生成二维码
void LoginRobot::genQrCode(const std::string& content) noexcept
{
    Renderer* renderer = Application::renderer();
    QRCode qrcode = QRCode::create(content);
    m_texture.realloc(renderer, qrcode.width(), qrcode.width());
    renderer->setTarget(&m_texture);
    for (int y = 0; y < qrcode.width(); y++)
    {
        for (int x = 0; x < qrcode.width(); x++)
        {
            int index = qrcode.width() * y + x;
            if (qrcode.data()[index] & 1)
                renderer->setColor(0xff, 0xff, 0xff);
            else
                renderer->setColor(0, 0, 0);
            renderer->drawPoint(x, y);
        }
    }
    renderer->setTarget();
}


void LoginRobot::onCreate(void* userdata)
{
    (void)(userdata);
    std::thread netThread(&LoginRobot::net, this);
    netThread.detach();
}

void LoginRobot::onDraw()
{
    Renderer* renderer = Application::renderer();
    renderer->setTarget();
    renderer->clear();
    renderer->copy(&m_texture);
    renderer->present();
}

void LoginRobot::onEvent(SDL_Event& ev)
{
    if (ev.type == SDL_QUIT)
    {
        m_listenSocket.shutdown();
        LOG("close");
    }
}

void LoginRobot::net()
{
    m_coopSocket = m_listenSocket.accept();
    if (m_coopSocket.valid())
    {
        LOG("Get connection from %s:%d", m_coopSocket.peerIp().c_str(), m_coopSocket.peerPort());
        Application::setJob(m_target, &m_coopSocket);
    }
}

}; // namespace Fisher