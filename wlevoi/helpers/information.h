//
// Created by voltas on 5/11/2026.
//

#ifndef WLEVOI_INFORMATION_H
#pragma once
#include "includes.h"
constexpr uint64_t pe_image_base = 0x140000000;
constexpr uint64_t va_upper_bound = 0x160000000;
constexpr size_t max_path_len = 1000;
constexpr size_t max_asset_size = 500 * 1024 * 1024;
constexpr size_t decompress_buffer_size = 8192;

struct section_info {
    std::string name;
    uint32_t vaddr;
    uint32_t raw_size;
    uint32_t raw_off;
};

using asset_map = std::map<std::string, std::pair<size_t, size_t>>;
#define WLEVOI_INFORMATION_H

#endif //WLEVOI_INFORMATION_H