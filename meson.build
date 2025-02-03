project('performance mixer', 'cpp', default_options : ['cpp_std=c++23'])

pipewire_dep = dependency('libpipewire-0.3')
wireplumber_dep = dependency('wireplumber-0.5')

boost_dep = dependency('boost', modules : ['system'])
grpc_dep = dependency('grpc')
grcpp_dep = dependency('grpc++')
protobuf_dep = dependency('protobuf')
yaml_cpp_dep = dependency('yaml-cpp')

include_directory = include_directories('include', '../pwcpp/include')
pmx_grpc_include_directory = include_directories('../pmx-grpc/cpp/')

subdir('src')
subdir('test')
