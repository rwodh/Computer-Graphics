// Q6 - 3D model viewer for Wavefront .OBJ files (OpenGL + freeglut)
// Usage:  q6_model_viewer model.obj      (with no file, a built-in torus knot is shown)
// Mouse: left drag = orbit, right drag = zoom, wheel = zoom
// Keys:  [s] smooth  [f] flat  [w] wireframe  [p] points  [b] bounding box  [x] axes
//        [l] lighting  [t] auto-turntable  [r] reset camera  [Esc] quit
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

struct Vec3 { float x, y, z; };
Vec3 operator-(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
Vec3 operator+(Vec3 a, Vec3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
Vec3 cross(Vec3 a, Vec3 b) { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
Vec3 normalize(Vec3 a) { float l = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z); return l > 0 ? Vec3{a.x / l, a.y / l, a.z / l} : a; }

// ---------------- 1. Model data ----------------
struct Mesh {
    std::vector<Vec3> v, n;          // positions and per-vertex normals
    std::vector<int> tri;            // 3 vertex indices per triangle
    std::vector<Vec3> faceN;         // one normal per triangle (flat shading)
    Vec3 bmin, bmax;                 // bounding box
    std::string name;
} mesh;

// ---------------- 2. Loader: parse "v" and "f" records, triangulate polygons ----------------
bool loadOBJ(const char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return false;
    char line[512];
    while (fgets(line, sizeof line, f)) {
        if (line[0] == 'v' && line[1] == ' ') {
            Vec3 p; sscanf(line + 2, "%f %f %f", &p.x, &p.y, &p.z); mesh.v.push_back(p);
        } else if (line[0] == 'f' && line[1] == ' ') {
            std::vector<int> idx;
            for (char* tok = strtok(line + 2, " \t\r\n"); tok; tok = strtok(nullptr, " \t\r\n")) {
                int i = atoi(tok);                          // handles v, v/vt, v//vn, v/vt/vn
                idx.push_back(i > 0 ? i - 1 : (int)mesh.v.size() + i);   // 1-based or negative
            }
            for (size_t k = 1; k + 1 < idx.size(); k++) {   // fan triangulation of n-gons
                mesh.tri.push_back(idx[0]); mesh.tri.push_back(idx[k]); mesh.tri.push_back(idx[k + 1]);
            }
        }
    }
    fclose(f);
    mesh.name = path;
    return !mesh.tri.empty();
}
void makeTorusKnot(int p = 2, int q = 3, int seg = 240, int ring = 16) {   // fallback model
    for (int i = 0; i < seg; i++)
        for (int j = 0; j < ring; j++) {
            float u = 2 * 3.14159265f * i / seg, w = 2 * 3.14159265f * j / ring;
            auto centre = [&](float a) { float r = 2 + cosf(q * a); return Vec3{r * cosf(p * a), r * sinf(p * a), -sinf(q * a)}; };
            Vec3 c = centre(u), T = normalize(centre(u + 0.01f) - c), B = normalize(cross(T, c)), N = cross(B, T);
            mesh.v.push_back(c + Vec3{0.4f * (cosf(w) * N.x + sinf(w) * B.x), 0.4f * (cosf(w) * N.y + sinf(w) * B.y), 0.4f * (cosf(w) * N.z + sinf(w) * B.z)});
            int a = i * ring + j, b = ((i + 1) % seg) * ring + j, c2 = ((i + 1) % seg) * ring + (j + 1) % ring, d = i * ring + (j + 1) % ring;
            int quad[6] = {a, b, c2, a, c2, d};
            mesh.tri.insert(mesh.tri.end(), quad, quad + 6);
        }
    mesh.name = "built-in torus knot";
}

// ---------------- 3. Pre-processing: normals and normalisation ----------------
void prepareMesh() {
    mesh.n.assign(mesh.v.size(), {0, 0, 0});
    mesh.faceN.clear();
    for (size_t i = 0; i < mesh.tri.size(); i += 3) {
        Vec3 a = mesh.v[mesh.tri[i]], b = mesh.v[mesh.tri[i + 1]], c = mesh.v[mesh.tri[i + 2]];
        Vec3 fn = cross(b - a, c - a);                     // area-weighted face normal
        mesh.faceN.push_back(normalize(fn));
        for (int k = 0; k < 3; k++) mesh.n[mesh.tri[i + k]] = mesh.n[mesh.tri[i + k]] + fn;
    }
    for (Vec3& nn : mesh.n) nn = normalize(nn);           // smooth vertex normals
    mesh.bmin = mesh.bmax = mesh.v[0];
    for (Vec3 p : mesh.v) {
        mesh.bmin = {fminf(mesh.bmin.x, p.x), fminf(mesh.bmin.y, p.y), fminf(mesh.bmin.z, p.z)};
        mesh.bmax = {fmaxf(mesh.bmax.x, p.x), fmaxf(mesh.bmax.y, p.y), fmaxf(mesh.bmax.z, p.z)};
    }
    Vec3 c = {(mesh.bmin.x + mesh.bmax.x) / 2, (mesh.bmin.y + mesh.bmax.y) / 2, (mesh.bmin.z + mesh.bmax.z) / 2};
    Vec3 e = mesh.bmax - mesh.bmin;
    float s = 2.0f / fmaxf(e.x, fmaxf(e.y, e.z));          // fit the model into a 2x2x2 cube
    for (Vec3& p : mesh.v) p = {(p.x - c.x) * s, (p.y - c.y) * s, (p.z - c.z) * s};
    mesh.bmin = {(mesh.bmin.x - c.x) * s, (mesh.bmin.y - c.y) * s, (mesh.bmin.z - c.z) * s};
    mesh.bmax = {(mesh.bmax.x - c.x) * s, (mesh.bmax.y - c.y) * s, (mesh.bmax.z - c.z) * s};
}

// ---------------- 4. Camera (orbit around the model) ----------------
float yaw = 30, pitch = 20, dist = 4.0f;
int lastX, lastY, dragButton = -1;
bool turntable = false;

// ---------------- 5. Rendering ----------------
enum Mode { SMOOTH, FLAT, WIRE, POINT_CLOUD } renderMode = SMOOTH;
bool showBox = true, showAxes = true, lighting = true;
const char* modeName[] = {"smooth (Gouraud)", "flat", "wireframe", "points"};

void drawMesh() {
    if (renderMode == POINT_CLOUD) {
        glPointSize(2); glBegin(GL_POINTS);
        for (size_t i = 0; i < mesh.v.size(); i++) { glNormal3fv(&mesh.n[i].x); glVertex3fv(&mesh.v[i].x); }
        glEnd(); return;
    }
    glPolygonMode(GL_FRONT_AND_BACK, renderMode == WIRE ? GL_LINE : GL_FILL);
    glShadeModel(renderMode == FLAT ? GL_FLAT : GL_SMOOTH);
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < mesh.tri.size(); i++) {
        if (renderMode == FLAT) glNormal3fv(&mesh.faceN[i / 3].x);
        else glNormal3fv(&mesh.n[mesh.tri[i]].x);
        glVertex3fv(&mesh.v[mesh.tri[i]].x);
    }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void drawBox() {
    Vec3 a = mesh.bmin, b = mesh.bmax;
    glColor3f(0.9f, 0.6f, 0.1f);
    glBegin(GL_LINES);
    float xs[2] = {a.x, b.x}, ys[2] = {a.y, b.y}, zs[2] = {a.z, b.z};
    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) {
        glVertex3f(a.x, ys[i], zs[j]); glVertex3f(b.x, ys[i], zs[j]);
        glVertex3f(xs[i], a.y, zs[j]); glVertex3f(xs[i], b.y, zs[j]);
        glVertex3f(xs[i], ys[j], a.z); glVertex3f(xs[i], ys[j], b.z);
    }
    glEnd();
}
void drawAxes() {
    glLineWidth(2); glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(1.4f, 0, 0);
    glColor3f(0, 0.7f, 0); glVertex3f(0, 0, 0); glVertex3f(0, 1.4f, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.4f);
    glEnd(); glLineWidth(1);
}
void hud() {
    int W = glutGet(GLUT_WINDOW_WIDTH), H = glutGet(GLUT_WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, W, 0, H);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);
    char buf[256]; glColor3f(1, 1, 1);
    snprintf(buf, sizeof buf, "%s   vertices: %zu   triangles: %zu", mesh.name.c_str(), mesh.v.size(), mesh.tri.size() / 3);
    glRasterPos2i(10, H - 20); glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)buf);
    snprintf(buf, sizeof buf, "mode: %s   lighting: %s   yaw %.0f  pitch %.0f  distance %.1f", modeName[renderMode], lighting ? "on" : "off", yaw, pitch, dist);
    glRasterPos2i(10, H - 38); glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)buf);
    glRasterPos2i(10, 10);
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)"[s]mooth [f]lat [w]ire [p]oints  [b]ox  a[x]es  [l]ight  [t]urntable  [r]eset   drag = orbit / zoom");
    glEnable(GL_DEPTH_TEST);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}
void display() {
    glClearColor(0.16f, 0.17f, 0.2f, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int W = glutGet(GLUT_WINDOW_WIDTH), H = glutGet(GLUT_WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluPerspective(45, (double)W / H, 0.1, 100);  // projection
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    GLfloat lightDir[] = {0.4f, 0.8f, 1.0f, 0};               // light fixed to the camera
    glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
    glTranslatef(0, 0, -dist); glRotatef(pitch, 1, 0, 0); glRotatef(yaw, 0, 1, 0);   // view (orbit)
    if (showAxes) drawAxes();
    if (showBox) drawBox();
    if (lighting && renderMode != WIRE) glEnable(GL_LIGHTING);
    glColor3f(renderMode == WIRE ? 0.4f : 0.85f, renderMode == WIRE ? 0.9f : 0.55f, renderMode == WIRE ? 1.0f : 0.3f);
    drawMesh();
    glDisable(GL_LIGHTING);
    hud();
    glutSwapBuffers();
}

// ---------------- 6. Interaction ----------------
void mouse(int button, int state, int x, int y) {
    if (button == 3 || button == 4) { dist *= (button == 3) ? 0.9f : 1.1f; glutPostRedisplay(); return; }  // wheel
    dragButton = (state == GLUT_DOWN) ? button : -1; lastX = x; lastY = y;
}
void motion(int x, int y) {
    if (dragButton == GLUT_LEFT_BUTTON) { yaw += (x - lastX) * 0.5f; pitch += (y - lastY) * 0.5f; }
    if (dragButton == GLUT_RIGHT_BUTTON) dist *= 1 + (y - lastY) * 0.01f;
    pitch = fmaxf(-89, fminf(89, pitch)); dist = fmaxf(1.5f, fminf(20, dist));
    lastX = x; lastY = y; glutPostRedisplay();
}
void keyboard(unsigned char k, int, int) {
    switch (k) {
        case 's': renderMode = SMOOTH; break;  case 'f': renderMode = FLAT; break;
        case 'w': renderMode = WIRE; break;    case 'p': renderMode = POINT_CLOUD; break;
        case 'b': showBox = !showBox; break;   case 'x': showAxes = !showAxes; break;
        case 'l': lighting = !lighting; break; case 't': turntable = !turntable; break;
        case 'r': yaw = 30; pitch = 20; dist = 4; break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}
void timer(int) { if (turntable) { yaw += 0.5f; glutPostRedisplay(); } glutTimerFunc(16, timer, 0); }

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    if (argc < 2 || !loadOBJ(argv[1])) {
        if (argc >= 2) printf("Could not load %s, showing built-in model\n", argv[1]);
        makeTorusKnot();
    }
    prepareMesh();
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 680);
    glutCreateWindow("Q6 - 3D model viewer");
    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHT0); glEnable(GL_COLOR_MATERIAL); glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLfloat spec[] = {0.6f, 0.6f, 0.6f, 1}, amb[] = {0.18f, 0.18f, 0.2f, 1};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec); glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 48);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    glutDisplayFunc(display); glutMouseFunc(mouse); glutMotionFunc(motion);
    glutKeyboardFunc(keyboard); glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}
