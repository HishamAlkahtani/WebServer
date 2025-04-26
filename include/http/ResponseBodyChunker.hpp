#pragma once

#include <filesystem>
#include <fstream>
#include <string>

/**
 * The numbers below are kind of arbitrary, probably need to do some
 * benchmarking to find the optimal values
 */

/**
 * Files larger than CHUNKED_TRANSFER_THRESHOLD will be sent in chunks
 * using Transfer-Encoding: chunked.
 */
#define CHUNKED_TRANSFER_THRESHOLD 2048

#define CHUNK_SIZE 2048

class ResponseBodyChunker
{
    std::filesystem::path path;

    std::ifstream *fileStream;

    bool chunkAvailable;
    bool lastChunk;

public:
    ResponseBodyChunker();

    ResponseBodyChunker(std::filesystem::path path);

    ~ResponseBodyChunker();

    std::string getChunk();

    bool hasChunk();
};