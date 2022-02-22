#ifndef FISHER_JOB_H
#define FISHER_JOB_H

#include <SDL2/SDL.h>

namespace Fisher
{

class Application;

class Job
{
    friend Application;
public:
    enum class State: int
    {
        nil,
        shown,
        hidden
    };

    Job() = default;
    virtual ~Job() = default;

protected:
    virtual void onCreate(void* userdata);
    virtual void onDelete();
    virtual void onShow();
    virtual void onHide();
    virtual void onDraw();
    virtual void onEvent(SDL_Event& ev);

private:
    State m_state = State::nil;
    virtual void m_innerCreate() final;
    virtual void m_innerDelete() final;
    virtual void m_innerShow() final;
    virtual void m_innerHide() final;

};

}; // namespace Fisher

#endif // FISHER_JOB_H