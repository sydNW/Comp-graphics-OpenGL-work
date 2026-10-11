#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

using namespace std;

// ======================================================
// WINDOW
// ======================================================

const int WIDTH = 800;
const int HEIGHT = 600;

// ======================================================
// CURRENT DRAWING
// ======================================================

int currentOption = 0;

int startX = 0;
int startY = 0;
int endX = 0;
int endY = 0;

int radius = 0;
int radiusX = 0;
int radiusY = 0;

// ======================================================
// CLIPPING WINDOW
// ======================================================

const int XMIN = -250;
const int XMAX = 250;
const int YMIN = -180;
const int YMAX = 180;

// ======================================================
// PLOT ONE PIXEL
// ======================================================

void plotPixel(int x, int y)
{
    glVertex2i(x, y);
}

// ======================================================
// DRAW X AND Y AXES
// ======================================================

void drawAxes()
{
    glColor3f(0.6f, 0.6f, 0.6f);

    glBegin(GL_LINES);

    // X axis
    glVertex2i(-400, 0);
    glVertex2i(400, 0);

    // Y axis
    glVertex2i(0, -300);
    glVertex2i(0, 300);

    glEnd();
}

// ======================================================
// 1. PLOT POINT
// ======================================================

void drawPoint(int x, int y)
{
    glColor3f(1.0f, 0.0f, 0.0f);

    glPointSize(8.0f);

    glBegin(GL_POINTS);

    plotPixel(x, y);

    glEnd();
}

// ======================================================
// 2. DDA LINE
// ======================================================

void drawDDA(int x1, int y1, int x2, int y2)
{
    glColor3f(0.0f, 0.0f, 1.0f);

    glPointSize(3.0f);

    int dx = x2 - x1;
    int dy = y2 - y1;

    int steps = max(abs(dx), abs(dy));

    if (steps == 0)
    {
        glBegin(GL_POINTS);
        plotPixel(x1, y1);
        glEnd();
        return;
    }

    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);

    for (int i = 0; i <= steps; i++)
    {
        plotPixel(round(x), round(y));

        x += xIncrement;
        y += yIncrement;
    }

    glEnd();
}

// ======================================================
// 3. BRESENHAM LINE
// ======================================================

void drawBresenham(int x1, int y1, int x2, int y2)
{
    glColor3f(0.0f, 0.6f, 0.0f);

    glPointSize(3.0f);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int error = dx - dy;

    glBegin(GL_POINTS);

    while (true)
    {
        plotPixel(x1, y1);

        if (x1 == x2 && y1 == y2)
        {
            break;
        }

        int e2 = 2 * error;

        if (e2 > -dy)
        {
            error -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            error += dx;
            y1 += sy;
        }
    }

    glEnd();
}

// ======================================================
// CIRCLE POINTS
// ======================================================

void plotCirclePoints(int xc, int yc, int x, int y)
{
    plotPixel(xc + x, yc + y);
    plotPixel(xc - x, yc + y);
    plotPixel(xc + x, yc - y);
    plotPixel(xc - x, yc - y);

    plotPixel(xc + y, yc + x);
    plotPixel(xc - y, yc + x);
    plotPixel(xc + y, yc - x);
    plotPixel(xc - y, yc - x);
}

// ======================================================
// 4. MIDPOINT CIRCLE
// ======================================================

void drawCircle(int xc, int yc, int r)
{
    glColor3f(1.0f, 0.0f, 1.0f);

    glPointSize(3.0f);

    int x = 0;
    int y = r;

    int p = 1 - r;

    glBegin(GL_POINTS);

    plotCirclePoints(xc, yc, x, y);

    while (x < y)
    {
        x++;

        if (p < 0)
        {
            p = p + 2 * x + 1;
        }
        else
        {
            y--;

            p = p + 2 * (x - y) + 1;
        }

        plotCirclePoints(xc, yc, x, y);
    }

    glEnd();
}

// ======================================================
// ELLIPSE POINTS
// ======================================================

void plotEllipsePoints(int xc, int yc, int x, int y)
{
    plotPixel(xc + x, yc + y);
    plotPixel(xc - x, yc + y);
    plotPixel(xc + x, yc - y);
    plotPixel(xc - x, yc - y);
}

// ======================================================
// 5. MIDPOINT ELLIPSE
// ======================================================

void drawEllipse(int xc, int yc, int rx, int ry)
{
    glColor3f(1.0f, 0.5f, 0.0f);

    glPointSize(3.0f);

    float x = 0;
    float y = ry;

    float rx2 = rx * rx;
    float ry2 = ry * ry;

    float dx = 2 * ry2 * x;
    float dy = 2 * rx2 * y;

    float p1 =
        ry2 - (rx2 * ry) + (0.25f * rx2);

    glBegin(GL_POINTS);

    // Region 1
    while (dx < dy)
    {
        plotEllipsePoints(
            xc,
            yc,
            round(x),
            round(y)
        );

        if (p1 < 0)
        {
            x++;

            dx += 2 * ry2;

            p1 += dx + ry2;
        }
        else
        {
            x++;
            y--;

            dx += 2 * ry2;
            dy -= 2 * rx2;

            p1 += dx - dy + ry2;
        }
    }

    float p2 =
        ry2 * (x + 0.5f) * (x + 0.5f)
        + rx2 * (y - 1) * (y - 1)
        - rx2 * ry2;

    // Region 2
    while (y >= 0)
    {
        plotEllipsePoints(
            xc,
            yc,
            round(x),
            round(y)
        );

        if (p2 > 0)
        {
            y--;

            dy -= 2 * rx2;

            p2 += rx2 - dy;
        }
        else
        {
            y--;
            x++;

            dx += 2 * ry2;
            dy -= 2 * rx2;

            p2 += dx - dy + rx2;
        }
    }

    glEnd();
}

// ======================================================
// COHEN-SUTHERLAND
// ======================================================

const int INSIDE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int BOTTOM = 4;
const int TOP = 8;

int computeCode(double x, double y)
{
    int code = INSIDE;

    if (x < XMIN)
        code |= LEFT;

    else if (x > XMAX)
        code |= RIGHT;

    if (y < YMIN)
        code |= BOTTOM;

    else if (y > YMAX)
        code |= TOP;

    return code;
}

// ======================================================
// COHEN-SUTHERLAND CLIPPING
// ======================================================

bool clipLine(
    double &x1,
    double &y1,
    double &x2,
    double &y2)
{
    int code1 = computeCode(x1, y1);
    int code2 = computeCode(x2, y2);

    bool accept = false;

    while (true)
    {
        if (code1 == 0 && code2 == 0)
        {
            accept = true;
            break;
        }

        else if (code1 & code2)
        {
            break;
        }

        else
        {
            double x;
            double y;

            int codeOut;

            if (code1 != 0)
                codeOut = code1;
            else
                codeOut = code2;

            // TOP
            if (codeOut & TOP)
            {
                x = x1 +
                    (x2 - x1) *
                    (YMAX - y1) /
                    (y2 - y1);

                y = YMAX;
            }

            // BOTTOM
            else if (codeOut & BOTTOM)
            {
                x = x1 +
                    (x2 - x1) *
                    (YMIN - y1) /
                    (y2 - y1);

                y = YMIN;
            }

            // RIGHT
            else if (codeOut & RIGHT)
            {
                y = y1 +
                    (y2 - y1) *
                    (XMAX - x1) /
                    (x2 - x1);

                x = XMAX;
            }

            // LEFT
            else
            {
                y = y1 +
                    (y2 - y1) *
                    (XMIN - x1) /
                    (x2 - x1);

                x = XMIN;
            }

            if (codeOut == code1)
            {
                x1 = x;
                y1 = y;

                code1 = computeCode(x1, y1);
            }
            else
            {
                x2 = x;
                y2 = y;

                code2 = computeCode(x2, y2);
            }
        }
    }

    return accept;
}

// ======================================================
// CLIPPING RECTANGLE
// ======================================================

void drawClippingRectangle()
{
    glColor3f(0.0f, 0.0f, 0.0f);

    glLineWidth(2.0f);

    glBegin(GL_LINE_LOOP);

    glVertex2i(XMIN, YMIN);
    glVertex2i(XMAX, YMIN);
    glVertex2i(XMAX, YMAX);
    glVertex2i(XMIN, YMAX);

    glEnd();
}

// ======================================================
// DRAW CLIPPED LINE
// ======================================================

void drawClippedLine(
    int x1,
    int y1,
    int x2,
    int y2)
{
    glColor3f(0.0f, 0.0f, 1.0f);

    glPointSize(4.0f);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int steps = max(dx, dy);

    glBegin(GL_POINTS);

    if (steps == 0)
    {
        plotPixel(x1, y1);
    }
    else
    {
        for (int i = 0; i <= steps; i++)
        {
            float t = (float)i / steps;

            int px =
                round(x1 + t * (x2 - x1));

            int py =
                round(y1 + t * (y2 - y1));

            plotPixel(px, py);
        }
    }

    glEnd();
}

// ======================================================
// DISPLAY
// ======================================================

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // Draw axes
    drawAxes();

    // 1. Point
    if (currentOption == 1)
    {
        drawPoint(startX, startY);
    }

    // 2. DDA
    else if (currentOption == 2)
    {
        drawDDA(
            startX,
            startY,
            endX,
            endY
        );
    }

    // 3. Bresenham
    else if (currentOption == 3)
    {
        drawBresenham(
            startX,
            startY,
            endX,
            endY
        );
    }

    // 4. Circle
    else if (currentOption == 4)
    {
        drawCircle(
            startX,
            startY,
            radius
        );
    }

    // 5. Ellipse
    else if (currentOption == 5)
    {
        drawEllipse(
            startX,
            startY,
            radiusX,
            radiusY
        );
    }

    // 6. Cohen-Sutherland
    else if (currentOption == 6)
    {
        // Original line - RED
        glColor3f(1.0f, 0.0f, 0.0f);

        glLineWidth(2.0f);

        glBegin(GL_LINES);

        glVertex2i(startX, startY);
        glVertex2i(endX, endY);

        glEnd();

        // Clipping rectangle
        drawClippingRectangle();

        // Make a copy for clipping
        double cx1 = startX;
        double cy1 = startY;

        double cx2 = endX;
        double cy2 = endY;

        // Clip the line
        if (clipLine(
                cx1,
                cy1,
                cx2,
                cy2))
        {
            drawClippedLine(
                round(cx1),
                round(cy1),
                round(cx2),
                round(cy2)
            );
        }
    }

    glutSwapBuffers();
}

// ======================================================
// SHOW MENU
// ======================================================

void showMenu()
{
    cout << endl;
    cout << "====================================" << endl;
    cout << "       COMPUTER GRAPHICS MENU" << endl;
    cout << "====================================" << endl;

    cout << "1. Plot Point" << endl;
    cout << "2. Draw Line using DDA" << endl;
    cout << "3. Draw Line using Bresenham" << endl;
    cout << "4. Draw Circle using Midpoint Algorithm" << endl;
    cout << "5. Draw Ellipse using Midpoint Algorithm" << endl;
    cout << "6. Clip Line using Cohen-Sutherland" << endl;
    cout << "7. Exit" << endl;

    cout << "====================================" << endl;
}

// ======================================================
// GET INPUT FROM USER
// ======================================================

void getInput(int choice)
{
    if (choice == 1)
    {
        cout << "\nEnter point coordinates (x y): ";

        cin >> startX >> startY;
    }

    else if (choice == 2)
    {
        cout << "\nEnter line coordinates (x1 y1 x2 y2): ";

        cin >>
            startX >>
            startY >>
            endX >>
            endY;
    }

    else if (choice == 3)
    {
        cout << "\nEnter line coordinates (x1 y1 x2 y2): ";

        cin >>
            startX >>
            startY >>
            endX >>
            endY;
    }

    else if (choice == 4)
    {
        cout << "\nEnter circle center (x y): ";

        cin >>
            startX >>
            startY;

        cout << "Enter radius: ";

        cin >> radius;
    }

    else if (choice == 5)
    {
        cout << "\nEnter ellipse center (x y): ";

        cin >>
            startX >>
            startY;

        cout << "Enter X radius and Y radius: ";

        cin >>
            radiusX >>
            radiusY;
    }

    else if (choice == 6)
    {
        cout << "\nEnter line coordinates (x1 y1 x2 y2): ";

        cin >>
            startX >>
            startY >>
            endX >>
            endY;
    }
}

// ======================================================
// KEYBOARD
// ======================================================

void keyboard(unsigned char key, int x, int y)
{
    // Press M to return to menu
    if (key == 'm' || key == 'M')
    {
        currentOption = 0;

        glutPostRedisplay();

        cout << endl;
        showMenu();

        cout << "Enter option (1-7): ";

        int choice;

        cin >> choice;

        if (choice == 7)
        {
            exit(0);
        }

        if (choice >= 1 && choice <= 6)
        {
            currentOption = choice;

            getInput(choice);

            glutPostRedisplay();

            cout << endl;
            cout << "Drawing displayed." << endl;
            cout << "Press M in the graphics window to return to the menu." << endl;
        }
        else
        {
            cout << "Invalid option." << endl;
        }
    }

    // Press ESC to exit
    if (key == 27)
    {
        exit(0);
    }
}

// ======================================================
// MAIN
// ======================================================

int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);

    glutInitDisplayMode(
        GLUT_DOUBLE |
        GLUT_RGB
    );

    glutInitWindowSize(
        WIDTH,
        HEIGHT
    );

    glutInitWindowPosition(
        100,
        100
    );

    // Create window
    glutCreateWindow(
        "Computer Graphics Assignment"
    );

    // White background
    glClearColor(
        1.0f,
        1.0f,
        1.0f,
        1.0f
    );

    // Coordinate system
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluOrtho2D(
        -400,
        400,
        -300,
        300
    );

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    // Register display function
    glutDisplayFunc(display);

    // Register keyboard function
    glutKeyboardFunc(keyboard);

    // Show menu in terminal
    showMenu();

    cout << "\nEnter option (1-7): ";

    int choice;

    cin >> choice;

    if (choice == 7)
    {
        return 0;
    }

    if (choice >= 1 && choice <= 6)
    {
        currentOption = choice;

        getInput(choice);

        glutPostRedisplay();

        cout << endl;
        cout << "Drawing displayed in the OpenGL window." << endl;
        cout << "Press M in the graphics window to return to the menu." << endl;
    }
    else
    {
        cout << "Invalid option." << endl;

        return 0;
    }

    // Start OpenGL
    glutMainLoop();

    return 0;
}