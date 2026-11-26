#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <dos.h>

/* Draw a thick pixel (3x3) */
void thickPixel(int x, int y, int color)
{
    int i, j;

    for(i = -1; i <= 1; i++)
    {
        for(j = -1; j <= 1; j++)
        {
            putpixel(x + i, y + j, color);
        }
    }
}

/* Plot 8 symmetric points with different colors */
void symmetry(int xc, int yc, int x, int y)
{
    thickPixel(xc + x, yc + y, RED);          // Octant 1 - (x, y)
    thickPixel(xc + y, yc + x, GREEN);        // Octant 2 - (y, x)
    thickPixel(xc - y, yc + x, BLUE);         // Octant 3 - (-y, x)
    thickPixel(xc - x, yc + y, CYAN);         // Octant 4 - (-x, y)
    thickPixel(xc - x, yc - y, MAGENTA);      // Octant 5 - (-x, -y)
    thickPixel(xc - y, yc - x, YELLOW);       // Octant 6 - (-y, -x)
    thickPixel(xc + y, yc - x, BROWN);        // Octant 7 - (y, -x)
    thickPixel(xc + x, yc - y, WHITE);        // Octant 8 - (x, -y)
}

void drawCircle(int xc, int yc, int R)
{
    int x = 0;
    int y = R;
    int d = 1 - R;

    symmetry(xc, yc, x, y);

    while(y > x)
    {
        if(d < 0)
        {
            d = d + 2 * x + 3;
            x++;
        }
        else
        {
            d = d + 2 * x - 2 * y + 5;
            x++;
            y--;
        }

        symmetry(xc, yc, x, y);
        delay(20);
    }
}

int main()
{
    int gd = DETECT, gm;
    int xc, yc, R;

    printf("Enter Center X : ");
    scanf("%d", &xc);

    printf("Enter Center Y : ");
    scanf("%d", &yc);

    printf("Enter Radius   : ");
    scanf("%d", &R);

    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");

    setbkcolor(BLACK);
    cleardevice();

    drawCircle(xc, yc, R);

    outtextxy(10, 10, "8-Coloured Midpoint Circle Drawing Algorithm");

    getch();
    closegraph();
    return 0;
}