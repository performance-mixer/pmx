#include "wpcpp/metadata_collection.h"

#include <pipewire/keys.h>

namespace wpcpp {
auto metadata_added_callback = +[ ](WpObjectManager *object_manager,
                                    gpointer object, gpointer user_data) {
  const auto metadata_collection = static_cast<wpcpp::MetadataCollection*>(
    user_data);

  const auto metadata = static_cast<WpMetadata*>(object);
  GValue global_properties_gvalue{0};
  g_object_get_property(G_OBJECT(metadata), "global-properties",
                        &global_properties_gvalue);

  if (global_properties_gvalue.data->v_pointer) {
    const auto global_properties = static_cast<WpProperties*>(
      global_properties_gvalue.data->v_pointer);
    auto name_c = wp_properties_get(global_properties, "metadata.name");
    if (name_c) {
      std::string name(name_c);
      if (name == "performance-mixer") {
        metadata_collection->set_metadata(metadata);
      }
    }
  }
};
}

bool wpcpp::MetadataCollection::get_existing_metadata_object(WpCore *core) {
  _object_manager = wp_object_manager_new();
  wp_object_manager_add_interest(_object_manager, WP_TYPE_METADATA, nullptr);
  wp_object_manager_request_object_features(_object_manager,
                                            WP_TYPE_GLOBAL_PROXY,
                                            WP_PIPEWIRE_OBJECT_FEATURES_MINIMAL);

  g_signal_connect(_object_manager, "object-added",
                   G_CALLBACK(+wpcpp::metadata_added_callback), this);

  wp_core_install_object_manager(core, _object_manager);

  return true;
}
