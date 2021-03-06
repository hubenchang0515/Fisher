#ifndef FISHER_LOGIN_ROBOT_H
#define FISHER_LOGIN_ROBOT_H

#include <Application.h>
#include <Renderer.h>
#include <Texture.h>
#include <QRCode.h>
#include <Job.h>
#include <Socket.h>

#include <cerrno>
#include <cstring>
#include <string>
#include <vector>

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
    void genQrCode(const std::string& content) noexcept;

protected:
    virtual void onCreate(void* userdata) override;
    virtual void onDraw() override;
    virtual void onEvent(SDL_Event& ev) override;

private:
    Job* m_target;
    Texture m_texture;
    Socket m_listenSocket;
    Socket m_coopSocket;

    void net();
}; // class LoginRobot

}; // namespace Fisher

#endif // FISHER_LOGIN_ROBOT_H