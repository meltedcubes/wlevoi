## wlevoi

# Introudction
Wlevoi is a c++ tool made for extracting and unpacking tauri applications

# How it works
Parses the pe header,

From the pe header, finds the .data section and extracts assets path from it, including compressed data pointers,

Decompresses each asset with brotli and writes the output to output path

# Asset format

Each asset entry in the binary has this format :
[path_ptr: u64][path_len: u64][data_ptr: u64][data_len: u64]

# Quick start 

The tool accepts arguments like wlevoi.exe myapp.exe -- out (dir) --verbose --help

--help displays a help message with a guide of how to use wlevoi

--verbose shows detailed extraction info

--out is the output path for extracted assets
