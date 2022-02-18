#ifndef FISHER_NETBUFFER_H
#define FISHER_NETBUFFER_H

#include <cstdint>
#include <cstddef>

namespace Fisher
{

class NetBuffer
{
public:
    NetBuffer() noexcept;
    NetBuffer(const void* data, size_t size);
    NetBuffer(const NetBuffer& src);
    NetBuffer(NetBuffer&& src) noexcept;
    ~NetBuffer();

    const void* data() const noexcept;
    bool seek(size_t pos=0) noexcept;
    size_t tell() const noexcept;
    size_t size() const noexcept;
    void clear() noexcept;

    void write(const void* data, size_t bytes);
    void write(uint8_t n);
    void write(uint16_t n);
    void write(uint32_t n);
    void write(const char* str);

    bool read(void* data, size_t bytes) noexcept;
    bool read(uint8_t* data) noexcept;
    bool read(uint16_t* data) noexcept;
    bool read(uint32_t* data) noexcept;

private:
    void* m_data;
    size_t m_current;
    size_t m_size;      // 数据量
    size_t m_capacity;  // 容量 

    void m_increase();
    void m_ensure(size_t need);
}; // class NetBuffer

}; // namespace Fisher

#endif // FISHER_NETBUFFER_H