#pragma once

#include <pwcpp/error.h>

#include <syslog.h>
#include <systemd/sd-journal.h>

#include <utility>
#include <vector>

namespace logging {
class Logger {
public:
  explicit Logger(std::string context)
    : _context(std::move(context)), _full_context("CONTEXT=" + context) {}

  void log_info(const std::string &message) {
    log(message, _priority_info, nullptr);
  }

  void log_info(const std::string &message, const char *log_fields, ...) {
    log(message, _priority_info, log_fields);
  }

  void log_warning(const std::string &message) {
    log(message, _priority_warning, nullptr);
  }

  void log_error(const std::string &message) {
    log(message, _priority_error, nullptr);
  }

  void log_error(const std::string &message, const char *log_fields, ...) {
    log(message, _priority_error, log_fields);
  }

  void log_error(const pwcpp::error &error) {
    log_error(error.message, _from_pwcpp.c_str());
  }

private:
  std::string _context;
  std::string _full_context;
  std::string _from_pwcpp = "FROM=pwcpp";
  std::string _priority_info = "PRIORITY=6";
  std::string _priority_warning = "PRIORITY=4";
  std::string _priority_error = "PRIORITY=3";

  void log(const std::string &message, const std::string &priority,
           const char *log_fields, ...) {
    auto full_message = "MESSAGE=" + message;
    std::vector log_entry{
      iovec{(void*)full_message.c_str(), full_message.size()},
      iovec{(void*)_full_context.c_str(), _full_context.size()},
      iovec{(void*)priority.c_str(), priority.size()}
    };

    sd_journal_sendv(log_entry.data(), log_entry.size());
  }
};
}
