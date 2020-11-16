set(WITH_STATIC OFF)
set(WITH_SHARED ON)
if("${SHADOW_ROOT}" STREQUAL "")
  set(SHADOW_ROOT "$ENV{HOME}/.shadow")
endif("${SHADOW_ROOT}" STREQUAL "")
if(EXISTS "${SHADOW_ROOT}")
  message(STATUS "SHADOW_ROOT = ${SHADOW_ROOT}")
else()
  message(FATAL_ERROR "SHADOW_ROOT path does not exist: '${SHADOW_ROOT}'")
endif(EXISTS "${SHADOW_ROOT}")

set(CMAKE_MODULE_PATH "${SHADOW_ROOT}/share/cmake/Modules")
include_directories(${CMAKE_MODULE_PATH})
include(ShadowTools)
add_compile_options(-fno-inline -fno-strict-aliasing )
add_definitions(-DTESTNET=1)
add_definitions(-DITALONET_SHADOW)
include_directories(${SHADOW_ROOT}/include)
