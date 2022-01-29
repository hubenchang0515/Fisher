#ifndef FISHER_LOGIN_ROBOT_H
#define FISHER_LOGIN_ROBOT_H

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

class LoginRobot : public Robot
{
public:
    LoginRobot();
    ~LoginRobot();

    void setTarget(Robot* target);

    std::vector<std::string> getIpList() noexcept;
    int listen() noexcept;
    std::string info(std::vector<std::string> ipList, int port) noexcept;
    Texture* qrcode(const std::string& content) noexcept;

protected:
    virtual void draw() override;
    virtual void event(SDL_Event& ev) override;
    virtual void other() override;

private:
    Robot* m_target;
    Texture* m_texture = nullptr;
    int m_listenSocket = -1;
    int m_coopSocket = -1;


    

}; // class LoginRobot

}; // namespace Fisher

#endif // FISHER_LOGIN_ROBOT_H