#pragma once

#include "pwcpp/filter_app.h"
#include <memory>
namespace pwcpp {

typedef std::shared_ptr<FilterApp> FilterAppPtr;

template <class MessageType> class FilterAppBuilder {
public:
  FilterAppBuilder(int argc, char *argv[]) {
    for (int i = 0; i < argc; ++i) {
      arguments.push_back(std::string(argv[i]));
    }
  }

  FilterAppBuilder &
  add_pod_input_port(std::string name, std::string dsp_format,
                     FilterApp::ByteReader<MessageType> byte_reader);

  FilterAppBuilder &add_byte_pod_output_port(std::string name,
                                             std::string dsp_format,
                                             FilterApp::PortPodWriter writer);

  FilterAppPtr build();

private:
  std::vector<std::string> arguments;
};

} // namespace pwcpp
