# CMakeLists.txt - Part of VScope, (C) Daniel Wagenaar 2025


######################################################################
cmake_minimum_required(VERSION 3.16)

######################################################################
# Set the project name
project(VScope)

set(CMAKE_PROJECT_HOMEPAGE_URL "http://danielwagenaar.net")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1) # for clangd

######################################################################
# Specify the C++ standard [must be done _before_ adding executable!]
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_POSITION_INDEPENDENT_CODE ON) # I thought this would do the trick...
#if (UNIX) 
#  add_compile_options(-fPIC) # ... but in fact, this is required on Linux
#endif()

# Qt stuff
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6
  REQUIRED COMPONENTS Widgets Xml)

######################################################################
# Figure out operating system
if(WIN32 OR CYGWIN)
  add_compile_definitions(vsdWIN32)
elseif(UNIX)
    add_compile_definitions(vsdLINUX)
endif()

######################################################################
# Add the executables
add_executable(vscope WIN32 MACOSX_BUNDLE src/toplevel/main.cpp)

include(src/CMakeLists.txt)

# Libraries to link to - Qt must be mentioned
target_link_libraries(vscope Qt6::Widgets)
target_link_libraries(vscope Qt6::Xml)

if (WIN32)
  target_link_libraries(vscope secur32.lib)
endif()
if (APPLE)
  set(MACOSX_BUNDLE_ICON_FILE eln.icns)
  add_custom_command(TARGET vscope POST_BUILD
     COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/vscope.app/Contents/Resources
     COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/src/eln.icns
        ${CMAKE_CURRENT_BINARY_DIR}/vscope.app/Contents/Resources
  )
endif()

# Produce configuration files
add_custom_target(version
  ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/src/config.h.in
                   -D DST=config.h
                   -P ${CMAKE_SOURCE_DIR}/GenerateVersion.cmake
  )
add_dependencies(vscope version)

#configure_file(src/config.h.in config.h)

# Include paths - Qt must be mentioned
target_include_directories(vscope PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(vscope PUBLIC "${Qt6_INCLUDE_DIRS}")


######################################################################
# General packaging / installation
set(CPACK_PACKAGE_VENDOR "Daniel Wagenaar")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "VScope - acquisition and analysis of VSD and electrophysiology")
set(CPACK_PACKAGE_DESCRIPTION
  "VScope is a package for acquisition and analysis of voltage-sensitive dye recordings along with electrophysiology.")
set(CPACK_PACKAGE_EXECUTABLES "vscope;VScope")

if (WIN32)
  # NSIS packaging - to be updated
  include(WinPack.CMakeLists.txt)
elseif (APPLE)
  include(MacDMG.CMakeLists.txt)
elseif (UNIX)
  include(UnixInstall.CMakeLists.txt)
  include(DebPack.CMakeLists.txt)
endif()

include(CPack)

