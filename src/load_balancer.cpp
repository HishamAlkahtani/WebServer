#include <thread>
#include <vector>
#include "http.h"

// what to do about config? where to get max threads from?
// look up synchronization and mutexes CPP

// when the thread recieves the socket, what to do about it?
// btw you should really look into error codes...
// this function also needs something to report status of socket.
void workerFunction(InternetHttpSocket &socket)  {
    // do work until socket dies or stuff done, then sleep until
    // woken up from LoadBalaner (see std::condition_variable)
}

// load balancer will do this very nasty line of code to swap out the
// socket for a fresh one before waking the thread up:
// *(&socket) = newsocket;

class LoadBalancer {
    std::vector<std::thread> threads;  
    std::vector<InternetHttpSocket> clientQueue; 

    int maxThreads;

public:
    LoadBalancer(int maxThreads) : maxThreads(maxThreads) {

    }

    void run() {
        // does this need it's own thread? yes!
        // if socket queue (client queue) has items {
        //      if numthreads < max create socket and assign
        //      else look for ready thread }
        // else sleep for x amount
    }

    void enqueu_socket(InternetHttpSocket socket) {

    }

};
