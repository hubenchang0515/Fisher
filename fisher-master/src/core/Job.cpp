#include <Job.h>

namespace Fisher
{

void Job::onCreate(void* userdata)
{
    (void)(userdata);
}

void Job::onDelete()
{

}

void Job::onShow()
{

}

void Job::onHide()
{

}

void Job::onDraw()
{

}

void Job::onEvent(SDL_Event& ev)
{
    (void)(ev);
}


void Job::m_innerCreate()
{
    
}

void Job::m_innerDelete()
{
    m_state = State::nil;
}

void Job::m_innerShow()
{
    m_state = State::shown;
}

void Job::m_innerHide()
{
    m_state = State::hidden;
}

}; // namespace Fisher