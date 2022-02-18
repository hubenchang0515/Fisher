#include <NetBuffer.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <arpa/inet.h>

namespace Fisher
{

NetBuffer::NetBuffer() noexcept:
    m_data(nullptr),
    m_current(0),
    m_size(0),
    m_capacity(0)
{

}

NetBuffer::NetBuffer(const void* data, size_t size):
    m_data(nullptr),
    m_current(0),
    m_size(0),
    m_capacity(0)
{
    m_data = malloc(size);
    if (m_data == nullptr)
    {
        throw std::bad_alloc();
    }

    memcpy(m_data, data, size);
    m_current = 0;
    m_size = size;
    m_capacity = size;
}

NetBuffer::NetBuffer(const NetBuffer& src)
{
    NetBuffer(src.m_data, src.m_size);
    m_current = src.m_current;
}

NetBuffer::NetBuffer(NetBuffer&& src) noexcept:
    m_data(src.m_data),
    m_current(src.m_current),
    m_size(src.m_size),
    m_capacity(src.m_capacity)
{
    src.m_data = nullptr;
    src.m_current = 0;
    src.m_size = 0;
    src.m_capacity = 0;
}

NetBuffer::~NetBuffer()
{
    clear();
}

void NetBuffer::m_increase()
{
    size_t capacity = m_capacity == 0 ? 8 : 2 * m_capacity;
    void* data = realloc(m_data, capacity);
    if (data == nullptr)
    {
        throw std::bad_alloc();
    }

    m_data = data;
    m_capacity = capacity;
}

void NetBuffer::m_ensure(size_t need)
{
    while(m_capacity < m_current + need)
        m_increase();
}

const void* NetBuffer::data() const noexcept
{
    return m_data;
}

bool NetBuffer::seek(size_t pos) noexcept
{
    if (pos > m_size)
    {
        return false;
    }

    m_current = pos;
    return true;
}
size_t NetBuffer::tell() const noexcept
{
    return m_current;
}

size_t NetBuffer::size() const noexcept
{
    return m_size;
}

void NetBuffer::clear() noexcept
{
    if (m_data != nullptr)
    {
        free(m_data);
    }

    m_data = nullptr;
    m_current = 0;
    m_size = 0;
    m_capacity = 0;
}

void NetBuffer::write(const void* data, size_t bytes)
{
    m_ensure(bytes);
    
    uint8_t* dst = static_cast<uint8_t*>(m_data) + m_current;
    memcpy(dst, data, bytes);

    m_current += bytes;
    if (m_current > m_size)
    {
        m_size = m_current;
    }
}

void NetBuffer::write(uint8_t n)
{
    write(&n, sizeof(uint8_t));
}

void NetBuffer::write(uint16_t n)
{
    uint16_t value = htons(n);
    write(&value, sizeof(uint16_t));
}

void NetBuffer::write(uint32_t n)
{
    uint32_t value = htonl(n);
    write(&value, sizeof(uint32_t));
}

void NetBuffer::write(const char* str)
{
    size_t bytes = strlen(str) + 1; // keep '\0'
    write(str, bytes);
}

bool NetBuffer::read(void* data, size_t bytes) noexcept
{
    if (m_current + bytes > m_size)
        return false;

    uint8_t* src = static_cast<uint8_t*>(m_data) + m_current;
    memcpy(data, src, bytes);
    m_current += bytes;
    return true;
}

bool NetBuffer::read(uint8_t* data) noexcept
{
    return read(data, sizeof(uint8_t));
}

bool NetBuffer::read(uint16_t* data) noexcept
{
    uint16_t value;
    if (read(&value, sizeof(uint16_t)))
    {
        *data = ntohs(value);
        return true;
    }

    return false;
}

bool NetBuffer::read(uint32_t* data) noexcept
{
    uint32_t value;
    if (read(&value, sizeof(uint32_t)))
    {
        *data = ntohl(value);
        return true;
    }

    return false;
}

}; // namespace Fisher