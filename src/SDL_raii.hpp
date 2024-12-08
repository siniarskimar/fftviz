#pragma once

#include <memory>
#include <type_traits>

#include <SDL2/SDL.h>
#include <SDL_mixer.h>

namespace raii {

struct SDL {
    inline ~SDL() noexcept { SDL_Quit(); }
};

struct SDL_WindowDeleter {
    inline void operator()(::SDL_Window *window) const noexcept {
        SDL_DestroyWindow(window);
    }
};

struct SDL_GLContextDeleter {
    inline void operator()(::SDL_GLContext gl_context) const noexcept {
        SDL_GL_DeleteContext(gl_context);
    }
};

struct SDL_mixer {
    inline ~SDL_mixer() noexcept { SDL_CloseAudio(); }
};

struct SDL_Mix_MusicDeleter {
    inline void operator()(::Mix_Music *music) const noexcept {
        Mix_FreeMusic(music);
    }
};

using SDL_Window = std::unique_ptr<::SDL_Window, SDL_WindowDeleter>;
using SDL_GLContext = std::unique_ptr<std::remove_pointer_t<::SDL_GLContext>,
                                      SDL_GLContextDeleter>;
using Mix_Music = std::unique_ptr<Mix_Music, SDL_Mix_MusicDeleter>;

}; // namespace raii
