#pragma once

#include <condition_variable>
#include <wp/wp.h>

namespace wpcpp {

class MetadataCollection {
public:
  void register_initial_metadata_object(WpCore * core) {
    auto metadata = wp_impl_metadata_new_full(core, "performance-mixer",
                                              nullptr);

    wp_metadata_set(reinterpret_cast<WpMetadata *>(metadata), 0,
                    "left_outputs", "Spa:String:JSON", "[]");
    wp_metadata_set(reinterpret_cast<WpMetadata *>(metadata), 0,
                    "right_outputs", "Spa:String:JSON", "[]");

    wp_object_activate(reinterpret_cast<WpObject *>(metadata),
                       WP_OBJECT_FEATURES_ALL, nullptr,
                       [](GObject * source_object, GAsyncResult * res,
                          gpointer data) {}, nullptr);
    _metadata = reinterpret_cast<WpMetadata *>(metadata);
  }

  void set_metadata_value(const std::string &key, const std::string &value) {
    std::lock_guard lock(_no_parallel_calls_mutex);

    struct user_data_t {
      WpMetadata * metadata;
      std::mutex &wait_for_completion_mutex;
      std::string key;
      std::string value;
      std::condition_variable condition;
    };

    auto user_data = user_data_t{_metadata, _wait_for_completion_mutex, key, value};
    std::unique_lock wait_lock(_wait_for_completion_mutex);

    g_idle_add_once([](gpointer user_data) {
      auto * data = static_cast<user_data_t *>(user_data);
      std::lock_guard lock(data->wait_for_completion_mutex);
      data->condition.notify_all();
    wp_metadata_set(data->metadata, 0,
                    data->key.c_str(), "Spa:String:JSON", data->value.c_str());
    }, &user_data);

    user_data.condition.wait(wait_lock);
  }

private:
  std::mutex _no_parallel_calls_mutex;
  std::mutex _wait_for_completion_mutex;
  WpMetadata * _metadata = nullptr;
};

}
