project('fftviz', 'cpp',
  default_options: [
    'cpp_std=c++20',
    'warning_level=2',
  ]
)

sdl_dep = dependency('sdl2', required: true)
sdlmain_dep = dependency('sdl2main', static: true, required: false)
sdlmixer_dep = dependency('sdl2_mixer', required: true)

glad_proj = subproject('glad')
glad_dep = glad_proj.get_variable('glad_dep')

if host_machine.system() != 'linux' and not sdlmain_dep.found()
  error('SDL_main is required for the host system')
endif

c_compiler = meson.get_compiler('c')
has_signal_h = c_compiler.check_header('signal.h')

fftviz_defines = []
if has_signal_h
  fftviz_defines += ['-DHAS_SIGNAL_H']
endif

fftviz_exe = executable('fftviz',
  'src/main.cpp',
  dependencies: [
    sdlmain_dep,
    sdl_dep,
    sdlmixer_dep,
    glad_dep
  ],
  override_options: [
    'warning_level=3'
  ],
  cpp_args: fftviz_defines
)
