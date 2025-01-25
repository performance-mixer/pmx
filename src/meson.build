executable(
  'pmx-midi-router',
  'applications/midi_router.cpp',
  'parameters.cpp',
  'processing.cpp',
  'interpolation.cpp',
  dependencies: [pipewire_dep],
  include_directories: [include_directory])

executable(
  'pmx-filter-chain-ctrl',
  'applications/filter_chain_ctrl.cpp',
  'parameters.cpp',
  'processing.cpp',
  'parse_osc_path.cpp',
  dependencies: [pipewire_dep],
  include_directories: [include_directory])

executable(
  'pmx-osc-network-router',
  'applications/osc_network_router.cpp',
  'parameters.cpp',
  'processing.cpp',
  'parse_osc_path.cpp',
  dependencies: [pipewire_dep, boost_dep],
  include_directories: [include_directory])

executable(
  'pmx-traktor-z1-router',
  'applications/traktor_z1_router.cpp',
  'parameters.cpp',
  'interpolation.cpp',
  dependencies: [pipewire_dep],
  include_directories: [include_directory])

