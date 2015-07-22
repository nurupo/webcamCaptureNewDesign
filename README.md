#Webcam Capture
Library aimed to provide a unified interface to video capture across different platforms.

##Project Goals

###Functionality
Library should allow a library user to do the following:

- Get a list of available backends
- Specify which backend to use
- Use multiple backends at once
- Get a list of all connected camera devices, with their respected unique camera ids and user-friendly device names
- Specify which camera to use based on on the unique id from the mentioned list
- Use multiple cameras at once
- Capture a video stream (video mode)
- Capture a still photo (photo mode)*
- Get a list of all supported image formats of a camera (YUV, RGB, MJPEG, etc.)
- Get a list of all supported resolutions when using a certain image format for capturing
- Get a list of all supported FPS when using a certain image format and resolution for capturing
- Specify in which image format, resolution and FPS to capture
- Get captured image frames in YUV or RGB image formats, possibly through a conversion if the original format differs
- Get captured image frames in the original format
- Get notified when a camera is connected/disconnected to/from a system
- Change brightness, contrast, saturation and possibly other settings that change how a captured image looks

###Supported platforms
- Windows
  - Windows XP and newer -- DirectShow backend
  - Windows Vista (Windows 7?) and newer -- Media Foundation backend
- OS X
  - OS X 10.7 and newer -- AV Foundation backend
- Linux
  - V4L/V4L2 backend

##Building
See [INSTALL.md](INSTALL.md).

##About
This is a Google Summer of Code 2015 project for [Project Tox organization](https://www.google-melange.com/gsoc/org2/google/gsoc2015/tox).

Note that we don't expect to reach all of our goals by the end of GSoC and that some of the goals are even out of scope for the GSoC project.

This project uses [Apache 2.0](http://www.apache.org/licenses/LICENSE-2.0) licensed code from [roxlu's Video Capture library](https://github.com/roxlu/video_capture).