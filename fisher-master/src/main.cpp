#include <Application.h>
#include <Window.h>
#include <Renderer.h>
#include <Texture.h>
#include <QRCode.h>
#include <LoginJob.h>
#include <CoopJob.h>

using namespace Fisher;

int main()
{
    Application app;

    Window window("fisher", 320, 320);
    Renderer renderer(&window);
    app.bind(&window, &renderer);

    CoopRobot coopRobot;

    LoginRobot loginRobot;
    loginRobot.setTarget(&coopRobot);

    app.setJob(&loginRobot);
    app.exec();
    return 0;
}