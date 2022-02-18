#ifndef FISHER_COOP_ROBOT_H
#define FISHER_COOP_ROBOT_H

#include <Application.h>
#include <Renderer.h>
#include <Texture.h>
#include <QRCode.h>
#include <Robot.h>
#include <NetBuffer.h>

#include <cerrno>
#include <cstring>
#include <string>
#include <vector>
#include <unistd.h>  
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace Fisher
{

class CoopRobot : public Robot
{
public:
    CoopRobot();
    ~CoopRobot();

    void setSocket(int fd);

protected:
    virtual void init(void* userdata) override;
    virtual void draw() override;
    virtual void event(SDL_Event& ev) override;
    virtual void other() override;

private:
    int m_stride = 0;
    int m_width = 0;
    int m_height = 0;
    int m_coopSocket = -1;
    Texture* m_texture = nullptr;
    SDL_mutex* m_mutex = nullptr;
    void* m_data = nullptr;
    NetBuffer* m_buffer = nullptr;

}; // class CoopRobot

} // namespace Fisher

#endif // FISHER_COOP_ROBOT_H