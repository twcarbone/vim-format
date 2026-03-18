#include <fstream>

#include "util.h"

std::string fix_path(const std::filesystem::path& acPath)
{
    std::string lsPath = acPath.string();
    std::string_view lsPathView(lsPath);

    if (lsPathView.back() == '/')
    {
        lsPathView.remove_suffix(1);
    }

    return std::string(lsPathView);
}

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

bool vf::startswith(std::string_view asStr, std::string_view asPrefix, std::string_view asDelim)
{
    size_t lnEnd = asDelim.empty() ? asPrefix.size() : asStr.find_first_of(asDelim);
    return asStr.substr(0, lnEnd) == asPrefix;
}

bool vf::startswith_int(std::string_view asStr, std::string_view& asOut)
{
    // 1. Start by finding the end any leading digits. Save it for back-tracking.
    const size_t lnDigitsEnd = asStr.find_first_not_of("0123456789");
    asOut = asStr.substr(0, lnDigitsEnd);

    // 2. Zero digits means this is not an integer. More than one digit means this is a
    //    base-10 integer. We can return immediately in either case.
    if (lnDigitsEnd != 1)
    {
        return lnDigitsEnd > 0;
    }

    // 3. Compute the base from the "base character". Return the leading digits if it's
    //    not a valid base character.
    int lnBase;
    switch (asStr[1])
    {
        case 'X':
        case 'x':
            lnBase = 16;
            break;
        case 'O':
        case 'o':
            lnBase = 8;
            break;
        case 'B':
        case 'b':
            lnBase = 2;
            break;
        default:
            return true;
    }

    // 4. Convert the "value" portion (example: 1234 from 0x1234).
    const size_t lnCandidateEnd = asStr.find_first_not_of("0123456789abcdefABCDEF", 2);
    const std::string_view lsValue = asStr.substr(2, lnCandidateEnd - 2);
    const char* pStart = lsValue.data();
    char* pEnd = nullptr;
    long l = std::strtol(pStart, &pEnd, lnBase);

    // 5. Zero valid characters, or one invalid character, followed the "base character".
    //    Return the leading digits.
    if (pEnd == pStart || pEnd != lsValue.end())
    {
        return true;
    }

    asOut = asStr.substr(0, lnCandidateEnd);
    return true;
}

bool vf::startswith_float(std::string_view asStr, std::string_view& asOut)
{
    // 1. The candidate string is everything until the first non-float character
    const size_t lnCandidateEnd = asStr.find_first_not_of("0123456789eE+-.");
    asOut = asStr.substr(0, lnCandidateEnd);

    // 2. Do the conversion
    const char* pStart = asOut.data();
    char* pEnd = nullptr;
    float f = std::strtof(pStart, &pEnd);

    // 3. The entire candidate needs to be parsed to make a valid float
    if (pEnd == pStart || pEnd != asOut.end())
    {
        return false;
    }

    // 4. It might've been an integer that was parsed
    if (asOut.find('.') == std::string_view::npos)
    {
        return false;
    }

    return true;
}

std::string vf::name(const std::filesystem::path& acPath)
{
    std::string lsPath = fix_path(acPath);
    size_t lnLastSep = lsPath.rfind('/');

    if (lnLastSep == std::string_view::npos)
    {
        return std::string { lsPath };
    }
    else if (true)
    {
        return lsPath.substr(lnLastSep + 1);
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

std::filesystem::path vf::with_tail(const std::filesystem::path& acPath, const std::string& asTail)
{
    std::filesystem::path lcPath(fix_path(acPath));

    if (vf::stem(lcPath).empty())
    {
        throw std::runtime_error("Cannot replace tail of path with no stem: " + lcPath.string());
    }

    return lcPath.parent_path() / std::filesystem::path(vf::stem(lcPath)).replace_extension(asTail);
}
