#include <stdexcept>
#include <Renderer.h>
#include <Texture.h>

namespace Fisher
{

Renderer::Renderer(const Window* window) noexcept:
    m_target(nullptr),
    m_red(0),
    m_green(0),
    m_blue(0),
    m_alpha(0xff),
    m_blendMode(SDL_BLENDMODE_NONE)
{
    m_renderer = SDL_CreateRenderer(window->m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if (m_renderer == nullptr)
    {
        ERR("%s", SDL_GetError());
        m_renderer = SDL_CreateRenderer(window->m_window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
    }

    if (m_renderer == nullptr)
    {
        ERR("%s", SDL_GetError());
    }
}

Renderer::~Renderer() noexcept
{
    if (m_renderer != nullptr)
        SDL_DestroyRenderer(m_renderer);

}

Texture* Renderer::target() const noexcept
{
    return m_target;
}

bool Renderer::setTarget(Texture* target) noexcept
{
    SDL_Texture* t = target == nullptr ? nullptr : target->m_texture;

    if (SDL_SetRenderTarget(m_renderer, t) < 0)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    m_target = target;
    return true;
}

uint8_t Renderer::red() const noexcept
{
    return m_red;
}

uint8_t Renderer::green() const noexcept
{
    return m_green;
}

uint8_t Renderer::blue() const noexcept
{
    return m_blue;
}

uint8_t Renderer::alpha() const noexcept
{
    return m_alpha;
}

bool Renderer::setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) noexcept
{
    if (SDL_SetRenderDrawColor(m_renderer, red, green, blue, alpha) < 0)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    m_red = red;
    m_green = green;
    m_blue = blue;
    m_alpha = alpha;
    return true;
}

bool Renderer::setRed(uint red) noexcept
{
    return setColor(red, m_green, m_blue, m_alpha);
}

bool Renderer::setGreen(uint8_t green) noexcept
{
    return setColor(m_red, green, m_blue, m_alpha);
}

bool Renderer::setBlue(uint8_t blue) noexcept
{
    return setColor(m_red, m_green, blue, m_alpha);
}

bool Renderer::setAlpha(uint8_t alpha) noexcept
{
    return setColor(m_red, m_green, m_blue, alpha);
}

SDL_BlendMode Renderer::blendMode() const noexcept
{
    return m_blendMode;
}

bool Renderer::setBlendMode(SDL_BlendMode mode) noexcept
{
    if (SDL_SetRenderDrawBlendMode(m_renderer, mode) < 0)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    m_blendMode = mode;
    return true;
}

bool Renderer::clear() const noexcept
{
    SDL_RenderClear(m_renderer);
    // if (SDL_RenderClear(m_renderer) < 0)
    // {
    //     ERR("%s", SDL_GetError());
    //     return false;
    // }

    return true;
}

void Renderer::present() const noexcept
{
    SDL_RenderPresent(m_renderer);
}

bool Renderer::copy(const Texture* src, SDL_Rect* srcRect, SDL_Rect* dstRect) const noexcept
{
    if (SDL_RenderCopy(m_renderer, src->m_texture, srcRect, dstRect) < 0)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    return true;
}

bool Renderer::drawPoint(int x, int y)
{
    if (SDL_RenderDrawPoint(m_renderer, x, y) < 0)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    return true;
}

}; // namespace Fisher