#include <fstream>

#include "util.h"

std::string vf::read_file(const std::filesystem::path& acPath)
{
    std::ifstream lcIfStream(acPath, std::ios::binary | std::ios::ate);

    if (!lcIfStream)
    {
        throw std::runtime_error("Error: cannot open: " + acPath.string());
    }

    std::streamsize lnFileSize = lcIfStream.tellg();
    lcIfStream.seekg(0, std::ios::beg);

    std::string lsBuffer(lnFileSize, '\0');

    if (!lcIfStream.read(&lsBuffer[0], lnFileSize))
    {
        throw std::runtime_error("Error: cannot read: " + acPath.string());
    }

    return lsBuffer;
}
