#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

struct ParsedGalacticCoordinates
{
    std::string prefix {};
    std::string x {};
    std::string y {};
    std::string z {};
    std::string ssi {};
};

struct HexPortalGlyphs
{
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
        ss << std::setfill('0') << std::setw(3) << systemIndex;
        ss << std::setfill('0') << std::setw(2) << y;
        ss << std::setfill('0') << std::setw(3) << z;
        ss << std::setfill('0') << std::setw(3) << x;

        return ss.str();
    }
};

int main()
{
    std::string galCoords{"FEOD:08E2:0088:0EB1:01B7"};
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

    std::cout << hexPortalGlyphs.toHex() << std::endl;

    return 0;
}