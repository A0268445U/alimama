#include"etcd_register.h"

std::string str_url = "http://etcd:2379";
std::string allNodes[6] = { "1", "2", "3", "4", "5", "6" };
std::string allNodesAddress[6] = { "", "", "", "", "", "" };

std::string getLocalIP() {
    struct ifaddrs* ifAddrStruct = NULL;
    void* tmpAddrPtr = NULL;
    std::string localIP;
    getifaddrs(&ifAddrStruct);
    while (ifAddrStruct != NULL) {
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr = &((struct sockaddr_in*)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            std::string interfaceName(ifAddrStruct->ifa_name);
            if (interfaceName == "en0" || interfaceName == "eth0") {
                return addressBuffer;
            }
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
    return "";
}

std::string getAddress(std::string local_ip, std::string port) {
    if (local_ip.empty())
        local_ip = getLocalIP();
    std::string external_address = local_ip + std::string(":") + port;
    return external_address;
}

// Called after all six nodes have finished loading data
// for service registration
void registerService(etcd::Client& etcdClient, std::string local_ip = "", std::string port = "50000") {
    std::string external_address = getAddress(local_ip, port);
    std::string key = std::string("/services/modelservice/") + external_address;
    std::string value("");
    etcdClient.set(key, value);
    //std::cout << "register " << key << std::endl;
}

// check whether all six nodes have finished loading data
bool checkAllNodesFinish(etcd::Client& etcdClient) {
    bool allFinished = true;
    for (int i = 0; i < 6; i++)
    {
        etcd::Response response = etcdClient.get(allNodes[i]).get();
        if (response.is_ok()) {
            allNodesAddress[i] = response.value().as_string();
            continue;
        }
        else {
            allFinished = false;
            //std::cout << "query failed: " << allNodes[i] << std::endl;
            break;
        }
    }
    return allFinished;
}

// etcd Part
// Called afetr loading data
// Check whether all six nodes have finished loading data
// Register service after all six nodes have finished loading data
void etcdPart(std::string key, std::string local_ip = "", std::string port = "50000") {
    std::string external_address = getAddress(local_ip, port);
    etcd::Client etcdClient(str_url);
    etcdClient.set(key, external_address);
    std::cout << "put " << key << std::endl;

    // Query every 100ms to see if etcd has the specified key
    //bool flag = checkAllNodesFinish(etcdClient);
    //while (!flag)
    //{
    //    flag = checkAllNodesFinish(etcdClient);
    //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}

    registerService(etcdClient);
}

std::string queryNodeAddress(int nodeId) {
    if (nodeId > 6 || nodeId < 1)
        return "";
    return allNodesAddress[nodeId-1];
}

void loadETCDAddress() {
    //etcd::Client etcdClient(str_url);
    //bool flag = false;
    //while (!flag)
    //{
    //    flag = checkAllNodesFinish(etcdClient);
    //    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //}
}

//int main(int argc, char** argv) {
//    std::string nodeId(argv[1]);
//    etcdPart(nodeId);
//    return 0;
//}
