#pragma once

class glos_exception : public std::exception {
public:
  std::string message{};

  glos_exception(std::string const &msg, std::source_location const &src_loc =
                                             std::source_location::current())
      : message{std::format("{}:{} - {}", src_loc.file_name(), src_loc.line(),
                            src_loc.function_name()) +
                ": " + msg} {}

  char const *what() const noexcept override { return message.c_str(); }
};
