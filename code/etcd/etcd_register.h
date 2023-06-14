#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <etcd/Client.hpp>
#include <etcd/Response.hpp>
#include <string>
#include <iostream>
#include <ifaddrs.h>
#include <sys/types.h>


std::string getLocalIP();

std::string getAddress(std::string local_ip, std::string port);

void registerService(etcd::Client& etcdClient, std::string local_ip, std::string port);

bool checkAllNodesFinish(etcd::Client& etcdClient);

void etcdPart(std::string key, std::string local_ip, std::string port);

std::string queryNodeAddress(int nodeId);

void loadETCDAddress();
