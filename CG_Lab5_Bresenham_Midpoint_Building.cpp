#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <stdlib.h>

// Bresenham's Midpoint Line Drawing Algorithm
void midPoint(int X1, int Y1, int X2, int Y2)
{
    int x = X1, y = Y1;
    int dx = X2 - X1;
    int dy = Y2 - Y1;

    putpixel(x, y, WHITE);

    if (dx >= dy) // horizontal / gentle line -> step x
    {
        int d = dy - (dx / 2); // for midpoint

        while (x < X2)
        {
            x++;
            if (d < 0)
                d = d + dy;
            else
            {
                d += (dy - dx);
                y++;
            }
            putpixel(x, y, WHITE);
        }
    }
    else // vertical / steep line -> step y
    {
        int d = dx - (dy / 2); // same formula, x and y swapped

        while (y < Y2)
        {
            y++;
            if (d < 0)
                d = d + dx;
            else
            {
                d += (dx - dy);
                x++;
            }
            putpixel(x, y, WHITE);
        }
    }
}

// Draw Rectangle Using Midpoint Line Algorithm
void rectangleMid(int X1, int Y1, int X2, int Y2)
{
    midPoint(X1, Y1, X2, Y1); // top
    midPoint(X1, Y2, X2, Y2); // bottom
    midPoint(X1, Y1, X1, Y2); // left
    midPoint(X2, Y1, X2, Y2); // right
}

int main()
{
    int gd = DETECT, gm;

    initgraph(&gd, &gm, (char *)"C:\\TURBOC3\\BGI");

    // Check for graphics errors
    int err = graphresult();
    if (err != grOk)
    {
        printf("Graphics error: %s\n", grapherrormsg(err));
        getch();
        exit(1);
    }

    // OUTER WALLS
    rectangleMid(200, 60, 460, 420);

    // FLOOR DIVISIONS
    midPoint(200, 180, 460, 180); // First floor / second floor
    midPoint(200, 300, 460, 300); // Second floor / ground floor

    // VERTICAL DIVISION (Only top two floors)
    midPoint(330, 60, 330, 300);

    // WINDOWS
    rectangleMid(225, 90, 290, 150); // Top-left Window
    rectangleMid(370, 90, 435, 150); // Top-right Window

    rectangleMid(225, 210, 290, 270); // Middle-left Window
    rectangleMid(370, 210, 435, 270); // Middle-right Window

    rectangleMid(225, 340, 280, 390); // Bottom-left Window
    rectangleMid(380, 340, 435, 390); // Bottom-right Window

    // CENTRAL DOOR
    rectangleMid(300, 330, 360, 420);

    getch();
    closegraph();

    return 0;
}