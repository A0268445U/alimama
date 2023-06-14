#include "grpc_server.h"

std::vector<std::string> stringSplit(std::string str, std::string delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  if ((pos = str.find(delimiter)) == std::string::npos) {
    tokens.push_back(str);
    return tokens;
  }
  while ((pos = str.find(delimiter)) != std::string::npos) {
    token = str.substr(0, pos);
    tokens.push_back(token);
    str.erase(0, pos + delimiter.length());
  }
  tokens.push_back(str);
  return tokens;
}

ServiceImpl::ServiceImpl(int id){
  nodeID = id;
}

// Logic and data behind the server's behavior.
Status ServiceImpl::Get(ServerContext* context, const Request* request,
                Response* reply){
  int n = request->slice_request_size();
  std::vector<std::string> reqs(7, "");
  std::vector<std::vector<std::string> > resp(7);
  for (int i = 0; i < n; i++) {
    SliceRequest s = request->slice_request(i);
    uint64_t partition = s.slice_partition();
    uint64_t start = s.data_start();
    uint64_t len = s.data_len();

//    std::cout<<"!!!"<<partition<<" "<<start<<" "<<len<<std::endl;
    int id = partition % 6 + 1;
    reqs[id] = reqs[id] + " *$* " + std::to_string(i + 1) + " " + std::to_string(partition) + " " +
    std::to_string(start) + " " + std::to_string(len);
  }

  // " *$* 1 1 1 1 *$* 2 2 2 2"
//  for (int id = 1; id < 7; id++) {
//    reqs[id] = reqs[id].substr(5); // -> "1 1 1 1 *$* 2 2 2 2"
//    if (reqs[id].size() == 0) continue;
//    if (id != nodeID) {
//      std::string s = sendTCP(id, reqs[id]);
//      resp[id] = stringSplit(s, " *$* ");
//    }
//  }

  for (int i = 0; i < n; i++) {
    SliceRequest s = request->slice_request(i);
    uint64_t partition = s.slice_partition();
    int id = partition % 6 + 1;
//    if (id != nodeID) {
//      std::string tmp = resp[id][0];
//      reply->add_slice_data(tmp.substr(tmp.find_first_of(" ") + 1));
//      resp[id].erase(resp[id].cbegin());
//    }else {
        uint64_t start = s.data_start();
        uint64_t len = s.data_len();
        std::string tmp = read_vr(partition, start, len);
//        std::cout<<"!!!"<<i<<" "<<id<<std::endl;
//        std::cout<<"!!!"<<tmp<<std::endl;
        reply->add_slice_data(tmp);
//    }
  }
  reply->set_status(0);
  return Status::OK;
}

void RunGrpc(int id) {
  std::string localhost = getLocalIP();
  std::string server_address(localhost + ":50000");
  ServiceImpl service(id);

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}
