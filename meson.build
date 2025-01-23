project('performance mixer', 'cpp', default_options : ['cpp_std=c++23'])

pipewire_dep = dependency('libpipewire-0.3')
boost_dep = dependency('boost', modules : ['system'])

include_directory = include_directories('include', '../pwcpp/include')

subdir('src')
subdir('test')
