//
// Created by voltas on 5/11/2026.
//

#ifndef WLEVOI_CON_H
#pragma once
#include "includes.h"
namespace con {
    constexpr auto R = "\033[31m";
    constexpr auto G = "\033[32m";
    constexpr auto Y = "\033[33m";
    constexpr auto C = "\033[36m";
    constexpr auto B = "\033[1m";
    constexpr auto X = "\033[0m";

    template<typename... Args>
    void info(const char* fmt, Args... args) {
        printf("%s%s[ info ]%s ", B, C, X);
        printf(fmt, args...);
    }

    template<typename... Args>
    void step(const char* fmt, Args... args) {
        printf("%s[ .. ]%s ", B, X);
        printf(fmt, args...);
    }

    void ok(const char* msg) {
        printf("%s%s[ success ]%s %s\n", B, G, X, msg);
    }

    void warn(const char* msg) {
        printf("%s%s[ warning ]%s %s\n", B, Y, X, msg);
    }

    void fail(const char* msg) {
        printf("%s%s[ fail ]%s %s\n", B, R, X, msg);
    }
}
#define WLEVOI_CON_H

#endif //WLEVOI_CON_H