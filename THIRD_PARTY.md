# 다른 프로젝트에서 가져온 파일

| 파일 | 출처 | 라이선스 |
| --- | --- | --- |
| `mel_filters.bin`, `mel_filters.npy` | Whisper 멜 필터 (Hugging Face `WhisperFeatureExtractor`와 같은 값) | MIT, Copyright (c) 2022 OpenAI |
| `vocab.bin`, `vocab_bytes.json` | Whisper 토큰 표 (`seastar105/whisper-small-komixv2` 토크나이저 = `openai/whisper-small`) | MIT, Copyright (c) 2022 OpenAI / Apache-2.0 (모델 카드) |
| `silero_vad.onnx` | [snakers4/silero-vad](https://github.com/snakers4/silero-vad) | MIT, Copyright (c) 2020-present Silero Team |
| `samples/fleurs_ko_30s.wav`, `.txt` | [FLEURS](https://huggingface.co/datasets/google/fleurs) 한국어 test 5문장을 0.6초 쉼을 두고 이어 붙임 (`.txt`는 정답) | CC BY 4.0, Google |
| `astk_client.py` | 같은 팀 [audio-stack](https://github.com/khu-cse-capstone-26h2-transcriber/audio-stack) `client/astk_client.py`를 그대로 복사 | audio-stack을 따름 |

모델 파일(`.bin`, 이 저장소에 없음)의 출처와 조건은 `README.md`의 '모델 출처와 라이선스'에 있습니다.

## MIT License (Whisper)

```
MIT License

Copyright (c) 2022 OpenAI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## MIT License (Silero VAD)

```
MIT License

Copyright (c) 2020-present Silero Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
