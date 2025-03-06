#pragma once

#include "console/messages.h"

#include <expected>
#include <sstream>
#include <error/error.h>
#include <sdcpp/bus.h>
#include <wpcpp/metadata.h>
#include <wpcpp/link_collection.h>
#include <wpcpp/proxy_collection.h>
#include <proxy/proxy_watcher.h>
#include <sdcpp/error.h>

namespace console {
std::expected<void, error::error> describe_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  proxy::ProxyWatcher &proxy_watcher, wpcpp::LinkCollection &link_collection);

std::expected<void, error::error> list_command(std::istringstream &stream,
                                               wpcpp::LinkCollection &
                                               link_collection,
                                               wpcpp::ProxyCollection &
                                               proxy_collection,
                                               sdcpp::Bus &bus);

std::expected<void, error::error> meta_command(std::istringstream &stream,
                                               wpcpp::Metadata &metadata);

std::expected<void, error::error> pmx_command(std::istringstream &stream,
                                              wpcpp::Metadata &metadata,
                                              wpcpp::ProxyCollection &
                                              proxy_collection,
                                              wpcpp::LinkCollection &
                                              link_collection, sd_bus *sd_bus);

std::expected<void, error::error> send_command(std::istringstream &stream,
                                               request_message &message);

std::expected<void, sdcpp::error> start_command(std::istringstream &stream,
                                                sdcpp::Bus &bus);

std::expected<void, sdcpp::error> status_command(
  std::istringstream &stream, wpcpp::ProxyCollection &proxy_collection,
  wpcpp::LinkCollection &link_collection, sdcpp::Bus &bus);

std::expected<void, error::error> setup_command(std::istringstream &stream,
                                                wpcpp::ProxyCollection &
                                                proxy_collection,
                                                proxy::ProxyWatcher &
                                                proxy_watcher,
                                                pw_main_loop *loop);

std::expected<void, error::error> watch_command(std::istringstream &stream,
                                                proxy::ProxyWatcher &
                                                proxy_watcher);
}
