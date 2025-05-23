install_data(
    'pmx-enable-filter-chains.fish',
    'pmx-disable-filter-chains.fish',
    'pmx-register-sooper-looper-updates.fish',
    install_dir : get_option('bindir'))

configure_file(
    input : 'pmx-start-sooperlooper.fish.in',
    output : 'pmx-start-sooperlooper.fish',
    configuration : {
        'BINDIR' : application_install_dir,
        'DATADIR' : application_data_dir
    },
    install : true,
    install_dir : application_install_dir
)
