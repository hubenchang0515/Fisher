#ifndef FISHER_WORKER_H
#define FISHER_WORKER_H

#include <Renderer.h>
#include <SDL2/SDL.h>

namespace Fisher
{

class Application;

class Robot
{
    friend Application;

public:
    virtual ~Robot() = default;

protected:
    virtual void init(void* userdata);
    virtual void draw();
    virtual void event(SDL_Event& ev);
    virtual void other();
}; // class Robot

}; // namespace Fisher

#endif // FISHER_WORKER_H