#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <dos.h>

void midPoint(int X1, int Y1, int X2, int Y2)
{
    int x = X1, y = Y1;
    int dx = X2 - X1;
    int dy = Y2 - Y1;
    int d = dy - (dx / 2);

    putpixel(x, y, WHITE);
    printf("(%d,%d); ", x, y);

    while (x < X2)
    {
        x++;

        // E or East is chosen
        if (d < 0)
            d = d + dy;

        // NE or North East is chosen
        else
        {
            d += (dy - dx);
            y++;
        }
        putpixel(x, y, WHITE);
        printf("(%d,%d); ", x, y);

        delay(100);
    }
}

int main()
{
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");

    // User predefined values
    int X1 = 150, Y1 = 180;
    int X2 = 200, Y2 = 230;
    midPoint(X1, Y1, X2, Y2);

    getch();
    return 0;
}