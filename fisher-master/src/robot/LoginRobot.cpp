#include <LoginRobot.h>

namespace Fisher
{

LoginRobot::LoginRobot()
{
    auto ipList = getIpList();
    auto port = listen();
    auto content = info(ipList, port);
    qrcode(content);
}

LoginRobot::~LoginRobot()
{
    if (m_texture != nullptr)
        delete m_texture;

    if (m_listenSocket != -1)
        close(m_listenSocket);
}

// 设置登录后跳转到目标
void LoginRobot::setTarget(Robot* target)
{
    m_target = target;
}

// 获取本机的全部IP地址
std::vector<std::string> LoginRobot::getIpList() noexcept
{
    std::vector<std::string> ipList;

    char hostName[_SC_HOST_NAME_MAX];
    gethostname(hostName, _SC_HOST_NAME_MAX);

    struct hostent* hostEntry = gethostbyname(hostName);
    for(int i = 0; hostEntry->h_addr_list[i]; i++) 
    {
        std::string ip = inet_ntoa(*(struct in_addr*)(hostEntry->h_addr_list[i]));
        ipList.emplace_back(ip);
        SDL_Log("%s\n", ip.c_str());
    }

    return ipList;
}

// 监听并返回端口号
int LoginRobot::listen() noexcept
{
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", strerror(errno));
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);

    if (bind(m_listenSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", strerror(errno));
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(m_listenSocket, (struct sockaddr*)&addr, &len) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", strerror(errno));
        return -1;
    }

    int port = ntohs(addr.sin_port);
    SDL_Log("port:%d", ntohs(addr.sin_port));

    ::listen(m_listenSocket, 1);

    return port;
}

// 生成连接信息，用于生成二维码
std::string LoginRobot::info(std::vector<std::string> ipList, int port) noexcept
{
    std::string content;
    for (auto& ip : ipList)
    {
        content += "IP: " + ip + "\n";
        content += "PORT: " + std::to_string(port) + "\n";
    }

    return content;
}

// 生成二维码
Texture* LoginRobot::qrcode(const std::string& content) noexcept
{
    Renderer* renderer = Application::renderer();
    QRCode qrcode = QRCode::create(content);
    m_texture = new Texture(renderer, qrcode.width(), qrcode.width());
    renderer->setTarget(m_texture);
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
    return m_texture;
}

void LoginRobot::draw()
{
    Renderer* renderer = Application::renderer();
    renderer->setTarget();
    renderer->clear();
    renderer->copy(m_texture);
    renderer->present();
}

void LoginRobot::event(SDL_Event& ev)
{
    if (ev.type == SDL_QUIT)
    {
        shutdown(m_listenSocket, SHUT_RDWR);
        SDL_Log("close");
    }
}

void LoginRobot::other()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    m_coopSocket = accept(m_listenSocket, (struct sockaddr*)&addr, &len);
    if (m_coopSocket < 0)
    {
        return;
    }
    SDL_Log("Get connection from %s:%d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    Application::setRobot(m_target, &m_coopSocket);
}

}; // namespace Fisher