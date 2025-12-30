cmake_minimum_required(VERSION 3.19.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(trigger_log_queue_compare VERSION 1.0.0 LANGUAGES CXX)

enable_testing()
set(GFLAGS_USE_TARGET_NAMESPACE ON)
find_package(gflags CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS lockfree)
# 用vcpkg推荐的方法导入protobuf库
find_package(protobuf CONFIG REQUIRED)
# 但是为了使用 protobuf_generate_cpp 函数, 也加载 CMake 自带的模块作为补充
if(NOT COMMAND protobuf_generate_cpp)
    find_package(Protobuf REQUIRED)
endif()
# set(protobuf_USE_EXTERNAL_GLOG ON)
find_package(absl CONFIG REQUIRED)
find_package(nanobench CONFIG REQUIRED)

set(PROTO_FILES 
    proto/component.proto
    proto/field.proto
    proto/risk_log.proto
    proto/risk_settings.proto
    proto/trade_event.proto
)
# set_source_files_properties(${PROTO_FILES} PROPERTIES 
#     PROTOBUF_IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
# )
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

set(CMS_SRCS
    cms/RiskTriggerLogThread.cpp
)

set(SRCS
    src/EpochBarrier.cpp
)

add_executable(
    trigger_log_queue_compare
    main.cpp
    threadbase.cpp
    ${PROTO_SRCS}
    ${CMS_SRCS}
    # ${SRCS}
)

message(STATUS "bin dir is ${CMAKE_CURRENT_BINARY_DIR}")

target_include_directories(
    trigger_log_queue_compare
    PRIVATE
    include
    cms
    ks
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(trigger_log_queue_compare PRIVATE
    gflags::gflags
    GTest::gtest GTest::gtest_main
    GTest::gmock GTest::gmock_main
    glog::glog
    Boost::lockfree
    protobuf::libprotobuf
    absl::log          
    absl::log_initialize
    absl::strings       
    nanobench::nanobench
)
