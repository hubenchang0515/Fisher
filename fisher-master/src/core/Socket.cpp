#include <Socket.h>
#include <SDL2/SDL.h>

namespace Fisher
{


Socket::Socket() noexcept:
    m_fd(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
{
    if (m_fd < 0)
    {
        ERR("%s", strerror(errno));
    }
}

Socket::Socket(int domain, int type, int protocol) noexcept:
    m_fd(socket(domain, type, protocol))
{
    if (m_fd < 0)
    {
        ERR("%s", strerror(errno));
    } 
}

Socket::Socket(Socket&& src) noexcept:
    m_fd(src.m_fd)
{
    src.m_fd = -1;
}

Socket::Socket(int fd) noexcept:
    m_fd(fd)
{

}

Socket& Socket::operator = (Socket&& src)
{
    m_fd = src.m_fd;
    src.m_fd = -1;
    return *this;
}

Socket::~Socket() noexcept
{
    if (m_fd >= 0)
        ::close(m_fd);
}

bool Socket::valid() const noexcept
{
    return m_fd >= 0;
}

bool Socket::close() noexcept
{
    if (m_fd < 0)
        return false;
    
    if (::close(m_fd) < 0)
        return false;

    m_fd = -1;
    return true;
}

string Socket::ip() const noexcept
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(m_fd, reinterpret_cast<struct sockaddr*>(&addr), &len) < 0)
    {
        ERR("%s", strerror(errno));
        return "";
    }

    return inet_ntoa(addr.sin_addr);
}

int Socket::port() const noexcept
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getsockname(m_fd, reinterpret_cast<struct sockaddr*>(&addr), &len) < 0)
    {
        ERR("%s", strerror(errno));
        return -1;
    }

    return ntohs(addr.sin_port);
}

bool Socket::bind(const string& ip, int port) const noexcept
{
    struct sockaddr_in addr;
    memset(&addr, 0 , sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

    if (::bind(m_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::listen(int n) const noexcept
{
    if (::listen(m_fd, n) < 0)
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

Socket Socket::accept(string* ip, int* port) const noexcept
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    if (fd < 0)
    {
        ERR("%s", strerror(errno));
    }

    if (ip != nullptr)
    {
        *ip = inet_ntoa(addr.sin_addr);
    }

    if (port != nullptr)
    {
        *port = ntohs(addr.sin_port);
    }

    return Socket(fd);
}

bool Socket::shutdown() const noexcept
{
    if (::shutdown(m_fd, SHUT_RDWR) < 0)
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::write(int8_t data) const noexcept
{
    return write(static_cast<uint8_t>(data));
}

bool Socket::write(uint8_t data) const noexcept
{
    if (::send(m_fd, &data, sizeof(uint8_t), 0) != 1)
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::write(int16_t data) const noexcept
{
    return write(static_cast<uint16_t>(data));
}

bool Socket::write(uint16_t data) const noexcept
{
    uint16_t netData = htons(data);
    if (::send(m_fd, &netData, sizeof(uint16_t), 0) != sizeof(uint16_t))
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::write(int32_t data) const noexcept
{
    return write(static_cast<uint32_t>(data));
}

bool Socket::write(uint32_t data) const noexcept
{
    uint32_t netData = htonl(data);
    if (::send(m_fd, &netData, sizeof(uint32_t), 0) != sizeof(uint32_t))
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::write(void* data, size_t len) const noexcept
{
    ssize_t done = send(m_fd, data, len, 0);
    if (done < 0 || static_cast<size_t>(done) != len)
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::read(int8_t* data) const noexcept
{
    return read(reinterpret_cast<uint8_t*>(data));
}

bool Socket::read(uint8_t* data) const noexcept
{
    if (recv(m_fd, data, sizeof(uint8_t), MSG_WAITALL) != sizeof(uint8_t))
    {
        ERR("%s", strerror(errno));
        return false;
    }

    return true;
}

bool Socket::read(int16_t* data) const noexcept
{
    return read(reinterpret_cast<uint16_t*>(data));
}

bool Socket::read(uint16_t* data) const noexcept
{
    uint16_t netData;
    if (recv(m_fd, &netData, sizeof(uint16_t), MSG_WAITALL) != sizeof(uint16_t))
    {
        ERR("%s", strerror(errno));
        return false;
    }

    *data = ntohs(netData);
    return true;
}

bool Socket::read(int32_t* data) const noexcept
{
    return read(reinterpret_cast<uint32_t*>(data));
}

bool Socket::read(uint32_t* data) const noexcept
{
    uint32_t netData;
    if (recv(m_fd, &netData, sizeof(uint32_t), MSG_WAITALL) != sizeof(uint32_t))
    {
        ERR("%s", strerror(errno));
        return false;
    }

    *data = ntohl(netData);
    return true;
}

bool Socket::read(void* data, size_t len) const noexcept
{
    size_t done = 0;
    do
    {
        ssize_t got = recv(m_fd, reinterpret_cast<uint8_t*>(data) + done, len - done, 0);
        if (got < 0)
        {
            ERR("%s", strerror(errno));
            return false;
        }
        if (got == 0)
        {
            LOG("client disconnected");
            return false;
        }
        done += got;
    } while (done < len);

    return true;
}


}; // namespace Fisher