#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <dos.h>

void drawline(int X0, int Y0, int X1, int Y1)
{
    int dx, dy, p, x, y;
    x = X0;
    y = Y0;

    dx = X1 - X0;
    dy = Y1 - Y0;
    p = 2 * dy - dx;

    while (x <= X1)
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
        delay(500);
    }
}

int main()
{
    int gdriver = DETECT, gmode, X0, Y0, X1, Y1;
    initgraph(&gdriver, &gmode, "C:\\TURBOC3\\BGI");

    // User input for line coordinates
    printf("Enter co-ordinates of 1st point: ");
    scanf("%d%d", &X0, &Y0);

    printf("Enter co-ordinates of 2nd point: ");
    scanf("%d%d", &X1, &Y1);

    drawline(X0, Y0, X1, Y1);

    getch();
    return 0;
}