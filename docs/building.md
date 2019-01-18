# Building

## Build Requirements

* [JavaScriptCore (WebKit)](https://webkit.org/)
* [CMake](https://cmake.org)
* [Boost](http://www.boost.org)
* [ICU](http://site.icu-project.org/)
* [curl](https://curl.haxx.se/libcurl/)

## Obtaining Nexus.js

Simply clone Nexus using git to your chosen directory.

```
git clone https://github.com/voodooattack/nexusjs.git nexusjs
```

## Dependencies Required

You need the following:

- CMake 3.3 (or newer)
- g++ 8
- ICU 6.0
- libcurl
- boost
- ruby and perl (for building WebKit)
- xdd (for JavaScript file embedding)

```
apt-get install build-essential cmake libboost-all-dev curl libcurl4-openssl-dev g++ \
  libicu-dev ruby perl xxd libicu60
```

## Building Nexus.js

After making sure you have all the dependencies installed, use CMake to build Nexus.js. I recommend using [CCMake](https://cmake.org/cmake/help/v3.0/manual/ccmake.1.html) (or your favourite CMake UI)
```
$ cd nexusjs/
$ mkdir build
$ cd build/
$ ccmake ..
$ make
```

### WebAssembly Support

To enable WebAssembly support, invoke the build with the following defined:

`cmake -DENABLE_WEBASSEMBLY=ON`

