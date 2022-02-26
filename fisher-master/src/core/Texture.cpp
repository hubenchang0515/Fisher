#include <Texture.h>

namespace Fisher
{

Texture::Texture() noexcept:
    m_texture(nullptr)
{

}

Texture::Texture(const Renderer* renderer, int width, int height, int32_t format, int access) noexcept
{
    m_texture = SDL_CreateTexture(renderer->m_renderer, format, access, width, height);
    if (m_texture == nullptr)
    {
        ERR("%s", SDL_GetError());
    }
}

Texture::Texture(Texture&& src) noexcept
{
    m_texture = src.m_texture;
    src.m_texture = nullptr;
}

Texture::~Texture() noexcept
{
    if (m_texture != nullptr)
    {
        SDL_DestroyTexture(m_texture);
    }
}

bool Texture::update(const void* data, int pitch, SDL_Rect* rect) noexcept
{
    if (SDL_UpdateTexture(m_texture, rect, data, pitch) < 0)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    return true;
}

bool Texture::realloc(const Renderer* renderer, int width, int height, int32_t format, int access) noexcept
{
    m_texture = SDL_CreateTexture(renderer->m_renderer, format, access, width, height);
    if (m_texture == nullptr)
    {
        ERR("%s", SDL_GetError());
        return false;
    }

    return true;
}

}; // namespace Fisher