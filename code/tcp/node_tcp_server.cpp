#include"node_tcp_server.h"
#include "../entrance/entrance.h"

#define PORT 8080
#define BACKLOG 5

std::vector<std::string> stringSplitTCP(std::string str, std::string delimiter) {
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

std::vector<int> strSplitToInt(std::string str) {
    std::vector<int> nums;

    std::stringstream ss(str);
    std::string token;
    while (ss >> token) {
        int num = std::stoi(token);
        nums.push_back(num);
    }
    return nums;
}

void runTCPServer() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // ���� socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // ���õ�ַ����ѡ�����˿�ռ������
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // �󶨵�ַ�Ͷ˿�
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    // ��ʼ�����˿�
    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    std::cout << "Server started, listening on port " << PORT << std::endl;

    // ���ܿͻ������ӣ����ṩ����
    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
            perror("accept");
            continue;
        }

        std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) << std::endl;

        /*
        Ԥ���ͻ��˷��͹��������ݸ�ʽΪ
            1 1 10 16 *$* 2 1 40 16 ����
            ����" *$* "Ϊ�ָ�����1 1 10 16�ֱ�Ϊ��š�slice��Ƭ����ʼλ�á���ȡ���ݳ���
        Ԥ�����ض�����ݣ�����1 xxxxx����ʽ���أ�����" "Ϊ�ָ�����ǰ��Ϊ��ţ�����Ϊ����
        */
        char buf[16000];
        int len;
        std::string delimiter(" *$* ");
        if ((len = recv(client_fd, buf, sizeof(buf), 0)) > 0) {
            buf[len] = '\0';
            std::string recMsg(buf);
            std::vector<std::string> requests = stringSplitTCP(recMsg, delimiter);
            for (std::string str : requests) {
                std::vector<int> reqs = strSplitToInt(str);
                std::string resp = read_vr(reqs[1], reqs[2], reqs[3]);
                std::string prefix = std::to_string(reqs[0]) + " ";
                std::string repMsg = prefix + resp + delimiter;
                send(client_fd, repMsg.c_str(), repMsg.size(), 0);
            }
        }
        
        std::cout << "Client disconnected: " << inet_ntoa(client_addr.sin_addr) << std::endl;

        // �رտͻ�������
        close(client_fd);
    }

    // �رշ����� socket
    close(server_fd);
}

//int main(int argc, char** argv)
//{
//    runTCPServer();
//
//    return 0;
//}