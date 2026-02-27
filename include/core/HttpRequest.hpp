#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>
#include <set>

enum HttpParseState
{
  PARSE_REQUEST_LINE,
  PARSE_HEADERS,
  PARSE_RESOLVE_SERVER,
  PARSE_BODY,
  PARSE_SUCCESS,
  PARSE_ERROR
};

class HttpRequest
{
private:
  HttpParseState state;
  std::string buffer;
  std::string method;
  std::string path;
  std::string version;
  std::map<std::string, std::string> queryParams;
  std::map<std::string, std::string> headers;
  std::string body;

  std::set<std::string> allowedMethods;

  void parseRequestLine();
  void parseHeaders();
  void parseBody();
  void parseUri(std::string uri);
  bool isMethodAllowed(const std::string &method) const;

public:
  HttpRequest();
  ~HttpRequest();
  HttpParseState getState() const;
  void setState(HttpParseState newState);
  void appendData(const char *data, size_t length);
  void parse();
  void print() const;
  std::string getMethod() const { return method; }
  std::string getPath() const { return path; }
  std::string getVersion() const { return version; }
  std::map<std::string, std::string> getHeaders() const { return headers; }
  std::string getHeader(const std::string &name) const;
  std::string getBody() const { return body; }
  class RequestLineTooLongException : public std::exception
  {
    const char *what() const throw()
    {
      return "Request line too long";
    }
  };

  class HeadersTooLongException : public std::exception
  {
    const char *what() const throw()
    {
      return "Headers too long";
    }
  };
};

#endif