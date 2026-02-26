#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    char hostname[256];
    gethostname(hostname, sizeof(hostname));

    std::cout << "Server listening on port 8080 (hostname: " << hostname << ")\n";

    while (true) {
        int client_fd = accept(server_fd, NULL, NULL);

        char buffer[1024];
        read(client_fd, buffer, 1024);

        std::string body = "Hello from " + std::string(hostname) + "\n";

        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;

        send(client_fd, response.c_str(), response.size(), 0);
        close(client_fd);
    }

    return 0;
}
