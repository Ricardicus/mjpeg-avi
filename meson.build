project('avimake' , ['c'], version: '1.0.0')

cc = meson.get_compiler('c')

includes = include_directories('src')
sources = ['src/avi.c', 'src/main.c']

network = executable('avimake',
  sources: [sources],
  include_directories: [includes]
)
