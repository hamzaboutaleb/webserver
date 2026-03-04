#include "core/HttpResponse.hpp"
#include "utils/MimeTypes.hpp"
#include "utils/Number.hpp"
#include "utils/Constants.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sstream>

HttpResponse::HttpResponse() : state(RESPONSE_IDLE), statusCode(Constants::HttpStatus::OK), headersSent(0), fileFd(-1), fileSize(0), bodySent(0) {}

HttpResponse::~HttpResponse()
{
  clear();
}

void HttpResponse::clear()
{
  if (fileFd != -1)
  {
    close(fileFd);
    fileFd = -1;
  }
  stringBody.clear();
  headersBuffer.clear();
  headers.clear();
  headersSent = 0;
  bodySent = 0;
  fileSize = 0;
  state = RESPONSE_IDLE;
}

void HttpResponse::prepareFromFile(const std::string &path, int status)
{
  clear();
  statusCode = status;
  fileFd = open(path.c_str(), O_RDONLY);
  if (fileFd == -1)
  {
    prepareFromError(Constants::HttpStatus::NotFound);
    return;
  }

  struct stat st;
  fstat(fileFd, &st);
  fileSize = st.st_size;

  setHeader("Content-Type", MimeTypes::getMimeType(path));
  setHeader("Content-Length", Number::toString(fileSize));
  setHeader("Connection", "keep-alive");

  generateHeaders();
  state = RESPONSE_SENDING_HEADERS;
}

void HttpResponse::prepareFromError(int status, const std::string &message)
{
  clear();
  statusCode = status;
  std::string msg = message.empty() ? getStatusMessage(status) : message;
  stringBody = "<html><body><h1>" + Number::toString(status) + " " + getStatusMessage(status) + "</h1><p>" + msg + "</p></body></html>";

  setHeader("Content-Type", "text/html");
  setHeader("Content-Length", Number::toString(stringBody.size()));
  setHeader("Connection", "close");

  generateHeaders();
  state = RESPONSE_SENDING_HEADERS;
}

void HttpResponse::prepareRedirect(int status, const std::string &urlOrBody)
{
  clear();
  statusCode = status;
  
  // If it's a redirect status code, use Location header
  if (status >= 300 && status < 400)
  {
    setHeader("Location", urlOrBody);
    setHeader("Content-Length", "0");
  }
  else
  {
    // Otherwise, treat the second argument as the response body
    stringBody = urlOrBody;
    setHeader("Content-Type", "text/plain");
    setHeader("Content-Length", Number::toString(stringBody.size()));
  }
  
  setHeader("Connection", "close");
  generateHeaders();
  state = RESPONSE_SENDING_HEADERS;
}

void HttpResponse::setHeader(const std::string &key, const std::string &value)
{
  headers[key] = value;
}

void HttpResponse::generateHeaders()
{
  std::stringstream ss;
  ss << "HTTP/1.1 " << statusCode << " " << getStatusMessage(statusCode) << "\r\n";
  for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
  {
    ss << it->first << ": " << it->second << "\r\n";
  }
  ss << "\r\n";
  headersBuffer = ss.str();
}

std::string HttpResponse::getStatusMessage(int code) const
{
  switch (code)
  {
  case Constants::HttpStatus::OK: return "OK";
  case Constants::HttpStatus::MovedPermanently: return "Moved Permanently";
  case Constants::HttpStatus::Found: return "Found";
  case Constants::HttpStatus::SeeOther: return "See Other";
  case Constants::HttpStatus::TemporaryRedirect: return "Temporary Redirect";
  case Constants::HttpStatus::PermanentRedirect: return "Permanent Redirect";
  case Constants::HttpStatus::BadRequest: return "Bad Request";
  case Constants::HttpStatus::Unauthorized: return "Unauthorized";
  case Constants::HttpStatus::Forbidden: return "Forbidden";
  case Constants::HttpStatus::NotFound: return "Not Found";
  case Constants::HttpStatus::MethodNotAllowed: return "Method Not Allowed";
  case Constants::HttpStatus::PayloadTooLarge: return "Payload Too Large";
  case Constants::HttpStatus::UriTooLong: return "URI Too Long";
  case Constants::HttpStatus::RequestHeaderFieldsTooLarge: return "Request Header Fields Too Large";
  case Constants::HttpStatus::InternalServerError: return "Internal Server Error";
  case Constants::HttpStatus::NotImplemented: return "Not Implemented";
  case Constants::HttpStatus::BadGateway: return "Bad Gateway";
  case Constants::HttpStatus::ServiceUnavailable: return "Service Unavailable";
  case Constants::HttpStatus::GatewayTimeout: return "Gateway Timeout";
  default: return "Unknown";
  }
}

ResponseState HttpResponse::getState() const { return state; }
const std::string &HttpResponse::getHeadersBuffer() const { return headersBuffer; }
size_t HttpResponse::getHeadersSent() const { return headersSent; }
void HttpResponse::updateHeadersSent(size_t bytes)
{
  headersSent += bytes;
  if (headersSent >= headersBuffer.size())
  {
    state = (fileFd != -1 || !stringBody.empty()) ? RESPONSE_SENDING_BODY : RESPONSE_FINISHED;
  }
}

int HttpResponse::getFileFd() const { return fileFd; }
size_t HttpResponse::getBodySent() const { return bodySent; }
size_t HttpResponse::getFileSize() const { return fileSize; }
void HttpResponse::updateBodySent(size_t bytes)
{
  bodySent += bytes;
  size_t total = (fileFd != -1) ? fileSize : stringBody.size();
  if (bodySent >= total)
  {
    state = RESPONSE_FINISHED;
  }
}

const std::string &HttpResponse::getStringBody() const { return stringBody; }