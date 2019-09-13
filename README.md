# NEXTGenSim
This is the NEXTGenSim scheduler simulator.
This software was developed as part of the EC H2020 funded project NEXTGenIO (Project ID: 671951)
www.nextgenio.eu

## Build instructions
Users should only build tagged releases from the master branch, others are in development and may be broken or exhibit undesired behaviour.

1. Run aclocal, autoheader, autoconf, automake --add-missing to ensure the correct autoconf files exist.
2. Make a build directory, it can be in the build tree, or outside
3. cd build
4. ../configure CXX=/usr/bin/g++ CXXFLAGS="-I../../otf2install/include -I../../otf2xxinstall/include -DOTF2XX_CHRONO_DURATION_TYPE=nanoseconds -std=gnu++14" LDFLAGS=-L../../otf2install/lib LIBS=-lotf2
5. OTF2 support requires both otf2 and otf2xx libraries
6. Incorrectly matching the OTF2XX_CHRONO_DURATION_TYPE between this and the library causes you to be 1000x out on timings.
