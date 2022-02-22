#ifndef FISHER_SOCKET_H
#define FISHER_SOCKET_H

#include <unistd.h>  
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>

#include <Log.h>

namespace Fisher
{

using std::string;

class Socket
{
public:
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

    bool bind(const string& ip, int port) const noexcept;
    bool listen(int n=10) const noexcept;
    Socket accept(string* ip=nullptr, int* port=nullptr) const noexcept;
    bool shutdown() const noexcept;
    
    bool write(int8_t data) const noexcept;
    bool write(uint8_t data) const noexcept;
    bool write(int16_t data) const noexcept;
    bool write(uint16_t data) const noexcept;
    bool write(int32_t data) const noexcept;
    bool write(uint32_t data) const noexcept;
    bool write(void* data, size_t len) const noexcept;

    bool read(int8_t* data) const noexcept;
    bool read(uint8_t* data) const noexcept;
    bool read(int16_t* data) const noexcept;
    bool read(uint16_t* data) const noexcept;
    bool read(int32_t* data) const noexcept;
    bool read(uint32_t* data) const noexcept;
    bool read(void* data, size_t len) const noexcept;

private:
    int m_fd;

    Socket(int fd) noexcept;

}; // class Socket

}; // namespace Fisher

#endif // FISHER_SOCKET_H