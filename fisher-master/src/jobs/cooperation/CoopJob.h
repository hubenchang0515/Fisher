#ifndef FISHER_COOP_ROBOT_H
#define FISHER_COOP_ROBOT_H

#include <Application.h>
#include <Renderer.h>
#include <Texture.h>
#include <QRCode.h>
#include <Job.h>
#include <NetBuffer.h>

#include <cerrno>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>  
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <mutex>

namespace Fisher
{

class CoopRobot : public Job
{
public:
    CoopRobot();
    ~CoopRobot();

    void setSocket(int fd);

protected:
    virtual void onCreate(void* userdata) override;
    virtual void onDraw() override;
    virtual void onEvent(SDL_Event& ev) override;

private:
    int m_stride = 0;
    int m_width = 0;
    int m_height = 0;
    int m_coopSocket = -1;
    Texture* m_texture = nullptr;
    std::mutex m_mutex;
    void* m_data = nullptr;
    NetBuffer* m_buffer = nullptr;

    void net();

}; // class CoopRobot

} // namespace Fisher

#endif // FISHER_COOP_ROBOT_H