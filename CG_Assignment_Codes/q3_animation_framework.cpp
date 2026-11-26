// Q3 - A minimal animation framework (OpenGL + freeglut)
// Layers: Platform (GLUT) -> Core (Clock, Easing) -> Animation (Track, Clip, Animator)
//         -> Scene (SceneNode graph) -> Renderer -> Application
// Keys: [1] wave clip  [2] nod clip  [space] pause  [+/-] time scale  [Esc] quit
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

// ===== Core layer =====
struct Clock {                         // converts wall-clock time into animation time
    float time = 0, scale = 1; bool paused = false; int last = 0;
    float tick() {
        int now = glutGet(GLUT_ELAPSED_TIME);
        float dt = (now - last) / 1000.0f; last = now;
        if (paused) dt = 0;
        dt *= scale; time += dt; return dt;
    }
};
namespace Ease {
    float linear(float u) { return u; }
    float inOut(float u) { return u * u * (3 - 2 * u); }
    float outBack(float u) { float c = 1.70158f; u -= 1; return 1 + (c + 1) * u * u * u + c * u * u; }
}

// ===== Animation layer =====
struct Key { float time, value; float (*ease)(float); };
struct Track {                          // animates one float property of one node
    float* target; std::vector<Key> keys;
    float sample(float t) const {
        if (t <= keys.front().time) return keys.front().value;
        for (size_t i = 0; i + 1 < keys.size(); i++)
            if (t <= keys[i + 1].time) {
                float u = (t - keys[i].time) / (keys[i + 1].time - keys[i].time);
                return keys[i].value + (keys[i + 1].value - keys[i].value) * keys[i + 1].ease(u);
            }
        return keys.back().value;
    }
};
struct Clip {                           // a named, reusable group of tracks
    std::string name; float duration; bool loop; std::vector<Track> tracks;
};
struct Animator {                       // plays clips: evaluates every track at local time
    Clip* current = nullptr; float localTime = 0;
    void play(Clip* c) { current = c; localTime = 0; }
    void update(float dt) {
        if (!current) return;
        localTime += dt;
        if (localTime > current->duration)
            localTime = current->loop ? fmodf(localTime, current->duration) : current->duration;
        for (const Track& tr : current->tracks) *tr.target = tr.sample(localTime);
    }
};

// ===== Scene layer =====
struct SceneNode {                      // hierarchical transform (scene graph)
    std::string name; float x = 0, y = 0, rot = 0, scale = 1;
    void (*shape)() = nullptr; std::vector<SceneNode*> children = {};
    SceneNode* add(SceneNode* c) { children.push_back(c); return c; }
};

// ===== Rendering layer =====
struct Renderer {
    int drawn = 0;
    void draw(SceneNode* n) {           // depth-first traversal with matrix stack
        glPushMatrix();
        glTranslatef(n->x, n->y, 0); glRotatef(n->rot, 0, 0, 1); glScalef(n->scale, n->scale, 1);
        if (n->shape) { n->shape(); drawn++; }
        for (SceneNode* c : n->children) draw(c);
        glPopMatrix();
    }
};

// ===== Application layer (the robot demo built on the framework) =====
void box(float w, float h, float r, float g, float b) { glColor3f(r, g, b); glRectf(-w / 2, 0, w / 2, h); }
void shBase() { box(0.5f, 0.15f, 0.3f, 0.3f, 0.35f); }
void shBody() { box(0.36f, 0.5f, 0.2f, 0.5f, 0.8f); glColor3f(1, 1, 1); glRectf(-0.1f, 0.3f, 0.1f, 0.42f); }
void shHead() { box(0.28f, 0.24f, 0.9f, 0.7f, 0.2f); glColor3f(0, 0, 0); glRectf(-0.08f, 0.12f, -0.03f, 0.17f); glRectf(0.03f, 0.12f, 0.08f, 0.17f); }
void shUpper() { box(0.08f, 0.32f, 0.85f, 0.3f, 0.3f); }
void shLower() { box(0.07f, 0.28f, 0.95f, 0.5f, 0.4f); }
void shHand()  { glColor3f(0.3f, 0.3f, 0.3f); glRectf(-0.06f, 0, 0.06f, 0.08f); }

Clock clk; Animator animator; Renderer renderer;
SceneNode root{"root"}, *body, *head, *upper, *lower;
Clip wave, nod;

void buildScene() {
    root.y = -0.6f; root.shape = shBase;
    body  = root.add(new SceneNode{"body", 0, 0.15f, 0, 1, shBody});
    head  = body->add(new SceneNode{"head", 0, 0.5f, 0, 1, shHead});
    upper = body->add(new SceneNode{"upperArm", 0.17f, 0.42f, -150, 1, shUpper});
    lower = upper->add(new SceneNode{"lowerArm", 0, 0.32f, 20, 1, shLower});
    lower->add(new SceneNode{"hand", 0, 0.28f, 0, 1, shHand});
    // A clip = key frames on named node properties
    wave = {"wave", 2.0f, true, {
        {&upper->rot, {{0, -150, Ease::linear}, {0.5f, -30, Ease::outBack}, {1.5f, -30, Ease::linear}, {2.0f, -150, Ease::inOut}}},
        {&lower->rot, {{0, 20, Ease::linear}, {0.6f, 40, Ease::inOut}, {0.9f, -40, Ease::inOut}, {1.2f, 40, Ease::inOut}, {1.5f, -40, Ease::inOut}, {2.0f, 20, Ease::inOut}}},
        {&head->rot,  {{0, 0, Ease::linear}, {0.5f, -10, Ease::inOut}, {1.5f, -10, Ease::linear}, {2.0f, 0, Ease::inOut}}}}};
    nod = {"nod", 1.2f, true, {
        {&head->rot, {{0, 0, Ease::linear}, {0.3f, 12, Ease::inOut}, {0.6f, -12, Ease::inOut}, {1.2f, 0, Ease::inOut}}},
        {&body->y,   {{0, 0.15f, Ease::linear}, {0.3f, 0.12f, Ease::inOut}, {0.6f, 0.15f, Ease::inOut}, {1.2f, 0.15f, Ease::linear}}},
        {&upper->rot,{{0, -150, Ease::linear}, {1.2f, -150, Ease::linear}}},
        {&lower->rot,{{0, 20, Ease::linear}, {1.2f, 20, Ease::linear}}}}};
    animator.play(&wave);
}

void text(float x, float y, const char* s) { glRasterPos2f(x, y); glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)s); }

void display() {                        // one frame = update -> render -> HUD
    float dt = clk.tick();
    animator.update(dt);
    glClearColor(0.93f, 0.95f, 0.98f, 1); glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.6f, 0.65f, 0.7f); glRectf(-1.3f, -1, 1.3f, -0.6f);
    renderer.drawn = 0;
    glLoadIdentity(); renderer.draw(&root);
    char buf[160]; glColor3f(0.1f, 0.1f, 0.2f);
    snprintf(buf, sizeof buf, "clip: %s   local time: %.2f / %.1f s   time scale: %.1fx   nodes drawn: %d",
             animator.current->name.c_str(), animator.localTime, animator.current->duration, clk.scale, renderer.drawn);
    text(-1.25f, 0.9f, buf);
    snprintf(buf, sizeof buf, "upperArm.rot = %.1f   lowerArm.rot = %.1f   head.rot = %.1f", upper->rot, lower->rot, head->rot);
    text(-1.25f, 0.82f, buf);
    text(-1.25f, -0.92f, "[1] wave  [2] nod  [space] pause  [+/-] speed");
    glutSwapBuffers();
}
void idle() { glutPostRedisplay(); }
void keyboard(unsigned char k, int, int) {
    if (k == '1') animator.play(&wave);
    if (k == '2') animator.play(&nod);
    if (k == ' ') clk.paused = !clk.paused;
    if (k == '+') clk.scale *= 1.5f;
    if (k == '-') clk.scale /= 1.5f;
    if (k == 27) exit(0);
}
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(-1.3, 1.3, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(780, 600);
    glutCreateWindow("Q3 - Mini animation framework");
    buildScene();
    clk.last = glutGet(GLUT_ELAPSED_TIME);
    glutDisplayFunc(display); glutIdleFunc(idle);
    glutKeyboardFunc(keyboard); glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
