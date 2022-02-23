#ifndef FISHER_COOP_ROBOT_H
#define FISHER_COOP_ROBOT_H

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
#include <mutex>

namespace Fisher
{

class CoopRobot : public Job
{
public:
    CoopRobot();
    ~CoopRobot();

protected:
    virtual void onCreate(void* userdata) override;
    virtual void onDraw() override;
    virtual void onEvent(SDL_Event& ev) override;

private:
    int m_width = 0;
    int m_height = 0;
    Socket  m_coopSocket;
    Texture* m_texture = nullptr;
    std::mutex m_mutex;
    void* m_data = nullptr;
    size_t m_size = 0;

    void net();
    bool uncompress(const void* jpeg, size_t inSize, void** rgb, size_t* outSize);

}; // class CoopRobot

} // namespace Fisher

#endif // FISHER_COOP_ROBOT_H