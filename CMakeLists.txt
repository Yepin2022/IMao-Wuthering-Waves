cmake_minimum_required(VERSION 3.14)
project(IMao-Core CXX C CSharp)

SET(DEMO_NAME "IMao-Core")

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
    
    include_external_msproject(
        IMao-WinUI
        ${CMAKE_SOURCE_DIR}/IMao-WinUI/IMao-WinUI.csproj
    )
    
    include_external_msproject(
        IMao-WinUI.Core
        ${CMAKE_SOURCE_DIR}/IMao-WinUI.Core/IMao-WinUI.Core.csproj
    )

    add_dependencies(IMao-WinUI ${DEMO_NAME})
endif()

if(MSVC)
    add_compile_options(/source-charset:utf-8 /execution-charset:utf-8)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexec-charset=UTF-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=UTF-8")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(PADDLE_LIB "" CACHE PATH "Location of paddle libraries")
SET(OPENCV_DIR "" CACHE PATH "Location of OpenCV libraries")

macro(safe_set_static_flag)
    foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endmacro()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "" FORCE)
endif()

ADD_DEFINITIONS(-DUSE_MKL)

if(NOT DEFINED PADDLE_LIB)
  message(FATAL_ERROR "please set PADDLE_LIB with -DPADDLE_LIB=/path/paddle/lib")
endif()

if(NOT DEFINED OPENCV_DIR)
    message(FATAL_ERROR "please set OPENCV_DIR with -DOPENCV_DIR=/path/opencv")
endif()

include_directories("${PADDLE_LIB}/paddle/include")
link_directories("${PADDLE_LIB}/paddle/lib")
find_package(OpenCV REQUIRED PATHS ${OPENCV_DIR} NO_DEFAULT_PATH)
include_directories(${OpenCV_INCLUDE_DIRS})

add_definitions("/DGOOGLE_GLOG_DLL_DECL=")
add_definitions("/DUNICODE" "/D_UNICODE")
set(FLAG_OPENMP "/openmp")
set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG} /bigobj ${FLAG_OPENMP}")
set(CMAKE_C_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE} /bigobj ${FLAG_OPENMP}")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /bigobj ${FLAG_OPENMP}")
set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE} /bigobj ${FLAG_OPENMP}")

safe_set_static_flag()
add_definitions(-DSTATIC_LIB)
add_definitions(-DYAML_CPP_STATIC_DEFINE)

message("cmake c debug flags " ${CMAKE_C_FLAGS_DEBUG})
message("cmake c release flags " ${CMAKE_C_FLAGS_RELEASE})
message("cmake cxx debug flags " ${CMAKE_CXX_FLAGS_DEBUG})
message("cmake cxx release flags " ${CMAKE_CXX_FLAGS_RELEASE})

include_directories("${PADDLE_LIB}/third_party/install/protobuf/include")
include_directories("${PADDLE_LIB}/third_party/install/glog/include")
include_directories("${PADDLE_LIB}/third_party/install/gflags/include")
include_directories("${PADDLE_LIB}/third_party/install/xxhash/include")
include_directories("${PADDLE_LIB}/third_party/install/zlib/include")
include_directories("${PADDLE_LIB}/third_party/install/onnxruntime/include")
include_directories("${PADDLE_LIB}/third_party/install/paddle2onnx/include")
include_directories("${PADDLE_LIB}/third_party/install/yaml-cpp/include")
include_directories("${PADDLE_LIB}/third_party/boost")
include_directories("${PADDLE_LIB}/third_party/eigen3")
include_directories("${CMAKE_SOURCE_DIR}/IMao-Core/packages/nlohmann")
include_directories("${CMAKE_SOURCE_DIR}/IMao-Core/packages/Microsoft.Windows.ImplementationLibrary.1.0.240803.1/include")
include_directories("${CMAKE_SOURCE_DIR}/IMao-Core/packages/robmikh.common.0.0.23-beta/include")
include_directories("${CMAKE_SOURCE_DIR}/IMao-Core/paddleocr_cpp_infer")

include_directories("${PADDLE_LIB}/third_party/install/mklml/include")
set(MATH_LIB 
    ${PADDLE_LIB}/third_party/install/mklml/lib/mklml.lib
    ${PADDLE_LIB}/third_party/install/mklml/lib/libiomp5md.lib
)

set(MKLDNN_PATH "${PADDLE_LIB}/third_party/install/onednn")
if(EXISTS ${MKLDNN_PATH})
    include_directories("${MKLDNN_PATH}/include")
    set(MKLDNN_LIB ${MKLDNN_PATH}/lib/mkldnn.lib)
endif()

link_directories("${PADDLE_LIB}/third_party/install/zlib/lib")
link_directories("${PADDLE_LIB}/third_party/install/protobuf/lib")
link_directories("${PADDLE_LIB}/third_party/install/glog/lib")
link_directories("${PADDLE_LIB}/third_party/install/gflags/lib")
link_directories("${PADDLE_LIB}/third_party/install/xxhash/lib")
link_directories("${PADDLE_LIB}/third_party/install/onnxruntime/lib")
link_directories("${PADDLE_LIB}/third_party/install/paddle2onnx/lib")
link_directories("${PADDLE_LIB}/third_party/install/yaml-cpp/lib")
link_directories("${PADDLE_LIB}/paddle/lib")

set(DEPS
    ${PADDLE_LIB}/paddle/lib/paddle_inference${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${MATH_LIB} ${MKLDNN_LIB}
    glog gflags_static libprotobuf xxhash
    libcmt shlwapi
)

Set(DEPS ${DEPS} ${CMAKE_SOURCE_DIR}/IMao-Core/paddleocr_cpp_infer/lib/ppocr.lib)
Set(DEPS ${DEPS} Windowscodecs.lib windowsapp.lib dwmapi.lib d3d11.lib shlwapi.lib)
set(DEPS ${DEPS} ${OpenCV_LIBS})

if (EXISTS "${PADDLE_LIB}/third_party/install/snappy/lib")
    set(DEPS ${DEPS} snappy)
endif()
if(EXISTS "${PADDLE_LIB}/third_party/install/snappystream/lib")
    set(DEPS ${DEPS} snappystream)
endif()

if (EXISTS "${PADDLE_LIB}/third_party/install/yaml-cpp/lib")
  set(DEPS ${DEPS} yaml-cpp)
endif()

file(GLOB_RECURSE CppFiles 
    "IMao-Core/src/*.cpp"
    "IMao-Core/src/App/*.cpp"
    "IMao-Core/src/Base/*.cpp"
    "IMao-Core/src/Coordinate/*.cpp"
    "IMao-Core/src/Feature/*.cpp"
    "IMao-Core/src/ImageProcessing/*.cpp"
    "IMao-Core/src/ImguiDraw/*.cpp"
    "IMao-Core/src/WindowsCapture/*.cpp"
)

file(GLOB_RECURSE HppFiles 
    "IMao-Core/src/*.h"
    "IMao-Core/src/App/*.h"
    "IMao-Core/src/Base/*.h"
    "IMao-Core/src/Coordinate/*.h"
    "IMao-Core/src/Feature/*.h"
    "IMao-Core/src/ImageProcessing/*.h"
    "IMao-Core/src/ImguiDraw/*.h"
    "IMao-Core/src/WindowsCapture/*.h"
)

file(GLOB_RECURSE ResourceFiles
    "IMao-Core/src/*.rc"
    "IMao-Core/src/Resource/*.json"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/IMao-Core/src PREFIX "" FILES ${CppFiles} ${HppFiles} ${ResourceFiles})

add_library(${DEMO_NAME} SHARED 
    ${CppFiles} ${HppFiles} ${ResourceFiles}
)

target_compile_definitions(${DEMO_NAME} PRIVATE 
    IMAO_CORE_EXPORTS  
)

set_target_properties(${DEMO_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/x64/$<CONFIG> 
)

target_include_directories(${DEMO_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${DEMO_NAME} ${DEPS})

add_custom_command(TARGET ${DEMO_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PADDLE_LIB}/third_party/install/mklml/lib/mklml.dll ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>/mklml.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PADDLE_LIB}/third_party/install/mklml/lib/libiomp5md.dll ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>/libiomp5md.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PADDLE_LIB}/third_party/install/onednn/lib/mkldnn.dll ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>/mkldnn.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PADDLE_LIB}/paddle/lib/paddle_inference.dll ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>/paddle_inference.dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PADDLE_LIB}/paddle/lib/common.dll ${CMAKE_SOURCE_DIR}/x64/$<CONFIG>/common.dll
)