cmake_minimum_required(VERSION 3.8)
project(JavaScript-Zip LANGUAGES C CXX)

add_library(JavaScript-Zip MODULE
  src/javascript_zip.cc
  zipper.cpp
  deps/miniz.c
)

set_property(TARGET JavaScript-Zip PROPERTY CXX_STANDARD 20)

target_include_directories(JavaScript-Zip PRIVATE ${CMAKE_JS_INC})

execute_process(
  COMMAND node -p "require('node-addon-api').include_dir"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
  ERROR_VARIABLE NODE_ADDON_API_ERR
  RESULT_VARIABLE NODE_ADDON_API_RC
)

string(STRIP "${NODE_ADDON_API_DIR}" NODE_ADDON_API_DIR)
target_include_directories(JavaScript-Zip PRIVATE
  "${NODE_ADDON_API_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/deps"
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_compile_definitions(JavaScript-Zip PRIVATE NAPI_VERSION=8)

target_link_libraries(JavaScript-Zip PRIVATE ${CMAKE_JS_LIB})
set_target_properties(JavaScript-Zip PROPERTIES PREFIX "" SUFFIX ".node")

if(MSVC)
  target_compile_definitions(JavaScript-Zip PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_options(JavaScript-Zip PRIVATE /EHsc)
endif()
