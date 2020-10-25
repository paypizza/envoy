# envoy

[![envoy proxy](/envoy.png "Envoy Proxy")](https://www.envoyproxy.io/)

Envoy Proxy with custom extensions

[![build status](https://badge.buildkite.com/516e0a4d526c0b76a7fb376e8e6a9662310de4be3087a2f8c1.svg?branch=master&theme=github&style=square "Build status")](https://buildkite.com/paypizza/envoy)

## Documentation

* [Official documentation](https://www.envoyproxy.io/)
* [FAQ](https://www.envoyproxy.io/docs/envoy/latest/faq/overview)

### List of extensions

#### HTTP filters

- **language**

    HTTP filter which matches the language the client is able to understand using [Unicode ICU](https://github.com/unicode-org), either based on
    an URL query parameter (optional), a cookie (optional) or an Accept-Language header ([RFC 2616 Section 14.4](https://tools.ietf.org/html/rfc2616#section-14.4)).

    Example configuration
    ```yaml
    name: envoy.filters.http.language
    typed_config:
      "@type": type.googleapis.com/envoy.extensions.filters.http.language.v3.Language
      default_language: en
      languages: [en, de, dk, es, fr, fr-CH, it, jp, ko, nl, pt, zh, zh-tw]
      query_param_name: "language"
      cookie_name: "LANGUAGE"
    ```

    ##### HTTP headers (created)

    - x-language

### Prerequisites

- [Bazel](https://bazel.build/)
- [Clang](https://clang.llvm.org/)

## Getting started

### Building the source code

It is recommended to use the prebuilt Clang+LLVM package from [LLVM](http://releases.llvm.org/download.html).
<br>
Extract the tar.xz and setup your environment

```
$ ./setup_clang.sh <PATH_TO_CLANG_LLVM_DIR>
```

Example

```
$ ./setup_clang.sh /usr
```

This will create a `clang.bazelrc` file in the root directory automatically.

Build

```sh
$ bazel build --config=clang //:envoy
```

Run

```sh
$ bazel run //:envoy -- --config-path envoy.yaml
```

##### Querying the build graph

Bazel constructs a graph out of build targets, which provides lots of useful information.<br>
Using the graphviz optional dependency, generate a program `dot` with [bazel query](https://docs.bazel.build/versions/master/query-how-to.html)

```sh
$ bazel query --output=graph '//...' | dot -Tpng > graph.png
```

### Deploying

[OCI Image Format](https://github.com/opencontainers/image-spec) is the default container image format.

Build the container image

```sh
$ bazel build --config=sizeopt --strip=always //:envoy_container_image.tar
```

Load the container image

```sh
$ podman load < bazel-bin/envoy_container_image.tar
```

List the container image

```sh
$ podman images
REPOSITORY             TAG                          IMAGE ID       CREATED          SIZE
localhost/bazel        envoy_container_image        d4bfad90e657   5 seconds ago    34.7 MB
```

Run the container

```sh
$ podman run -d --name envoy --network host --rm \
  -v "$PWD/envoy.yaml":/etc/envoy/envoy.yaml \
  localhost/bazel:envoy_container_image
```

### Testing

Integration test

```sh
$ bazel test //:echo2_integration_test //src/...
```

Binary test

```sh
$ bazel test //:envoy_binary_test
```

Regular Envoy Proxy tests (from this project)

```sh
$ bazel test @envoy//test/...
```
