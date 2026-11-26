#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <graphics.h>

void floodFill8(int x, int y, int oldColor, int newColor)
{
    if (getpixel(x, y) == oldColor)
    {
        delay(10); // Optional delay for visualization
        putpixel(x, y, newColor);

        // 4-connected neighbors
        floodFill8(x + 1, y, oldColor, newColor);
        floodFill8(x - 1, y, oldColor, newColor);
        floodFill8(x, y + 1, oldColor, newColor);
        floodFill8(x, y - 1, oldColor, newColor);

        // Diagonal neighbors
        floodFill8(x + 1, y + 1, oldColor, newColor);
        floodFill8(x - 1, y + 1, oldColor, newColor);
        floodFill8(x + 1, y - 1, oldColor, newColor);
        floodFill8(x - 1, y - 1, oldColor, newColor);
    }
}

int main()
{
    int gd = DETECT, gm;

    initgraph(&gd, &gm, "");

    // Draw a closed rectangle
    rectangle(100, 100, 150, 150);

    // Fill inside
    floodFill8(125, 125, BLACK, RED);

    getch();
    closegraph();

    return 0;
}