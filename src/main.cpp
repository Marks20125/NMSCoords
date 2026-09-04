#include <iostream>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>

#include "colored-cout.h"

struct ParsedGalacticCoordinates
{
    //PPPP:XXXX:YYYY:ZZZZ:SSSI

    std::string prefix {};
    std::string x {};
    std::string y {};
    std::string z {};
    std::string ssi {};
};

struct HexPortalGlyphs
{
    //P + SSI + YY + ZZZ + XXX
    // YY = y + 0x81
    // ZZZ = z + 0x801
    // XXX = x + 0x801

    int planetIndex {};
    int systemIndex {};
    int y {};
    int z {};
    int x {};

    std::string toHex()
    {
        std::stringstream ss;
        ss << std::hex << std::uppercase;
        ss << planetIndex;
        ss << ' ';
        ss << std::setfill('0') << std::setw(3) << systemIndex;
        ss << ' ';
        ss << std::setfill('0') << std::setw(2) << y;
        ss << ' ';
        ss << std::setfill('0') << std::setw(3) << z;
        ss << ' ';
        ss << std::setfill('0') << std::setw(3) << x;

        return ss.str();
    }
};

HexPortalGlyphs galCoordsToHexPortalGlyphs(const std::string& galCoords) {
    std::stringstream galCoordsStream{galCoords};
    std::string part;
    std::vector<std::string> parts;

    while (std::getline(galCoordsStream, part, ':'))
    {
        parts.push_back(part);
    }

    ParsedGalacticCoordinates parsedGalCoords {};
    parsedGalCoords.prefix = parts[0];
    parsedGalCoords.x = parts[1];
    parsedGalCoords.y = parts[2];
    parsedGalCoords.z = parts[3];
    parsedGalCoords.ssi = parts[4];

    HexPortalGlyphs hexPortalGlyphs {};
    hexPortalGlyphs.planetIndex = 1;
    hexPortalGlyphs.systemIndex = std::stoi(parsedGalCoords.ssi, nullptr, 16) & 0x0FFF;
    hexPortalGlyphs.y = (std::stoi(parsedGalCoords.y, nullptr, 16) + 0x81) & 0xFF;
    hexPortalGlyphs.z = (std::stoi(parsedGalCoords.z, nullptr, 16) + 0x801) & 0x0FFF;
    hexPortalGlyphs.x = (std::stoi(parsedGalCoords.x, nullptr, 16) + 0x801) & 0x0FFF;

    return hexPortalGlyphs;
};

int main(int argc, char* argv[])
{
    std::filesystem::path homeDir {std::getenv("HOME")};
    std::filesystem::path jsonPath {homeDir / ".config" / "NMSCoordConverter.json"};
    if (!std::filesystem::exists(jsonPath))
    {
        std::ofstream readJsonFile {jsonPath};
        readJsonFile << "{}";
        readJsonFile.close();
    }

    if (argc == 1)
    {
        std::ifstream readJsonFile {jsonPath};
        auto jsonData = nlohmann::json::parse(readJsonFile);
        std::vector<std::string> names{};

        for (const auto& [key, value] : jsonData.items())
        {
            names.push_back(key);
        }

        for (int i = 0; i < names.size(); ++i)
        {
            std::cout << clr::green <<  i+1 << ". " << clr::gray << names[i] << '\n';
        }
        std::cout << "Select a number: ";
        int selection;
        std::cin >> selection;

        std::string code {jsonData[names[selection - 1]]["code"]};

        HexPortalGlyphs hexPortalGlyphs {galCoordsToHexPortalGlyphs(code)};
        std::cout << hexPortalGlyphs.toHex() << '\n';
    }

    if (argc > 2 && std::string_view(argv[1]) == "add")
    {
        std::cout << "Write a name for this entry: ";
        std::string name;
        std::getline(std::cin, name);

        nlohmann::json temp;
        std::ifstream readJsonFile {jsonPath};
        readJsonFile >> temp;
        temp[name]["code"] = std::string_view(argv[2]);
        std::ofstream writeJsonFile {jsonPath};
        writeJsonFile << temp.dump(4);
        writeJsonFile.close();
    }
}
