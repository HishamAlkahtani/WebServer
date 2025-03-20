#include "http.hpp"
#include "WebServer.cpp"

int main()
{
    spdlog::set_level(spdlog::level::trace);
    WebServer server;
    server.start();
}

/*int main() {
    initWorkingDirectory();
    std::cout << "Server started from with root directory: " << workingDirectory << std::endl;
    ServerSocket socket(8080);
    InternetHttpSocket peer = socket.getConnection();
    for (int i = 0; i <= 10; i++) {
        HttpRequest request = peer.recieve();

        std::cout << "\nMessage Recieved!\n";
        std::cout << " user wants path: " << request.getRequestedPath();

        peer.snd(createResponse(request));
    }

    std::cout << "Program terminating!";
    return 0;
}*/