#include <stdio.h>
#include <graphics.h>
#include <math.h>
#include <dos.h>
#include <conio.h>

int main()
{
    float x, y, x1, y1, x2, y2, dx, dy, step;
    int i, gd = DETECT, gm;

    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");
    setbkcolor(BLACK);
    setcolor(WHITE);

    // User input for line coordinates
    printf("Enter x1 and y1 values: ");
    scanf("%f %f", &x1, &y1);
    printf("Enter x2 and y2 values: ");
    scanf("%f %f", &x2, &y2);

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    if (dx >= dy)
        step = dx;
    else
        step = dy;

    dx = dx / step;
    dy = dy / step;

    x = x1 + 0.5;
    y = y1 + 0.5;

    for (i = 1; i <= step; i++)
    {
        putpixel(x, y, WHITE); // line draw points color setting
        x = x + dx;
        y = y + dy;
        delay(100);
    }
    getch();
    closegraph();
    return 0;
}