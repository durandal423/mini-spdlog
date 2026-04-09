FROM mcr.microsoft.com/devcontainers/cpp:bookworm

ARG HTTP_PROXY
ARG HTTPS_PROXY
ARG NO_PROXY
ARG CMAKE_VERSION=3.31.11

ENV HTTP_PROXY=${HTTP_PROXY}
ENV HTTPS_PROXY=${HTTPS_PROXY}
ENV NO_PROXY=${NO_PROXY}
ENV http_proxy=${HTTP_PROXY}
ENV https_proxy=${HTTPS_PROXY}
ENV no_proxy=${NO_PROXY}
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

RUN apt-get update

RUN apt-get install -y --no-install-recommends \
    ca-certificates \
    wget \
    ninja-build \
    clangd \
    pkg-config \
    sccache

RUN arch="$(dpkg --print-architecture)" && \
    case "$arch" in \
        amd64) cmake_arch="linux-x86_64" ;; \
        arm64) cmake_arch="linux-aarch64" ;; \
        *) echo "Unsupported architecture: $arch" >&2; exit 1 ;; \
    esac && \
    wget -q "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-${cmake_arch}.sh" \
        -O /tmp/cmake-installer.sh && \
    sh /tmp/cmake-installer.sh --skip-license --prefix=/usr/local && \
    rm -f /tmp/cmake-installer.sh

RUN cmake --version

RUN rm -rf /var/lib/apt/lists/*