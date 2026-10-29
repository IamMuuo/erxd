# erxd

Build and run the proxy with a YAML configuration:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
./build/erxd config/proxy.yaml
```

The configuration contains a listening port and an ordered list of backend
endpoints. The executable uses `config/proxy.yaml` when no path is supplied.
