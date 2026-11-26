// Q5 - Fractal generator (OpenGL + freeglut)
// Three generation methods, each shown growing step by step:
//   [1] Koch snowflake   - recursive geometric subdivision (level 0..6)
//   [2] Barnsley fern    - iterated function system (points accumulate)
//   [3] Mandelbrot set   - escape-time algorithm (iteration limit grows)
// Keys: [1][2][3] fractal  [+/-] level / speed  [a] auto-generate on/off  [r] restart
//       Mandelbrot: left click = zoom in at point, right click = zoom out   [Esc] quit
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <vector>

const int W = 640, H = 640;
int mode = 1;                 // which fractal
bool autoGen = true;          // advance the generation automatically
int frame = 0;                // frames since generation started

// ---------------- Koch snowflake ----------------
int kochLevel = 0;
std::vector<float> kochPts;   // x,y pairs of the outline
void kochSegment(float x1, float y1, float x2, float y2, int level) {
    if (level == 0) { kochPts.push_back(x1); kochPts.push_back(y1); return; }
    float dx = (x2 - x1) / 3, dy = (y2 - y1) / 3;
    float ax = x1 + dx, ay = y1 + dy;                 // 1/3 point
    float bx = x1 + 2 * dx, by = y1 + 2 * dy;         // 2/3 point
    float px = ax + dx * 0.5f - dy * 0.8660254f;      // peak: middle third rotated by +60 degrees
    float py = ay + dy * 0.5f + dx * 0.8660254f;
    kochSegment(x1, y1, ax, ay, level - 1);
    kochSegment(ax, ay, px, py, level - 1);
    kochSegment(px, py, bx, by, level - 1);
    kochSegment(bx, by, x2, y2, level - 1);
}
void buildKoch() {
    kochPts.clear();
    float r = 0.8f, a[3];
    for (int i = 0; i < 3; i++) a[i] = 3.14159265f / 2 + i * 2 * 3.14159265f / 3;   // equilateral triangle
    int order[3] = {0, 2, 1};  // clockwise order so the bumps point outward
    for (int k = 0; k < 3; k++) {
        int i = order[k], j = order[(k + 1) % 3];
        kochSegment(r * cosf(a[i]), r * sinf(a[i]) - 0.1f, r * cosf(a[j]), r * sinf(a[j]) - 0.1f, kochLevel);
    }
}
void drawKoch() {
    // Concave fill with the stencil buffer (even-odd rule): every pixel covered an odd
    // number of times by the triangle fan is inside the snowflake.
    glClear(GL_STENCIL_BUFFER_BIT); glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 0, 1); glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
    glBegin(GL_TRIANGLE_FAN); glVertex2f(kochPts[0], kochPts[1]);
    for (size_t i = 2; i < kochPts.size(); i += 2) glVertex2f(kochPts[i], kochPts[i + 1]);
    glEnd();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, 1, 1); glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColor3f(0.75f, 0.88f, 1.0f); glRectf(-1, -1, 1, 1);
    glDisable(GL_STENCIL_TEST);
    glColor3f(0.1f, 0.25f, 0.6f); glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < kochPts.size(); i += 2) glVertex2f(kochPts[i], kochPts[i + 1]);
    glEnd();
}

// ---------------- Barnsley fern (IFS) ----------------
long fernCount = 0;
unsigned int rngState = 12345;
float rnd() { rngState = rngState * 1664525u + 1013904223u; return (rngState >> 8) / 16777216.0f; }
std::vector<float> fernPts;
void growFern(int n) {        // apply one of 4 affine maps chosen with fixed probabilities
    static float x = 0, y = 0;
    if (fernPts.empty()) { x = y = 0; rngState = 12345; }
    for (int i = 0; i < n; i++) {
        float r = rnd(), nx, ny;
        if (r < 0.01f)      { nx = 0;                    ny = 0.16f * y; }                  // stem
        else if (r < 0.86f) { nx = 0.85f * x + 0.04f * y; ny = -0.04f * x + 0.85f * y + 1.6f; }  // smaller copy
        else if (r < 0.93f) { nx = 0.2f * x - 0.26f * y;  ny = 0.23f * x + 0.22f * y + 1.6f; }   // left leaflet
        else                { nx = -0.15f * x + 0.28f * y; ny = 0.26f * x + 0.24f * y + 0.44f; } // right leaflet
        x = nx; y = ny;
        fernPts.push_back(x / 5.5f); fernPts.push_back(y / 5.5f - 0.95f);   // fit into the window
    }
    fernCount = fernPts.size() / 2;
}
void drawFern() {
    glPointSize(1); glColor3f(0.1f, 0.55f, 0.2f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < fernPts.size(); i += 2) glVertex2f(fernPts[i], fernPts[i + 1]);
    glEnd();
}

// ---------------- Mandelbrot set (escape time) ----------------
int maxIter = 8;
double cx = -0.6, cy = 0.0, span = 3.0;         // view centre and width in the complex plane
std::vector<unsigned char> img(W * H * 3);
void palette(double v, unsigned char* p) {        // smooth blue-orange colour map
    p[0] = (unsigned char)(255 * (0.5 + 0.5 * cos(6.2832 * (v + 0.0))));
    p[1] = (unsigned char)(255 * (0.5 + 0.5 * cos(6.2832 * (v + 0.1))));
    p[2] = (unsigned char)(255 * (0.5 + 0.5 * cos(6.2832 * (v + 0.2))));
}
void computeMandelbrot() {
    for (int j = 0; j < H; j++)
        for (int i = 0; i < W; i++) {
            double x0 = cx + (i - W / 2.0) * span / W, y0 = cy + (j - H / 2.0) * span / W;
            double x = 0, y = 0; int n = 0;
            while (x * x + y * y <= 4 && n < maxIter) { double xt = x * x - y * y + x0; y = 2 * x * y + y0; x = xt; n++; }  // z = z^2 + c
            unsigned char* p = &img[(j * W + i) * 3];
            if (n == maxIter) { p[0] = p[1] = p[2] = 0; }          // inside the set
            else {
                double smoothN = n + 1 - log(log(sqrt(x * x + y * y))) / log(2.0);   // continuous colouring
                palette(0.5 + smoothN * 0.02, p);
            }
        }
}
void drawMandelbrot() { glRasterPos2f(-1, -1); glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, img.data()); }

// ---------------- generation control ----------------
void restart() {
    frame = 0;
    if (mode == 1) { kochLevel = 0; buildKoch(); }
    if (mode == 2) { fernPts.clear(); growFern(200); }
    if (mode == 3) { maxIter = 8; computeMandelbrot(); }
}
void step() {                                     // one generation step
    if (mode == 1 && kochLevel < 6) { kochLevel++; buildKoch(); }
    if (mode == 2 && fernCount < 200000) growFern((int)(fernCount * 0.6) + 200);
    if (mode == 3 && maxIter < 512) { maxIter *= 2; computeMandelbrot(); }
}

void text(float x, float y, const char* s) { glRasterPos2f(x, y); glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)s); }
void display() {
    glClearColor(1, 1, 1, 1); glClear(GL_COLOR_BUFFER_BIT);
    char buf[160];
    if (mode == 1) {
        drawKoch();
        long segs = 3; for (int i = 0; i < kochLevel; i++) segs *= 4;
        snprintf(buf, sizeof buf, "Koch snowflake   level %d   segments = 3x4^%d = %ld   perimeter = %.2f x P0",
                 kochLevel, kochLevel, segs, pow(4.0 / 3.0, kochLevel));
    } else if (mode == 2) {
        drawFern();
        snprintf(buf, sizeof buf, "Barnsley fern (IFS, 4 affine maps)   points = %ld", fernCount);
    } else {
        drawMandelbrot();
        snprintf(buf, sizeof buf, "Mandelbrot  z = z^2 + c   max iterations = %d   width = %.2e", maxIter, span);
    }
    if (mode == 3) {                              // dark bands keep the labels readable
        glColor3f(0, 0, 0); glRectf(-1, 0.9f, 1, 1); glRectf(-1, -1, 1, -0.91f);
        glColor3f(1, 1, 1);
    } else glColor3f(0, 0, 0);
    text(-0.97f, 0.93f, buf);
    text(-0.97f, -0.97f, "[1] Koch  [2] fern  [3] Mandelbrot  [+/-] step  [a] auto  [r] restart");
    glutSwapBuffers();
}
void timer(int) {
    if (autoGen && ++frame % 60 == 0) step();     // one generation step per second
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}
void keyboard(unsigned char k, int, int) {
    if (k >= '1' && k <= '3') { mode = k - '0'; restart(); }
    if (k == '+') step();
    if (k == '-' && mode == 1 && kochLevel > 0) { kochLevel--; buildKoch(); }
    if (k == '-' && mode == 3 && maxIter > 8) { maxIter /= 2; computeMandelbrot(); }
    if (k == 'a') autoGen = !autoGen;
    if (k == 'r') restart();
    if (k == 27) exit(0);
}
void mouse(int button, int state, int x, int y) {
    if (mode != 3 || state != GLUT_DOWN) return;
    cx += (x - W / 2.0) * span / W; cy += (H / 2.0 - y) * span / W;   // recentre on the click
    span *= (button == GLUT_LEFT_BUTTON) ? 0.4 : 2.5;
    computeMandelbrot();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
    glutInitWindowSize(W, H);
    glutCreateWindow("Q5 - Fractal generator");
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    restart();
    glutDisplayFunc(display); glutKeyboardFunc(keyboard); glutMouseFunc(mouse);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
