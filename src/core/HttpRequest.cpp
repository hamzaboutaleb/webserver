#include "core/HttpRequest.hpp"
#include "utils/Number.hpp"
#include "utils/String.hpp"
#include <iostream>

HttpRequest::HttpRequest() : state(PARSE_REQUEST_LINE), buffer("") {
  allowedMethods.insert("GET");
  allowedMethods.insert("POST");
  allowedMethods.insert("DELETE");
}

HttpRequest::~HttpRequest() {}

void HttpRequest::appendData(const char *data, size_t length) {
  buffer.append(data, length);
}

HttpParseState HttpRequest::getState() const { return state; }

void HttpRequest::parse() {
  if (state == PARSE_SUCCESS || state == PARSE_ERROR)
    return;

  if (state == PARSE_REQUEST_LINE) {
    parseRequestLine();
    if (state == PARSE_REQUEST_LINE)
      return;
  }

  if (state == PARSE_HEADERS) {
    parseHeaders();
    if (state == PARSE_HEADERS)
      return;
  }

  if (state == PARSE_PROCESS_HEADERS)
    return; // resolve outside of this class

  if (state == PARSE_BODY) {
    parseBody();
  }
}
void HttpRequest::parseRequestLine() {
  std::size_t pos = buffer.find("\r\n");
  if (pos == std::string::npos) {
    if (buffer.size() > 8192) {
      state = PARSE_ERROR;
      return;
    }
    return;
  }
  std::string requestLine = buffer.substr(0, pos);
  buffer.erase(0, pos + 2);

  std::size_t methodEnd = requestLine.find(' ');
  if (methodEnd == std::string::npos) {
    state = PARSE_ERROR;
    return;
  }
  method = requestLine.substr(0, methodEnd);

  if (!isMethodAllowed(method)) {
    state = PARSE_ERROR;
    return;
  }

  std::size_t uriEnd = requestLine.find(' ', methodEnd + 1);
  if (uriEnd == std::string::npos) {
    state = PARSE_ERROR;
    return;
  }
  std::string uri = requestLine.substr(methodEnd + 1, uriEnd - methodEnd - 1);
  parseUri(uri);
  version = requestLine.substr(uriEnd + 1);
  state = PARSE_HEADERS;
}

void HttpRequest::parseUri(std::string uri) {
  std::size_t queryPos = uri.find('?');
  if (queryPos == std::string::npos) {
    path = uri;
    return;
  }
  path = uri.substr(0, queryPos);
  std::string queryString = uri.substr(queryPos + 1);
  std::size_t pos = 0;
  while ((pos = queryString.find('&')) != std::string::npos) {
    std::string param = queryString.substr(0, pos);
    queryString.erase(0, pos + 1);
    std::size_t equalPos = param.find('=');
    if (equalPos != std::string::npos) {
      std::string name = param.substr(0, equalPos);
      std::string value = param.substr(equalPos + 1);
      queryParams[name] = value;
    }
  }
  if (!queryString.empty()) {
    std::size_t equalPos = queryString.find('=');
    if (equalPos != std::string::npos) {
      std::string name = queryString.substr(0, equalPos);
      std::string value = queryString.substr(equalPos + 1);
      queryParams[name] = value;
    }
  }
}
// TODO: after parsing the request line, we should resolve server based on Host
// header and then resolve the URI based on the server's root and the request
// URI
void HttpRequest::parseHeaders() {
  std::size_t pos;
  while ((pos = buffer.find("\r\n")) != std::string::npos) {
    std::string line = buffer.substr(0, pos);
    buffer.erase(0, pos + 2);

    if (line.empty()) {
      state = PARSE_PROCESS_HEADERS;
      return;
    }

    std::size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
      state = PARSE_ERROR;
      return;
    }

    std::string name = String::toLower(line.substr(0, colonPos));
    std::string value = line.substr(colonPos + 1);
    value = String::trim(value);
    headers[name] = value;
  }

  if (buffer.size() > 65536) {
    state = PARSE_ERROR;
    return;
  }
}

void HttpRequest::parseBody() {
  if (headers.find("content-length") == headers.end()) {
    state = PARSE_SUCCESS;
    return;
  }

  std::size_t contentLength = Number::toInt(headers["content-length"]);
  if (buffer.size() < contentLength) {
    return;
  }
  body = buffer.substr(0, contentLength);
  buffer.erase(0, contentLength);
  state = PARSE_SUCCESS;
}

bool HttpRequest::isMethodAllowed(const std::string &method) const {
  return allowedMethods.count(method) > 0;
}

void HttpRequest::setState(HttpParseState newState) { state = newState; }

std::string HttpRequest::getHeader(const std::string &name) const {
  std::map<std::string, std::string>::const_iterator it = headers.find(name);
  if (it != headers.end())
    return it->second;
  return "";
}

void HttpRequest::print() const {
  std::cout << "--- HTTP Request ---" << std::endl;
  std::cout << "Method:  [" << method << "]" << std::endl;
  std::cout << "Path:     [" << path << "]" << std::endl;
  std::cout << "Query Parameters:" << std::endl;
  for (std::map<std::string, std::string>::const_iterator it =
           queryParams.begin();
       it != queryParams.end(); ++it) {
    std::cout << "  " << it->first << ": " << it->second << std::endl;
  }
  std::cout << "Version: [" << version << "]" << std::endl;
  std::cout << "Headers:" << std::endl;
  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); ++it) {
    std::cout << "  " << it->first << ": " << it->second << std::endl;
  }
  if (!body.empty()) {
    std::cout << "Body (" << body.size() << " bytes):" << std::endl;
    std::cout << body << std::endl;
  }
  std::cout << "--------------------" << std::endl;
}