#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <vector>
#include "utils/Report.hpp"

class ErrorReporter
{
private:
  std::vector<Report> reports;
  const std::string &buffer;
  std::string &filename;

  void print(Report &report);

public:
  ErrorReporter(const std::string &buffer, std::string &filename);
  void report(Report report);
  bool hasErrors();
  void printAll();
};

#endif