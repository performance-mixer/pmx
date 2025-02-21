#pragma once

#include <algorithm>
#include <vector>
#include <mutex>
#include <string>
#include <wp/wp.h>

namespace wpcpp {
enum class proxy_type { port, node, unknown };

struct proxy {
  int object_id;
  std::string name;
  std::string path;
  proxy_type type;
};

class ProxyCollection {
public:
  explicit ProxyCollection(WpObjectManager *object_manager) : _object_manager(
    object_manager) {}

  void push_back(const proxy &proxy) {
    std::lock_guard lock(_mutex);
    _ports.push_back(proxy);
  }

  std::vector<proxy> get_proxies() {
    std::lock_guard lock(_mutex);
    std::vector<proxy> result_proxies;
    std::copy(_ports.begin(), _ports.end(), std::back_inserter(result_proxies));
    return result_proxies;
  }

  void setup();

private:
  std::vector<proxy> _ports;
  std::mutex _mutex;
  WpObjectManager *_object_manager;
};
}
