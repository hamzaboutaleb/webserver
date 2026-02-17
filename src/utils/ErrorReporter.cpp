#include "utils/ErrorReporter.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

ErrorReporter::ErrorReporter(const std::string &buffer, const std::string &filename) : buffer(buffer), filename(filename)
{
}

void ErrorReporter::report(Report report)
{
  reports.push_back(report);
}

bool ErrorReporter::hasErrors()
{
  return reports.size() > 0;
}

void ErrorReporter::printAll()
{
  for (unsigned int i = 0; i < reports.size(); i++)
  {
    Report report = reports.at(i);
    print(report);
    std::cerr << std::endl;
  }
}

std::string ErrorReporter::getline(unsigned int index)
{
  unsigned int start = index;
  unsigned int end = index;
  while (start > 0 && buffer[start - 1] != '\n')
    start--;
  while (end < buffer.size() && buffer[end] != '\n')
    end++;
  return buffer.substr(start, end - start);
}

void ErrorReporter::print(Report &report)
{
  std::ostringstream oss;
  oss << report.span.start.line;
  std::string lineNum = oss.str();

  std::string line = getline(report.span.start.index);
  std::cerr << "Error " << filename << report.span.start << ": " << report.message << std::endl;
  std::cerr << lineNum << " | " << line << std::endl;
  std::cerr << std::string(lineNum.length(), ' ') << " | "
            << std::string(report.span.start.col - 1, ' ') << "^" << std::endl;
}