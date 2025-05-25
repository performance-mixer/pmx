configure_file(
    input : 'pmx-metadata-manager.service.in',
    output : 'pmx-metadata-manager.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-filter-chain-ctrl.service.in',
    output : 'pmx-filter-chain-ctrl.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-grpc-api.service.in',
    output : 'pmx-grpc-api.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-midi-router.service.in',
    output : 'pmx-midi-router.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-osc-network-receiver.service.in',
    output : 'pmx-osc-network-receiver.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-osc-sl-network-receiver.service.in',
    output : 'pmx-osc-sl-network-receiver.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-osc-network-sender.service.in',
    output : 'pmx-osc-network-sender.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-osc-sl-network-sender.service.in',
    output : 'pmx-osc-sl-network-sender.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-traktor-z1-router.service.in',
    output : 'pmx-traktor-z1-router.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-filter-chains.service.in',
    output : 'pmx-filter-chains.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-sooperlooper.service.in',
    output : 'pmx-sooperlooper.service',
    configuration : {
        'BINDIR' : application_install_dir,
        'DATADIR' : application_data_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-lp-mini-init.service.in',
    output : 'pmx-lp-mini-init.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)

configure_file(
    input : 'pmx-slp-ctrl.service.in',
    output : 'pmx-slp-ctrl.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : 'lib/systemd/user'
)


install_data(
    'performance-mixer.target',
    'pmx-jack-midi-clock.service',
    install_dir : 'lib/systemd/user')