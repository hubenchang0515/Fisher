#ifndef FISHER_LOGIN_ROBOT_H
#define FISHER_LOGIN_ROBOT_H

#include <Application.h>
#include <Renderer.h>
#include <Texture.h>
#include <QRCode.h>
#include <Job.h>

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

class LoginRobot : public Job
{
public:
    LoginRobot();
    ~LoginRobot();

    void setTarget(Job* target);

    std::vector<std::string> getIpList() noexcept;
    int listen() noexcept;
    std::string info(std::vector<std::string> ipList, int port) noexcept;
    Texture* qrcode(const std::string& content) noexcept;

protected:
    virtual void onCreate(void* userdata) override;
    virtual void onDraw() override;
    virtual void onEvent(SDL_Event& ev) override;

private:
    Job* m_target;
    Texture* m_texture = nullptr;
    int m_listenSocket = -1;
    int m_coopSocket = -1;

    void net();
}; // class LoginRobot

}; // namespace Fisher

#endif // FISHER_LOGIN_ROBOT_H