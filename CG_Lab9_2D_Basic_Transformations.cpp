// Program for 2D Geometric Transformations (Translation, Rotation, Scaling)
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>

// Triangle points (global, so all functions can use them)
float X1, Y1, X2, Y2, X3, Y3;

// Copy of the original points (every operation starts from these)
float OX1, OY1, OX2, OY2, OX3, OY3;

// Moves the text cursor to a row (only Turbo C has gotoxy)
void moveCursor(int row)
{
#ifdef __TURBOC__
    gotoxy(1, row);
#endif
}

// Prints the first two lines. outtextxy() gives the space before them.
void showHeading()
{
    outtextxy(60, 0, "Program for Basic Operations");
    outtextxy(60, 10, "Enter the points of triangle:");
}

// Draws the current triangle in the given colour
void drawTriangle(int color)
{
    setcolor(color);
    line((int)X1, (int)Y1, (int)X2, (int)Y2);
    line((int)X2, (int)Y2, (int)X3, (int)Y3);
    line((int)X3, (int)Y3, (int)X1, (int)Y1);
}

// Goes back to the original triangle points
void resetTriangle()
{
    X1 = OX1;  Y1 = OY1;
    X2 = OX2;  Y2 = OY2;
    X3 = OX3;  Y3 = OY3;
}

// 1. Translation: moves every point by Tx and Ty
void translation()
{
    float tx, ty;
    printf("Enter the Translation Factor:\n");

    printf("Tx = ");
    scanf("%f", &tx);

    printf("Ty = ");
    scanf("%f", &ty);

    resetTriangle();

    X1 = X1 + tx;
    Y1 = Y1 + ty;

    X2 = X2 + tx;
    Y2 = Y2 + ty;

    X3 = X3 + tx;
    Y3 = Y3 + ty;

    drawTriangle(YELLOW);
}

// 2. Rotation: turns the triangle around the first point (X1, Y1)
void rotation()
{
    float angle, rad;
    float nx2, ny2, nx3, ny3;

    printf("Enter the Angle of Rotation: ");
    scanf("%f", &angle);

    resetTriangle();

    rad = angle * 3.14159 / 180;

    nx2 = X1 + (X2 - X1) * cos(rad) - (Y2 - Y1) * sin(rad);
    ny2 = Y1 + (X2 - X1) * sin(rad) + (Y2 - Y1) * cos(rad);

    nx3 = X1 + (X3 - X1) * cos(rad) - (Y3 - Y1) * sin(rad);
    ny3 = Y1 + (X3 - X1) * sin(rad) + (Y3 - Y1) * cos(rad);

    X2 = nx2;
    Y2 = ny2;

    X3 = nx3;
    Y3 = ny3;

    drawTriangle(GREEN);
}

// 3. Scaling: makes the triangle bigger/smaller from the first point (X1, Y1)
void scaling()
{
    float sx, sy;
    printf("Enter the Scaling Factor:\n");

    printf("Sx = ");
    scanf("%f", &sx);

    printf("Sy = ");
    scanf("%f", &sy);

    resetTriangle();

    X2 = X1 + (X2 - X1) * sx;
    Y2 = Y1 + (Y2 - Y1) * sy;

    X3 = X1 + (X3 - X1) * sx;
    Y3 = Y1 + (Y3 - Y1) * sy;

    drawTriangle(CYAN);
}

int main()
{
    int gd = DETECT, gm;
    int choice;
    initgraph(&gd, &gm, "C:\\Turboc3\\BGI");

    showHeading();
    moveCursor(4);

    printf("Enter x1 y1: ");
    scanf("%f%f", &X1, &Y1);

    printf("Enter x2 y2: ");
    scanf("%f%f", &X2, &Y2);

    printf("Enter x3 y3: ");
    scanf("%f%f", &X3, &Y3);

    // Save the original points and draw the original triangle in white
    OX1 = X1;  OY1 = Y1;
    OX2 = X2;  OY2 = Y2;
    OX3 = X3;  OY3 = Y3;
    drawTriangle(WHITE);

    printf("\nOperations:\n\n");
    printf("1. Translation\n");
    printf("2. Rotation\n");
    printf("3. Scaling\n");
    printf("4. Exit\n\n");

    while (1)
    {
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1)
            translation();

        else if (choice == 2)
            rotation();

        else if (choice == 3)
            scaling();

        else if (choice == 4)
            break;

        else printf("Invalid choice!\n");
    }

    closegraph();
    return 0;
}
