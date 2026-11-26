#include <stdio.h>
#include <graphics.h>
#include <conio.h>

int main() {
    
    int gd = DETECT, gm; // Detect the graphics driver and mode automatically
    
    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI"); // Initialize the graphics system

    // Draw a line from coordinate (100, 150) to (400, 150)
    line(100, 150, 400, 150); // Syntax: line(x1, y1, x2, y2);

    getch(); // Hold the output screen until a key is pressed
    closegraph(); // Deallocate graphics memory and close the window
    return 0;
}