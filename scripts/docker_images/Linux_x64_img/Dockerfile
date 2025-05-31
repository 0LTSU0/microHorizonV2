FROM ubuntu:22.04


#note to self: this probably has a lot of useless stuff but getting SFML to compile is impossible
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake \
    git \
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


# Optional: Set up a working user
RUN useradd -ms /bin/bash devuser
USER devuser
WORKDIR /home/devuser
