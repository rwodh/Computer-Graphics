# Computer Graphics

**Course:** Computer Graphics (0613-CSE-4103)
**Author:** Anuruddha Saha

This repo holds all my work for the Computer Graphics course: the weekly lab programs, the OpenGL assignment, and links to my course projects.

## Contents

| Section | What's inside |
|---------|---------------|
| [Assignment: OpenGL programs](CG_Assignment_Codes/) | 6 freeglut programs on animation, AR, fractals and 3D model viewing. Ready-to-run `.exe` files are included. |
| [Lab programs](#lab-programs) | Lab 1–10: line, circle, fill and transformation algorithms using `graphics.h` |
| [Projects](#projects) | My larger course projects, each in its own repository |
| [Project ideas](project%20ideas/) | Concept images from project planning |

## Projects

| Project | Description |
|---------|-------------|
| [**Clockwork Garden**](https://github.com/rwodh/clockwork-garden) | Interactive 3D animated story experience |
| [**Lenia**](https://github.com/rwodh/Lenia) | Continuous artificial-life explorer: lifelike organisms emerge from one simple rule (WebGL) |

## Assignment: OpenGL programs

See the [assignment README](CG_Assignment_Codes/README.md) for full details and controls.

| # | Program | Topic |
|---|---------|-------|
| Q1 | [q1_animation_process](CG_Assignment_Codes/q1_animation_process.cpp) | Storyboard → objects → key frames → in-betweens |
| Q2 | [q2_twelve_principles](CG_Assignment_Codes/q2_twelve_principles.cpp) | The twelve principles of animation |
| Q3 | [q3_animation_framework](CG_Assignment_Codes/q3_animation_framework.cpp) | A minimal animation framework |
| Q4 | [q4_ar_filter](CG_Assignment_Codes/q4_ar_filter.cpp) | Landmark-anchored AR face filter |
| Q5 | [q5_fractal_generator](CG_Assignment_Codes/q5_fractal_generator.cpp) | Koch snowflake, Barnsley fern, Mandelbrot set |
| Q6 | [q6_model_viewer](CG_Assignment_Codes/q6_model_viewer.cpp) | Wavefront OBJ 3D model viewer |

On Windows, open `CG_Assignment_Codes/` and double-click any `.exe`.

## Lab programs

These use the classic `graphics.h` (WinBGI) library. Most labs come in two versions: **UserDefined** uses fixed coordinates in the code, and **UserInput** asks for coordinates when it runs.

| Lab | Topic | Files |
|-----|-------|-------|
| 1 | Drawing a line | [CG_Lab1_Line.cpp](CG_Lab1_Line.cpp) |
| 2 | DDA line algorithm | [UserDefined](CG_Lab2_DDA_Line_UserDefined.cpp) · [UserInput](CG_Lab2_DDA_Line_UserInput.cpp) |
| 3 | Bresenham line algorithm | [UserDefined](CG_Lab3_Bresenham_Line_UserDefined.cpp) · [UserInput](CG_Lab3_Bresenham_Line_UserInput.cpp) |
| 4 | Bresenham midpoint line algorithm | [UserDefined](CG_Lab4_Bresenham_Midpoint_Line_UserDefined.cpp) · [UserInput](CG_Lab4_Bresenham_Midpoint_Line_UserInput.cpp) |
| 5 | Drawing a building with the midpoint line algorithm | [Code](CG_Lab5_Bresenham_Midpoint_Building.cpp) · [Diagram](CG_Lab5_Bresenham_Building_Diagram.png) · [Coordinate diagram](CG_Lab5_Bresenham_Building_Coordinate_Diagram.png) |
| 6 | Bresenham circle algorithm | [UserDefined](CG_Lab6_Bresenham_Circle_UserDefined.cpp) · [UserInput](CG_Lab6_Bresenham_Circle_UserInput.cpp) · [8-octant notes](CG_Lab6_Circle_8_Octants.txt) |
| 7 | Boundary fill | [4-connected](CG_Lab7_Boundaryfill_4connected.cpp) · [8-connected](CG_Lab7_Boundaryfill_8connected.cpp) |
| 8 | Flood fill | [4-connected](CG_Lab8_Floodfill_4connected.cpp) · [8-connected](CG_Lab8_Floodfill_8connected.cpp) |
| 9 | 2D transformations | [Basic (translate, rotate, scale)](CG_Lab9_2D_Basic_Transformations.cpp) · [Geometric](CG_Lab9_2D_Geometric_Transformations.cpp) |
| 10 | 2D shearing | [CG_Lab10_2D_Shearing_Transformation.cpp](CG_Lab10_2D_Shearing_Transformation.cpp) |

Labs 7, 8 and 10 include pre-built `.exe` files.

### Building the labs

To compile `graphics.h` programs you need a WinBGI-enabled MinGW, for example the **graphics-h-compiler** VS Code extension, Code::Blocks with WinBGIm, or Dev-C++. With the extension, open a lab file and run *Graphics.h: Compile and Run*.
