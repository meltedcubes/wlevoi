#pragma once
#include "includes.h"
#include "offsets.h"
#include "compression.h"
namespace extraction {

    asset_map find_assets(const std::vector<uint8_t>& data,
                      const std::vector<section_info>& sections,
                      auto va_to_offset) {
    std::map<uint64_t, std::pair<size_t, size_t>> path_va_map;

    std::regex path_regex(R"(/(?:assets|_next|vercel\.svg|next\.svg)[/a-zA-Z0-9_.@\-% ]{1,200})");
    std::string data_str(reinterpret_cast<const char*>(data.data()), data.size());

    for (auto it = std::sregex_iterator(data_str.begin(), data_str.end(), path_regex);
         it != std::sregex_iterator(); ++it) {
        size_t raw_off = it->position();
        size_t match_len = it->length();

        for (const auto& sec : sections) {
            if (sec.raw_off <= raw_off && raw_off < sec.raw_off + sec.raw_size) {
                uint64_t va = pe_image_base + (raw_off - sec.raw_off) + sec.vaddr;
                path_va_map[va] = {raw_off, match_len};
                break;
            }
        }
    }

    asset_map assets;
    std::vector<uint8_t> needle_buf(8);

    for (const auto& [path_va, path_info] : path_va_map) {
        *reinterpret_cast<uint64_t*>(needle_buf.data()) = path_va;

        size_t search_from = 0;
        while (true) {
            auto it = std::search(data.begin() + search_from, data.end(),
                                  needle_buf.begin(), needle_buf.end());
            if (it == data.end()) break;

            size_t ptr_pos = it - data.begin();
            search_from = ptr_pos + 1;

            if (ptr_pos + 32 > data.size()) continue;

            uint64_t path_ptr = *reinterpret_cast<const uint64_t*>(&data[ptr_pos]);
            uint64_t path_len = *reinterpret_cast<const uint64_t*>(&data[ptr_pos + 8]);
            uint64_t data_ptr = *reinterpret_cast<const uint64_t*>(&data[ptr_pos + 16]);
            uint64_t data_len = *reinterpret_cast<const uint64_t*>(&data[ptr_pos + 24]);

            if (path_ptr != path_va) continue;
            if (path_len == 0 || path_len >= max_path_len) continue;
            if (!offsets::is_valid_va(data_ptr)) continue;
            if (data_len == 0 || data_len >= max_asset_size) continue;

            auto data_off = va_to_offset(data_ptr);
            if (!data_off.has_value() || *data_off + data_len > data.size()) continue;

            std::string path(reinterpret_cast<const char*>(&data[path_info.first]), path_len);

            if (assets.find(path) == assets.end()) {
                assets[path] = {*data_off, static_cast<size_t>(data_len)};
            }
        }
    }

    return assets;
}
void extract(const std::string& exe_path, const std::string& out_dir, bool verbose = false) {
        con::step("reading %s\n", exe_path.c_str());

        std::ifstream file(exe_path, std::ios::binary);
        if (!file) {
            con::fail("cannot open file");
            return;
        }

        std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
        file.close();

        double mb = data.size() / 1024.0 / 1024.0;
        con::info("%zu bytes (%.1f mb)\n", data.size(), mb);

        auto sections = pe::parse_pe_sections(data);
        auto va_to_offset = offsets::make_va_to_offset(sections);

        if (verbose) {
            con::step("pe sections:\n");
            for (const auto& sec : sections) {
                double size_mb = sec.raw_size / 1024.0 / 1024.0;
                printf("      %-10s raw=0x%08x  size=%.1f MB\n",
                       sec.name.c_str(), sec.raw_off, size_mb);
            }
        }

        con::step("scanning for asset table entries...\n");
        auto assets = find_assets(data, sections, va_to_offset);

        size_t compressed_total = 0;
        for (const auto& [path, info] : assets) {
            compressed_total += info.second;
        }
        double compressed_mb = compressed_total / 1024.0 / 1024.0;
        con::info("found %zu assets (%.1f mb compressed)\n", assets.size(), compressed_mb);

        std::filesystem::create_directories(out_dir);

        size_t extracted = 0;
        size_t failed = 0;
        size_t output_total = 0;

        for (const auto& [path, info] : assets) {
            size_t raw_off = info.first;
            size_t compressed_len = info.second;

            std::string relative = path;
            if (relative[0] == '/') relative = relative.substr(1);
            std::replace(relative.begin(), relative.end(), '/', '\\');

            std::filesystem::path out_path = std::filesystem::path(out_dir) / relative;
            std::filesystem::create_directories(out_path.parent_path());

            std::vector<uint8_t> compressed_data(data.begin() + raw_off,
                                                 data.begin() + raw_off + compressed_len);

            std::vector<uint8_t> decompressed;
            bool success = compression::brotli_decompress(compressed_data, decompressed);

            if (!success) {
                decompressed = std::move(compressed_data);
                failed++;
                con::warn(("brotli failed, storing raw: " + path).c_str());
            }

            std::ofstream out_file(out_path, std::ios::binary);
            out_file.write(reinterpret_cast<const char*>(decompressed.data()), decompressed.size());
            out_file.close();

            extracted++;
            output_total += decompressed.size();

            if (verbose) {
                printf("  %10zu -> %10zu  %s\n", compressed_len, decompressed.size(), path.c_str());
            }
        }

        printf("\n");
        con::ok(("extracted " + std::to_string(extracted) + " files").c_str());
        double output_mb = output_total / 1024.0 / 1024.0;
        con::info("output: %.1f mb -> '%s/'\n", output_mb, out_dir.c_str());
        if (failed) {
            con::warn(("stored raw: " + std::to_string(failed) + " file(s) (brotli failed)").c_str());
        }
    }
}