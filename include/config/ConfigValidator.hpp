#ifndef CONFIG_VALIDATOR_HPP
#define CONFIG_VALIDATOR_HPP
#include "parser/ast/Config.hpp"
#include "utils/ErrorReporter.hpp"
#include "config/Context.hpp"
#include <map>
#include <string>
#include <vector>
#include <utility>

class ConfigValidator
{
private:
  typedef bool (ConfigValidator::*ValidatorFunc)(const Directive &);
  std::map<std::string, ValidatorFunc> directiveValidators;
  ErrorReporter &errorReporter;
  std::map<std::pair<std::string, std::string>, Span> usedServerNameLocationPairs;

public:
  ConfigValidator(ErrorReporter &errorReporter);
  void validate(Config &config);

private:
  const Directive *getDirective(const std::vector<Directive> &directives, const std::string &key);
  bool isDirectivePresent(const std::vector<Directive> &directives, const std::string &key);
  void validateDirective(const Directive &directive, Context context);
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
  bool isValidSizeValue(const std::string &value);
  bool isValidIP(const std::string &value);
  // Specific checks for certain directives
  bool checkListenDirective(const Directive &directive);
  bool checkRootDirective(const Directive &directive);
  bool checkServerNameDirective(const Directive &directive);
  bool checkClientMaxBodySizeDirective(const Directive &directive);
  bool checkIndexDirective(const Directive &directive);
  bool checkAutoindexDirective(const Directive &directive);
  bool checkErrorPageDirective(const Directive &directive);
  bool checkReturnDirective(const Directive &directive);
  bool checkUploadStoreDirective(const Directive &directive);
  bool checkCgiExtensionDirective(const Directive &directive);
  bool checkMethodsDirective(const Directive &directive);
  // Helper function to report invalid directives
  void reportInvalidDirective(const Directive &directive, const std::string &message);
  void reportError(const Span &span, const std::string &message);
};

#endif