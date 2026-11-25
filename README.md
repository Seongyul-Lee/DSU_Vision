# DSU_Vision

MFC(SDI) + OpenCV + OpenGL(freeglut) 기반 영상처리 프로젝트

## 빌드 환경

- Visual Studio 2022 (v145 toolset)
- Platform: x64
- MFC Dynamic Linking

## 라이브러리 설정

프로젝트의 props 파일이 아래 경로를 참조하므로, 라이브러리를 **C드라이브 루트**에 배치해야 합니다.

| 라이브러리 | Include | Lib | DLL |
|---|---|---|---|
| OpenCV 4.12.0 | `C:\opencv 4120\build\include` | `C:\opencv 4120\build\x64\vc16\lib` | `C:\opencv 4120\build\x64\vc16\bin` |
| freeglut 3.8.0 | `C:\freeglut-3.8.0\include` | `C:\freeglut-3.8.0\x64\lib` | `C:\freeglut-3.8.0\x64\bin` |

### DLL 배치

실행에 필요한 DLL은 출력 디렉터리(`x64\Debug` 또는 `x64\Release`)에 복사해야 합니다.

- **Debug**: `opencv_world4120d.dll`, `freeglutd.dll`
- **Release**: `opencv_world4120.dll`, `freeglut.dll`

## 구현된 기능

1. **이미지 열기 및 출력** — 파일 대화상자로 이미지를 열어 뷰에 표시
2. **그레이스케일 변환** — 원본(좌)과 흑백 변환 결과(우)를 나란히 비교 표시
3. **카메라 실시간 출력** — DirectShow 카메라를 통한 실시간 영상 표시 (~30fps)
