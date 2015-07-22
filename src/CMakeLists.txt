set(TARGET webcam_capture)

# set debug define if in Debug mode
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions(-DWEBCAM_CAPTURE_DEBUG)
endif()

option(BUILD_STATIC "Build the library as static library" OFF)
if (BUILD_STATIC)
  set(LIBRARY_TYPE "STATIC")
  message(STATUS "Building static library")
else()
  set(LIBRARY_TYPE "SHARED")
  message(STATUS "Building shared library")
endif()

# backend options

if (WIN32)
  #shared winapi functional
  aux_source_directory(winapi_shared WINAPI_SHARED_SRC_LIST)
  file(GLOB WINAPI_SHARED_INCLUDE_LIST winapi_shared/*.h)
  set(BACKEND_SRC_LIST ${BACKEND_SRC_LIST} ${WINAPI_SHARED_SRC_LIST} ${WINAPI_SHARED_INCLUDE_LIST})

  message(STATUS "Direct Show backend...")
  option(BACKEND_DIRECT_SHOW "Build with Direct Show backend support" OFF)
  if (BACKEND_DIRECT_SHOW)
    set(LIBS ${LIBS}
      strmiids
      wmcodecdspuuid
    )

    add_definitions(-DWEBCAM_CAPTURE_BACKEND_DIRECT_SHOW)

    aux_source_directory(direct_show DIRECT_SHOW_SRC_LIST)
    file(GLOB DIRECT_SHOW_INCLUDE_LIST direct_show/*.h)
    set(BACKEND_SRC_LIST ${BACKEND_SRC_LIST} ${DIRECT_SHOW_SRC_LIST} ${DIRECT_SHOW_INCLUDE_LIST})

    message(STATUS "...ENABLED")
  else()
    message(STATUS "...DISABLED")
  endif()

  message(STATUS "Media Foundation backend...")
  option(BACKEND_MEDIA_FOUNDATION "Build with Media Foundation backend support" OFF)
  if (BACKEND_MEDIA_FOUNDATION)
    set(LIBS ${LIBS}
      mf
      mfplat
      mfreadwrite
      mfuuid
      shlwapi
    )

    add_definitions(-DWEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION)

    aux_source_directory(media_foundation MEDIA_FOUNDATION_SRC_LIST)
    file(GLOB MEDIA_FOUNDATION_INCLUDE_LIST media_foundation/*.h)
    set(BACKEND_SRC_LIST ${BACKEND_SRC_LIST} ${MEDIA_FOUNDATION_SRC_LIST} ${MEDIA_FOUNDATION_INCLUDE_LIST})

    message(STATUS "...ENABLED")
  else()
    message(STATUS "...DISABLED")
  endif()

endif()

# add new backends here
# please follow the same output pattern as well as option and define naming patterns

if (NOT LIBS)
  message(FATAL_ERROR "You are building the library with no backends enabled, which doesn't make sense. Please enable at least one backend. Use cmake -LH to get a list of backends.")
endif()

include (GenerateExportHeader)
add_compiler_export_flags()

aux_source_directory(. SRC_LIST)

# get a list of public headers for them to appear in Qt Creator
# also used for installation
file(GLOB INCLUDE_LIST ${CMAKE_SOURCE_DIR}/include/*.h)

add_library(${TARGET} ${LIBRARY_TYPE} ${SRC_LIST} ${INCLUDE_LIST} ${BACKEND_SRC_LIST})
target_link_libraries(${TARGET} ${LIBS})

generate_export_header(${TARGET})
set(INCLUDE_LIST ${INCLUDE_LIST} ${PROJECT_BINARY_DIR}/src/${TARGET}_export.h)

# make public headers globally available
target_include_directories(${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/src)


if (WIN32)
  set(WINDOWS_TARGET_OS "NONE" CACHE STRING "Target OS: WindowsXP, WindowsVista, Windows7 or Windows8")

  if (${WINDOWS_TARGET_OS} STREQUAL "NONE")
    message(FATAL_ERROR "WINDOWS_TARGET_OS is not set.")
  endif()
  if ((NOT (${WINDOWS_TARGET_OS} STREQUAL "WindowsXP")) AND
      (NOT (${WINDOWS_TARGET_OS} STREQUAL "WindowsVista")) AND
      (NOT (${WINDOWS_TARGET_OS} STREQUAL "Windows7")) AND
      (NOT (${WINDOWS_TARGET_OS} STREQUAL "Windows8"))
     )
    message(FATAL_ERROR "WINDOWS_TARGET_OS is set to an invalid value '${WINDOWS_TARGET_OS}'.")
  endif()

  set(WINDOWS_TARGET_ARCH "NONE" CACHE STRING "Target architecture: x86, x64 or ARM. ARM is available for WINDOWS_TARGET_OS=Windows8 only.")

  if (${WINDOWS_TARGET_ARCH} STREQUAL "NONE")
    message(FATAL_ERROR "WINDOWS_TARGET_ARCH is not set.")
  endif()
  if ((NOT (${WINDOWS_TARGET_ARCH} STREQUAL "x86")) AND
      (NOT (${WINDOWS_TARGET_ARCH} STREQUAL "x64")) AND
      (NOT (${WINDOWS_TARGET_ARCH} STREQUAL "ARM"))
     )
    message(FATAL_ERROR "WINDOWS_TARGET_ARCH is set to an invalid value '${WINDOWS_TARGET_ARCH}'.")
  endif()
  if ((${WINDOWS_TARGET_ARCH} STREQUAL "ARM") AND (NOT (${WINDOWS_TARGET_OS} STREQUAL "Windows8")))
    message(FATAL_ERROR "WINDOWS_TARGET_ARCH=ARM can be used only with WINDOWS_TARGET_OS=Windows8.")
  endif()

  # all possible error checking for WINDOWS_TARGET_OS and WINDOWS_TARGET_ARCH is done above, so don't do any here
  # values come from:
  #   SUBSYSTEM:WINDOWS: https://msdn.microsoft.com/en-us/library/fcc1zstk.aspx
  #                      https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions
  #   WINVER, _WIN32_WINNT: https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx#macros_for_conditional_declarations
  #   _USING_V110_SDK71_: http://blogs.msdn.com/b/vcblog/archive/2012/10/08/windows-xp-targeting-with-c-in-visual-studio-2012.aspx
  if (${WINDOWS_TARGET_OS} STREQUAL "WindowsXP")
    add_definitions(-D_USING_V110_SDK71_)
    add_definitions(-D_USING_V120_SDK71_)

    if (${WINDOWS_TARGET_ARCH} STREQUAL "x86")
      set(WINVER    "_WIN32_WINNT_WINXP")
      set(SUBSYSTEM "5.01")
    elseif(${WINDOWS_TARGET_ARCH} STREQUAL "x64")
      set(WINVER    "_WIN32_WINNT_WS03")
      set(SUBSYSTEM "5.02")
    endif()
  elseif (${WINDOWS_TARGET_OS} STREQUAL "WindowsVista")
    set(WINVER    "_WIN32_WINNT_VISTA")
    set(SUBSYSTEM "6.00")
  elseif (${WINDOWS_TARGET_OS} STREQUAL "Windows7")
    set(WINVER    "_WIN32_WINNT_WIN7")
    set(SUBSYSTEM "6.00")
  elseif (${WINDOWS_TARGET_OS} STREQUAL "Windows8")
    set(WINVER    "_WIN32_WINNT_WIN8")

    if (${WINDOWS_TARGET_ARCH} STREQUAL "ARM")
      set(SUBSYSTEM "6.02")
    else()
      set(SUBSYSTEM "6.00")
    endif()
  endif()

  set_target_properties(${TARGET} PROPERTIES
        COMPILE_DEFINITIONS "WINVER=${WINVER};_WIN32_WINNT=${WINVER}"
        LINK_FLAGS          "/SUBSYSTEM:WINDOWS,${SUBSYSTEM}"
  )
endif()

# set a list of public headers for the further installation of them
set_target_properties(${TARGET} PROPERTIES PUBLIC_HEADER "${INCLUDE_LIST}")

INSTALL(TARGETS ${TARGET}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/${TARGET}
)
