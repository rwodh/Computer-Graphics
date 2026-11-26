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
    outtextxy(0, 0, "Program for 2D Geometric Transformations (Translation, Rotation, Scaling)");
    outtextxy(0, 10, "Enter the points of triangle:");
}

// Draws the current triangle in the given colour
void drawTriangle(int color)
{
    setcolor(color);
    line((int)X1, (int)Y1, (int)X2, (int)Y2);
    line((int)X2, (int)Y2, (int)X3, (int)Y3);
    line((int)X3, (int)Y3, (int)X1, (int)Y1);
    outtextxy((int)X3 + 5, (int)Y3 + 5, "Original Triangle");
}

// Goes back to the original triangle points
void resetTriangle()
{
    X1 = OX1;  Y1 = OY1;
    X2 = OX2;  Y2 = OY2;
    X3 = OX3;  Y3 = OY3;
}

// 1. Translation: moves the triangle by Tx and Ty
void translation()
{
    float tx, ty;

    resetTriangle();

    printf("\nEnter translation factors (Tx Ty): ");
    scanf("%f%f", &tx, &ty);

    X1 = X1 + tx;
    Y1 = Y1 + ty;

    X2 = X2 + tx;
    Y2 = Y2 + ty;

    X3 = X3 + tx;
    Y3 = Y3 + ty;

    drawTriangle(YELLOW);
    outtextxy(X3 + 5, Y3 + 5, "Translated Triangle");
    getch();
}

// 2. Rotation: rotates the triangle by an angle about a point (origin or fixed point)
void rotation()
{
    float angle, rad;
    float nx1, ny1, nx2, ny2, nx3, ny3;
    float px, py;
    int rchoice, pchoice;

    printf("\nRotation Options:\n");
    printf("1. Standard Rotation (About Origin)\n");
    printf("2. Rotation About Fixed Point\n\n");

    printf("Enter your choice & the Angle of Rotation: ");
    scanf("%d %f", &rchoice, &angle);

    resetTriangle();

    rad = angle * 3.14159 / 180;

    if (rchoice == 1) // Standard Rotation about Origin
    {
        px = 0;
        py = 0;
    }

    else if (rchoice == 2) // Select Fixed Pivot Point
    {
        printf("\nSelect Pivot Point:\n");
        printf("1. Point 1 (%.2f, %.2f)\n", X1, Y1);
        printf("2. Point 2 (%.2f, %.2f)\n", X2, Y2);
        printf("3. Point 3 (%.2f, %.2f)\n", X3, Y3);
        printf("4. Custom Point\n\n");

        printf("Enter your choice: ");
        scanf("%d", &pchoice);

        if (pchoice == 1)
        {
            px = X1;
            py = Y1;
        }

        else if (pchoice == 2)
        {
            px = X2;
            py = Y2;
        }

        else if (pchoice == 3)
        {
            px = X3;
            py = Y3;
        }

        else if (pchoice == 4)
        {
            printf("Enter Custom Pivot Point (x y): ");
            scanf("%f%f", &px, &py);
        }

        else
        {
            printf("Invalid pivot choice!\n");
            getch();
            return;
        }
    }

    else
    {
        printf("Invalid rotation choice!\n");
        getch();
        return;
    }

    // Rotation of Point 1
    nx1 = px + (X1 - px) * cos(rad) - (Y1 - py) * sin(rad);
    ny1 = py + (X1 - px) * sin(rad) + (Y1 - py) * cos(rad);

    // Rotation of Point 2
    nx2 = px + (X2 - px) * cos(rad) - (Y2 - py) * sin(rad);
    ny2 = py + (X2 - px) * sin(rad) + (Y2 - py) * cos(rad);

    // Rotation of Point 3
    nx3 = px + (X3 - px) * cos(rad) - (Y3 - py) * sin(rad);
    ny3 = py + (X3 - px) * sin(rad) + (Y3 - py) * cos(rad);

    // Update Triangle Points
    X1 = nx1;
    Y1 = ny1;

    X2 = nx2;
    Y2 = ny2;

    X3 = nx3;
    Y3 = ny3;

    drawTriangle(GREEN);
    outtextxy(X3 + 5, Y3 + 5, "Rotated Triangle");
    getch();
}

// 3. Scaling: scales the triangle by Sx and Sy about a point (origin or fixed point)

void scaling()
{
    int method, pivotChoice;
    float sx, sy, px, py;
    float nx1, ny1, nx2, ny2, nx3, ny3;

    printf("\nScaling Options:\n");
    printf("1. Standard Scaling (About Origin)\n");
    printf("2. Scaling About Fixed Point\n");
    printf("Enter your choice: ");
    scanf("%d", &method);

    resetTriangle();

    if(method == 1)
    {
        px = 0;
        py = 0;
    }
    else if(method == 2)
    {
        printf("\nSelect Fixed Point:\n");
        printf("1. Point 1 (%.2f, %.2f)\n", X1, Y1);
        printf("2. Point 2 (%.2f, %.2f)\n", X2, Y2);
        printf("3. Point 3 (%.2f, %.2f)\n", X3, Y3);
        printf("4. Custom Point\n");
        printf("Enter your choice: ");
        scanf("%d", &pivotChoice);

        if(pivotChoice == 1)
        {
            px = X1;
            py = Y1;
        }
        else if(pivotChoice == 2)
        {
            px = X2;
            py = Y2;
        }
        else if(pivotChoice == 3)
        {
            px = X3;
            py = Y3;
        }
        else if(pivotChoice == 4)
        {
            printf("Enter custom point (x y): ");
            scanf("%f%f", &px, &py);
        }
        else
        {
            printf("Invalid pivot choice!");
            getch();
            return;
        }
    }
    else
    {
        printf("Invalid scaling method!");
        getch();
        return;
    }

    printf("\nEnter scaling factors (Sx Sy): ");
    scanf("%f%f", &sx, &sy);

    // scaling transformation
    nx1 = X1 * sx + px * (1 - sx);
    ny1 = Y1 * sy + py * (1 - sy);

    nx2 = X2 * sx + px * (1 - sx);
    ny2 = Y2 * sy + py * (1 - sy);

    nx3 = X3 * sx + px * (1 - sx);
    ny3 = Y3 * sy + py * (1 - sy);

    X1 = nx1;
    Y1 = ny1;
    X2 = nx2;
    Y2 = ny2;
    X3 = nx3;
    Y3 = ny3;

    drawTriangle(CYAN);
    outtextxy(X3 + 5, Y3 + 5, "Scaled Triangle");
    getch();
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
    printf("4. Exit\n");

    while (1)
    {
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) translation();
        else if (choice == 2) rotation();
        else if (choice == 3) scaling();
        else if (choice == 4) break;
        else printf("Invalid choice!\n");
    }
    closegraph();
    return 0;
}