project('glad', 'c',
  default_options: [
    'warning_level=2'
  ],
)

inc = include_directories('.')

# GL is required, but loaded at runtime by glad
# only include directories are used
gl_dep = dependency('gl', required: true)
gl_inc_dep = gl_dep.partial_dependency(includes: true, compile_args: true)

glad = static_library('glad',
  'glad.c',
  include_directories: inc,
  install: false,
  dependencies: gl_inc_dep
)

glad_dep = declare_dependency(
  include_directories: inc,
  link_with: glad,
)
