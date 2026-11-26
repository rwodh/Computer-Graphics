// Q2 - The twelve principles of animation, each demonstrated in its own viewport (OpenGL + freeglut)
// Faded "ghost" copies show earlier frames so the motion is visible in a still image.
// Keys: [t] toggle motion trails  [space] pause  [Esc] quit
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>

const float PI = 3.14159265f;
float t = 0.0f, gAlpha = 1.0f;
bool paused = false, trails = true;
const float FLOOR_Y = -0.7f;

// ---------- drawing helpers ----------
void col(float r, float g, float b) { glColor4f(r, g, b, gAlpha); }
void ellipse(float cx, float cy, float rx, float ry, int seg = 40) {
    glBegin(GL_TRIANGLE_FAN); glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) { float a = 2 * PI * i / seg; glVertex2f(cx + rx * cosf(a), cy + ry * sinf(a)); }
    glEnd();
}
void circle(float cx, float cy, float r) { ellipse(cx, cy, r, r); }
void line(float x0, float y0, float x1, float y1) { glBegin(GL_LINES); glVertex2f(x0, y0); glVertex2f(x1, y1); glEnd(); }
void text(float x, float y, const char* s, void* f = GLUT_BITMAP_HELVETICA_12) { glRasterPos2f(x, y); glutBitmapString(f, (const unsigned char*)s); }
void floorLine(float y = FLOOR_Y) { glColor3f(0.4f, 0.4f, 0.45f); glLineWidth(2); line(-0.95f, y, 0.95f, y); }
float smooth(float u) { u = u < 0 ? 0 : (u > 1 ? 1 : u); return u * u * (3 - 2 * u); }
float wrap(float x, float p) { return fmodf(fmodf(x, p) + p, p); }
// A simple blob character: body ellipse with two eyes, squash/stretch via sx, sy
void blob(float x, float y, float sx, float sy, float r, float g, float b) {
    col(r, g, b); ellipse(x, y + 0.18f * sy, 0.18f * sx, 0.18f * sy);
    col(1, 1, 1); circle(x - 0.06f * sx, y + 0.23f * sy, 0.045f); circle(x + 0.06f * sx, y + 0.23f * sy, 0.045f);
    col(0, 0, 0); circle(x - 0.05f * sx, y + 0.23f * sy, 0.02f); circle(x + 0.07f * sx, y + 0.23f * sy, 0.02f);
}
// Height of a bouncing ball: parabola repeating every period
float bounceY(float time, float period, float height) { float u = wrap(time, period) / period; return height * 4 * u * (1 - u); }

// ---------- 1. Squash and stretch ----------
void p1(float tt) {
    float T = 1.0f, u = wrap(tt, T) / T, sx = 1, sy = 1;
    float contact = fminf(u, 1 - u);
    if (contact < 0.08f) { float a = 1 - contact / 0.08f; sy = 1 - 0.35f * a; sx = 1 / sy; }         // squash on impact
    else { float speed = fabsf(1 - 2 * u); sy = 1 + 0.35f * speed; sx = 1 / sy; }                     // stretch when fast
    float x = -0.75f + 1.5f * wrap(tt, 3 * T) / (3 * T);
    float r = 0.15f;
    col(0.95f, 0.45f, 0.1f); ellipse(x, FLOOR_Y + r * sy + bounceY(tt, T, 1.1f), r * sx, r * sy);
}
// ---------- 2. Anticipation ----------
void p2(float tt) {
    float u = wrap(tt, 2.4f), sx = 1, sy = 1, x = -0.5f, y = FLOOR_Y;
    if (u > 0.6f && u < 1.1f) { float a = smooth((u - 0.6f) / 0.5f); sy = 1 - 0.4f * a; sx = 1 + 0.3f * a; x -= 0.12f * a; } // crouch & pull back
    else if (u >= 1.1f && u < 1.9f) { float v = (u - 1.1f) / 0.8f; x = -0.62f + 1.1f * v; y += 1.0f * 4 * v * (1 - v); sy = 1.25f; sx = 0.85f; }
    else if (u >= 1.9f) { x = 0.48f; float a = 1 - smooth((u - 1.9f) / 0.5f); sy = 1 - 0.25f * a; sx = 1 + 0.25f * a; }
    blob(x, y, sx, sy, 0.3f, 0.6f, 0.95f);
}
void d2() { glColor3f(0.3f, 0.3f, 0.3f); text(-0.85f, -0.85f, "crouch + pull back -> jump"); }
// ---------- 3. Staging ----------
void d3() {
    glColor3f(0.12f, 0.12f, 0.18f); glRectf(-1, -1, 1, 1);
    glColor4f(1, 1, 0.7f, 0.25f);                                    // spotlight cone
    glBegin(GL_TRIANGLES); glVertex2f(0, 1); glVertex2f(-0.35f, FLOOR_Y); glVertex2f(0.35f, FLOOR_Y); glEnd();
    glColor4f(1, 1, 0.7f, 0.35f); ellipse(0, FLOOR_Y, 0.35f, 0.06f);
}
void p3(float tt) {
    blob(-0.7f, FLOOR_Y, 0.8f, 0.8f, 0.3f, 0.3f, 0.35f); blob(0.7f, FLOOR_Y, 0.8f, 0.8f, 0.3f, 0.3f, 0.35f);   // dim extras
    float bob = 0.03f * sinf(tt * 6);
    blob(0, FLOOR_Y + bob, 1.2f, 1.2f, 1.0f, 0.8f, 0.1f);                                                    // the star
    col(1.0f, 0.8f, 0.1f); glLineWidth(6);
    float a = 0.6f * sinf(tt * 5);                                                                           // waving arm
    line(0.2f, FLOOR_Y + 0.25f, 0.2f + 0.25f * cosf(0.9f + a), FLOOR_Y + 0.25f + 0.25f * sinf(0.9f + a));
}
// ---------- 4. Straight-ahead action vs pose-to-pose ----------
void p4(float tt) {
    // straight ahead: each frame is computed from the previous one (physics stepping)
    float u = wrap(tt, 2.2f), x = -0.85f, y = 0.6f, vx = 0.75f, vy = 0, dt = 1.0f / 120;
    for (float s = 0; s < u; s += dt) { vy -= 3.5f * dt; x += vx * dt; y += vy * dt; if (y < 0.1f) { y = 0.1f; vy = -0.7f * vy; } }
    col(0.9f, 0.3f, 0.3f); circle(x, y + 0.06f, 0.06f);
    // pose to pose: the animator sets key poses, the computer fills the in-betweens
    float kx[] = {-0.85f, -0.1f, 0.6f}, ky[] = {-0.3f, -0.85f, -0.4f};
    float v = fminf(u / 1.6f, 1.0f) * 2; int i = v >= 2 ? 1 : (int)v; float w = smooth(v - i);
    col(0.2f, 0.5f, 0.9f); circle(kx[i] + (kx[i + 1] - kx[i]) * w, ky[i] + (ky[i + 1] - ky[i]) * w, 0.06f);
}
void d4() {
    floorLine(0.1f); glColor3f(0.3f, 0.3f, 0.3f);
    text(-0.9f, 0.72f, "straight ahead (simulated)"); text(-0.9f, -0.05f, "pose to pose (3 keys)");
    float kx[] = {-0.85f, -0.1f, 0.6f}, ky[] = {-0.3f, -0.85f, -0.4f};
    glLineWidth(1.5f);
    for (int i = 0; i < 3; i++) {
        glColor3f(0.1f, 0.3f, 0.7f);
        glBegin(GL_LINE_LOOP); glVertex2f(kx[i] - .08f, ky[i] - .08f); glVertex2f(kx[i] + .08f, ky[i] - .08f);
        glVertex2f(kx[i] + .08f, ky[i] + .08f); glVertex2f(kx[i] - .08f, ky[i] + .08f); glEnd();
        char b[8]; snprintf(b, 8, "K%d", i + 1); text(kx[i] - 0.05f, ky[i] + 0.11f, b);
    }
}
// ---------- 5. Follow-through and overlapping action ----------
float boxX(float tt) {                       // box moves right, stops, moves back, stops
    float u = wrap(tt, 3.0f);
    if (u < 1.0f) return -0.6f + 1.2f * smooth(u);
    if (u < 1.5f) return 0.6f;
    if (u < 2.5f) return 0.6f - 1.2f * smooth(u - 1.5f);
    return -0.6f;
}
void p5(float tt) {
    // the antenna is a chain of damped springs driven by the box's acceleration (simulated)
    float ang[4] = {0}, vel[4] = {0}, dt = 1.0f / 240, start = tt - wrap(tt, 3.0f) - 3.0f;
    for (float s = start; s < tt; s += dt) {
        float acc = (boxX(s + dt) - 2 * boxX(s) + boxX(s - dt)) / (dt * dt);
        for (int i = 0; i < 4; i++) {
            float parent = i == 0 ? 0 : ang[i - 1];
            float force = -60 * (ang[i] - parent) - 3.0f * vel[i] + (i == 0 ? 2.5f * acc : 0);
            vel[i] += force * dt; ang[i] += vel[i] * dt;
        }
    }
    float x = boxX(tt), px = x, py = FLOOR_Y + 0.35f;
    col(0.55f, 0.35f, 0.75f); glRectf(x - 0.2f, FLOOR_Y, x + 0.2f, FLOOR_Y + 0.35f);
    col(1, 1, 1); circle(x - 0.07f, FLOOR_Y + 0.24f, 0.04f); circle(x + 0.07f, FLOOR_Y + 0.24f, 0.04f);
    col(0.3f, 0.2f, 0.4f); glLineWidth(4);
    glBegin(GL_LINE_STRIP); glVertex2f(px, py);
    for (int i = 0; i < 4; i++) { px += 0.12f * sinf(ang[i]); py += 0.12f * cosf(ang[i]); glVertex2f(px, py); }
    glEnd();
    col(1, 0.3f, 0.3f); circle(px, py, 0.05f);
}
// ---------- 6. Slow in and slow out ----------
void d6() {
    glColor3f(0.3f, 0.3f, 0.3f); text(-0.9f, 0.5f, "linear (no easing)"); text(-0.9f, -0.35f, "slow in & slow out");
    for (int i = 0; i <= 10; i++) {          // equal time steps -> spacing of the drawings
        float u = i / 10.0f;
        glColor4f(0.2f, 0.2f, 0.2f, 0.35f); circle(-0.8f + 1.6f * u, 0.25f, 0.025f);
        circle(-0.8f + 1.6f * smooth(u), -0.6f, 0.025f);
    }
}
void p6(float tt) {
    float u = fminf(wrap(tt, 2.0f) / 1.5f, 1.0f);
    col(0.9f, 0.3f, 0.3f); circle(-0.8f + 1.6f * u, 0.25f, 0.08f);
    col(0.2f, 0.7f, 0.4f); circle(-0.8f + 1.6f * smooth(u), -0.6f, 0.08f);
}
// ---------- 7. Arcs ----------
void d7() {
    glColor4f(0.3f, 0.3f, 0.3f, 0.6f); glLineWidth(1);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 40; i++) { float a = -0.9f + 1.8f * i / 40; glVertex2f(1.2f * sinf(a), 0.75f - 1.2f * cosf(a)); }
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); circle(0, 0.75f, 0.03f);
}
void p7(float tt) {
    float a = 0.9f * sinf(tt * 2.2f);           // pendulum: natural motion follows an arc
    float x = 1.2f * sinf(a), y = 0.75f - 1.2f * cosf(a);
    col(0.3f, 0.3f, 0.3f); glLineWidth(2); line(0, 0.75f, x, y);
    col(0.95f, 0.6f, 0.1f); circle(x, y, 0.1f);
}
// ---------- 8. Secondary action ----------
void p8(float tt) {
    float x = -0.75f + 1.5f * wrap(tt, 4.0f) / 4.0f;      // primary action: walking across
    float bob = 0.05f * fabsf(sinf(tt * 6));
    float swing = 0.7f * sinf(tt * 6);                    // secondary: arm swing
    float y = FLOOR_Y + 0.15f + bob;
    col(0.2f, 0.2f, 0.2f); glLineWidth(5);
    line(x, y, x + 0.12f * sinf(swing), y - 0.15f);      // legs
    line(x, y, x - 0.12f * sinf(swing), y - 0.15f);
    col(0.2f, 0.6f, 0.5f); ellipse(x, y + 0.18f, 0.12f, 0.2f);                       // body
    col(0.95f, 0.75f, 0.55f); glLineWidth(5);                                       // arms swing opposite legs
    line(x - 0.1f, y + 0.3f, x - 0.1f - 0.22f * sinf(0.8f * swing), y + 0.3f - 0.22f * cosf(0.8f * swing));
    line(x + 0.1f, y + 0.3f, x + 0.1f + 0.22f * sinf(0.8f * swing), y + 0.3f - 0.22f * cosf(0.8f * swing));
    col(1.0f, 0.85f, 0.7f); circle(x, y + 0.5f, 0.12f);                              // head
    bool blink = wrap(tt, 1.7f) < 0.12f;                                             // secondary: blinking
    col(0, 0, 0);
    if (blink) { glLineWidth(2); line(x + 0.01f, y + 0.52f, x + 0.07f, y + 0.52f); }
    else circle(x + 0.05f, y + 0.52f, 0.02f);
    col(0.9f, 0.2f, 0.2f); glLineWidth(4);                                           // secondary: scarf flutter
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 6; i++) glVertex2f(x - 0.04f * i, y + 0.38f + 0.03f * sinf(tt * 10 - i));
    glEnd();
}
// ---------- 9. Timing ----------
void d9() { glColor3f(0.3f, 0.3f, 0.3f); text(-0.85f, 0.7f, "heavy: fast, low"); text(0.15f, 0.7f, "light: slow, high"); }
void p9(float tt) {
    col(0.15f, 0.15f, 0.2f); circle(-0.45f, FLOOR_Y + 0.14f + bounceY(tt, 0.55f, 0.35f), 0.14f);    // bowling ball
    col(1.0f, 0.4f, 0.6f);   circle(0.45f, FLOOR_Y + 0.14f + bounceY(tt, 1.6f, 1.25f), 0.14f);     // beach ball
}
// ---------- 10. Exaggeration ----------
void d10() { glColor3f(0.3f, 0.3f, 0.3f); text(-0.75f, 0.7f, "realistic"); text(0.2f, 0.7f, "exaggerated"); }
void jumper(float x, float tt, float height, float amount) {
    float u = wrap(tt, 1.4f) / 1.4f, sx = 1, sy = 1;
    float c = fminf(u, 1 - u);
    if (c < 0.1f) { float a = 1 - c / 0.1f; sy = 1 - amount * a; sx = 1 + amount * a; }
    else { sy = 1 + amount * fabsf(1 - 2 * u); sx = 1 / sy; }
    blob(x, FLOOR_Y + height * 4 * u * (1 - u), sx, sy, 0.95f, 0.5f, 0.2f);
}
void p10(float tt) { jumper(-0.45f, tt, 0.35f, 0.1f); jumper(0.45f, tt, 1.15f, 0.55f); }
// ---------- 11. Solid drawing (3D form, volume, lighting) ----------
void p11(float tt) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluPerspective(40, 1, 0.1, 20);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    gluLookAt(0, 0.8, 3.2, 0, 0, 0, 0, 1, 0);
    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_COLOR_MATERIAL);
    GLfloat pos[] = {2, 3, 3, 0}, spec[] = {1, 1, 1, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec); glMaterialf(GL_FRONT, GL_SHININESS, 40);
    glColor3f(0.2f, 0.55f, 0.85f);
    glRotatef(tt * 40, 0, 1, 0);
    glutSolidTeapot(0.75);
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
    glColor4f(0, 0, 0, 0.2f); ellipse(0, -0.62f, 0.55f, 0.08f);     // contact shadow
}
// ---------- 12. Appeal ----------
void p12(float tt) {
    float bob = 0.03f * sinf(tt * 3);
    col(0.55f, 0.85f, 0.95f); ellipse(0, -0.35f + bob, 0.38f, 0.3f);                   // body
    col(1.0f, 0.88f, 0.75f);  circle(0, 0.2f + bob, 0.45f);                             // big round head
    col(0.4f, 0.25f, 0.15f);  ellipse(0, 0.52f + bob, 0.38f, 0.16f);                    // hair
    bool blink = wrap(tt, 2.5f) < 0.15f;
    for (int s = -1; s <= 1; s += 2) {
        float ex = s * 0.17f, ey = 0.2f + bob;
        if (blink) { col(0.1f, 0.1f, 0.1f); glLineWidth(3); line(ex - 0.08f, ey, ex + 0.08f, ey); continue; }
        col(1, 1, 1); ellipse(ex, ey, 0.11f, 0.13f);                                    // big eyes
        col(0.15f, 0.1f, 0.3f); circle(ex, ey - 0.02f, 0.075f);
        col(1, 1, 1); circle(ex + 0.03f, ey + 0.02f, 0.025f);                           // eye highlight
        col(1.0f, 0.55f, 0.6f); ellipse(s * 0.28f, 0.03f + bob, 0.06f, 0.035f);         // rosy cheeks
    }
    col(0.6f, 0.2f, 0.2f); glLineWidth(3);                                              // smile
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++) { float a = PI + PI * i / 10; glVertex2f(0.1f * cosf(a), 0.02f + bob + 0.06f * sinf(a)); }
    glEnd();
}

// ---------- principle table ----------
struct Principle { const char* name; void (*draw)(float); void (*decor)(); int ghosts; float gap; };
void dFloor() { floorLine(); }
Principle P[12] = {
    {"1. Squash and stretch", p1, dFloor, 10, 0.06f}, {"2. Anticipation", p2, dFloor, 8, 0.09f},
    {"3. Staging", p3, d3, 0, 0}, {"4. Straight ahead vs pose to pose", p4, d4, 8, 0.12f},
    {"5. Follow-through & overlapping", p5, dFloor, 6, 0.08f}, {"6. Slow in and slow out", p6, d6, 0, 0},
    {"7. Arcs", p7, d7, 8, 0.08f}, {"8. Secondary action", p8, dFloor, 0, 0},
    {"9. Timing", p9, d9, 8, 0.05f}, {"10. Exaggeration", p10, d10, 8, 0.07f},
    {"11. Solid drawing", p11, 0, 0, 0}, {"12. Appeal", p12, 0, 0, 0}};

void display() {
    int W = glutGet(GLUT_WINDOW_WIDTH), H = glutGet(GLUT_WINDOW_HEIGHT), cw = W / 4, ch = H / 3;
    glClearColor(0.75f, 0.75f, 0.78f, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < 12; i++) {
        glViewport((i % 4) * cw + 2, (2 - i / 4) * ch + 2, cw - 4, ch - 4);   // one viewport per principle
        glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1, 1, -1, 1);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        glColor3f(0.97f, 0.97f, 0.95f); glRectf(-1, -1, 1, 1);
        if (P[i].decor) P[i].decor();
        if (trails)
            for (int k = P[i].ghosts; k >= 1; k--) { gAlpha = 0.5f * (1 - (float)k / (P[i].ghosts + 1)); P[i].draw(t - k * P[i].gap); }
        gAlpha = 1; P[i].draw(t);
        if (i == 2) glColor3f(1, 1, 1); else glColor3f(0.05f, 0.05f, 0.2f);   // title (white on dark stage)
        text(-0.92f, 0.88f, P[i].name, GLUT_BITMAP_HELVETICA_18);
    }
    glutSwapBuffers();
}
void timer(int) { if (!paused) t += 1.0f / 60; glutPostRedisplay(); glutTimerFunc(16, timer, 0); }
void keyboard(unsigned char k, int, int) { if (k == ' ') paused = !paused; if (k == 't') trails = !trails; if (k == 27) exit(0); }

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1600, 1200);
    glutCreateWindow("Q2 - The twelve principles of animation");
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_NORMALIZE);
    glutDisplayFunc(display); glutKeyboardFunc(keyboard); glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
