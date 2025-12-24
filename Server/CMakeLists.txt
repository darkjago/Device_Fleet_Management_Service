# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.15)
# Define the project name
project(FleetManagement)
# Specify the C++ standard to use
set(CMAKE_CXX_STANDARD 17)

# Protobuf
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${protobuf_VERSION}")

# Protobuf-compiler
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

# gRPC
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)


#Proto file
get_filename_component(HW_PROTO "../service.proto" ABSOLUTE)
get_filename_component(HW_PROTO_PATH "${HW_PROTO}" PATH)
message(STATUS "path hw proto ${HW_PROTO}")
message(STATUS "path ${HW_PROTO_PATH}")

#Generated files path
set(PROTO_SRC "${CMAKE_CURRENT_BINARY_DIR}/service.pb.cc")
set(GRPC_SRC "${CMAKE_CURRENT_BINARY_DIR}/service.grpc.pb.cc")

#protoc command
add_custom_command(
    OUTPUT "${PROTO_SRC}" "${GRPC_SRC}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
         -I "${CMAKE_CURRENT_SOURCE_DIR}" "${HW_PROTO}"
         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
         --proto_path="${HW_PROTO_PATH}"
    DEPENDS "${HW_PROTO}"
)

#Generated headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

#Server executable
add_executable(fleet_management server_main.cpp ${PROTO_SRC} ${GRPC_SRC})
target_link_libraries(fleet_management gRPC::grpc++ protobuf::libprotobuf)
