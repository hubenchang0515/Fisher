#ifndef FISHER_TEXTURE_H
#define FISHER_TEXTURE_H

#include <SDL2/SDL.h>
#include <Renderer.h>

namespace Fisher
{

class Texture
{
    friend Renderer;
public:
    Texture(const Renderer* renderer, int w, int h, int32_t format=SDL_PIXELFORMAT_RGB24, int access=SDL_TEXTUREACCESS_TARGET) noexcept;
    Texture(Texture&& src) noexcept;
    ~Texture() noexcept;

    bool update(const void* data, int pitch, SDL_Rect* rect=nullptr) noexcept;

private:
    SDL_Texture* m_texture;

}; // class Texture

}; // namespace Fisher

#endif // FISHER_TEXTURE_H