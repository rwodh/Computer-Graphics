#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <dos.h>

void drawline(int x0, int y0, int x1, int y1)
{
    int dx, dy, p, x, y;
    x = x0;
    y = y0;
    dx = x1 - x0;
    dy = y1 - y0;
    p = 2 * dy - dx;

    while (x <= x1)
    {
        if (p >= 0)
        {
            putpixel(x, y, 7);
            y = y + 1;
            p = p + 2 * dy - 2 * dx;
        }
        else
        {
            putpixel(x, y, 7);
            p = p + 2 * dy;
        }
        x = x + 1;
        delay(100);
    }
}

int main()
{
    int gdriver = DETECT, gmode;

    // User predefined values
    int x0 = 100, y0 = 120;
    int x1 = 200, y1 = 220;

    initgraph(&gdriver, &gmode, "C:\\TURBOC3\\BGI");

    drawline(x0, y0, x1, y1);

    getch();
    return 0;
}