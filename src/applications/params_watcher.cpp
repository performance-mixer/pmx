
#include <iostream>
#include <ranges>
#include <string_view>

#include <pipewire/main-loop.h>
#include <pmx/constants.h>
#include <proxy/proxy_watcher.h>

int main(int argc, char *argv[]) {
  pw_init(&argc, &argv);
  auto loop = pw_main_loop_new(nullptr);

  pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGINT,
                     [](void *user_data, int signal_number) { const auto loop =
                     static_cast<pw_main_loop * >(user_data); pw_main_loop_quit(
                       loop); }, loop);

  pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGTERM,
                     [](void *user_data, int signal_number) { const auto loop =
                     static_cast<pw_main_loop * >(user_data); pw_main_loop_quit(
                       loop); }, loop);

  proxy::ProxyWatcher watcher;
  auto context = pw_context_new(pw_main_loop_get_loop(loop), nullptr, 0);
  auto core = pw_context_connect(context, nullptr, 0);
  auto registry = pw_core_get_registry(core, PW_VERSION_REGISTRY, 0);
  watcher.register_callback(registry);


  auto services_without_suffix = pmx::constants::all_services |
    std::views::transform([](const std::string &service) {
      constexpr std::string_view suffix = ".service";
      if (service.size() >= suffix.size() && service.compare(
        service.size() - suffix.size(), suffix.size(), suffix) == 0) {
        return service.substr(0, service.size() - suffix.size());
      }
      return service;
    });

  auto watcher_callback = [](
    std::span<std::tuple<std::string, pwcpp::property::property_value_type>>
    updates, const proxy::Proxy &proxy) {
    for (auto &update : updates) {
      std::cout << proxy.name << ": " << std::get<0>(update) << ": " << std::get
        <1>(update) << std::endl;
    }
  };

  for (const auto &service : pmx::constants::filter_chain_node_names) {
    auto result = watcher.watch_proxy_by_name(service, watcher_callback);
  }

  for (const auto &service : services_without_suffix) {
    auto result = watcher.watch_proxy_by_name(service, watcher_callback);
  }

  pw_main_loop_run(loop);
  pw_main_loop_destroy(loop);
  pw_deinit();

  return 0;
}
