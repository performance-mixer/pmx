executable(
    'pmx-midi-router',
    sources : [
        'applications/midi_router.cpp',
        'parameters.cpp',
        'processing.cpp',
        'interpolation.cpp',
    ],
    dependencies : [pipewire_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-lp-mini-init',
    sources : [
        'applications/lp_mini_init.cpp',
    ],
    dependencies : [pipewire_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-ctrl-pipe',
    sources : [
        'applications/ctrl_pipe.cpp',
    ],
    dependencies : [pipewire_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-slp-ctrl',
    sources : [
        'applications/slp_ctrl.cpp',
    ],
    dependencies : [pipewire_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-filter-chain-ctrl',
    sources : [
        'applications/filter_chain_ctrl.cpp',
        'parameters.cpp',
        'processing.cpp',
        'parse_osc_path.cpp',
        'proxy_watcher.cpp',
        'pipewire_tools.cpp',
    ],
    dependencies : [pipewire_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-osc-network-sender',
    sources : [
        'applications/osc_network_sender.cpp',
        'parameters.cpp',
        'processing.cpp',
        'parse_osc_path.cpp',
    ],
    dependencies : [pipewire_dep, boost_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-osc-network-receiver',
    sources : [
        'applications/osc_network_receiver.cpp',
        'parameters.cpp',
        'processing.cpp',
        'parse_osc_path.cpp',
    ],
    dependencies : [pipewire_dep, boost_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-traktor-z1-router',
    sources : [
        'applications/traktor_z1_router.cpp',
        'parameters.cpp',
        'interpolation.cpp',
    ],
    dependencies : [pipewire_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-metadata-manager',
    sources : [
        'applications/metadata_manager.cpp',
        'metadata.cpp',
        'metadata_watcher.cpp',
        'prefix.cpp',
        'config.cpp'
    ],
    dependencies : [pipewire_dep, wireplumber_dep, yaml_cpp_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-console',
    sources : [
        'applications/console.cpp',
        'metadata.cpp',
        'link_collection.cpp',
        'proxy_collection.cpp',
        'units.cpp',
        'colors.cpp',
        'constants.cpp',
        'proxy_watcher.cpp',
        'pipewire_tools.cpp',
        'commands/describe.cpp',
        'commands/list.cpp',
        'commands/meta.cpp',
        'commands/pmx.cpp',
        'commands/send.cpp',
        'commands/start.cpp',
        'commands/status.cpp',
        'commands/setup.cpp',
        'commands/watch.cpp'
    ],
    dependencies : [pipewire_dep, wireplumber_dep, replxx_dep, libsystemd_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-params-watcher',
    sources : [
        'applications/params_watcher.cpp',
        'constants.cpp',
        'proxy_watcher.cpp',
    ],
    dependencies : [pipewire_dep],
    include_directories : [include_directory],
    install : true)

executable(
    'pmx-grpc-api',
    sources : [
        'applications/grpc_api.cpp',
        'port_collection.cpp',
        'metadata.cpp',
        'prefix.cpp',
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
    dependencies : [wireplumber_dep, grpc_dep, protobuf_dep, grcpp_dep, libsystemd_dep],
    include_directories : [include_directory, pmx_grpc_include_directory],
    install : true)
