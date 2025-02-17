#include "metadata/metadata_watcher.h"

#include <string>

void metadata::MetadataWatcher::push_metadata_update(const guint subject,
                                                     const std::string &key,
                                                     const std::string &type,
                                                     const std::string &value) {
  std::lock_guard lock(_mutex);
  _metadata_updates.push(metadata_update{subject, key, type, value});
  _condition_variable.notify_all();
}

void metadata::MetadataWatcher::push_metadata_deletion(const guint subject,
                                                       const std::string &key) {
  std::lock_guard lock(_mutex);
  _metadata_updates.push(metadata_deletion{subject, key});
  _condition_variable.notify_all();
}

metadata::metadata_event metadata::MetadataWatcher::pop_metadata_update() {
  std::lock_guard lock(_mutex);
  auto update = _metadata_updates.front();
  _metadata_updates.pop();
  return update;
}

bool metadata::MetadataWatcher::has_metadata_updates() {
  std::lock_guard lock(_mutex);
  return !_metadata_updates.empty();
}

namespace metadata {
auto metadata_changed_callback = [](WpMetadata *metadata, const guint subject,
                                    const gchar *key, const gchar *type,
                                    const gchar *value, gpointer user_data) {
  const auto metadata_watcher = static_cast<metadata::MetadataWatcher*>(
    user_data);
  if (value != nullptr) {
    metadata_watcher->push_metadata_update(subject, key, type, value);
  } else {
    metadata_watcher->push_metadata_deletion(subject, key);
  }
};
}

void metadata::MetadataWatcher::set_metadata_and_connect_signal(
  WpMetadata *const metadata) {
  _metadata = metadata;
  g_signal_connect(metadata, "changed", G_CALLBACK(+metadata_changed_callback),
                   this);
}
