#pragma once

#include <memory>

#include "wpcpp/proxy_collection.h"

namespace wpcpp {
struct interest {
  GType gtype;
};

class ProxyCollectionBuilder {
public:
  std::shared_ptr<ProxyCollection> build(WpCore *core) {
    auto object_manager = wp_object_manager_new();

    for (auto &interest : _interests) {
      wp_object_manager_add_interest(object_manager, interest.gtype, nullptr);
    }

    wp_object_manager_request_object_features(object_manager,
                                              WP_TYPE_GLOBAL_PROXY,
                                              WP_PIPEWIRE_OBJECT_FEATURES_MINIMAL);

    wp_core_install_object_manager(core, object_manager);

    auto proxy_collection = std::make_shared<ProxyCollection>(object_manager);
    proxy_collection->setup();
    return proxy_collection;
  }

  ProxyCollectionBuilder &add_interest(GType type) {
    _interests.push_back({type});
    return *this;
  }

private:
  std::vector<interest> _interests;
};
}
