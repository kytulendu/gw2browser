Building Gw2Browser
-------------------

* [Building on Windows](#building-on-windows)

    * [Required software](#required-software)

    * [Optional software](#optional-software)

    * [Required libraries](#required-libraries)

    * [Optional libraries](#optional-libraries)

    * [Getting the source code](#getting-the-source-code)

    * [Getting the required library](#getting-the-required-library)

    * Building instructions

        * [Building with Visual Studio](#building-with-visual-studio)

        * [Building with TDM-GCC](#building-with-tdm-gcc)

* [Building on Linux](#building-on-linux)

    * [Building instructions](#building-instructions-1)

* [Cross compile for Windows from Linux](#cross-compile-for-windows-from-linux)

---

## Building on Windows

### Required software

* [Git](https://git-scm.com/downloads)

* [CMake](https://cmake.org/)

* Microsoft Visual Studio 2022 or [Visual Studio Community 2022](https://www.visualstudio.com/products/visual-studio-community-vs)

  or

* [TDM-GCC](https://jmeubank.github.io/tdm-gcc/)

* [CodeBlocks](http://www.codeblocks.org/)

### Optional software

* [Cppcheck](http://cppcheck.sourceforge.net/) a static code analysis tool for the C and C++.

If using Visual Studio, also download [Visual Studio integration add-in for Cppcheck](https://github.com/VioletGiraffe/cppcheck-vs-addin/releases)

### Required libraries

* [mpg123](https://www.mpg123.de)
* [OpenAL-Soft](https://openal-soft.org/)
* [wxWidgets 3.2.11 or higher](http://wxwidgets.org/)
* [FreeType](http://www.freetype.org/) Included
* [gw2dattools](https://github.com/kytulendu/gw2dattools) Included
* [gw2formats](https://github.com/kytulendu/gw2formats) Included
* [libogg](https://github.com/xiph/ogg) Included
* [libvorbis](https://github.com/xiph/vorbis) Included
* [libwebp](https://developers.google.com/speed/webp/download) Included
* [OpenGL Mathematics](http://glm.g-truc.net/) Included
* [The OpenGL Extension Wrangler Library](http://glew.sourceforge.net/) Included
* [TinyXML2](https://github.com/leethomason/tinyxml2) Included

### Optional libraries

* [Visual Leak Detector](http://vld.codeplex.com/) (for use with Visual Studio only)

If you want to use Visual Leak Detector, remove the comment for `#include <vld.h>` in `gw2browser.cpp`.

### Getting the source code

* Crate a directory for source code some where, for example, `C:\DEV`.
* Open Git Bash window and change directory to the one you create above.
* Use this command to download the source code.

      git clone --recursive https://github.com/kytulendu/gw2browser.git

This will download gw2browser and all included library source code.

### Getting the required library

* Download mpg123 binaries from [here](https://www.mpg123.de/), choose the lastest
Win32 or Win64 file and extract it to the same directory that gw2browser directory is in.
Or download the source code and build it your self.

* Download OpenAL-Soft binaries from [here](http://kcat.strangesoft.net/openal.html),
extract it to the same directory that gw2browser directory is in. Or download the source code
and build it your self.

* Download wxWidgets source code from [here](https://github.com/wxWidgets/wxWidgets/releases),
choose `wxWidgets-3.2.11.zip` or `wxWidgets-3.2.11.7z` and extract it to the same directory
that gw2browser directory is in. Or download and use the binaries.

**Note:** The library and source code directory must be in directory like this.
The ROOT is the directory you created in Getting the source code.

        [ROOT]
         |
         +--gw2browser
         |    |
         |    +--src
         |    +--...
         |
         +--mpg123
         |
         +--openal-soft
         |    |
         |    +--bin
         |    +--include
         |    +--lib
         |    +--...
         |
         +--wxWidgets-3.2.11
              +--include
              +--src
              +--...

### Building instructions

---

### Building with Visual Studio

#### Compile wxWidgets:

* Use solution file corresponding with your VS version in directory `wxWidgets-3.2.11/build/msw`.
  For example, VS2022 is `wx_vc17.sln`, if there is no corresponding solution file for your newer VS,
  just open the highest version of solution file available.

#### Make libmpg123 library:

* Open a `x64 Native Tools Command Prompt for VS 2022` or `x86 Native Tools Command Prompt for VS 2022`
  and change it's directory to mpg123's directory.

* Rename export definition file by using this command.

      ren libmpg123-0.dll.def libmpg123-0.def

* Build the library.

  for 64 bit binary, use this command.

      lib /MACHINE:X64 /DEF:libmpg123-0.def /OUT:libmpg123-0.lib

  for 32 bit binary, use this command.

      lib /DEF:libmpg123-0.def /OUT:libmpg123-0.lib

#### Compile libwebp:

* Open `x64 Native Tools Command Prompt for VS 2022`
  If you want to compile 32 bit library, use `x86 Native Tools Command Prompt for VS 2022` instread.

* Change directory to `gw2browser/extern/libweb`

* Use this command to compile libwebp

      nmake /f Makefile.vc CFG=debug-static RTLIBCFG=dynamic OBJDIR=obj
      nmake /f Makefile.vc CFG=release-static RTLIBCFG=dynamic OBJDIR=obj

#### Compile freetype:

* Create vc2022 directory in `gw2browser/extern/freetype/builds/windows`

* Copy all content from vc2010 directory to vc2022 directory

* Open `freetype.sln` solution file in `gw2browser/extern/freetype/builds/windows/vc2022` with VS 2022

* Choose `Debug Static` or `Release Static` configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile freetype.

#### Compile tinyxml2:

* Open cmake-gui, put path of directory `gw2browser/extern/tinyxml2` to `Where is the source code:`

* Create a directory named `build-vc` in `gw2browser/extern/tinyxml2`

* Put path of directory `gw2browser/extern/tinyxml2/build-vc` to `Where to build the binaries`

* Click `Configure` button

* Choose `Visual Studio 17 2022` then click `OK`

* Click `Generate` button

* Open `tinyxml2.sln` solution file in `gw2browser/extern/tinyxml2/build-vc`

* Choose Debug or Release configuration.

* Press F7 or in MenuBar -> Build -> Build Solution to compile tinyxml2.

#### Compile gw2browser:

* Open `gw2browser.sln` solution file in `gw2browser/prj`.

* Choose Debug or Release configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile gw2browser.

#### Copy required binaries:

* Copy `soft_oal.dll` from `openal-soft/bin/Win32` or `openal-soft/bin/Win64` directory to `gw2browser/bin` and rename it to `OpenAL32.dll`

* Copy `libmpg123-0.dll` from mpg123 directory to `gw2browser/bin`

---

### Building with TDM-GCC

Make sure you have Cmake in your path.

Download TDM-GCC and install it to any location, for example `C:\TDM-GCC-64`.

You must also add TDM-GCC to your path.

for example set your path to `%path%;C:\TDM-GCC-64\bin`

#### Make libmpg123 library:

* Open `libmpg123-0.def` with any text editor, like Notepad++ and insert this to the first line.

      LIBRARY libmpg123-0.dll

  It will look some thing like this.

      LIBRARY libmpg123-0.dll
      EXPORTS
      mpg123_add_string
      ...

* Open Git Bash command line window and change directory to mpg123 directory and use these command.

      dlltool -d libmpg123-0.def -l libmpg123-0.a

#### Compile wxWidgets:

* Open Git Bash command line window and change directory to `wxWidgets-3.2.11/build/msw` and use these command.

      mingw32-make -j 4 -f makefile.gcc BUILD=debug SHARED=1

  If it give error, re-type it again.
  Wait for it to finish, then use these command.

      mingw32-make -j 4 -f makefile.gcc BUILD=release SHARED=1

  This will build wxWidgets in debug and release configuration as a dynamiclink library.
  If you want static library, remove `SHARED=1` from the commandline. But you have to change
  library search path in CodeBlocks project file from `gcc_dll` to `gcc_lib`.

      mingw32-make -j 4 -f makefile.gcc BUILD=debug
      mingw32-make -j 4 -f makefile.gcc BUILD=release

* If you need to rebuild, use "clean" target first.

      mingw32-make -j 4 -f makefile.gcc BUILD=debug SHARED=1 clean
      mingw32-make -j 4 -f makefile.gcc BUILD=release SHARED=1 clean
      mingw32-make -j 4 -f makefile.gcc BUILD=debug clean
      mingw32-make -j 4 -f makefile.gcc BUILD=release clean

#### Compile glew:

* Open Git Bash command line window and change directory to `gw2browser/extern/glew` and use these command

      cmake -G "MinGW Makefiles" ./build/cmake
      mingw32-make

#### Compile libogg:

* Open Git Bash command line window and change directory to `gw2browser/extern/libogg` and use these command

      mkdir build && cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON
      mingw32-make

#### Compile libvorbis:

* Open Git Bash command line window and change directory to `gw2browser/extern/libvorbis` and use these command

      mkdir build && cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON -DOGG_INCLUDE_DIR="D:/devel/gw2browser/extern/libogg/include" -D OGG_LIBRARY="D:/devel/gw2browser/extern/libogg/build/libogg.dll.a"
      mingw32-make

#### Compile libwebp:

* Open Git Bash command line window and change directory to `gw2browser/extern/libwebp` and use these command

      mkdir build && cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON
      mingw32-make

#### Compile freetype:

* Open Command Prompt and change directory to `gw2browser/extern/freetype` and use these command

      mkdir build && cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON
      mingw32-make

#### Compile tinyxml2:

* Open Command Prompt and change directory to `gw2browser/extern/tinyxml2` and use these command

      mkdir build && cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON
      mingw32-make

#### Compile gw2browser:

* Open workspace file `gw2browser.workspace` in `gw2browser/prj` with CodeBlocks.

* Compile gw2browser by right click and select gw2browser as active project,
  choose debug or release build, then click the gear button in toolbar
  or press CTRL+F9 key or in MenuBar -> Build -> Build.

#### Copy required binaries:

* Copy `libgomp_64-1.dll` from TDM-GCC install directory to `gw2browser/bin`

* Copy `soft_oal.dll` from `openal-soft/bin/Win32` or `openal-soft/bin/Win64` directory to `gw2browser/bin` and rename it to `OpenAL32.dll`

* Copy `libmpg123-0.dll` from mpg123 directory to `gw2browser/bin`

* Copy `glew32.dll from `gw2browser/extern/glew/bin` directory to `gw2browser/bin`

* Copy `libtinyxml2.dll` from `gw2browser/extern/tinyxml2/build` directory to `gw2browser/bin`

* Copy `libogg.dll` from `gw2browser/extern/libogg/build` directory to `gw2browser/bin`

* Copy `libvorbis.dll` and `libvorbisfile.dll` from `gw2browser/extern/libvorbis/build/lib` directory to `gw2browser/bin`

* Copy `libwebpdecoder.dll` from `gw2browser/extern/libwebp/build` directory to `gw2browser/bin`

* Copy following dll files from `wxWidgets-3.2.11/lib/gcc_dll` directory to `gw2browser/bin`

      wxbase32u_gcc_custom.dll
      wxmsw32u_aui_gcc_custom.dll
      wxmsw32u_core_gcc_custom.dll
      wxmsw32u_gl_gcc_custom

  If debug build, also copy these dll files.

      wxbase32ud_gcc_custom.dll
      wxmsw32ud_aui_gcc_custom.dll
      wxmsw32ud_core_gcc_custom.dll
      wxmsw32ud_gl_gcc_custom

---

## Building on Linux

For this guide, I was using Kubuntu and ArchLinux, but this guide can be adapt to work with any Linux distribution.

### Building instructions

#### Getting the required library and tools:

* Download wxWidgets 3.2 source code from [here](https://github.com/wxWidgets/wxWidgets/releases),
choose wxWidgets-3.2.11.tar.bz2 and extract it to somewhere, for example, your home directory.
If your Linux distribution have wxWidgets 3.2 package, you can skip this and use a command like below command to install wxWidgets 3.2.

  for Debian and it's derivative

      sudo apt install libwxbase3.2-dev libwxgtk3.2-dev

  for ArchLinux and it's derivative

      sudo pacman -S wxwidgets-common wxwidgets-gtk3

  Or download wxWidgets's repository using this command.

      git clone https://github.com/wxWidgets/wxWidgets.git

* Open a terminal window, the directory would be in your home directory.
* Install git by using this command.
  You can skip this step if you have already install git.

  for Debian and it's derivative

      sudo apt install git git-gui

  for ArchLinux and it's derivative

      sudo pacman -S git git-gui

* Use this command to install required library and tools.

  for Debian and it's derivative

      sudo apt install build-essential codeblocks cmake cmake-gui libwebp-dev libglew-dev libopenal-dev libmpg123-dev libvorbis-dev libogg-dev libfreetype6 libfreetype6-dev libtinyxml2-dev libglm-dev

  for ArchLinux and it's derivative

      sudo pacman -S gcc codeblocks cmake libwebp glew openal mpg123 libvorbis libogg freetype2 tinyxml2 glm

#### Getting the source code:

* Use this command to download the source code.

      git clone --recursive https://github.com/kytulendu/gw2browser.git

This will download gw2browser and all included library source code, although we won't use most of the library that was included in the repo.

#### Compiling wxWidgets:

* If your system have wxWidgets 3.2 package, you can skip this step.
* Open a terminal window and change directory to wxWidgets source code directory you have extracted.
* Make a directory using this command

      mkdir gtk-build

* Change directory to the directory you make previously, in this case, gtk-build directory.

      cd gtk-build

* Use this command to config the build script and wait for it to finish.

      ../configure --enable-unicode --enable-debug --with-opengl

* Use this command to compile wxWidgets source code.

      make

  or

      make -j 4

* When finished, use this command to install wxWidgets

      sudo make install

* After that use this command

      sudo ldconfig

#### Compiling gw2dattools:

* Open a terminal window and change directory to extern/gw2dattools source code directory you have extracted.
* Make a directory using this command

      mkdir build

* Change directory to the directory you make previously, in this case, build directory.

      cd build

* Use this command to and wait for it to finish.

      cmake ..

* Use this command to compile gw2dattools source code.

      make

  or

      make -j 4

* When finished, use this command to install gw2dattools

      sudo make install

#### Compiling gw2formats:

* Open a terminal window and change directory to extern/gw2formats source code directory you have extracted.
* Make a directory using this command

      mkdir build

* Change directory to the directory you make previously, in this case, build directory.

      cd build

* Use this command to and wait for it to finish.

      cmake ..

* Use this command to compile gw2formats source code.

      make

  or

      make -j 4

* When finished, use this command to install gw2formats

      sudo make install

#### Compiling gw2browser:

* Open a terminal window and change directory to gw2browser directory.
* Make a directory using this command

      mkdir build

* Change directory to the directory you make previously, in this case, build directory.

      cd build

* Use this command to and wait for it to finish.

      cmake ..

* Use this command to compile gw2browser source code.

      make

  or

      make -j 4

* When finished, use this command to install gw2browser

      sudo make install

---

## Cross compile for Windows from Linux

Todo
