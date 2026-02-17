# Webserv

### A high-performance, non-blocking HTTP/1.1 Web Server built from scratch in C++98.

Webserv is a custom-built web server designed to handle high-concurrency connections using an event-driven architecture. Developed according to the strict C++98 standard, it focuses on efficient I/O multiplexing and a robust, Nginx-inspired configuration system.

## Features


- Non-blocking I/O Multiplexing: Utilizes epoll() to manage thousands of concurrent connections within a single-threaded event loop, ensuring zero blocking during data transmission.

- Nginx-style Configuration: Features a custom-built parser that processes complex configuration files, supported by a Robust ErrorReporter for precise syntax diagnostics.
