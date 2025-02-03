#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <config/config.h>
#include <wp/wp.h>

namespace metadata {
struct metadata_update {
  guint subject;
  std::string key;
  std::string type;
  std::string value;
};

struct metadata_deletion {
  guint subject;
  std::string key;
};

using metadata_event = std::variant<metadata_update, metadata_deletion>;

class MetadataWatcher {
public:
  void set_metadata_and_connect_signal(WpMetadata *const metadata);
  void push_metadata_update(guint subject, const std::string &key,
                            const std::string &type, const std::string &value);
  void push_metadata_deletion(guint subject, const std::string &key);
  metadata_event pop_metadata_update();
  bool has_metadata_updates();

  void wait_for(auto duration) {
    std::unique_lock lock(_mutex);
    _condition_variable.wait_for(lock, duration);
  }

private:
  WpObjectManager *_object_manager = nullptr;
  WpMetadata *_metadata = nullptr;
  std::mutex _mutex;
  std::queue<metadata_event> _metadata_updates;
  std::condition_variable _condition_variable;
  std::atomic<bool> _running{true};
};
}
