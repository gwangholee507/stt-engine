#!/bin/sh
# Build on the board (needs g++: sudo apt install -y g++)
#   whisper_npu : stdin/stdout runner used by the Python tools
#   asr_cli     : C++-only example (wav -> text)
#   bin_info    : print a .bin's inputs/outputs (names, types, quantization)
#   libwhisper_asr.a + whisper_asr.h : library for the C++ application
cd "$(dirname "$0")"
F="-O3 -std=c++17 -Iinclude"
g++ $F -c whisper_asr.cpp -o whisper_asr.o && ar rcs libwhisper_asr.a whisper_asr.o &&
g++ $F whisper_npu.cpp libwhisper_asr.a -ldl -o whisper_npu &&
g++ $F asr_cli.cpp libwhisper_asr.a -ldl -o asr_cli &&
g++ $F bin_info.cpp -ldl -o bin_info && echo "built ./whisper_npu ./asr_cli ./bin_info libwhisper_asr.a"
