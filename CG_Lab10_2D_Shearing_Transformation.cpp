#include <stdio.h>
#include <conio.h>
#include <graphics.h>

// User-defined function for X-axis, Y-axis Shearing, and XY-Shearing
void performShearing(int x1, int y1, int x2, int y2, int x3, int y3)
{
    float shx, shy;
    int xsh1, ysh1, xsh2, ysh2, xsh3, ysh3;

    // 1. X-axis Shearing
    printf("\nEnter X-axis shearing factor (shx): ");
    scanf("%f", &shx);

    xsh1 = (int)(x1 + shx * y1);
    ysh1 = y1;

    xsh2 = (int)(x2 + shx * y2);
    ysh2 = y2;

    xsh3 = (int)(x3 + shx * y3);
    ysh3 = y3;

    setcolor(RED);
    line(xsh1, ysh1, xsh2, ysh2);
    line(xsh2, ysh2, xsh3, ysh3);
    line(xsh3, ysh3, xsh1, ysh1);
    outtextxy(xsh3 + 5, ysh3 + 5, "X-Sheared");

    // 2. Y-axis Shearing
    printf("\nEnter Y-axis shearing factor (shy): ");
    scanf("%f", &shy);

    xsh1 = x1;
    ysh1 = (int)(y1 + shy * x1);

    xsh2 = x2;
    ysh2 = (int)(y2 + shy * x2);

    xsh3 = x3;
    ysh3 = (int)(y3 + shy * x3);

    setcolor(GREEN);
    line(xsh1, ysh1, xsh2, ysh2);
    line(xsh2, ysh2, xsh3, ysh3);
    line(xsh3, ysh3, xsh1, ysh1);
    outtextxy(xsh3 + 5, ysh3 + 5, "Y-Sheared");

    // 3. XY-Shear / Combined / Compound Shearing
    printf("\nEnter X-axis shearing factor for combined shearing (shx): ");
    scanf("%f", &shx);
    printf("Enter Y-axis shearing factor for combined shearing (shy): ");
    scanf("%f", &shy);

    xsh1 = (int)(x1 + shx * y1);
    ysh1 = (int)(y1 + shy * x1);

    xsh2 = (int)(x2 + shx * y2);
    ysh2 = (int)(y2 + shy * x2);

    xsh3 = (int)(x3 + shx * y3);
    ysh3 = (int)(y3 + shy * x3);

    setcolor(CYAN);
    line(xsh1, ysh1, xsh2, ysh2);
    line(xsh2, ysh2, xsh3, ysh3);
    line(xsh3, ysh3, xsh1, ysh1);
    outtextxy(xsh3 + 5, ysh3 + 5, "XY-Sheared");
}

int main()
{
    int gd = DETECT, gm;
    int x1, y1, x2, y2, x3, y3;

    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");

    printf("Implement 2D Shearing Transformation\n");
    printf("For X-axis, Y-axis, and XY-Shearing (Object on Triangle)\n");

    printf("\nEnter the coordinates of triangle:\n");
    printf("Enter x1 y1: ");
    scanf("%d %d", &x1, &y1);

    printf("Enter x2 y2: ");
    scanf("%d %d", &x2, &y2);

    printf("Enter x3 y3: ");
    scanf("%d %d", &x3, &y3);

    // Original Triangle
    setcolor(WHITE);
    line(x1, y1, x2, y2);
    line(x2, y2, x3, y3);
    line(x3, y3, x1, y1);
    outtextxy(x3 + 5, y3 + 5, "Original");

    // Call user-defined function
    performShearing(x1, y1, x2, y2, x3, y3);

    getch();
    closegraph();
    return 0;
}