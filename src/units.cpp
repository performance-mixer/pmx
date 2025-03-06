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

  result = sd_bus_message_open_container(request, SD_BUS_TYPE_ARRAY, "s");
  if (result < 0) {
    sd_bus_message_unref(request);
    return std::unexpected(
      error::error::systemd_message_building_error(strerror(-result)));
  }

  for (const auto &unit_name : unit_names) {
    std::cout << "Appending unit name" << std::endl;
    result = sd_bus_message_append_basic(request, 's', unit_name.c_str());
    if (result < 0) {
      sd_bus_message_unref(request);
      return std::unexpected(
        error::error::systemd_message_building_error(strerror(-result)));
    }
  }

  result = sd_bus_message_close_container(request);
  if (result < 0) {
    sd_bus_message_unref(request);
    return std::unexpected(
      error::error::systemd_message_building_error(strerror(-result)));
  }

  std::cout << "Appending bools" << std::endl;
  result = sd_bus_message_append(request, "bb", false, true);
  if (result < 0) {
    sd_bus_message_unref(request);
    return std::unexpected(
      error::error::systemd_message_building_error(strerror(-result)));
  }

  std::cout << "Calling" << std::endl;
  result = sd_bus_call(bus.bus(), request, 0, &error, &reply);
  std::cout << "Systemd response: " << result << std::endl;
  if (result < 0) {
    return std::unexpected(
      error::error::systemd_call_method(strerror(-result)));
  } else {
    const char *changes = nullptr;
    while (sd_bus_message_enter_container(reply, SD_BUS_TYPE_ARRAY, "(ss)") >
      0) {
      while (sd_bus_message_read(reply, "(ss)", &changes, &changes) > 0) {
        std::cout << "Systemd response: " << changes << std::endl;
      }
      sd_bus_message_exit_container(reply);
    }

    return {};
  }
}

std::expected<void, sdcpp::error> sdcpp::start_units(
  Bus &bus, const std::span<const std::string> &unit_names) {
  for (auto &unit_name : unit_names) {
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = nullptr;
    sd_bus_message *request = nullptr;
    const auto result = sd_bus_call_method(bus.bus(),
                                           "org.freedesktop.systemd1",
                                           "/org/freedesktop/systemd1",
                                           "org.freedesktop.systemd1.Manager",
                                           "StartUnit", &error, &reply, "ss",
                                           unit_name.c_str(), "replace");
    if (result < 0) {
      return std::unexpected(
        error::error::systemd_call_method(strerror(-result)));
    }
    sd_bus_message_unref(reply);
    sd_bus_message_unref(request);
  }
  return {};
}
