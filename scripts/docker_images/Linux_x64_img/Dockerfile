FROM ubuntu:22.04

#stuff that is available in the github default linux runner
RUN apt-get update && apt-get install -y \
    git \
    curl \
    ca-certificates \
    unzip \
    openssh-client \
    git-lfs \
    nodejs

#note to self: this probably has a lot of useless stuff but getting SFML to compile is impossible
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libfreetype6-dev \
    libxext-dev \
    libudev-dev \
    libopenal-dev \
    libvorbis-dev \
    libflac-dev \
    libxcb1-dev \
    libx11-xcb-dev \
    libxcb-randr0-dev \
    libxcb-image0-dev && \
    apt-get --fix-broken install
