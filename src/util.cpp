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

std::string vf::name(const std::filesystem::path& acPath)
{
    std::string lsPath = acPath.string();
    std::string_view lsPathView(lsPath);

    if (lsPathView.back() == '/')
    {
        lsPathView.remove_suffix(1);
    }

    size_t lnLastSep = lsPathView.rfind('/');

    if (lnLastSep == std::string_view::npos)
    {
        return std::string { lsPathView };
    }
    else if (true)
    {
        return std::string { lsPathView.substr(lnLastSep + 1) };
    }
}

std::string vf::stem(const std::filesystem::path& acPath)
{
    const std::string lsName = vf::name(acPath);
    size_t lnPos = lsName.find('.', 1);
    return lsName.substr(0, lnPos);
}

std::string vf::tail(const std::filesystem::path& acPath)
{
    std::string lsFilename = vf::name(acPath);
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
