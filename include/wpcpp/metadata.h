#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

#include <wp/wp.h>

namespace wpcpp {
class Metadata {
public:
  bool get_existing_metadata_object(WpCore *core);

  std::vector<std::tuple<std::string, std::string>> get_metadata_values() {
    std::lock_guard lock(_no_parallel_calls_mutex);

    struct user_data_t {
      WpMetadata *metadata;
      std::mutex &wait_for_completion_mutex;
      std::condition_variable condition;
      std::vector<std::tuple<std::string, std::string>> result;
    };

    auto user_data = user_data_t{_metadata, _wait_for_completion_mutex};

    std::unique_lock wait_lock(_wait_for_completion_mutex);

    g_idle_add_once([](gpointer user_data) {
      auto *data = static_cast<user_data_t*>(user_data);
      std::lock_guard lock(data->wait_for_completion_mutex);

      auto metadata_iterator = wp_metadata_new_iterator(data->metadata, 0);
      GValue value = {0};
      while (wp_iterator_next(metadata_iterator, &value)) {
        auto metadata_item = static_cast<WpMetadataItem*>(value.data->
                                                                     v_pointer);
        auto key = wp_metadata_item_get_key(metadata_item);
        auto metadata_value = wp_metadata_item_get_value(metadata_item);
        data->result.emplace_back(key, metadata_value);
      }

      data->condition.notify_all();
    }, &user_data);

    user_data.condition.wait(wait_lock);
    return user_data.result;
  }

  void set_metadata_value_async(const std::string &key,
                                const std::string &value) {
    std::lock_guard lock(_no_parallel_calls_mutex);
    wp_metadata_set(_metadata, 0, key.c_str(), "Spa:String:JSON",
                    value.c_str());
  }

  void set_metadata_value(const std::string &key, const std::string &value) {
    std::lock_guard lock(_no_parallel_calls_mutex);

    struct user_data_t {
      WpMetadata *metadata;
      std::mutex &wait_for_completion_mutex;
      std::string key;
      std::string value;
      std::condition_variable condition;
    };

    if (g_main_context_is_owner(g_main_context_default())) {
      wp_metadata_set(_metadata, 0, key.c_str(), "Spa:String:JSON",
                      value.c_str());
    } else {
      auto user_data = user_data_t{
        _metadata, _wait_for_completion_mutex, key, value
      };
      std::unique_lock wait_lock(_wait_for_completion_mutex);

      g_idle_add_once([](gpointer user_data) {
        auto *data = static_cast<user_data_t*>(user_data);
        std::lock_guard lock(data->wait_for_completion_mutex);
        wp_metadata_set(data->metadata, 0, data->key.c_str(), "Spa:String:JSON",
                        data->value.c_str());
        data->condition.notify_all();
      }, &user_data);

      user_data.condition.wait(wait_lock);
    }
  }

  void clear_metadata_value(const std::string &key) {
    std::lock_guard lock(_no_parallel_calls_mutex);

    struct user_data_t {
      WpMetadata *metadata;
      std::mutex &wait_for_completion_mutex;
      std::string key;
      std::condition_variable condition;
    };

    auto user_data = user_data_t{_metadata, _wait_for_completion_mutex, key};
    std::unique_lock wait_lock(_wait_for_completion_mutex);

    g_idle_add_once([](gpointer user_data) {
      auto *data = static_cast<user_data_t*>(user_data);
      std::lock_guard lock(data->wait_for_completion_mutex);
      wp_metadata_set(data->metadata, 0, data->key.c_str(), "Spa:String:JSON",
                      nullptr);
      data->condition.notify_all();
    }, &user_data);

    user_data.condition.wait(wait_lock);
  }

  void set_metadata(WpMetadata *const metadata) { _metadata = metadata; }
  [[nodiscard]] WpMetadata *metadata() const { return _metadata; }

private:
  std::mutex _no_parallel_calls_mutex;
  std::mutex _wait_for_completion_mutex;
  WpMetadata *_metadata = nullptr;
  WpObjectManager *_object_manager = nullptr;
};
}
