# WeeK OS Build Environment
# Derleme için gerekli tüm araçları içeren Docker imajı

FROM ubuntu:22.04

LABEL maintainer="WeeK OS Team"
LABEL description="WeeK OS build environment"

#互動 modu kapat
ENV DEBIAN_FRONTEND=noninteractive

# Gerekli paketleri kur
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    nasm \
    ld \
    binutils \
    grub-pc-bin \
    grub-common \
    grub2-common \
    xorriso \
    mtools \
    git \
    curl \
    wget \
    make \
    cmake \
    pkg-config \
    libfreetype6-dev \
    libfontconfig1-dev \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

# Cross-compilation toolchain (i686-elf)
RUN mkdir -p /opt/cross && \
    cd /opt/cross && \
    curl -LO https://ftp.gnu.org/gnu/binutils/binutils-2.41.tar.xz && \
    tar xf binutils-2.41.tar.xz && \
    mkdir build-binutils && cd build-binutils && \
    ../binutils-2.41/configure --target=i686-elf --prefix=/opt/cross --with-sysroot --disable-nls --disable-werror && \
    make -j$(nproc) && make install && \
    cd / && rm -rf /opt/cross/build-binutils /opt/cross/binutils-2.41*

ENV PATH="/opt/cross/bin:${PATH}"

# Çalışma dizini
WORKDIR /workspace

# Build script'ini kopyala
COPY build.sh /workspace/build.sh
RUN chmod +x /workspace/build.sh

# Varsayılan komut
CMD ["./build.sh"]
