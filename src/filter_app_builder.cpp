#include "pwcpp/filter_app_builder.h"
#include <memory>

pwcpp::FilterAppPtr pwcpp::FilterAppBuilder::build() {
  return std::make_shared<FilterApp>();
}
