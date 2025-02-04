application_install_dir = join_paths(get_option('prefix'), get_option('bindir'))

user_systemd_dir_cmd = run_command(
    'sh', '-c', 'echo ${XDG_CONFIG_HOME:-$HOME/.config}/systemd/user', check : true)

user_systemd_dir = user_systemd_dir_cmd.stdout().strip()

configure_file(
    input : 'pmx-metadata-manager.service.in',
    output : 'pmx-metadata-manager.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)

configure_file(
    input : 'pmx-filter-chain-ctrl.service.in',
    output : 'pmx-filter-chain-ctrl.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)

configure_file(
    input : 'pmx-grpc-api.service.in',
    output : 'pmx-grpc-api.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)

configure_file(
    input : 'pmx-midi-router.service.in',
    output : 'pmx-midi-router.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)

configure_file(
    input : 'pmx-osc-network-receiver.service.in',
    output : 'pmx-osc-network-receiver.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)

configure_file(
    input : 'pmx-osc-network-sender.service.in',
    output : 'pmx-osc-network-sender.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)

configure_file(
    input : 'pmx-traktor-z1-router.service.in',
    output : 'pmx-traktor-z1-router.service',
    configuration : {
        'BINDIR' : application_install_dir
    },
    install : true,
    install_dir : user_systemd_dir
)
