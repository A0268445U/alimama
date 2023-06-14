#include"node_tcp_client.h"

#define PORT 8080

std::string sendTCP(int nodeId, std::string message) {
    std::string re("");

    std::string serverAddress = queryNodeAddress(nodeId);
    size_t pos = serverAddress.find(":");  // ���ҷָ�����λ��
    if (pos != std::string::npos) {
        std::string prefix = serverAddress.substr(0, pos);  // ��ȡǰ�沿��
        std::cout << "Prefix: " << prefix << std::endl;
        serverAddress = prefix;
    }
    else {
        std::cout << "Separator not found" << std::endl;
    }

    std::cout << serverAddress << std::endl;
    const char* server = serverAddress.c_str();

    // ������������ַ
    struct hostent* he;
    if ((he = gethostbyname(server)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    // ���� socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // ���÷�������ַ�Ͷ˿�
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *((struct in_addr*)he->h_addr);

    // ���ӷ�����
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    const char* data = message.c_str();
    int size = message.size();

    int ret = send(sockfd, data, size, 0);
    if (ret == -1) {
        perror("send");
        close(sockfd);
        return "";
    }

    char buffer[1024];
    while (true) {
        ret = recv(sockfd, buffer, sizeof(buffer), 0);
        if (ret == -1) {
            perror("recv");
            close(sockfd);
            return "";
        }
        else if (ret == 0) {
            break;
        }
        buffer[ret] = '\0';
        re = re + buffer;
    }

    // �ر� socket
    close(sockfd);

    return re;
}

// ��nodeId����Ҫģ���reques����
//int main(int argc, char** argv)
//{
//    loadETCDAddress();
//    int nodeId = atoi(argv[1]);
//    std::string message = "1 1 0 16 *$* 2 2 0 16 *$* 3 3 0 16 *$* 4 4 0 16 *$* 5 5 0 16 *$* 6 6 0 16 *$* 7 7 0 16 *$* 8 8 0 16 *$* 9 9 0 16 *$* 10 10 0 16 *$* 11 11 0 16";
//    if (argc == 3)
//        message = argv[2];
//    std::cout << sendTCP(nodeId, message) << std::endl;
//    return 0;
//}