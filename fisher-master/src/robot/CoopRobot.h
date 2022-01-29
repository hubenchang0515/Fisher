#ifndef FISHER_SYNC_ROBOT_H
#define FISHER_SYNC_ROBOT_H

#include <Application.h>
#include <Renderer.h>
#include <Texture.h>
#include <QRCode.h>
#include <Robot.h>

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
    int m_width = 432;
    int m_height = 960;
    int m_coopSocket = -1;
    Texture* m_texture = nullptr;
    SDL_mutex* m_mutex = nullptr;
    void* m_data;

}; // class CoopRobot

} // namespace Fisher

#endif // FISHER_SYNC_ROBOT_H