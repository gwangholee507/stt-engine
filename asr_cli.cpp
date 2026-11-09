// Example for integrating wasr::Engine (whisper_asr.h): transcribe wav files with the C++ library only.
//   ./asr_cli [--zc] [--vocab-map dec_10s_pv.vocab] [--prefix dec_10s_pv_pre.bin] enc_10s.bin dec_10s.bin a.wav [b.wav ...]
// wav: 16 kHz, 16-bit PCM (mono or multi-channel, averaged). Prints timing and text per file.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include "whisper_asr.h"

static std::vector<float> read_wav(const std::string& path) {
  FILE* f = fopen(path.c_str(), "rb"); if (!f) throw std::runtime_error("cannot open " + path);
  char id[4]; uint32_t sz; uint16_t fmt = 0, ch = 1, bits = 16; uint32_t rate = 0;
  if (fread(id, 1, 4, f) != 4 || memcmp(id, "RIFF", 4) || fread(&sz, 4, 1, f) != 1 || fread(id, 1, 4, f) != 4 || memcmp(id, "WAVE", 4))
    throw std::runtime_error(path + ": not a wav");
  std::vector<float> out;
  while (fread(id, 1, 4, f) == 4 && fread(&sz, 4, 1, f) == 1) {
    if (!memcmp(id, "fmt ", 4)) {
      std::vector<uint8_t> b(sz); if (fread(b.data(), 1, sz, f) != sz) break;
      memcpy(&fmt, &b[0], 2); memcpy(&ch, &b[2], 2); memcpy(&rate, &b[4], 4); memcpy(&bits, &b[14], 2);
    } else if (!memcmp(id, "data", 4)) {
      if (fmt != 1 || bits != 16 || rate != 16000) throw std::runtime_error(path + ": need 16 kHz 16-bit PCM");
      std::vector<int16_t> s(sz / 2); size_t got = fread(s.data(), 2, s.size(), f);
      out.resize(got / ch);
      for (size_t i = 0; i < out.size(); i++) { float a = 0; for (int c = 0; c < ch; c++) a += s[i * ch + c] / 32768.f; out[i] = a / ch; }
      break;
    } else fseek(f, sz + (sz & 1), SEEK_CUR);
  }
  fclose(f);
  return out;
}

int main(int argc, char** argv) {
  wasr::Options opt; std::vector<std::string> args;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--zc")) opt.zero_copy = true;
    else if (!strcmp(argv[i], "--vocab-map") && i + 1 < argc) opt.vocab_map = argv[++i];
    else if (!strcmp(argv[i], "--prefix") && i + 1 < argc) opt.prefix_bin = argv[++i];
    else args.push_back(argv[i]);
  }
  if (args.size() < 3) { fprintf(stderr, "usage: %s [--zc] [--vocab-map file] [--prefix pre.bin] enc.bin dec.bin a.wav ...\n", argv[0]); return 1; }
  std::string exe = argv[0], dir = exe.find('/') == std::string::npos ? "." : exe.substr(0, exe.rfind('/'));
  try {
    wasr::Engine asr(args[0], args[1], dir, opt);
    for (size_t i = 2; i < args.size(); i++) {
      auto pcm = read_wav(args[i]);
      wasr::Stats st;
      std::string text = asr.transcribe(pcm.data(), pcm.size(), &st);
      printf("%s | audio %.1fs | mel %.0fms enc %.0fms dec %.0fms (%d steps, npu %.0fms) total %.0fms | %s\n",
             args[i].c_str(), pcm.size() / 16000.0, st.mel_ms, st.enc_ms, st.dec_ms, st.steps, st.npu_ms, st.total_ms, text.c_str());
      fflush(stdout);
    }
  } catch (const std::exception& e) { fprintf(stderr, "error: %s\n", e.what()); return 1; }
  return 0;
}
