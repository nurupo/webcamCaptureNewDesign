##Installation
- [Windows](#windows)
  - [Supported Backends](#supported-backends)
  - [Building](#building)
    - [Prerequisites](#prerequisites)
    - [CMake Options](#cmake-options)
    - [Build Instructions](#build-instructions)

###Windows

####Supported Backends
Backend | Supported systems
---|---
Media Foundation | Windows 7 and onwards
Direct Show | Windows XP and onwards

####Building
#####Prerequisites
  - CMake 2.8.11 or newer
  - Windows SDK (7.1A for targeting Windows XP, 8.1 for targeting Windows Vista and newer)
  - Visual C++ Compiler (VC) 2013 or newer
  - (Optional, for the test app only) Qt 5.2 or newer, compatible with your VC version

Building with MinGW should be possible, but wasn't attempted.
We will gladly accept a PR that adds MinGW build instructions.

#####CMake Options

Name | Description | Default
---|---|---
|BACKEND_MEDIA_FOUNDATION | Build with Media Foundation backend support. | OFF
|BACKEND_DIRECT_SHOW | Build with DirectShow backend support. | OFF
|WINDOWS_TARGET_OS | Target OS: WindowsXP, WindowsVista, Windows7 or Windows8. | "NONE"
|WINDOWS_TARGET_ARCH | Target architecture: x86, x64 or ARM. ARM is available for WINDOWS_TARGET_OS=Windows8 only. | "NONE"
|BUILD_STATIC | Build the library as a static library. When off, builds as a shared library. | OFF
|TEST_APP | Build the test application. | OFF
|TEST_APP_WINDOWS_QT5_PATH | Path to Qt5 directory in which bin, lib and include subdirs reside. | "NONE"
|CMAKE_BUILD_TYPE | Build type of the produced binaries: Release or Debug. | "Release"
|CMAKE_INSTALL_PREFIX | Path to where everything should be installed. | "C:/Program Files/webcam_capture"

(Usage: `cmake -DBACKEND_MEDIA_FOUNDATION=ON ...` or `cmake -DWINDOWS_TARGET_OS="WindowsXP" ...`)

Note that options are cached in CMakeCache.txt.

#####Build Instructions

We will show you how to build a 64-bit shared version of the library, together with the test application, targeting Windows 8 system. You can easily adjust these instructions for anything you'd like.

Get contents of this repository either by downloading and unpacking [a zip of the master branch](https://github.com/abibas/webcamCaptureNewDesign/archive/master.zip) or using git.

Start `cmd.exe` and navigate to the root directory of this project.

Create "build" directory for out-of-source build and navigate into it:
```cmd
mkdir build
cd build
```

Load VC 2013 environment variables into current `cmd.exe` session. The following line specifies to load variables for producing binaries for 64-bit platform:
```cmd
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64
```

You can get a list of all supported platforms by VC 2013 by calling:
```cmd
"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" help
```

We are targeting Windows 8 in this example, but if you are targeting Windows XP, it's important that you use Windows SDK v7.1A, since Windows SDKs after the 7th one don't support targeting Windows XP. VC 2013 defaults to using Windows SDK 8.1. In order to make it use v7.1A, we need to modify some of the variables set by `vcvarsall.bat` script **after** the script is called. To modify the variabes, execute the following:

```cmd
set INCLUDE=%ProgramFiles(x86)%\Microsoft SDKs\Windows\v7.1A\Include;%INCLUDE%
set PATH=%ProgramFiles(x86)%\Microsoft SDKs\Windows\v7.1A\Bin;%PATH%

:when targeting x86
set LIB=%ProgramFiles(x86)%\Microsoft SDKs\Windows\v7.1A\Lib;%LIB%
:when targeting x64
set LIB=%ProgramFiles(x86)%\Microsoft SDKs\Windows\v7.1A\Lib\x64;%LIB%
```

Now that the compiler-related enviroment variables are set up, we can call CMake:
```cmd
cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE="Release" -DBUILD_STATIC=OFF -DWINDOWS_TARGET_OS="Windows8" -DWINDOWS_TARGET_ARCH="x64" -DBACKEND_MEDIA_FOUNDATION=ON -DTEST_APP=ON -DTEST_APP_WINDOWS_QT5_PATH="C:/Qt/5.4/msvc2013_64_opengl" -DCMAKE_INSTALL_PREFIX:PATH=prefix ..
```

Read the output of the cmake command to see if there are any errors or warnings, even if the command exited successfully, and check that its output doesn't contradict with the options you set (i.e. you set "Release" build type, but cmake printed out "Debug"). Fix any issue there is. You might want to delete the `CMakeCache.txt` file for new option to work, if you are re-running cmake with different options.

See [CMake Options](#cmake-options) for available CMake options on Windows.

After cmake is happy with your options and you are happy with its output, build the library and the test application:
```cmd
nmake
```

If no issues araise from building, install the library, public header files and the test application into the "build/prefix":
```cmd
nmake install
```

Lastly, open up the installation directory in Explorer:
```cmd
cd prefix
explorer.exe .
```
