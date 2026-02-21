# WebServ Configuration Documentation

This document describes the syntax and supported directives for the WebServ configuration file.

## General Syntax
- **Blocks**: Defined by curly braces `{ }`. Currently supports `server` and `location` blocks.
- **Directives**: Defined by a keyword followed by one or more values, ending with a semicolon `;`.
- **Comments**: Any text following a `#` is ignored until the end of the line.
- **Paths**: Absolute paths must start with `/`.

---

## Contexts
| Context | Description |
| :--- | :--- |
| **Server** | Defines a virtual server instance. |
| **Location** | Defines rules for specific URL paths within a server. |

---

## Directives Reference

### `listen`
- **Description**: Sets the IP address and port on which the server accepts requests.
- **Syntax**: `listen [interface:]port;`
- **Context**: Server only.
- **Example**: `listen 8080;` or `listen 127.0.0.1:8080;`

### `server_name`
- **Description**: Defines the domain names/identifiers for the virtual server.
- **Syntax**: `server_name name1 name2 ...;`
- **Context**: Server only.
- **Example**: `server_name example.com www.example.com;`

### `root`
- **Description**: Defines the base directory for file lookups. Must be an existing directory.
- **Syntax**: `root /absolute/path;`
- **Context**: Server, Location.

### `methods`
- **Description**: Restricts allowed HTTP methods.
- **Values**: `GET`, `POST`, `DELETE`.
- **Context**: Server, Location.
- **Example**: `methods GET POST;`

### `client_max_body_size`
- **Description**: Limits the size of the client request body. Supports units (case-insensitive).
- **Suffixes**: `K` (Kilobytes), `M` (Megabytes), `G` (Gigabytes).
- **Limit**: Maximum 2GB.
- **Context**: Server, Location.
- **Example**: `client_max_body_size 100M;`

### `index`
- **Description**: Defines default files to serve when a directory is requested.
- **Constraint**: Must be relative paths; cannot contain `..`.
- **Context**: Server, Location.
- **Example**: `index index.html index.htm;`

### `autoindex`
- **Description**: Enables or disables directory listing.
- **Values**: `on`, `off`.
- **Context**: Server, Location.

### `error_page`
- **Description**: Defines custom URI for specific HTTP error codes.
- **Syntax**: `error_page code1 [code2...] /uri;`
- **Constraint**: Codes must be between 300 and 599.
- **Context**: Server, Location.
- **Example**: `error_page 404 405 /40x.html;`

### `return`
- **Description**: Stops processing and returns a status code and/or redirect URL.
- **Syntax**: `return [status] url;`
- **Context**: Server, Location.
- **Example**: `return 301 https://newsite.com;`

### `upload_store`
- **Description**: Sets the directory where uploaded files will be saved.
- **Context**: Server, Location.

### `cgi_extension`
- **Description**: Maps a file extension to a specific CGI binary handler.
- **Syntax**: `cgi_extension .ext /path/to/binary;`
- **Constraint**: Binary must exist and be executable.
- **Context**: Server, Location.
- **Example**: `cgi_extension .py /usr/bin/python3;`

---

## Example Configuration

```nginx
server {
    listen 8080;
    server_name localhost;
    root /var/www/html;
    index index.html;
    client_max_body_size 10M;

    error_page 404 /errors/404.html;

    location / {
        methods GET;
        autoindex on;
    }

    location /scripts {
        methods GET POST;
        cgi_extension .py /usr/bin/python3;
        cgi_extension .php /usr/bin/php-cgi;
    }

    location /uploads {
        methods POST;
        upload_store /var/www/html/data;
    }
}
```
