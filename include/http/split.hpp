#pragma once

#include <string>
#include <vector>

#define CRLF std::string("\r\n")

/**
 * @brief Splits a string by a specified delimiter
 *
 * @param str String to split
 * @param delimiter String that marks where to split (can be multi-character)
 * @return std::vector<std::string> Vector containing the split substrings
 *
 * Splits the input string at each occurrence of the delimiter. The delimiter is not
 * included in the results. Empty strings are preserved when delimiters appear
 * consecutively or at the string boundaries.
 */
inline std::vector<std::string> split(std::string &str, std::string delimiter)
{

    std::vector<std::string> result;
    std::size_t startOfLine = 0;
    while (startOfLine < str.size())
    {
        std::size_t nextOccurrence = str.find(delimiter, startOfLine);

        std::size_t endOfLine;
        if (nextOccurrence != std::string::npos)
            endOfLine = nextOccurrence;
        else
            endOfLine = str.size();

        std::size_t length = endOfLine - startOfLine;

        result.push_back(str.substr(startOfLine, length));
        startOfLine = endOfLine + delimiter.length();
    }
    return result;
}
