// Print the inputs/outputs of a QNN context binary (name, type, shape, scale/offset). Needs only libQnnSystem.so (no NPU).
//   ./bin_info dec_10s.bin
#include <dlfcn.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include "System/QnnSystemInterface.h"

int main(int argc, char** argv) {
  if (argc < 2) { fprintf(stderr, "usage: %s model.bin\n", argv[0]); return 1; }
  void* hs = dlopen("libQnnSystem.so", RTLD_NOW | RTLD_LOCAL); if (!hs) { fprintf(stderr, "%s\n", dlerror()); return 1; }
  auto fn = (Qnn_ErrorHandle_t(*)(const QnnSystemInterface_t***, uint32_t*))dlsym(hs, "QnnSystemInterface_getProviders");
  const QnnSystemInterface_t** p; uint32_t n; if (!fn || fn(&p, &n) != QNN_SUCCESS || !n) return 1;
  auto q = p[0]->QNN_SYSTEM_INTERFACE_VER_NAME;
  FILE* f = fopen(argv[1], "rb"); if (!f) return 1; fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
  std::vector<uint8_t> blob(sz); if (fread(blob.data(), 1, sz, f) != (size_t)sz) return 1; fclose(f);
  QnnSystemContext_Handle_t sc; q.systemContextCreate(&sc);
  const QnnSystemContext_BinaryInfo_t* info; Qnn_ContextBinarySize_t isz;
  if (q.systemContextGetBinaryInfo(sc, blob.data(), blob.size(), &info, &isz) != QNN_SUCCESS) { fprintf(stderr, "getBinaryInfo failed\n"); return 1; }
  uint32_t ng; QnnSystemContext_GraphInfo_t* gs;
  switch (info->version) {
    case QNN_SYSTEM_CONTEXT_BINARY_INFO_VERSION_1: ng = info->contextBinaryInfoV1.numGraphs; gs = info->contextBinaryInfoV1.graphs; break;
    case QNN_SYSTEM_CONTEXT_BINARY_INFO_VERSION_2: ng = info->contextBinaryInfoV2.numGraphs; gs = info->contextBinaryInfoV2.graphs; break;
    default: ng = info->contextBinaryInfoV3.numGraphs; gs = info->contextBinaryInfoV3.graphs; break;
  }
  for (uint32_t g = 0; g < ng; g++) {
    auto& gi = gs[g].graphInfoV1;
    printf("graph %s: %u inputs, %u outputs\n", gi.graphName, gi.numGraphInputs, gi.numGraphOutputs);
    for (int io = 0; io < 2; io++) {
      uint32_t cnt = io ? gi.numGraphOutputs : gi.numGraphInputs; Qnn_Tensor_t* ts = io ? gi.graphOutputs : gi.graphInputs;
      for (uint32_t i = 0; i < cnt; i++) {
        auto& v = ts[i].v1;
        printf("%s %-22s type 0x%04x dims [", io ? "out" : "in ", v.name, (unsigned)v.dataType);
        for (uint32_t d = 0; d < v.rank; d++) printf("%s%u", d ? "," : "", v.dimensions[d]);
        printf("]");
        if (v.quantizeParams.encodingDefinition == QNN_DEFINITION_DEFINED && v.quantizeParams.quantizationEncoding == QNN_QUANTIZATION_ENCODING_SCALE_OFFSET)
          printf(" scale %.9g offset %d", v.quantizeParams.scaleOffsetEncoding.scale, v.quantizeParams.scaleOffsetEncoding.offset);
        printf("\n");
      }
    }
  }
  return 0;
}
