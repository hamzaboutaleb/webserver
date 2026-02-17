#include "utils/Report.hpp"

Report::Report(Span span, const std::string &message, bool isWarning) : span(span), message(message), isWarning(isWarning)
{
}