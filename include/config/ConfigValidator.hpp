#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP
#include "parser/ast/Config.hpp"
#include "utils/ErrorReporter.hpp"

class ConfigValidator
{
private:
  ErrorReporter &errorReporter;

public:
  ConfigValidator(ErrorReporter &errorReporter);
  void validate(Config &config);

private:
  void validateDirective(const Directive &directive);
  void validateServerConfig(const ServerConfig &serverConfig);
  void validateLocationConfig(const LocationConfig &locationConfig);
  // Helper function to validate if a string is a valid port number
  bool isValidPort(const std::string &value);
  bool checkListenDirective(const Directive &directive);

  void reportInvalidDirective(const Directive &directive, const std::string &message);
};

#endif