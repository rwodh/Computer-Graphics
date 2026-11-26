#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <dos.h>

void symmetry(int xc, int yc, int x, int y)
{
    putpixel(xc + x, yc + y, WHITE);
    putpixel(xc - x, yc + y, RED);
    putpixel(xc + x, yc - y, GREEN);
    putpixel(xc - x, yc - y, BLUE);

    putpixel(xc + y, yc + x, YELLOW);
    putpixel(xc - y, yc + x, CYAN);
    putpixel(xc + y, yc - x, MAGENTA);
    putpixel(xc - y, yc - x, BROWN);
}

void drawCircle(int xc, int yc, int R)
{
    int x, y, d;

    x = 0;
    y = R;

    /* Initial decision parameter */
    d = 1 - R;

    /* Draw the initial points */
    symmetry(xc, yc, x, y);

    while (y > x)
    {
        if (d > 0)
        {
            /* South-East pixel */
            d = d + 2 * x - 2 * y + 5;
            x++;
            y--;
        }
        else
        {
            /* East pixel */
            d = d + 2 * x + 3;
            x++;
        }

        symmetry(xc, yc, x, y);
        delay(50);
    }
}

int main()
{
    int gd = DETECT, gm;

    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");

    drawCircle(300, 250, 150);

    getch();
    closegraph();
    return 0;
}