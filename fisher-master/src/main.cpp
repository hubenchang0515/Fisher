#include <Application.h>
#include <Window.h>
#include <Renderer.h>
#include <LoginJob.h>
#include <CoopJob.h>

using namespace Fisher;

int main(int argc, char* argv[])
{
    Application app{argc, argv};

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