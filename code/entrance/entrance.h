//
// Created by Minghao Han on 2023/6/14.
//
#include"../etcd/etcd_register.h"
#include"../tcp/node_tcp_server.h"
#include"../model_slice_reader/dataLoader.h"
#include"../grpc/grpc_server.h"
#include<thread>

extern std::vector<ModelSliceReader> vr;

std::string read_vr(uint64_t slice_partition, uint64_t start, uint64_t len);

int main(int argc, char** argv);