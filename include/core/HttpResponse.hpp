#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>
#include <sys/types.h>

enum ResponseState
{
  RESPONSE_IDLE,
  RESPONSE_SENDING_HEADERS,
  RESPONSE_SENDING_BODY,
  RESPONSE_FINISHED
};

class HttpResponse
{
private:
  ResponseState state;
  int statusCode;
  std::map<std::string, std::string> headers;
  std::string headersBuffer;
  size_t headersSent;

  // Body source
  int fileFd;
  size_t fileSize;
  size_t bodySent;
  std::string stringBody;

public:
  HttpResponse();
  ~HttpResponse();

  void prepareFromFile(const std::string &path, int status);
  void prepareFromError(int status, const std::string &message = "");
  void prepareRedirect(int status, const std::string &url);
  void setHeader(const std::string &key, const std::string &value);

  // Streaming interface
  ResponseState getState() const;
  const std::string &getHeadersBuffer() const;
  size_t getHeadersSent() const;
  void updateHeadersSent(size_t bytes);

  int getFileFd() const;
  size_t getBodySent() const;
  size_t getFileSize() const;
  void updateBodySent(size_t bytes);

  const std::string &getStringBody() const;
  void clear();

private:
  void generateHeaders();
  std::string getStatusMessage(int code) const;
};

#endif