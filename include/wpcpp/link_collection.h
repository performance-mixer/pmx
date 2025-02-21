#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <wp/core.h>

namespace wpcpp {
struct link {
  int object_id;
  int output_port_id;
  int input_port_id;
  int output_node_id;
  int input_node_id;
};

inline std::ostream &operator<<(std::ostream &os, const link &link) {
  os << "link: " << link.object_id << " (" << link.output_node_id << ", " <<
    link.output_port_id << ") -> (" << link.input_node_id << ", " << link.
    input_port_id << ")";
  return os;
}

class LinkCollection {
public:
  explicit LinkCollection(WpObjectManager *object_manager) : _object_manager(
    object_manager) {}

  static std::shared_ptr<LinkCollection> create_and_setup(WpCore *core);

  void push_back(const link &link) {
    std::lock_guard lock(_mutex);
    _links.push_back(link);
  }

  std::vector<link> get_links() {
    std::lock_guard lock(_mutex);
    std::vector<link> result_links;
    std::copy(_links.begin(), _links.end(), std::back_inserter(result_links));
    return result_links;
  }

private:
  WpObjectManager *_object_manager;
  std::vector<link> _links;
  std::mutex _mutex;
};
}
