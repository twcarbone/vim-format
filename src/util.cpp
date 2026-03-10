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

std::string vf::base_name(const std::filesystem::path& acPath)
{
    const std::string lsFilename = acPath.filename().string();
    const size_t lnPos = lsFilename.find('.');
    return lsFilename.substr(0, lnPos);
}

std::string vf::tail(const std::filesystem::path& acPath)
{
    std::string lsFilename = acPath.filename().string();
    size_t lnPos = lsFilename.find('.');

    if (lnPos == 0)
    {
        lnPos = lsFilename.find('.', 1);
    }

    if (lnPos == std::string::npos)
    {
        return "";
    }

    return lsFilename.substr(lnPos + 1);
}
