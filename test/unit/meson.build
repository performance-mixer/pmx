osc_path_parsing_test = executable(
    'pmx-unit-tests',
    sources : [
        'test_osc_path_parsing.cpp',
        'test_interpolation.cpp',
        '../../src/parse_osc_path.cpp',
        '../../src/parameters.cpp',
        '../../src/interpolation.cpp'
    ],
    dependencies : [pipewire_dep, gtest_dep, gtest_main_dep],
    include_directories : [include_directory])

test(
    'GoogleTest',
    osc_path_parsing_test)
