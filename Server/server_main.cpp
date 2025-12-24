#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdint>

#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"

//gRPC
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

//From service.proto
using fleet::DeviceManager;
using fleet::RegisterRequest;
using fleet::StatusRequest;
using fleet::InfoRequest;
using fleet::ActionRequest;
using fleet::ActionResponse;
using fleet::ActionStatusRequest;
using fleet::ActionStatusResponse;
using fleet::DeviceResponse;

struct Device
{
    std::string id;
    fleet::Status status;
};

class DeviceManagerServiceImpl final : public DeviceManager::Service
{
    private:
        std::mutex mutex_;
        std::map<std::string, Device> devices_;
        std::map<std::string, fleet::ActionState> actions_;
        std::uint32_t action_counter_ = 0U;

    public:
        Status RegisterDevice(ServerContext* context, const RegisterRequest* request, DeviceResponse* response) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            //Register device
            Device device = {request->device_id(), request->initial_status()};
            devices_[device.id] = device;

            //Fill response
            response->set_device_id(device.id);
            response->set_status(device.status);
            response->set_message("Device registered successfully.");

            return Status::OK;
        }

        Status GetDeviceInfo(ServerContext* context, const InfoRequest* request, DeviceResponse* response) override
        {
            std::lock_guard<std::mutex> lock(mutex_);

            //Check if device in memory
            if(devices_.find(request->device_id()) == devices_.end())
            {
                return Status(grpc::StatusCode::NOT_FOUND, "Device not found");
            }

            //Fill response
            const auto& device = devices_[request->device_id()];
            response->set_device_id(device.id);
            response->set_status(device.status);

            return Status::OK;
        }

        Status InitiateDeviceAction(ServerContext* context, const ActionRequest* request, ActionResponse* response) override
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::string device_id = request->device_id();

            //Check if device in memory
            if(devices_.find(device_id) == devices_.end())
            {
                return Status(grpc::StatusCode::NOT_FOUND, "Device not found");
            }

            //Create Unique Action Id
            std::string action_id = "act-" + std::to_string(++action_counter_);

            //Update states
            devices_[device_id].status = fleet::Status::UPDATING;
            actions_[action_id] = fleet::ActionState::RUNNING;

            //Simulate async update
            std::thread([this, device_id, action_id]()
            {
                std::this_thread::sleep_for(std::chrono::seconds(20)); //TODO: remove magic number

                std::lock_guard<std::mutex> lock(mutex_);
                actions_[action_id] = fleet::ActionState::COMPLETED;
                devices_[device_id].status = fleet::Status::IDLE;
                std::cout << "Action " << action_id << " for device " << device_id << " completed" << std::endl;
            }).detach();

            response->set_action_id(action_id);
            return Status::OK;
        }

        Status GetActionStatus(ServerContext* context, const ActionStatusRequest* request, ActionStatusResponse* response) override
        {
            std::lock_guard<std::mutex> lock(mutex_);

            //Search action
            if(actions_.find(request->action_id()) == actions_.end())
            {
                return Status(grpc::StatusCode::NOT_FOUND, "Action ID not found");
            }

            response->set_state(actions_[request->action_id()]);
            return Status::OK;
        }
};

void RunServer()
{
    std::string server_address("0.0.0.0:50051"); //TODO: remove hardcoded direction
    DeviceManagerServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}


int main(int argc, char** argv)
{
    RunServer();
    return 0;
}