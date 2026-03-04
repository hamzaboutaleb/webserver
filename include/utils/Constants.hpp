#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <stddef.h>

namespace Constants {
  namespace Network {
    static const int EpollMaxEvents = 1024;
    static const int EpollWaitTimeout = 1000; // ms
  }

  namespace Http {
    static const size_t MaxRequestLine = 8192;    // 8 KB
    static const size_t MaxHeaderSize = 65536;    // 64 KB
    static const size_t DefaultMaxBodySize = 1048576; // 1 MB
  }

  namespace HttpStatus {
    static const int OK = 200;
    static const int MovedPermanently = 301;
    static const int Found = 302;
    static const int SeeOther = 303;
    static const int TemporaryRedirect = 307;
    static const int PermanentRedirect = 308;
    static const int BadRequest = 400;
    static const int Unauthorized = 401;
    static const int Forbidden = 403;
    static const int NotFound = 404;
    static const int MethodNotAllowed = 405;
    static const int PayloadTooLarge = 413;
    static const int UriTooLong = 414;
    static const int RequestHeaderFieldsTooLarge = 431;
    static const int InternalServerError = 500;
    static const int NotImplemented = 501;
    static const int BadGateway = 502;
    static const int ServiceUnavailable = 503;
    static const int GatewayTimeout = 504;
  }

  namespace Buffer {
    static const size_t ReadBufferSize = 4096;
    static const size_t WriteChunkSize = 8192;
  }

  namespace Timeout {
    static const int ConnectionIdle = 60; // seconds
  }
}

#endif
