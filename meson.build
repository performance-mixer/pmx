project('performance mixer', 'cpp', default_options : ['cpp_std=c++23'])

add_project_arguments('-Wno-deprecated-declarations', language : 'cpp')
add_project_arguments('-Wno-sign-compare', language : 'cpp')
add_project_arguments('-Wno-deprecated-enum-enum-conversion', language : 'cpp')

application_install_dir = join_paths(get_option('prefix'), get_option('bindir'))
application_data_dir = join_paths(get_option('prefix'), get_option('datadir'), 'pmx')

pipewire_dep = dependency('libpipewire-0.3')
wireplumber_dep = dependency('wireplumber-0.5')
boost_dep = dependency('boost', modules : ['system'])
grpc_dep = dependency('grpc')
grcpp_dep = dependency('grpc++')
protobuf_dep = dependency('protobuf')
yaml_cpp_dep = dependency('yaml-cpp')
libsystemd_dep = dependency('libsystemd')
gtest_dep = dependency('gtest')
gtest_main_dep = dependency('gtest_main')
replxx_dep = dependency('replxx')

include_directory = include_directories('include', '../pwcpp/include', 'external')
pmx_grpc_include_directory = include_directories('../pmx-grpc/cpp/')

subdir('src')
subdir('test')
subdir('systemd')
subdir('wireplumber')
subdir('scripts')
subdir('sooperlooper')