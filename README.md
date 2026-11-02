# erxd

A mediocre attempt at making my own Nginx.

erxd is a small reverse proxy written in C++ with Boost.Asio. It accepts client
connections and forwards requests to configured backend servers.

It is mostly a learning project and a work in progress.

## Features

- Asynchronous TCP connection handling with Boost.Asio
- YAML-based configuration
- Backend registration with generated IDs
- Sequential backend selection
- Request and backend logging with Boost.Log
- Catch2 tests for configuration, backend selection, and HTTP forwarding

## Build

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```sh
./build/erxd config/proxy.yaml
```

If no path is supplied, erxd looks for `config/proxy.yaml` in the current
working directory.

## Configuration

```yaml
listen:
  host: 0.0.0.0
  port: 1337

backends:
  - host: 127.0.0.1
    port: 9001
  - host: 127.0.0.1
    port: 9002
```

The proxy listens on the configured port and forwards requests to the listed
backends.

## Tests

```sh
ctest --test-dir build --output-on-failure
```

## Current limitations

This is not production-ready. HTTP parsing, connection reuse, health checks,
timeouts, retries, TLS, and advanced load-balancing policies are still being
developed.
