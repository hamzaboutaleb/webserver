#ifndef REPORT_HPP
#define REPORT_HPP
#include "utils/Span.hpp"
#include <string>

class Report
{
public:
  Span span;
  std::string message;
  bool isWarning;
  Report(Span span, const std::string &message, bool isWarning);
};

#endif