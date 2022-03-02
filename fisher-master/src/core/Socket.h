#ifndef FISHER_SOCKET_H
#define FISHER_SOCKET_H

#include <base.h>

#ifdef OS_WIN
    #include <winsock2.h>
    
    #define SOCK_T SOCKET
    #define SOCK_INVALID(sock) (sock == 0)
    #define SOCKLEN_T int
    #define SOCK_CLOSE(sock) closesocket(sock)
    #define SOCK_ERROR() strerror(WSAGetLastError())
    #define SOCK_SHUT_FLAG SD_BOTH
    #define SOCK_RECV(sock, buf, bytes, flags) recv(sock, reinterpret_cast<char*>(buf), bytes, flags)
    #define SOCK_SEND(sock, buf, bytes, flags) send(sock, reinterpret_cast<const char*>(buf), bytes, flags)
#else
    #include <unistd.h>  
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>

    #define SOCK_T int
    #define SOCK_INVALID(sock) (sock < 0)
    #define SOCKLEN_T socklen_t
    #define SOCK_CLOSE(sock) ::close(sock)
    #define SOCK_ERROR() strerror(errno)
    #define SOCK_SHUT_FLAG SHUT_RDWR
    #define SOCK_RECV(sock, buf, bytes, flags) recv(sock, reinterpret_cast<void*>(buf), bytes, flags)
    #define SOCK_SEND(sock, buf, bytes, flags) send(sock, reinterpret_cast<const void*>(buf), bytes, flags)
#endif

#include <string>
#include <base.h>

namespace Fisher
{

using std::string;

class Socket
{
public:
    static void init();

    Socket() noexcept;
    Socket(int domain, int type, int protocol) noexcept;
    Socket(const Socket& src) = delete;
    Socket(Socket&& src) noexcept;
    ~Socket() noexcept;

    Socket& operator = (const Socket& src) = delete;
    Socket& operator = (Socket&& src);

    bool valid() const noexcept;
    bool close() noexcept;

    string ip() const noexcept;
    int port() const noexcept;
    string peerIp() const noexcept;
    int peerPort() const noexcept;

    bool bind(const string& ip="0.0.0.0", int port=0) const noexcept;
    bool listen(int n=10) const noexcept;
    Socket accept(string* ip=nullptr, int* port=nullptr) const noexcept;
    bool shutdown() const noexcept;
    
    bool write(int8_t data) const noexcept;
    bool write(uint8_t data) const noexcept;
    bool write(int16_t data) const noexcept;
    bool write(uint16_t data) const noexcept;
    bool write(int32_t data) const noexcept;
    bool write(uint32_t data) const noexcept;
    bool write(const void* data, size_t len) const noexcept;

    bool read(int8_t* data) const noexcept;
    bool read(uint8_t* data) const noexcept;
    bool read(int16_t* data) const noexcept;
    bool read(uint16_t* data) const noexcept;
    bool read(int32_t* data) const noexcept;
    bool read(uint32_t* data) const noexcept;
    bool read(void* data, size_t len) const noexcept;

private:
    SOCK_T m_fd;

    Socket(SOCK_T fd) noexcept;

}; // class Socket

}; // namespace Fisher

#endif // FISHER_SOCKET_H