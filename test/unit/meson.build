osc_path_parsing_test = executable(
    'pmx-unit-tests',
    sources : [
        'test_osc_path_parsing.cpp',
        'test_interpolation.cpp',
        'test_ump_midi_parsing_v1.cpp',
        'test_ump_midi_parsing_v2.cpp',
        'test_ump_midi_parsing_errors.cpp',
        'launchpad/mini/test_parse_session_midi_buttons.cpp',
        'launchpad/mini/test_parse_session_midi_buttons_errors.cpp',
        '../../src/ump/parsing.cpp',
        '../../src/parse_osc_path.cpp',
        '../../src/parameters.cpp',
        '../../src/interpolation.cpp',
        '../../src/launchpad/mini/parse_session_midi.cpp'
    ],
    dependencies : [pipewire_dep, gtest_dep, gtest_main_dep],
    include_directories : [include_directory])

test(
    'GoogleTest',
    osc_path_parsing_test)
