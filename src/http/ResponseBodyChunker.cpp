#include "http/ResponseBodyChunker.hpp"
#include "http/split.hpp"

#include <string>
#include <iomanip>
#include <filesystem>

std::string long_to_hex(long num)
{
    std::stringstream stream;
    stream << std::hex << num;
    return stream.str();
}

ResponseBodyChunker::ResponseBodyChunker(std::filesystem::path path)
    : path(path), fileStream(nullptr), lastChunk(false)
{
    chunkAvailable = std::filesystem::exists(path);
}

ResponseBodyChunker::ResponseBodyChunker()
    : path(), fileStream(nullptr), chunkAvailable(false), lastChunk(false)
{
}

ResponseBodyChunker::~ResponseBodyChunker()
{
    if (fileStream != nullptr)
    {
        fileStream->close();
        delete fileStream;
    }
}

std::string ResponseBodyChunker::getChunk()
{
    if (lastChunk)
    {
        chunkAvailable = false;
        return std::string("0") + CRLF + CRLF;
    }

    if (fileStream == nullptr)
    {
        if (path.empty())
            throw std::runtime_error("Unable to get chunk: Path is empty!");
        else
            fileStream = new std::ifstream(path);
    }

    if (!chunkAvailable)
        throw std::runtime_error("Unable to get chunk: no chunk available!");

    char *buffer = new char[CHUNK_SIZE]();

    fileStream->read(buffer, CHUNK_SIZE);
    std::string chunkBody(buffer, fileStream->gcount());

    delete[] buffer;
    buffer = nullptr;

    std::string chunkSize = long_to_hex(chunkBody.size());

    std::string chunk(chunkSize + CRLF + chunkBody + CRLF);

    if (fileStream->eof())
        lastChunk = true;

    return chunk;
}

bool ResponseBodyChunker::hasChunk()
{
    return chunkAvailable;
}