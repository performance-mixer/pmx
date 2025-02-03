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
    sources : [
        'applications/filter_chain_ctrl.cpp',
        'parameters.cpp',
        'processing.cpp',
        'parse_osc_path.cpp',
    ],
    dependencies : [pipewire_dep],
    include_directories : [include_directory])

executable(
    'pmx-osc-network-router',
    sources : [
        'applications/osc_network_router.cpp',
        'parameters.cpp',
        'processing.cpp',
        'parse_osc_path.cpp',
    ],
    dependencies : [pipewire_dep, boost_dep],
    include_directories : [include_directory])

executable(
    'pmx-traktor-z1-router',
    sources : [
        'applications/traktor_z1_router.cpp',
        'parameters.cpp',
        'interpolation.cpp',
    ],
    dependencies : [pipewire_dep],
    include_directories : [include_directory])

executable(
    'pmx-metadata-manager',
    sources : [
        'applications/metadata_manager.cpp',
        'metadata_collection.cpp',
        'metadata_watcher.cpp',
        'config.cpp'
    ],
    dependencies : [pipewire_dep, wireplumber_dep, yaml_cpp_dep],
    include_directories : [include_directory])

executable(
    'pmx-grpc-api',
    sources : [
        'applications/grpc_api.cpp',
        'port_collection.cpp',
        'metadata_collection.cpp',
        '../../pmx-grpc/cpp/input_ports.grpc.pb.cc',
        '../../pmx-grpc/cpp/input_ports.pb.cc',
        '../../pmx-grpc/cpp/mono_port.grpc.pb.cc',
        '../../pmx-grpc/cpp/mono_port.pb.cc',
        '../../pmx-grpc/cpp/output_ports.grpc.pb.cc',
        '../../pmx-grpc/cpp/output_ports.pb.cc',
        '../../pmx-grpc/cpp/pmx_grpc.grpc.pb.cc',
        '../../pmx-grpc/cpp/pmx_grpc.pb.cc',
        '../../pmx-grpc/cpp/port.grpc.pb.cc',
        '../../pmx-grpc/cpp/port.pb.cc',
        '../../pmx-grpc/cpp/generic.grpc.pb.cc',
        '../../pmx-grpc/cpp/generic.pb.cc',
        '../../pmx-grpc/cpp/stereo_port.grpc.pb.cc',
        '../../pmx-grpc/cpp/stereo_port.pb.cc',
    ],
    dependencies : [wireplumber_dep, grpc_dep, protobuf_dep, grcpp_dep],
    include_directories : [include_directory, pmx_grpc_include_directory])
