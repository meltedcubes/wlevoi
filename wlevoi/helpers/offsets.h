//
// Created by voltas on 5/11/2026.
//

#ifndef WLEVOI_OFFSETS_H
#pragma once
#include "includes.h"
namespace offsets {
    auto make_va_to_offset(const std::vector<section_info>& sections) {
        return [&sections](uint64_t va) -> std::optional<size_t> {
            uint64_t rva = va - pe_image_base;
            for (const auto& sec : sections) {
                if (sec.vaddr <= rva && rva < sec.vaddr + sec.raw_size) {
                    return (rva - sec.vaddr) + sec.raw_off;
                }
            }
            return std::nullopt;
        };
    }

    inline bool is_valid_va(uint64_t va) {
        return va >= pe_image_base && va <= va_upper_bound;
    }
}
#define WLEVOI_OFFSETS_H

#endif //WLEVOI_OFFSETS_H