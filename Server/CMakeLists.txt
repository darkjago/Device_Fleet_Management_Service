# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.10)
# Define the project name
project(FleetManagement)
# Specify the C++ standard to use
set(CMAKE_CXX_STANDARD 17)

# GRPC and Protobuf
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)

#Generated files path
set(PROTO_SRC "${CMAKE_CURRENT_BINARY_DIR/service.pb.cc}")
set(GRPC_SRC "${CMAKE_CURRENT_BINARY_DIR/service.grpc.pb.cc}")

#protoc command
add_custom_command
(
    OUTPUT "${PROTO_SRC}" "${GRPC_SRC}"
    COMMAND protobuf::protoc
    ARGS    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}" -- grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
            --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
            -I "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/service.proto"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/service.proto"
)

#Generated headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

#Server executable
add_executable(fleet_management server_main.cpp ${PROTO_SRC} ${GRPC_SRC})
target_link_libraries(fleet_server gRPC::grpc++ protobuf::libprotobuf)cmake_minimum_required(VERSION 3.10)
