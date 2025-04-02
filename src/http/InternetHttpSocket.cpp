#include "http/InternetHttpSocket.hpp"
#include "Config.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

InternetHttpSocket::InternetHttpSocket(sockaddr_in &addr, int fd)
{
    sin_family = addr.sin_family;
    sin_port = addr.sin_port;
    ip = inet_ntoa(addr.sin_addr);
    connection_fd = fd;
    active = true;
    maxRequestSize = Config::getConfigs().getMaxRequestSize();
}

InternetHttpSocket::InternetHttpSocket()
{
    active = false;
    connection_fd = -1;
}

void InternetHttpSocket::closeSocket()
{
    active = false;
    close(connection_fd);
}

std::string InternetHttpSocket::getIp()
{
    return ip;
}

HttpRequest InternetHttpSocket::receive()
{
    if (!active)
    {
        return HttpRequest();
    }

    std::vector<char> receiveBuffer(2048);
    std::size_t offset = 0;

    /* Keep trying to receive until we have a valid request. If the request is not fully
       received, increase buffer size and receive the remaining part of the request. */
    while (receiveBuffer.size() < maxRequestSize)
    {
        // offset is how many bytes have been received already
        char *buffer = receiveBuffer.data() + offset;
        ssize_t length = recv(connection_fd, (void *)buffer, receiveBuffer.size() - offset, 0);

        // recv error
        if (length == -1)
        {
            std::cout << "Failed to recieve. errno: " << errno << std::endl;
            if (errno == 9)
            {
                std::cout << "File Descriptor Not Vaild! Closing socket with file descriptor: " << connection_fd << std::endl;
                closeSocket();
            }

            return HttpRequest();
        }
        // no messages available & peer has performed an orderly shutdown
        else if (length == 0)
        {
            closeSocket();
            return HttpRequest();
        }

        HttpRequest request(receiveBuffer.data());

        if (request.isGood())
            return request;

        offset += length;
        receiveBuffer.resize(receiveBuffer.size() * 2);
    }

    // If a valid HttpRequest can not be received, terminate connection.
    closeSocket();
    return HttpRequest();
}

size_t InternetHttpSocket::snd(HttpResponse *response)
{
    if (!active)
        return -1;

    std::string message = response->getData();
    return send(connection_fd, message.c_str(), message.length() * sizeof(char), 0); // is this causing the segfaults?
}

bool InternetHttpSocket::isActive()
{
    return active;
}