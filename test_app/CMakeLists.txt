set(TARGET test_app)

if (POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif (POLICY CMP0020)

if (POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif (POLICY CMP0043)

# on Windows we need user to specify the path to Qt5
if (WIN32)
  set(TEST_APP_WINDOWS_QT5_PATH "NONE" CACHE PATH "Path to Qt5 directory in which bin, lib and include subdirs reside")
  
  if (${TEST_APP_WINDOWS_QT5_PATH} STREQUAL "NONE")
    message(FATAL_ERROR "The TEST_APP_WINDOWS_QT5_PATH cmake variable was not set. Please set it to the path of Qt5 directory in which bin, lib and include subdirs reside")
  else()
    set(CMAKE_PREFIX_PATH ${TEST_APP_WINDOWS_QT5_PATH})
    message(STATUS "Using TEST_APP_WINDOWS_QT5_PATH: ${TEST_APP_WINDOWS_QT5_PATH}")
    
    # make sure the user gave us the correct path
    if (NOT ((EXISTS "${TEST_APP_WINDOWS_QT5_PATH}/bin/") OR
             (EXISTS "${TEST_APP_WINDOWS_QT5_PATH}/lib/") OR
             (EXISTS "${TEST_APP_WINDOWS_QT5_PATH}/include/")
            )
        )
      message(FATAL_ERROR "TEST_APP_WINDOWS_QT5_PATH seems to be set incorrectly. Can't find bin, lib and include subdirs in ${TEST_APP_WINDOWS_QT5_PATH}, i.e. ${TEST_APP_WINDOWS_QT5_PATH}/bin/, ${TEST_APP_WINDOWS_QT5_PATH}/lib/ and ${TEST_APP_WINDOWS_QT5_PATH}/include/ don't exist")
    endif()
    
  endif()

endif()

find_package(Qt5 5.2.0 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

aux_source_directory(. SRC_LIST)

file(GLOB UI_LIST *.ui)

# generate ui_<name>.h files out of <name>.ui files
qt5_wrap_ui(WRAPPED_UI_LIST ${UI_LIST})

if (WIN32)
  # prevent console window from appearing along with the Qt GUI on Windows 
  set(EXECUTABLE_TYPE WIN32)
endif()

add_executable(${TARGET} ${EXECUTABLE_TYPE} ${WRAPPED_UI_LIST} ${SRC_LIST})

qt5_use_modules(${TARGET} Core Gui Widgets)

target_link_libraries(${TARGET} webcam_capture)

INSTALL(TARGETS ${TARGET}
  RUNTIME DESTINATION bin
)
