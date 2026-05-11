//
// Created by voltas on 5/11/2026.
//

#ifndef WLEVOI_COMPRESSION_H
#pragma once
#include "includes.h"
namespace compression {
    bool brotli_decompress(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& output) {
        BrotliDecoderState* state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
        if (!state) return false;

        size_t available_in = compressed.size();
        const uint8_t* next_in = compressed.data();

        std::vector<uint8_t> temp_out(decompress_buffer_size);
        bool success = false;

        while (true) {
            size_t available_out = temp_out.size();
            uint8_t* next_out = temp_out.data();

            BrotliDecoderResult result = BrotliDecoderDecompressStream(
                state, &available_in, &next_in, &available_out, &next_out, nullptr);

            output.insert(output.end(), temp_out.data(), next_out);

            if (result == BROTLI_DECODER_RESULT_SUCCESS) {
                success = true;
                break;
            } else if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
                temp_out.resize(temp_out.size() * 2);
            } else {
                break;
            }
        }

        BrotliDecoderDestroyInstance(state);
        return success;
    }
}

#define WLEVOI_COMPRESSION_H

#endif //WLEVOI_COMPRESSION_H