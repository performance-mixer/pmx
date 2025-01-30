executable(
  'pmx-midi-router',
  'applications/midi_router.cpp',
  'parameters.cpp',
  'processing.cpp',
  'interpolation.cpp',
  dependencies : [pipewire_dep],
  include_directories : [include_directory])

executable(
  'pmx-filter-chain-ctrl',
  'applications/filter_chain_ctrl.cpp',
  'parameters.cpp',
  'processing.cpp',
  'parse_osc_path.cpp',
  dependencies : [pipewire_dep],
  include_directories : [include_directory])

executable(
  'pmx-osc-network-router',
  'applications/osc_network_router.cpp',
  'parameters.cpp',
  'processing.cpp',
  'parse_osc_path.cpp',
  dependencies : [pipewire_dep, boost_dep],
  include_directories : [include_directory])

executable(
  'pmx-traktor-z1-router',
  'applications/traktor_z1_router.cpp',
  'parameters.cpp',
  'interpolation.cpp',
  dependencies : [pipewire_dep],
  include_directories : [include_directory])

executable(
  'pmx-grpc-api',
  'applications/grpc_api.cpp',
  '../../pmx-grpc/cpp/output_ports.pb.cc',
  '../../pmx-grpc/cpp/output_ports.grpc.pb.cc',
  '../../pmx-grpc/cpp/port.pb.cc',
  '../../pmx-grpc/cpp/port.grpc.pb.cc',
  '../../pmx-grpc/cpp/pmx_grpc.pb.cc',
  '../../pmx-grpc/cpp/pmx_grpc.grpc.pb.cc',
  dependencies : [wireplumber_dep, grpc_dep, protobuf_dep, grcpp_dep],
  include_directories : [include_directory, pmx_grpc_include_directory])
