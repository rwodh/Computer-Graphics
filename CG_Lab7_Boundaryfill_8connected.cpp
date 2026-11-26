#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <graphics.h>

void boundaryFill8(int x, int y, int fillColor, int boundaryColor)
{
    if (getpixel(x, y) != boundaryColor && getpixel(x, y) != fillColor)
    {
        putpixel(x, y, fillColor);
        delay(10); // Optional delay for visualization

        // 4-connected neighbors
        boundaryFill8(x + 1, y, fillColor, boundaryColor);
        boundaryFill8(x - 1, y, fillColor, boundaryColor);
        boundaryFill8(x, y + 1, fillColor, boundaryColor);
        boundaryFill8(x, y - 1, fillColor, boundaryColor);

        // Diagonal neighbors
        boundaryFill8(x + 1, y + 1, fillColor, boundaryColor);
        boundaryFill8(x - 1, y + 1, fillColor, boundaryColor);
        boundaryFill8(x + 1, y - 1, fillColor, boundaryColor);
        boundaryFill8(x - 1, y - 1, fillColor, boundaryColor);
    }
}

int main()
{
    int gd = DETECT, gm;

    initgraph(&gd, &gm, "C:\\Turboc3\\BGI");

    // Draw boundary
    rectangle(50, 50, 100, 100);

    // Fill inside the rectangle
    boundaryFill8(70, 70, BLUE, WHITE);
    
    getch();
    closegraph();

    return 0;
}