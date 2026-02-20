#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP
#include "parser/ast/Config.hpp"
#include "utils/ErrorReporter.hpp"
#include <map>
class ConfigValidator
{
private:
  ErrorReporter &errorReporter;
  std::map<std::pair<std::string, std::string>, Span> usedServerNameLocationPairs;

public:
  ConfigValidator(ErrorReporter &errorReporter);
  void validate(Config &config);

private:
  void validateDirective(const Directive &directive);
  void validateServerConfig(const ServerConfig &serverConfig);
  void validateLocationConfig(const LocationConfig &locationConfig);
  // Helper functions for validating directive values
  void validateRequiredDirectives(const ServerConfig &serverConfig);
  void validateServerNameUniqueness(const ServerConfig &serverConfig);
  void validateNoDuplicateDirectives(const ServerConfig &serverConfig);
  void validateNoDuplicateLocations(const ServerConfig &serverConfig);
  // Helper function to validate if a string is a valid port number
  bool isValidPort(const std::string &value);
  bool isValidRootPath(const std::string &value);
  bool isValidServerName(const std::string &value);
  // Specific checks for certain directives
  bool checkListenDirective(const Directive &directive);
  bool checkRootDirective(const Directive &directive);
  bool checkServerNameDirective(const Directive &directive);
  // Helper function to report invalid directives
  void reportInvalidDirective(const Directive &directive, const std::string &message);
  void reportError(const Span &span, const std::string &message);
};

#endif