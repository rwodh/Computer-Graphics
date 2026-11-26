// Q1 - The four steps of the animation process, implemented in OpenGL (freeglut)
// Step 1: storyboard   Step 2: object definitions
// Step 3: key frames   Step 4: generation of in-between frames
// Keys: [k] key-frame view  [o] in-between (onion-skin) view  [p] play view
//       [space] pause  [r] restart  [Esc] quit
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>   // exit()

const float PI = 3.14159265f;
float t = 0.0f;            // animation clock in seconds
bool  paused = false;
int   viewMode = 0;        // 0 = play, 1 = key frames, 2 = in-betweens
float gAlpha = 1.0f;       // transparency used for "ghost" drawings

// ---------------- STEP 1: STORYBOARD ----------------
// The action is broken into shots, each with a time range and a description.
struct Shot { float start, end; const char* text; };
Shot storyboard[] = {
    {0.0f, 2.0f, "Shot 1: the car drives in from the left and slows down"},
    {2.0f, 3.5f, "Shot 2: it brakes (nose dips) and waits at the red light"},
    {3.5f, 4.6f, "Shot 3: light turns green, car leans back (anticipation)"},
    {4.6f, 7.0f, "Shot 4: the car speeds away to the right"}};
const int   NSHOTS = 4;
const float DURATION = 7.0f;

// ---------------- STEP 2: OBJECT DEFINITIONS ----------------
// Every object is a function of its animatable parameters.
void col(float r, float g, float b) { glColor4f(r, g, b, gAlpha); }
void rect(float x0, float y0, float x1, float y1) { glRectf(x0, y0, x1, y1); }
void circle(float cx, float cy, float r, int seg = 40) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) {
        float a = 2 * PI * (float)i / (float)seg;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}
void text(float x, float y, const char* s, void* font = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2f(x, y);
    glutBitmapString(font, (const unsigned char*)s);
}
const float GROUND = -0.35f, WHEEL_R = 0.08f;

void drawWheel(float x, float angleDeg) {
    glPushMatrix();
    glTranslatef(x, 0, 0);
    glRotatef(angleDeg, 0, 0, 1);
    col(0.1f, 0.1f, 0.1f); circle(0, 0, WHEEL_R);
    col(0.75f, 0.75f, 0.8f); circle(0, 0, WHEEL_R * 0.55f);
    col(0.3f, 0.3f, 0.3f);
    glLineWidth(2);
    glBegin(GL_LINES);                          // spokes make the rotation visible
    for (int i = 0; i < 3; i++) {
        float a = (float)i * PI / 3;
        glVertex2f(-0.045f * cosf(a), -0.045f * sinf(a));
        glVertex2f(0.045f * cosf(a), 0.045f * sinf(a));
    }
    glEnd();
    glPopMatrix();
}

// Car parameters: x position, body tilt (degrees), wheel angle (degrees)
void drawCar(float x, float tilt, float wheelAngle) {
    glPushMatrix();
    glTranslatef(x, GROUND + WHEEL_R, 0);       // hierarchical model: car -> body, wheels
    glPushMatrix();
    glRotatef(tilt, 0, 0, 1);
    col(0.85f, 0.15f, 0.15f); rect(-0.26f, 0.0f, 0.26f, 0.12f);     // lower body
    col(0.85f, 0.15f, 0.15f); rect(-0.14f, 0.12f, 0.12f, 0.23f);    // cabin
    col(0.65f, 0.85f, 1.0f);  rect(-0.11f, 0.14f, -0.02f, 0.21f);   // windows
    rect(0.01f, 0.14f, 0.09f, 0.21f);
    col(1.0f, 0.95f, 0.4f);   rect(0.23f, 0.05f, 0.26f, 0.09f);     // head lamp
    glPopMatrix();
    drawWheel(-0.15f, wheelAngle);
    drawWheel(0.15f, wheelAngle);
    glPopMatrix();
}

void drawTrafficLight(bool green) {
    col(0.3f, 0.3f, 0.3f); rect(0.36f, GROUND, 0.38f, 0.15f);       // pole
    col(0.12f, 0.12f, 0.12f); rect(0.32f, 0.15f, 0.42f, 0.42f);      // box
    green ? col(0.3f, 0.05f, 0.05f) : col(1.0f, 0.15f, 0.1f);
    circle(0.37f, 0.35f, 0.035f);
    green ? col(0.2f, 1.0f, 0.3f) : col(0.05f, 0.25f, 0.05f);
    circle(0.37f, 0.22f, 0.035f);
}

void drawBackground() {
    glBegin(GL_QUADS);                                                // sky gradient
    glColor3f(0.55f, 0.8f, 1.0f); glVertex2f(-1.6f, 0.8f); glVertex2f(1.6f, 0.8f);
    glColor3f(0.85f, 0.95f, 1.0f); glVertex2f(1.6f, GROUND); glVertex2f(-1.6f, GROUND);
    glEnd();
    glColor3f(1.0f, 0.9f, 0.3f); circle(1.25f, 0.55f, 0.1f);          // sun
    glColor3f(0.35f, 0.7f, 0.3f); rect(-1.6f, -0.8f, 1.6f, GROUND);   // grass
    glColor3f(0.35f, 0.35f, 0.38f); rect(-1.6f, -0.5f, 1.6f, GROUND); // road
    glColor3f(1, 1, 1);
    for (float x = -1.6f; x < 1.6f; x += 0.3f) rect(x, -0.43f, x + 0.15f, -0.415f);
}

// ---------------- STEP 3: KEY-FRAME SPECIFICATION ----------------
// Each animated parameter has its own list of (time, value) keys.
struct Key { float time, value; };
Key carX[] = {{0.0f, -1.3f}, {2.0f, -0.15f}, {4.4f, -0.15f}, {7.0f, 1.3f}};
Key carTilt[] = {{0.0f, 0}, {1.8f, 0}, {2.1f, -6}, {2.5f, 0}, {3.6f, 0},
                 {4.2f, 5}, {4.6f, -2}, {5.0f, 0}, {7.0f, 0}};
const int NX = 4, NTILT = 9;
const float LIGHT_GREEN_AT = 3.5f;     // a "step" (hold) key: no interpolation

// ---------------- STEP 4: IN-BETWEEN GENERATION ----------------
float easeInOut(float u) { return u * u * (3 - 2 * u); }   // smoothstep: slow in, slow out
float interpolate(const Key* k, int n, float time) {
    if (time <= k[0].time) return k[0].value;
    for (int i = 0; i < n - 1; i++)
        if (time <= k[i + 1].time) {
            float u = (time - k[i].time) / (k[i + 1].time - k[i].time);
            return k[i].value + (k[i + 1].value - k[i].value) * easeInOut(u);
        }
    return k[n - 1].value;
}
// Draws the car exactly as it looks at a given time (one generated frame).
void carAt(float time) {
    float x = interpolate(carX, NX, time);
    float wheel = -(x / WHEEL_R) * 180.0f / PI;    // rolling without slipping
    drawCar(x, interpolate(carTilt, NTILT, time), wheel);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();
    gAlpha = 1.0f;
    drawTrafficLight(t >= LIGHT_GREEN_AT);
    char buf[128];

    if (viewMode == 1) {                         // show the key frames only
        gAlpha = 0.45f;
        for (int i = 0; i < NX; i++) carAt(carX[i].time);
        gAlpha = 1.0f;
        glColor3f(0.1f, 0.1f, 0.4f);
        for (int i = 0; i < NX; i++) {          // label every key pose
            snprintf(buf, sizeof buf, "K%d (t=%.1fs)", i, carX[i].time);
            text(carX[i].value - 0.17f, 0.02f + (i == 2 ? 0.07f : 0.0f), buf);
        }
        glColor3f(0, 0, 0);
        text(-1.55f, -0.6f, "Position keys:");
        for (int i = 0; i < NX; i++) {
            snprintf(buf, sizeof buf, "K%d  t=%.1fs  x=%.2f", i, carX[i].time, carX[i].value);
            text(-1.55f + 0.78f * (float)i, -0.68f, buf);
        }
        text(-1.55f, -0.76f, "Tilt keys: 9 (brake dip at 2.1 s, lean back at 4.2 s)   Light: step key at 3.5 s");
    } else if (viewMode == 2) {                  // onion skin of generated in-betweens
        gAlpha = 0.22f;
        for (float ft = 0; ft <= DURATION; ft += 0.25f) carAt(ft);
        gAlpha = 1.0f;
        carAt(t);
        glColor3f(0, 0, 0);
        text(-1.55f, -0.68f, "Ghosts = in-between frames every 0.25 s. Close spacing = slow (ease in/out), wide spacing = fast.");
    } else {
        carAt(t);
    }

    int shot = 0;                                // caption from the storyboard
    for (int i = 0; i < NSHOTS; i++) if (t >= storyboard[i].start) shot = i;
    glColor3f(0.05f, 0.05f, 0.25f);
    text(-1.55f, 0.72f, storyboard[shot].text, GLUT_BITMAP_HELVETICA_18);
    const char* modeName[] = {"play", "key frames", "in-betweens"};
    snprintf(buf, sizeof buf, "t = %.2f s   frame %d @ 60 fps   view: %s", t, (int)(t * 60), modeName[viewMode]);
    text(-1.55f, 0.63f, buf);
    if (viewMode == 0) {
        glColor3f(0, 0, 0);
        text(-1.55f, -0.72f, "[k] key frames   [o] in-betweens   [p] play   [space] pause   [r] restart");
    }
    glutSwapBuffers();
}

void timer(int) {
    if (!paused) { t += 1.0f / 60.0f; if (t > DURATION) t = 0; }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case ' ': paused = !paused; break;
        case 'r': t = 0; break;
        case 'k': viewMode = 1; break;
        case 'o': viewMode = 2; break;
        case 'p': viewMode = 0; break;
        case 27: exit(0);
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(-1.6, 1.6, -0.8, 0.8);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1000, 500);
    glutCreateWindow("Q1 - Animation process: storyboard, objects, key frames, in-betweens");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
