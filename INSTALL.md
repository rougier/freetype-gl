# Install

## Ubuntu 15.10

The procedure is detailed in the [.travis.yml](.travis.yml).

If you want to reuse distribution packages as much as possible,
you need at the very least to:

-   compile and install AntTweakBar from source.

    There is no Ubuntu package for it, and that project is marked as unmaintained.

-   compile and install GLFW from source because of a Debian packaging bug with CMake:
    <https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=812853>

-   apply the fix at
    <https://github.com/rougier/freetype-gl/blob/a4cfb9abac19a0ab62b625a9b6f856e032fe3732/.travis.yml#L23>
    to the installed GLFW files

Then:

    mkdir build
    cd build
    cmake ..
    make

You can then run some demos under:

    cd demos
