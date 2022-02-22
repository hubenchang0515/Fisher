#ifndef FISHER_RENDERER_H
#define FISHER_RENDERER_H

#include <SDL2/SDL.h>
#include <Window.h>
#include <Log.h>

namespace Fisher
{

class Texture;

class Renderer
{
    friend Texture;
public:
    Renderer(const Window* window) noexcept;
    ~Renderer() noexcept;

    Texture* target() const noexcept;
    bool setTarget(Texture* target=nullptr) noexcept;

    uint8_t red() const noexcept;
    uint8_t green() const noexcept;
    uint8_t blue() const noexcept;
    uint8_t alpha() const noexcept;

    bool setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha=0xff) noexcept;
    bool setRed(uint red) noexcept;
    bool setGreen(uint8_t green) noexcept;
    bool setBlue(uint8_t blue) noexcept;
    bool setAlpha(uint8_t alpha) noexcept;

    SDL_BlendMode blendMode() const noexcept;
    bool setBlendMode(SDL_BlendMode mode) noexcept;

    bool clear() const noexcept;
    void present() const noexcept;
    bool copy(const Texture* src, SDL_Rect* srcRect=nullptr, SDL_Rect* dstRect=nullptr) const noexcept;
    bool drawPoint(int x, int y);


private:
    SDL_Renderer* m_renderer;
    Texture* m_target;
    uint8_t m_red;
    uint8_t m_green;
    uint8_t m_blue;
    uint8_t m_alpha;
    SDL_BlendMode m_blendMode;

}; // class Renderer

}; // namespace Fisher

#endif // FISHER_RENDERER_H