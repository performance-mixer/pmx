osc_path_parsing_test = executable(
  'osc path parsing test',
  sources : [
    'test_osc_path_parsing.cpp',
    '../../src/parse_osc_path.cpp',
    '../../src/parameters.cpp'
  ],
  dependencies : [pipewire_dep],
  include_directories : [include_directory])

test(
  'osc path parsing test',
  osc_path_parsing_test)

interpolation_test = executable(
  'interpolation test',
  sources : [
    'test_interpolation.cpp',
    '../../src/interpolation.cpp',
    '../../src/parameters.cpp'
  ],
  dependencies : [pipewire_dep],
  include_directories : [include_directory])
