
class HttpRequest {

    public:
        // Parses raw http request from C libs
        HttpRequest(void* data, size_t length) {

        }

};

class HttpResponse {

  public:

    // To be used by C libs, otherwise do not use!
    void* getData(size_t& length) {

    }
};