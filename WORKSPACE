workspace(
    name = "envoy_custom",
)

local_repository(
    name = "envoy_build_config",
    path = "envoy_build_config",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "envoy",
    sha256 = "cec9a84d5a5d16bccb6f1846a7c78f1a64f10b357d6660a5cfc60ec1bb6ad0da",
    strip_prefix = "envoy-2d0a2f67eccc741f8d093c56b1ed4ea3f1382c06",
    url = "https://github.com/envoyproxy/envoy/archive/2d0a2f67eccc741f8d093c56b1ed4ea3f1382c06.tar.gz",
)

load("@envoy//bazel:api_binding.bzl", "envoy_api_binding")

envoy_api_binding()

load("@envoy//bazel:api_repositories.bzl", "envoy_api_dependencies")

envoy_api_dependencies()

load("@envoy//bazel:repositories.bzl", "envoy_dependencies")

envoy_dependencies()

load("@envoy//bazel:repositories_extra.bzl", "envoy_dependencies_extra")

envoy_dependencies_extra()

load("@envoy//bazel:dependency_imports.bzl", "envoy_dependency_imports")

envoy_dependency_imports()

http_archive(
    name = "icu",
    build_file = "//third_party:icu.bazel",
    patches = ["//third_party:icu.icu4c-68-1.patch"],
    sha256 = "5b3cfb519c20511c1c0429b093ec16960f6a6a0d7968a9065fda393f9eba48fc",
    strip_prefix = "icu-release-68-1/icu4c",
    url = "https://github.com/unicode-org/icu/archive/release-68-1.tar.gz",
)

http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "4521794f0fba2e20f3bf15846ab5e01d5332e587e9ce81629c7f96c793bb7036",
    strip_prefix = "rules_docker-0.14.4",
    url = "https://github.com/bazelbuild/rules_docker/releases/download/v0.14.4/rules_docker-v0.14.4.tar.gz",
)

load("@io_bazel_rules_docker//toolchains/docker:toolchain.bzl", docker_toolchain_configure = "toolchain_configure")

# Override the default docker toolchain configuration.
docker_toolchain_configure(
    name = "docker_config",
    client_config = "~/.docker/config.json",
    docker_flags = [
        "--log-level=info",
    ],
    docker_path = "/usr/bin/podman",
)

load("@io_bazel_rules_docker//repositories:repositories.bzl", container_repositories = "repositories")

container_repositories()

load("@io_bazel_rules_docker//repositories:deps.bzl", container_deps = "deps")

container_deps()

load("@io_bazel_rules_docker//repositories:pip_repositories.bzl", "pip_deps")

pip_deps()

load("@io_bazel_rules_docker//container:container.bzl", "container_pull")

container_pull(
    name = "alpine",
    digest = "sha256:81e3913585994746c189ad123b7682eef0d71ca10cbebbca4e10850049f93cde",
    registry = "index.docker.io",
    repository = "frolvlad/alpine-glibc",
    tag = "alpine-3.12",
)
