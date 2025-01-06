project('performance mixer', 'cpp', default_options : ['cpp_std=c++23'])
pipewire_dep = dependency('libpipewire-0.3')

include_directory = include_directories('include')

executable(
  'pmx-midi-router',
  'src/midi_router_main.cpp',
  'src/filter_app.cpp',
  'src/filter_app_builder.cpp',
  'src/parameters.cpp',
  dependencies: [pipewire_dep],
  include_directories: [include_directory])
