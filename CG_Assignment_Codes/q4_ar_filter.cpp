// Q4 - An AR face filter (sunglasses + puppy ears + nose + tongue trigger) in OpenGL (freeglut)
// Pipeline per frame: camera frame -> landmark detection -> smoothing -> pose estimation
//                     -> anchor the virtual objects -> composite over the frame
// The "camera" here is a synthetic moving face so the program runs without a webcam;
// with a real tracker (e.g. MediaPipe Face Mesh) only detectLandmarks() would change.
// Keys: [f] filter on/off  [d] debug landmarks  [space] pause  [Esc] quit
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>

const float PI = 3.14159265f;
float t = 0.0f;
bool paused = false, showFilter = true, debugView = false;

struct Vec2 { float x, y; };
Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
Vec2 operator-(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
Vec2 operator*(Vec2 a, float s) { return {a.x * s, a.y * s}; }
float len(Vec2 a) { return sqrtf(a.x * a.x + a.y * a.y); }

void ellipse(float cx, float cy, float rx, float ry, int seg = 48) {
    glBegin(GL_TRIANGLE_FAN); glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) { float a = 2 * PI * i / seg; glVertex2f(cx + rx * cosf(a), cy + ry * sinf(a)); }
    glEnd();
}
void text(float x, float y, const char* s) { glRasterPos2f(x, y); glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)s); }

// ---------- 1. Camera frame (simulated user moving in front of a webcam) ----------
struct HeadPose { Vec2 pos; float scale, roll, mouth; };
HeadPose truePose(float tt) {
    return {{0.35f * sinf(0.7f * tt), -0.1f + 0.08f * sinf(1.1f * tt)},
            0.36f + 0.08f * sinf(0.5f * tt), 0.32f * sinf(0.9f * tt), fmaxf(0.0f, sinf(0.8f * tt + 1.0f)) * 0.28f};
}
// Face landmarks in the face's own coordinate system (face is about 1.8 x 2.3 units)
enum { L_EYE, R_EYE, NOSE, UPPER_LIP, LOWER_LIP, CHIN, JAW0, NLM = JAW0 + 5 };
Vec2 faceLocal(int i, float mouth) {
    switch (i) {
        case L_EYE: return {-0.38f, 0.15f};  case R_EYE: return {0.38f, 0.15f};
        case NOSE: return {0.0f, -0.22f};    case UPPER_LIP: return {0.0f, -0.5f};
        case LOWER_LIP: return {0.0f, -0.52f - mouth}; case CHIN: return {0.0f, -1.15f};
        default: { float a = PI + PI * (i - JAW0 + 0.5f) / 5; return {0.9f * cosf(a), 1.15f * sinf(a)}; }
    }
}
Vec2 toImage(Vec2 p, const HeadPose& h) {                 // rotate, scale, translate
    float c = cosf(h.roll), s = sinf(h.roll);
    return Vec2{c * p.x - s * p.y, s * p.x + c * p.y} * h.scale + h.pos;
}
void drawCameraFrame(float tt) {
    glBegin(GL_QUADS);                                     // room background
    glColor3f(0.86f, 0.83f, 0.78f); glVertex2f(-1.34f, 1); glVertex2f(1.34f, 1);
    glColor3f(0.7f, 0.68f, 0.64f); glVertex2f(1.34f, -1); glVertex2f(-1.34f, -1);
    glEnd();
    glColor3f(0.75f, 0.88f, 0.98f); glRectf(0.6f, 0.2f, 1.2f, 0.85f);   // window
    glColor3f(0.95f, 0.95f, 0.95f); glRectf(0.88f, 0.2f, 0.92f, 0.85f);
    HeadPose h = truePose(tt);
    glPushMatrix();
    glTranslatef(h.pos.x, h.pos.y, 0); glRotatef(h.roll * 180 / PI, 0, 0, 1); glScalef(h.scale, h.scale, 1);
    glColor3f(0.25f, 0.45f, 0.6f); ellipse(0, -2.1f, 1.5f, 0.9f);       // shoulders
    glColor3f(0.85f, 0.67f, 0.52f); glRectf(-0.3f, -1.6f, 0.3f, -0.9f); // neck
    glColor3f(0.22f, 0.14f, 0.1f); ellipse(0, 0.25f, 1.0f, 1.2f);       // hair
    glColor3f(0.93f, 0.76f, 0.62f); ellipse(0, 0, 0.9f, 1.15f);         // face
    glColor3f(0.22f, 0.14f, 0.1f); ellipse(0, 0.85f, 0.85f, 0.35f);     // fringe
    for (int s = -1; s <= 1; s += 2) {
        glColor3f(1, 1, 1); ellipse(s * 0.38f, 0.15f, 0.15f, 0.09f);
        glColor3f(0.3f, 0.2f, 0.1f); ellipse(s * 0.38f, 0.15f, 0.06f, 0.06f);
    }
    glColor3f(0.85f, 0.6f, 0.48f); ellipse(0, -0.22f, 0.1f, 0.07f);     // nose
    glColor3f(0.55f, 0.15f, 0.15f); ellipse(0, -0.51f - h.mouth / 2, 0.2f, 0.03f + h.mouth / 2);   // mouth
    glPopMatrix();
}

// ---------- 2. Landmark detection (simulated detector output with measurement noise) ----------
float noise(float tt, int i, int axis) { return 0.012f * sinf(tt * 37.0f + i * 12.9898f + axis * 78.233f); }
void detectLandmarks(float tt, Vec2* out) {
    HeadPose h = truePose(tt);
    for (int i = 0; i < NLM; i++) {
        Vec2 p = toImage(faceLocal(i, h.mouth), h);
        out[i] = {p.x + noise(tt, i, 0), p.y + noise(tt, i, 1)};
    }
}
// ---------- 3. Temporal smoothing (average of the last 3 detections removes jitter) ----------
void smoothedLandmarks(float tt, Vec2* out) {
    Vec2 a[NLM], b[NLM], c[NLM];
    detectLandmarks(tt, a); detectLandmarks(tt - 1 / 30.0f, b); detectLandmarks(tt - 2 / 30.0f, c);
    for (int i = 0; i < NLM; i++) out[i] = (a[i] + b[i] + c[i]) * (1.0f / 3);
}

// ---------- 4. Pose estimation from landmarks ----------
struct FacePose { Vec2 center; float eyeDist, rollDeg, mouthOpen; };
FacePose estimatePose(const Vec2* lm) {
    Vec2 eyeVec = lm[R_EYE] - lm[L_EYE];
    FacePose f;
    f.center = (lm[L_EYE] + lm[R_EYE]) * 0.5f;
    f.eyeDist = len(eyeVec);                                    // gives the filter's scale
    f.rollDeg = atan2f(eyeVec.y, eyeVec.x) * 180 / PI;          // gives the filter's rotation
    f.mouthOpen = len(lm[LOWER_LIP] - lm[UPPER_LIP]) / f.eyeDist;   // expression trigger
    return f;
}

// ---------- 5. Virtual objects, drawn in "filter space" (1 unit = eye distance) ----------
void sunglasses() {
    for (int s = -1; s <= 1; s += 2) {
        glColor4f(0.05f, 0.05f, 0.1f, 0.92f); ellipse(s * 0.5f, 0, 0.36f, 0.26f);
        glColor4f(0.6f, 0.8f, 1.0f, 0.5f);  ellipse(s * 0.5f - 0.12f, 0.08f, 0.1f, 0.05f);   // lens glare
    }
    glColor3f(0.05f, 0.05f, 0.1f); glLineWidth(5);
    glBegin(GL_LINES); glVertex2f(-0.15f, 0.05f); glVertex2f(0.15f, 0.05f);
    glVertex2f(-0.86f, 0.05f); glVertex2f(-1.15f, 0.12f); glVertex2f(0.86f, 0.05f); glVertex2f(1.15f, 0.12f); glEnd();
}
void puppyEars() {
    for (int s = -1; s <= 1; s += 2) {
        glPushMatrix(); glTranslatef(s * 0.95f, 1.35f, 0); glRotatef(s * -25.0f, 0, 0, 1);
        glColor3f(0.55f, 0.33f, 0.18f); ellipse(0, 0, 0.32f, 0.6f);
        glColor3f(0.95f, 0.65f, 0.7f); ellipse(0, -0.05f, 0.17f, 0.4f);
        glPopMatrix();
    }
}
void puppyNose() { glColor3f(0.1f, 0.08f, 0.08f); ellipse(0, 0, 0.22f, 0.14f); glColor3f(1, 1, 1); ellipse(-0.06f, 0.05f, 0.05f, 0.025f); }
void tongue(float open) { glColor3f(0.95f, 0.35f, 0.45f); ellipse(0, -0.12f * open, 0.16f, 0.2f * open); }

// Place an object: translate to the landmark, rotate by face roll, scale by eye distance
void anchor(Vec2 at, const FacePose& f) {
    glTranslatef(at.x, at.y, 0); glRotatef(f.rollDeg, 0, 0, 1); glScalef(f.eyeDist, f.eyeDist, 1);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawCameraFrame(t);                                  // step 1
    Vec2 lm[NLM]; smoothedLandmarks(t, lm);              // steps 2-3
    FacePose f = estimatePose(lm);                       // step 4
    if (showFilter) {                                    // step 5: render + composite
        glPushMatrix(); anchor(f.center, f); puppyEars(); sunglasses(); glPopMatrix();
        glPushMatrix(); anchor(lm[NOSE], f); puppyNose(); glPopMatrix();
        if (f.mouthOpen > 0.12f) {                        // trigger: mouth open -> tongue
            float open = fminf((f.mouthOpen - 0.12f) * 4, 1.0f);
            glPushMatrix(); anchor(lm[LOWER_LIP], f); tongue(open); glPopMatrix();
        }
    }
    if (debugView) {
        glPointSize(7); glColor3f(0.1f, 0.9f, 0.2f);
        glBegin(GL_POINTS); for (int i = 0; i < NLM; i++) glVertex2f(lm[i].x, lm[i].y); glEnd();
        glLineWidth(2); glColor3f(1, 0.1f, 0.1f);
        glBegin(GL_LINES); glVertex2f(lm[L_EYE].x, lm[L_EYE].y); glVertex2f(lm[R_EYE].x, lm[R_EYE].y); glEnd();
        glPushMatrix(); anchor(f.center, f);                       // filter-space axes
        glColor3f(0.1f, 0.3f, 1); glBegin(GL_LINES); glVertex2f(0, 0); glVertex2f(0, 1.2f); glEnd();
        glPopMatrix();
    }
    char buf[160];
    glColor4f(0, 0, 0, 0.55f); glRectf(-1.34f, 0.8f, 0.3f, 1.0f);
    glColor3f(1, 1, 1);
    snprintf(buf, sizeof buf, "landmarks: %d   center: (%.2f, %.2f)   scale(eye dist): %.3f", NLM, f.center.x, f.center.y, f.eyeDist);
    text(-1.3f, 0.93f, buf);
    snprintf(buf, sizeof buf, "roll: %.1f deg   mouth open: %.2f %s", f.rollDeg, f.mouthOpen, f.mouthOpen > 0.12f ? "-> tongue ON" : "");
    text(-1.3f, 0.86f, buf);
    glColor3f(0, 0, 0); text(-1.3f, -0.95f, "[f] filter  [d] landmarks  [space] pause");
    glutSwapBuffers();
}
void timer(int) { if (!paused) t += 1.0f / 60; glutPostRedisplay(); glutTimerFunc(16, timer, 0); }
void keyboard(unsigned char k, int, int) {
    if (k == 'f') showFilter = !showFilter;
    if (k == 'd') debugView = !debugView;
    if (k == ' ') paused = !paused;
    if (k == 27) exit(0);
}
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1.34, 1.34, -1, 1);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(804, 600);
    glutCreateWindow("Q4 - AR face filter");
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutDisplayFunc(display); glutReshapeFunc(reshape); glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
