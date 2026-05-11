#include "wlevoi/helpers/includes.h"

int main(int argc, char* argv[]) {
    printf("wlevoi\n");

    std::string exe_path;
    std::string out_dir = "extracted";
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--out" && i + 1 < argc) {
            out_dir = argv[++i];
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg[0] != '-') {
            exe_path = arg;
        } else if (arg == "--help" || arg == "-h") {
            printf("usage: wlevoi.exe <app.exe> [--out <output_dir>] [--verbose]\n");
            printf("\n");
            printf("examples:\n");
            printf("  wlevoi.exe myapp.exe\n");
            printf("  wlevoi.exe myapp.exe --out ./assets\n");
            printf("  wlevoi.exe myapp.exe --verbose\n");
            return 0;
        }
    }

    if (exe_path.empty()) {
        con::fail("no executable specified");
        printf("\nusage: wlevoi.exe <app.exe> [--out <output_dir>] [--verbose]\n");
        return 1;
    }

    extraction::extract(exe_path, out_dir, verbose);

    return 0;
}