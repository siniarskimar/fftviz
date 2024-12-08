#include <format>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL_mixer.h>
#include <glad.h>

#ifdef HAS_SIGNAL_H
#include <csignal>
#endif

#include "SDL_raii.hpp"

template <typename... Args>
void showError(std::format_string<Args...> fmt, Args &&...args) {
    const auto err_msg =
        std::format(fmt, std::forward<decltype(args)>(args)...);
    std::cerr << err_msg << std::endl;
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL error", err_msg.c_str(),
                             NULL);
}

static int g_sigint_count = 0;

void sigIntHandler(int) { g_sigint_count += 1; }

int main(int argc, char **argv) {
    const char *filepath = nullptr;

    if (argc < 2) {
        // TODO: start idle
        std::cerr << "expected a filepath\n"
                  << "usage: fftviz <filepath>" << std::endl;
        return 2;
    }

    filepath = argv[1];

    SDL_AudioSpec audio_spec;
    audio_spec.freq = MIX_DEFAULT_FREQUENCY;
    audio_spec.format = MIX_DEFAULT_FORMAT;
    audio_spec.channels = MIX_DEFAULT_CHANNELS;

    raii::SDL raii_sdl;
    if (SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        showError("Failed to initialize SDL video subsystem: {}\n",
                  SDL_GetError());
        return 1;
    }

    if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        showError("Failed to initialize SDL audio subsystem: {}\n",
                  SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    raii::SDL_Window window(SDL_CreateWindow("fftviz", SDL_WINDOWPOS_CENTERED,
                                             SDL_WINDOWPOS_CENTERED, 800, 600,
                                             SDL_WINDOW_OPENGL));
    if (window == nullptr) {
        showError("Failed to create SDL window: {}\n", SDL_GetError());
        return 1;
    }

    raii::SDL_GLContext gl_context(SDL_GL_CreateContext(window.get()));
    if (gl_context == nullptr) {
        showError("Failed to create GL context: {}\n", SDL_GetError());
        return 1;
    }

    SDL_GL_MakeCurrent(window.get(), gl_context.get());
    SDL_GL_SetSwapInterval(1);

    int gl_version = gladLoadGLLoader(SDL_GL_GetProcAddress);
    if (gl_version == 0) {
        showError("Failed to load GL context\n");
        return 1;
    }


#ifdef HAVE_SIGNAL_H
    signal(SIGINT, sigIntHandler);
#endif

    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,
                      MIX_DEFAULT_CHANNELS, 2048) != 0) {
        showError("Failed to open audio device: {}\n", SDL_GetError());
        return 1;
    }
    raii::SDL_mixer raii_sdlmixer;

    Mix_VolumeMusic(MIX_MAX_VOLUME * 0.5f);

    raii::Mix_Music music(Mix_LoadMUS(filepath));
    if (music == nullptr) {
        showError("Failed to open {}: {}\n", filepath, SDL_GetError());
        return 1;
    }

    Mix_FadeInMusic(music.get(), 0, 200);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    bool should_close = false;
    while (!should_close) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                should_close = true;
                break;
            default:
                break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window.get());
    }
    Mix_HaltMusic();

    return 0;
}
