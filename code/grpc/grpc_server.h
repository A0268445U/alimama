#ifndef SERVER_IMPL_H
#define SERVER_IMPL_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include<vector>
#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "alimama.grpc.pb.h"
#include "../etcd/etcd_register.h"
#include "../tcp/node_tcp_client.h"
#include "../model_slice_reader/dataLoader.h"

using alimama::proto::ModelService;
using alimama::proto::Request;
using alimama::proto::Response;
using alimama::proto::SliceRequest;

class ServiceImpl final : public ModelService::Service{
 public:
  ServiceImpl(int id);
 private:
  Status Get(ServerContext* context, const Request* request,
             Response* reply) override;
  int nodeID;
};

void RunGrpc(int id);
#endif // SERVER_IMPL_H