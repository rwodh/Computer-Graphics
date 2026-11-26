# Computer Graphics Assignment — OpenGL Programs

**Course:** Computer Graphics (0613-CSE-4103)
**Author:** Anuruddha Saha

Six OpenGL / freeglut programs on animation, AR, fractals and 3D model viewing.

## Quick start (Windows)

You don't need to install anything. **Double-click any `.exe`** in this folder.

> Keep `libfreeglut.dll` in the same folder as the `.exe` files. The programs will not start without it.

## Programs

| # | Program | Topic | Controls |
|---|---------|-------|----------|
| Q1 | [`q1_animation_process`](q1_animation_process.cpp) | The four steps of the animation process: storyboard → object definitions → key frames → in-between frames | `K` key-frame view · `O` in-between (onion-skin) view · `P` play view · `Space` pause · `R` restart |
| Q2 | [`q2_twelve_principles`](q2_twelve_principles.cpp) | The twelve principles of animation, each in its own viewport, with "ghost" copies of earlier frames | `T` motion trails on/off · `Space` pause |
| Q3 | [`q3_animation_framework`](q3_animation_framework.cpp) | A small layered animation framework (Clock, Easing, Track, Clip, Animator, Scene graph) driving a robot | `1` wave clip · `2` nod clip · `Space` pause · `+` / `-` time scale |
| Q4 | [`q4_ar_filter`](q4_ar_filter.cpp) | AR face filter pipeline: landmark detection → smoothing → pose estimation → anchored sunglasses, ears and nose. Uses a synthetic face, so no webcam is needed. | `F` filter on/off · `D` debug landmarks · `Space` pause |
| Q5 | [`q5_fractal_generator`](q5_fractal_generator.cpp) | Fractals generated step by step: Koch snowflake, Barnsley fern (IFS), Mandelbrot set | `1` `2` `3` choose fractal · `+` / `-` level or speed · `A` auto-generate · `R` restart · Mandelbrot: left-click zoom in, right-click zoom out |
| Q6 | [`q6_model_viewer`](q6_model_viewer.cpp) | 3D viewer for Wavefront `.obj` models | Mouse: left-drag orbit, right-drag / wheel zoom · `S` smooth · `F` flat · `W` wireframe · `P` points · `B` bounding box · `X` axes · `L` lighting · `T` turntable · `R` reset |

`Esc` quits every program.

### Opening a model in Q6

- If you double-click `q6_model_viewer.exe`, it shows a built-in torus knot.
- To view the sample vase, **drag `vase.obj` onto `q6_model_viewer.exe`**, or run:
  ```
  q6_model_viewer.exe vase.obj
  ```

## Files

```
CG_Assignment_Codes/
├── q1_animation_process.cpp / .exe
├── q2_twelve_principles.cpp / .exe
├── q3_animation_framework.cpp / .exe
├── q4_ar_filter.cpp / .exe
├── q5_fractal_generator.cpp / .exe
├── q6_model_viewer.cpp / .exe
├── vase.obj            sample model for Q6
├── libfreeglut.dll     needed by the .exe files
└── README.md
```

## Building from source

### Windows (MSYS2 MinGW-w64)

1. Install [MSYS2](https://www.msys2.org/), then in the **MSYS2 MINGW64** shell run:
   ```
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-freeglut
   ```
2. Add `C:\msys64\mingw64\bin` to your `PATH`.
3. Build one program:
   ```
   g++ q1_animation_process.cpp -o q1_animation_process.exe -lfreeglut -lglu32 -lopengl32
   ```
   Build all six in PowerShell:
   ```powershell
   foreach ($f in Get-ChildItem q*.cpp) { g++ $f.Name -o "$($f.BaseName).exe" -lfreeglut -lglu32 -lopengl32 }
   ```

### Linux

```
sudo apt install freeglut3-dev
g++ q1_animation_process.cpp -o q1 -lglut -lGLU -lGL
```

Q3 needs C++14 or newer. Current GCC uses C++14 or newer by default.

### Troubleshooting

- **`GL/freeglut.h: No such file or directory`**: the compiler can't find freeglut. Use the MSYS2 `g++` above. The *graphics-h-compiler* VS Code extension's Run button won't work for these programs, because it only supports `graphics.h` (WinBGI).
- **`libfreeglut.dll was not found`** when starting an `.exe`: copy `libfreeglut.dll` into the same folder as the `.exe`.
