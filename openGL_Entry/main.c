#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 640
#define HEIGHT 640
#define N 4
#define M 100
#define PI 3.14



GLfloat t;
GLint i;
GLuint vao, vbo;
GLuint vao1, vbo1;
GLuint vao2, vbo2;
GLuint vao3, vbo3;

// letároljuk a 8 kör tulajdonságait.
GLuint vaoCircle[8], vboCircle[8];

// köröket alkoto pontok
GLfloat Circles[8][300];


//hermit vektorainak (kezdõ, vég pontjai)
GLfloat points[24] = { -.5f,   .25f,  .0f,
                       -.25f,  .5f,   .0f,
                        .25f,  .5f,   .0f,
                        .5f,   .5f,   .0f,
                        .5f,  -.25f,  .0f,
                        .25f, -.5f,   .0f,
                       -.25f, -.5f,   .0f,
                       -.5f,  -.25f,  .0f,
};


// megfogott pont index 0,1,2 vagy -1 ha nincs semmi megfogva
GLint dragged = -1;

// hemit ívek pontjai (linspace)
GLfloat HermiteCurve1[3 * M];
GLfloat HermiteCurve2[3 * M];
GLfloat HermiteCurve3[3 * M];


// koordináták négyzetes távolsága
GLfloat dist2_2d(GLfloat P1x, GLfloat P1y, GLfloat P2x, GLfloat P2y) {

    GLfloat dx = P1x - P2x;
    GLfloat dy = P1y - P2y;
    return dx * dx + dy * dy;
}


// Ez veszi a (X pixel, Y pixel) jelenlegi helyzetét az egérnek és át vetíti a (X float, Y float) rendszerbe, ami az openglnek kell
GLint getActivePoint(GLfloat* p, GLfloat sensitivity, GLfloat x, GLfloat y) {

    GLfloat		s = sensitivity * sensitivity;
    GLfloat		xNorm = -1 + x / (WIDTH / 2);
    GLfloat		yNorm = -1 + (HEIGHT - y) / (HEIGHT / 2);

    for (GLint i = 0; i < 8; i++)
        if (dist2_2d(p[i * 3], p[i * 3 + 1], xNorm, yNorm) < s)
            return i;

    return -1;
}


// ez számolja ki pontoknak, hogy körként jelenjen meg
void generateCircles() {
    GLfloat x0;
    GLfloat y0;
    GLfloat r = 0.035f;
    GLfloat t;
    GLint i;
    GLfloat step = (2 * PI) / 100;
    for (GLint j = 0; j < sizeof(points) / 4; j = j + 3) {
        x0 = points[j];
        y0 = points[j + 1];
        for (i = 0; i < 100; i++) {
            t = i * step;
            Circles[j / 3][i * 3] = x0 + r * cos(t);
            Circles[j / 3][i * 3 + 1] = y0 + r * sin(t);
            Circles[j / 3][i * 3 + 2] = 0.0f;
        }
    }
}


// bal clicre JELÖLJE KI melyik pontra kattintott
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double	x, y;

        glfwGetCursorPos(window, &x, &y);
        dragged = getActivePoint(points, 0.1f, x, y);
    }


    // felengedésre pedig tegye le
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragged = -1;
}


// számoljuk ki a 3 hermitív pontjait
void hermit(int whichCurve, GLfloat P1x, GLfloat P1y, GLfloat P2x, GLfloat P2y, GLfloat P3x, GLfloat P3y, GLfloat P4x, GLfloat P4y) {

    GLfloat step = 1.0f / (M - 1); // t \in [0,1]

    GLfloat R1x = P2x - P1x;
    GLfloat R1y = P2y - P1y;

    GLfloat R2x = P4x - P3x;
    GLfloat R2y = P4y - P3y;

    if (whichCurve == 1)
    {

        HermiteCurve1[0] = P1x;
        HermiteCurve1[1] = P1y;
        HermiteCurve1[2] = 0.0f;
        for (i = 1; i < 100; i++) {
            t = i * step;
            HermiteCurve1[i * 3] = P1x * (2.0f * t * t * t - 3.0f * t * t + 1.0f) + P3x * (-2.0 * t * t * t + 3.0f * t * t) + R1x * (t * t * t - 2.0f * t * t + t) + R2x * (t * t * t - t * t);
            HermiteCurve1[i * 3 + 1] = P1y * (2.0f * t * t * t - 3.0f * t * t + 1.0f) + P3y * (-2.0 * t * t * t + 3.0f * t * t) + R1y * (t * t * t - 2.0f * t * t + t) + R2y * (t * t * t - t * t);
            HermiteCurve1[i * 3 + 2] = 0.0f;
        }

        HermiteCurve1[(3 * M) - 3] = P3x;
        HermiteCurve1[(3 * M) - 2] = P3y;
        HermiteCurve1[(3 * M) - 1] = 0.0f;
    }

    if (whichCurve == 2)
    {

        HermiteCurve2[0] = P1x;
        HermiteCurve2[1] = P1y;
        HermiteCurve2[2] = 0.0f;
        for (i = 1; i < 100; i++) {
            t = i * step;
            HermiteCurve2[i * 3] = P1x * (2.0f * t * t * t - 3.0f * t * t + 1.0f) + P3x * (-2.0 * t * t * t + 3.0f * t * t) + R1x * (t * t * t - 2.0f * t * t + t) + R2x * (t * t * t - t * t);
            HermiteCurve2[i * 3 + 1] = P1y * (2.0f * t * t * t - 3.0f * t * t + 1.0f) + P3y * (-2.0 * t * t * t + 3.0f * t * t) + R1y * (t * t * t - 2.0f * t * t + t) + R2y * (t * t * t - t * t);
            HermiteCurve2[i * 3 + 2] = 0.0f;
        }

        HermiteCurve2[(3 * M) - 3] = P3x;
        HermiteCurve2[(3 * M) - 2] = P3y;
        HermiteCurve2[(3 * M) - 1] = 0.0f;
    }

    if (whichCurve == 3)
    {

        HermiteCurve3[0] = P1x;
        HermiteCurve3[1] = P1y;
        HermiteCurve3[2] = 0.0f;
        for (i = 1; i < 100; i++) {
            t = i * step;
            HermiteCurve3[i * 3] = P1x * (2.0f * t * t * t - 3.0f * t * t + 1.0f) + P3x * (-2.0 * t * t * t + 3.0f * t * t) + R1x * (t * t * t - 2.0f * t * t + t) + R2x * (t * t * t - t * t);
            HermiteCurve3[i * 3 + 1] = P1y * (2.0f * t * t * t - 3.0f * t * t + 1.0f) + P3y * (-2.0 * t * t * t + 3.0f * t * t) + R1y * (t * t * t - 2.0f * t * t + t) + R2y * (t * t * t - t * t);
            HermiteCurve3[i * 3 + 2] = 0.0f;
        }

        HermiteCurve3[(3 * M) - 3] = P3x;
        HermiteCurve3[(3 * M) - 2] = P3y;
        HermiteCurve3[(3 * M) - 1] = 0.0f;
    }
}







void cursorPosCallback(GLFWwindow* window, double x, double y) {

    if (dragged >= 0) {

        // normalizáljuk a koordinátarendszt a [-1, 1] intervallumba amit a openGL is tud kezelni

        GLfloat		xNorm = -1 + x / (WIDTH / 2);
        GLfloat		yNorm = -1 + (HEIGHT - y) / (HEIGHT / 2);

        points[3 * dragged] = xNorm;  // x coord
        points[3 * dragged + 1] = yNorm;  // y coord





        // minden mouse eventnél számolja újra a pontokat

        hermit(1, points[0], points[1], points[3], points[4], points[6], points[7], points[9], points[10]);
        hermit(2, points[6], points[7], points[9], points[10], points[12], points[13], points[15], points[16]);
        hermit(3, points[12], points[13], points[15], points[16], points[18], points[19], points[21], points[22]);

        generateCircles();
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        for (int i = 0; i < 8; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vboCircle[i]);
            glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(GLfloat), Circles[i], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo1);
        glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve1, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve2, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo3);
        glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve3, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }




}


// inicializáljuk amíg nem történik egy mouse event
void init() {

    // kör init
    for (int i = 0; i < 8; i++) {
        glGenBuffers(1, &vboCircle[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vboCircle[i]);
        glBufferData(GL_ARRAY_BUFFER, 300 * sizeof(GLfloat), Circles[i], GL_STATIC_DRAW);
        glGenVertexArrays(1, &vaoCircle[i]);
        glBindVertexArray(vaoCircle[i]);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vboCircle[i]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    }

    
    // 1. hermit ív init
    glGenBuffers(1, &vbo1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve1, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


    // 2. hermit ív init
    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve2, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


    // 3. hermit ív init
    glGenBuffers(1, &vbo3);
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve3, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao3);
    glBindVertexArray(vao3);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


    // pontok init
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), points, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}



int main() {
    GLFWwindow* window = NULL;
    const GLubyte* renderer;
    const GLubyte* version;

    hermit(1, points[0], points[1], points[3], points[4], points[6], points[7], points[9], points[10]);
    hermit(2, points[6], points[7], points[9], points[10], points[12], points[13], points[15], points[16]);
    hermit(3, points[12], points[13], points[15], points[16], points[18], points[19], points[21], points[22]);

    // kiszámoljuk a körök pontjait
    generateCircles();
    const char* vertex_shader =
        "#version 330\n"
        "in vec3 vp;"
        "void main () {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

    const char* fragment_shaderLine =
        "#version 330\n"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4(1.0, 0.0, 0.0, 1.0);"
        "}";

    const char* fragment_shaderCircle =
        "#version 330\n"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4(0.5, 0.5, 0.5, 1.0);"
        "}";

    const char* fragment_shaderHermit =
        "#version 330\n"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4(0.7, 0.9, 0.0, 1.0);"
        "}";

    GLuint vert_shader, frag_shaderLine, frag_shaderCircle, frag_shaderHermit;
    GLuint shader_programmeLine, shader_programmeCircle, shader_programmeHermit;

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Drag&Drop", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    init();



    // GL shader attach
    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vertex_shader, NULL);
    glCompileShader(vert_shader);

    frag_shaderLine = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shaderLine, 1, &fragment_shaderLine, NULL);
    glCompileShader(frag_shaderLine);

    frag_shaderCircle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shaderCircle, 1, &fragment_shaderCircle, NULL);
    glCompileShader(frag_shaderCircle);

    frag_shaderHermit = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shaderHermit, 1, &fragment_shaderHermit, NULL);
    glCompileShader(frag_shaderHermit);

    shader_programmeLine = glCreateProgram();
    glAttachShader(shader_programmeLine, frag_shaderLine);
    glAttachShader(shader_programmeLine, vert_shader);
    glLinkProgram(shader_programmeLine);

    shader_programmeCircle = glCreateProgram();
    glAttachShader(shader_programmeCircle, frag_shaderCircle);
    glAttachShader(shader_programmeCircle, vert_shader);
    glLinkProgram(shader_programmeCircle);

    shader_programmeHermit = glCreateProgram();
    glAttachShader(shader_programmeHermit, frag_shaderHermit);
    glAttachShader(shader_programmeHermit, vert_shader);
    glLinkProgram(shader_programmeHermit);


    // az ablakot hozzákapcsolja a programhoz és az egeret
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glPointSize(15.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programmeCircle);

        // rajzoljuk ki a 8 kört
        for (int i = 0; i < 8; i++) {
            glBindVertexArray(vaoCircle[i]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 100);
            glBindVertexArray(0);
        }

        
        /* Rajzoljuk ki a vektorokat */
        // 1. vektor
        glUseProgram(shader_programmeLine);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, 2);
        glBindVertexArray(0);

        // 2. vektor
        glUseProgram(shader_programmeLine);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 2, 2);
        glBindVertexArray(0);

        // 3. vektor
        glUseProgram(shader_programmeLine);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 4, 2);
        glBindVertexArray(0);

        // 4. vektor
        glUseProgram(shader_programmeLine);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 6, 2);
        glBindVertexArray(0);


        /* Rajzoljuk ki a hermit íveket*/
        // 1. ív
        glUseProgram(shader_programmeHermit);
        glBindVertexArray(vao1);
        glDrawArrays(GL_LINE_STRIP, 0, M);
        glBindVertexArray(0);

        // 2. ív
        glUseProgram(shader_programmeHermit);
        glBindVertexArray(vao2);
        glDrawArrays(GL_LINE_STRIP, 0, M);
        glBindVertexArray(0);

        // 3. ív
        glUseProgram(shader_programmeHermit);
        glBindVertexArray(vao3);
        glDrawArrays(GL_LINE_STRIP, 0, M);
        glBindVertexArray(0);


        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}