# Prepare base image
FROM ubuntu:18.10
MAINTAINER Abdullah Ali <voodooattack@hotmail.com>

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get --yes --no-install-recommends install git-all build-essential cmake libboost-all-dev curl \
  libcurl4-openssl-dev ninja-build g++ libicu-dev ruby perl xxd libicu60 subversion

COPY . /src

RUN mkdir /build

WORKDIR /build

RUN cmake /src -DCMAKE_BUILD_TYPE="Release" -G "Ninja" -DENABLE_STATIC_JSC=ON -DENABLE_WEBASSEMBLY=ON -DCMAKE_INSTALL_PREFIX=/usr \
  && ninja install

WORKDIR /

RUN apt-get --yes remove git-all cmake libboost-all-dev automake \
      libcurl4-openssl-dev ninja-build g++ libicu-dev ruby perl xxd subversion && \
  apt-get clean autoclean && \
  apt-get autoremove --yes && \
  rm -rf /var/lib/{apt,dpkg,cache,log}/ /src /build


