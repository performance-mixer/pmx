#include "sdcpp/units.h"

#include <iostream>
#include <systemd/sd-bus.h>

std::expected<std::vector<sdcpp::unit>, sdcpp::error>
sdcpp::list_units(Bus &bus) {
  std::vector<unit> units;
  sd_bus_message *reply;
  sd_bus_error error = SD_BUS_ERROR_NULL;
  auto result = sd_bus_call_method(bus.bus(), "org.freedesktop.systemd1",
                                   "/org/freedesktop/systemd1",
                                   "org.freedesktop.systemd1.Manager",
                                   "ListUnits", &error, &reply, nullptr);
  if (result < 0) {
    sd_bus_message_unref(reply);
    return std::unexpected(
      error::error::systemd_call_method(strerror(-result)));
  }

  while (sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY,
                                        "(ssssssouso)") > 0) {
    while (!sd_bus_message_at_end(reply, false)) {
      result = sd_bus_message_enter_container(reply, SD_BUS_TYPE_STRUCT,
                                              "ssssssouso");
      if (result <= 0) {
        return std::unexpected(
          error::error::systemd_method_parsing_error(strerror(-result)));
      }

      const char *name = nullptr;
      const char *description = nullptr;
      const char *load_state = nullptr;
      const char *active_state = nullptr;
      const char *sub_state = nullptr;
      const char *following = nullptr;
      const char *object_path = nullptr;
      std::uint32_t id;
      const char *job_type = nullptr;
      const char *job_path = nullptr;

      result = sd_bus_message_read(reply, "ssssssouso", &name, &description,
                                   &load_state, &active_state, &sub_state,
                                   &following, &object_path, &id, &job_type,
                                   &job_path);

      if (result < 0) {
        return std::unexpected(
          error::error::systemd_method_parsing_error(strerror(-result)));
      }

      units.emplace_back(unit{
        name, description, load_state, active_state, sub_state, following,
        object_path, id, job_type, job_path
      });

      sd_bus_message_exit_container(reply);
    }
  }
  sd_bus_message_exit_container(reply);
  sd_bus_error_free(&error);
  sd_bus_message_unref(reply);
  return units;
}

std::expected<std::vector<sdcpp::unit_file>, sdcpp::error>
sdcpp::list_unit_files(sdcpp::Bus &bus) {
  std::vector<unit_file> units;
  sd_bus_message *reply;
  sd_bus_error error = SD_BUS_ERROR_NULL;
  auto result = sd_bus_call_method(bus.bus(), "org.freedesktop.systemd1",
                                   "/org/freedesktop/systemd1",
                                   "org.freedesktop.systemd1.Manager",
                                   "ListUnitFiles", &error, &reply, nullptr);
  if (result < 0) {
    sd_bus_message_unref(reply);
    return std::unexpected(
      error::error::systemd_call_method(strerror(-result)));
  }

  while (sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ss)") > 0) {
    while (!sd_bus_message_at_end(reply, false)) {
      result = sd_bus_message_enter_container(reply, SD_BUS_TYPE_STRUCT, "ss");
      if (result <= 0) {
        return std::unexpected(
          error::error::systemd_method_parsing_error(strerror(-result)));
      }

      const char *name = nullptr;
      const char *state = nullptr;

      result = sd_bus_message_read(reply, "ss", &name, &state);

      if (result < 0) {
        return std::unexpected(
          error::error::systemd_method_parsing_error(strerror(-result)));
      }

      units.emplace_back(unit_file{name, state});

      sd_bus_message_exit_container(reply);
    }
  }
  sd_bus_message_exit_container(reply);
  sd_bus_error_free(&error);
  sd_bus_message_unref(reply);
  return units;
}

std::expected<void, sdcpp::error> sdcpp::enable_units(
  Bus &bus, const std::span<const std::string> &unit_names) {
  sd_bus_error error = SD_BUS_ERROR_NULL;
  sd_bus_message *reply = nullptr;
  sd_bus_message *request = nullptr;
  auto result = sd_bus_message_new_method_call(bus.bus(), &request,
                                               "org.freedesktop.systemd1",
                                               "/org/freedesktop/systemd1",
                                               "org.freedesktop.systemd1.Manager",
                                               "EnableUnitFiles");

  if (result < 0) {
    return std::unexpected(
      error::error::systemd_message_building_error(strerror(-result)));
  }

  const char **unit_names_c = new const char*[unit_names.size()];
  for (std::size_t i = 0; i < unit_names.size(); ++i) {
    unit_names_c[i] = unit_names[i].c_str();
  }

  result = sd_bus_message_append_array(request, 's', unit_names_c, 1);
  delete[] unit_names_c;

  if (result < 0) {
    sd_bus_message_unref(request);
    return std::unexpected(
      error::error::systemd_message_building_error(strerror(-result)));
  }

  result = sd_bus_message_append(request, "bb", false, true);
  if (result < 0) {
    sd_bus_message_unref(request);
    return std::unexpected(
      error::error::systemd_message_building_error(strerror(-result)));
  }

  result = sd_bus_call(bus.bus(), request, 0, &error, &reply);
  if (result < 0) {
    return std::unexpected(
      error::error::systemd_call_method(strerror(-result)));
  } else { return {}; }
}

std::expected<void, sdcpp::error> sdcpp::start_units(
  Bus &bus, std::span<std::string> &unit_names) {
  return {};
}
