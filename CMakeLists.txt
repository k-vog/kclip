cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(kclip CXX)

#
# Sources
#

add_executable(kclip
  "${CMAKE_CURRENT_LIST_DIR}/kclip.cc"
)

if(APPLE)
  target_sources(kclip PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/kclip_os_macos.mm"
  )
  target_link_libraries(kclip PRIVATE
    objc
    "-framework Cocoa"
  )
endif()

#
# Compiler options
#

# C++20
target_compile_features(kclip PRIVATE cxx_std_20)

if(MSVC)
  # @@ MSVC options
  target_compile_options(kclip PRIVATE
    /W3
  )
endif()

if(NOT MSVC)
  # Not MSVC options
  target_compile_options(kclip PRIVATE
    -Wall -Wextra -Wpedantic
  )
endif()
