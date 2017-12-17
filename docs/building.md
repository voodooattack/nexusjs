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
cd nexusjs 
git submodule update --init --recursive
```

If you're facing problems, try:

```
git submodule sync
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
