#include "asset_pipeline.hpp"

// bunch of libraries that I may not use
#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>
#include <bitset>

#define WINDOW_WIDTH 600

PPU466::Tile get_tile( std::string filename, glm::uvec2 size, std::vector<glm::u8vec4> &data, std::map<std::vector<uint32_t>, uint8_t> &palette);

void load_tiles(PPU466 *ppu) {
    /*
        palettes is a really large map containing all possible palette colors loaded from a 4 x 8 png
        [key is (rgba) u8vec4, value is an 8-bit index b]
        b = [b0, b1, b2, b3, b4, b5, b6, b7]
        b0-b2: unset    b3-b5: palette index    b6-b7: 2 bit index

        Issues with this design:
        1.  you can't have duplicate colors across palettes (they will overwrite each other)
        2.  need a special case for transparent pixels, 
            since all palettes will have one transparent color (at position 0)
    */
    assert(ppu);

    std::cout << "Loading palettes." << std::endl;

    std::map<std::vector<uint32_t>, uint8_t> palette;   // tile palette identifier
    std::vector<glm::u8vec4> data;
    std::vector<uint32_t> key;
    glm::u8vec4 color;
    glm::uvec2 size;

    load_png("assets/palettes.png", &size, &data, LowerLeftOrigin);
    
    for (uint32_t y = 0; y < size.y; ++y) {         // for every palette y
        for (uint32_t x = 0; x < size.x; ++x) {     // for every color x in palette y
            color = data[x+4*y];
            key = {uint32_t(color.r), uint32_t(color.g), uint32_t(color.b), uint32_t(color.a)};

            palette[key] = (y << 2) | x;        // add into palette map

            ppu->palette_table[y][x] = color;   // add into palette table
        }
    }

    std::cout << "Loading sprites." << std::endl;
    uint32_t index = 0;
    std::string filename = "assets";

    // tiles 0-15 dedicated to player
    for (uint8_t i = 0; i < 4; ++i) {
        filename = "assets/sprites/L" + std::to_string(i) + ".png";
        ppu->tile_table[index] = get_tile(filename, size, data, palette);

        filename = "assets/sprites/R" + std::to_string(i) + ".png";
        ppu->tile_table[index+4] = get_tile(filename, size, data, palette);

        filename = "assets/sprites/D" + std::to_string(i) + ".png";
        ppu->tile_table[index+8] = get_tile(filename, size, data, palette);
        
        filename = "assets/sprites/U" + std::to_string(i) + ".png";
        ppu->tile_table[index+12] = get_tile(filename, size, data, palette);
        index++;
    }

    // tiles 20-35 dedicated to enemy
    index = 20;
    for (uint8_t i = 0; i < 4; ++i) {
        filename = "assets/sprites/RL" + std::to_string(i) + ".png";
        ppu->tile_table[index] = get_tile(filename, size, data, palette);

        filename = "assets/sprites/RR" + std::to_string(i) + ".png";
        ppu->tile_table[index+4] = get_tile(filename, size, data, palette);

        filename = "assets/sprites/RD" + std::to_string(i) + ".png";
        ppu->tile_table[index+8] = get_tile(filename, size, data, palette);
        
        filename = "assets/sprites/RU" + std::to_string(i) + ".png";
        ppu->tile_table[index+12] = get_tile(filename, size, data, palette);
        index++;
    }

    std::cout << "Loading game tiles." << std::endl;
    index = 36;

    // tiles 36 - 39 are the bush tiles
    for (uint8_t i = 0; i < 4; ++i) {
        filename = "assets/tiles/B" + std::to_string(i) + ".png";
        ppu->tile_table[index] = get_tile(filename, size, data, palette);
        index++;
    }

    // tile 40 is the coin tile
    ppu->tile_table[index] = get_tile("assets/tiles/C.png", size, data, palette);
    index++;

    // tiles 41 - 44 are the grass tiles
    for (uint8_t i = 0; i < 4; ++i) {
        filename = "assets/tiles/G" + std::to_string(i) + ".png";
        ppu->tile_table[index] = get_tile(filename, size, data, palette);
        index++;
    }

    // tiles 45 - 55 are the number tiles
    for (uint8_t i = 0; i < 11; ++i) {
        filename = "assets/tiles/" + std::to_string(i) + ".png";
        ppu->tile_table[index] = get_tile(filename, size, data, palette);
        index++;
    }
    /*
    // Directory iterator docs: https://en.cppreference.com/w/cpp/filesystem/directory_iterator
    // retired: doesn't call tiles in alphabetical order -> just do it manually
    for (const auto & entry : std::filesystem::directory_iterator("assets/tiles")) {
        ppu->tile_table[index] = get_tile(entry.path().string(), size, data, palette);
        index++;
    }*/
    std::cout << "Loaded " << index << " game assets." << std::endl;
}

PPU466::Tile get_tile(
    std::string filename, 
    glm::uvec2 size, 
    std::vector<glm::u8vec4> &data, 
    std::map<std::vector<uint32_t>, uint8_t> &palette) 
{
    load_png(filename, &size, &data, LowerLeftOrigin);

    glm::u8vec4 color;
    PPU466::Tile tile;
    std::vector<uint32_t> key;

    // structure modified from PlayMode.cpp + https://en.cppreference.com/w/cpp/container/map
    for (uint32_t y = 0; y < size.y; ++y) {
        uint8_t bit0 = 0;
        uint8_t bit1 = 0;
        for (uint32_t x = 0; x < size.x; ++x) {
            color = data[x+size.x*y];
            key = {uint32_t(color.r), uint32_t(color.g), uint32_t(color.b), uint32_t(color.a)};
            bit0 |= (palette[key] & 0x01) << x;
            bit1 |= ((palette[key] >> 1) & 0x01) << x;
        }
        tile.bit0[y] = bit0;
        tile.bit1[y] = bit1;
    }
    return tile;
}