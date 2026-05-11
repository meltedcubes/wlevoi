#pragma once
#include "includes.h"
namespace pe {
    std::vector<section_info> parse_pe_sections(const std::vector<uint8_t>& data) {
        uint32_t e_lfanew = *reinterpret_cast<const uint32_t*>(&data[0x3C]);
        size_t coff_offset = e_lfanew + 4;
        uint16_t num_sections = *reinterpret_cast<const uint16_t*>(&data[coff_offset + 2]);
        uint16_t opt_header_size = *reinterpret_cast<const uint16_t*>(&data[coff_offset + 16]);
        size_t section_table_offset = coff_offset + 20 + opt_header_size;

        std::vector<section_info> sections;
        sections.reserve(num_sections);

        for (int i = 0; i < num_sections; ++i) {
            size_t entry = section_table_offset + i * 40;
            std::string name(reinterpret_cast<const char*>(&data[entry]), 8);
            name = name.c_str();

            sections.push_back({
                name,
                *reinterpret_cast<const uint32_t*>(&data[entry + 12]),
                *reinterpret_cast<const uint32_t*>(&data[entry + 16]),
                *reinterpret_cast<const uint32_t*>(&data[entry + 20])
            });
        }
        return sections;
    }
}