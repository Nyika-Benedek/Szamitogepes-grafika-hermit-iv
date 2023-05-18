/*
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

        
        // Rajzoljuk ki a vektorokat 
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


        // Rajzoljuk ki a hermit íveket
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

/**/



/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Dr Anton Gerdelan, Trinity College Dublin, Ireland.                          |
| See individual libraries for separate legal notices                          |
|******************************************************************************|
| Virtual Camera - create and modify VIEW and PROJECTION matrices              |
| keyboard controls: W,S,A,D,left and right arrows                             |
\******************************************************************************/



/*-------------- VEDD KI INNEN A ZÁRÓ KOMMENTET, HOGY VÁLTS A MÁSIKRA -----------------------------------------*/
/**/


#include "gl_utils.h"
#include "maths_funcs.h"
#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define GL_LOG_FILE "gl.log"

// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow* g_window = NULL;

int main() {
    restart_gl_log();
    //------------------------------start GL context------------------------------
    start_gl();

    //------------------------------create geometry-------------------------------
    GLfloat points[] = { 
        // X       Y      Z  
        -0.5f,   0.5f,  0.5f,
        -0.5f,  -0.5f,  0.5f,
        0.5f,   - 0.5f, 0.5f,

        0.5f,   -0.5f,  0.5f,
        0.5f,    0.5f,  0.5f,
        -0.5f,   0.5f,  0.5f,
    };

    GLfloat points_BACK[] = {
        // X       Y      Z  
        -0.5f,   0.5f,  -0.5f,
        -0.5f,  -0.5f,  -0.5f,
        0.5f,   -0.5f,  -0.5f,

        0.5f,   -0.5f,  -0.5f,
        0.5f,    0.5f,  -0.5f,
        -0.5f,   0.5f,  -0.5f,
    };

    GLfloat points_LEFT[] = {
        -0.5f,   0.5f, 0.5f,
        -0.5f, - 0.5f, 0.5f,
        -0.5f, - 0.5f, - 0.5f,

        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, 0.5f
    };

    GLfloat points_TOP[] = {
        // X     Y      Z 
        -0.5f, 0.5f, - 0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f,  0.5f, 0.5f,

        0.5f,  0.5f,   0.5f,
        0.5f,  0.5f,  -0.5f,
        -0.5f, 0.5f,  -0.5f,

    };

    GLfloat points_BOTTOM[] = {
        // X     Y      Z 
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f,  -0.5f, 0.5f,

        0.5f,  -0.5f,   0.5f,
        0.5f,  -0.5f,  -0.5f,
        -0.5f, -0.5f,  -0.5f,

    };

    GLfloat points_RIGHT[] = {
    0.5f,   0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f, 0.5f
    };









    GLfloat felle[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    GLfloat forgas[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };

    GLfloat colours[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
    GLfloat colours_LEFT[] = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
    GLfloat colours_TOP[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat colours_BACK[] = { 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
    GLfloat colours_RIGHT[] = { 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
    GLfloat colours_BOTTOM[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };


    GLuint points_vbo;
    glGenBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

    GLuint points_LEFT_vbo;
    glGenBuffers(1, &points_LEFT_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_LEFT_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), points_LEFT, GL_STATIC_DRAW);

    GLuint points_TOP_vbo;
    glGenBuffers(1, &points_TOP_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_TOP_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), points_TOP, GL_STATIC_DRAW);

    GLuint points_BACK_vbo;
    glGenBuffers(1, &points_BACK_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_BACK_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), points_BACK, GL_STATIC_DRAW);

    GLuint points_RIGHT_vbo;
    glGenBuffers(1, &points_RIGHT_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_RIGHT_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), points_RIGHT, GL_STATIC_DRAW);

    GLuint points_BOTTOM_vbo;
    glGenBuffers(1, &points_BOTTOM_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_BOTTOM_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), points_BOTTOM, GL_STATIC_DRAW);




    GLuint colours_vbo;
    glGenBuffers(1, &colours_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), colours, GL_STATIC_DRAW);

    GLuint colours_LEFT_vbo;
    glGenBuffers(1, &colours_LEFT_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_LEFT_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), colours_LEFT, GL_STATIC_DRAW);

    GLuint colours_TOP_vbo;
    glGenBuffers(1, &colours_TOP_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_TOP_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), colours_TOP, GL_STATIC_DRAW);

    GLuint colours_BACK_vbo;
    glGenBuffers(1, &colours_BACK_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_BACK_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), colours_BACK, GL_STATIC_DRAW);

    GLuint colours_RIGHT_vbo;
    glGenBuffers(1, &colours_RIGHT_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_RIGHT_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), colours_RIGHT, GL_STATIC_DRAW);

    GLuint colours_BOTTOM_vbo;
    glGenBuffers(1, &colours_BOTTOM_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colours_BOTTOM_vbo);
    glBufferData(GL_ARRAY_BUFFER, 2 * 9 * sizeof(GLfloat), colours_BOTTOM, GL_STATIC_DRAW);






    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint vao_LEFT;
    glGenVertexArrays(1, &vao_LEFT);
    glBindVertexArray(vao_LEFT);
    glBindBuffer(GL_ARRAY_BUFFER, points_LEFT_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_LEFT_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint vao_TOP;
    glGenVertexArrays(1, &vao_TOP);
    glBindVertexArray(vao_TOP);
    glBindBuffer(GL_ARRAY_BUFFER, points_TOP_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_TOP_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint vao_BACK;
    glGenVertexArrays(1, &vao_BACK);
    glBindVertexArray(vao_BACK);
    glBindBuffer(GL_ARRAY_BUFFER, points_BACK_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_BACK_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint vao_RIGHT;
    glGenVertexArrays(1, &vao_RIGHT);
    glBindVertexArray(vao_RIGHT);
    glBindBuffer(GL_ARRAY_BUFFER, points_RIGHT_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_RIGHT_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint vao_BOTTOM;
    glGenVertexArrays(1, &vao_BOTTOM);
    glBindVertexArray(vao_BOTTOM);
    glBindBuffer(GL_ARRAY_BUFFER, points_BOTTOM_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, colours_BOTTOM_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);



    //------------------------------create shaders--------------------------------
    char vertex_shader[1024 * 256];
    char fragment_shader[1024 * 256];
    //parse_file_into_str("test_vs.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("mukodo.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("test_fs.glsl", fragment_shader, 1024 * 256);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* p = (const GLchar*)vertex_shader;
    glShaderSource(vs, 1, &p, NULL);
    glCompileShader(vs);

    // check for compile errors
    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
        print_shader_info_log(vs);
        return 1; // or exit or something
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    p = (const GLchar*)fragment_shader;
    glShaderSource(fs, 1, &p, NULL);
    glCompileShader(fs);

    // check for compile errors
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
        print_shader_info_log(fs);
        return 1; // or exit or something
    }

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params) {
        fprintf(stderr, "ERROR: could not link shader programme GL index %i\n", shader_programme);
        print_programme_info_log(shader_programme);
        return false;
    }

    //--------------------------create camera matrices----------------------------
    // create PROJECTION MATRIX 
    // input variables
    float near = 0.1f;                                   // clipping plane
    float far = 100.0f;                                 // clipping plane
    float fov = 67.0f * ONE_DEG_IN_RAD;                 // convert 67 degrees to radians
    float aspect = (float)g_gl_width / (float)g_gl_height; // aspect ratio
    // matrix components
    float inverse_range = 1.0f / tan(fov * 0.5f);
    float Sx = inverse_range / aspect;
    float Sy = inverse_range;
    float Sz = -(far + near) / (far - near);
    float Pz = -(2.0f * far * near) / (far - near);
    GLfloat proj_mat[] = { Sx, 0.0f, 0.0f, 0.0f, 0.0f, Sy, 0.0f, 0.0f, 0.0f, 0.0f, Sz, -1.0f, 0.0f, 0.0f, Pz, 0.0f };

    // create VIEW MATRIX 
    float cam_speed = 1.0f;                 // 1 unit per second
    float cam_yaw_speed = 10.0f;                // 10 degrees per second
    float cam_pos[] = { 0.0f, 0.0f, 2.0f }; // don't start at zero, or we will be too close
    float cam_yaw = 0.0f;                 // y-rotation in degrees
    mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
    mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
    mat4 view_mat = R * T;

    // get location numbers of matrices in shader programme 
    GLint view_mat_location = glGetUniformLocation(shader_programme, "view");
    GLint felle_location = glGetUniformLocation(shader_programme, "felle");
    GLint forgas_location = glGetUniformLocation(shader_programme, "forgas");
    GLint proj_mat_location = glGetUniformLocation(shader_programme, "proj");
    // use program (make current in state machine) and set default matrix values//
    glUseProgram(shader_programme);
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
    glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);
    glUniformMatrix4fv(felle_location, 1, GL_FALSE, felle);
    glUniformMatrix4fv(forgas_location, 1, GL_FALSE, forgas);

    //------------------------------rendering loop--------------------------------//
    // some rendering defaults 
    glEnable(GL_CULL_FACE); // cull face
    glCullFace(GL_FRONT);    // cull back face
    glFrontFace(GL_CW);     // GL_CCW for counter clock-wise


    static double previous_seconds = glfwGetTime();
    double current_seconds = glfwGetTime();
    double elapsed_seconds = current_seconds - previous_seconds;
    previous_seconds = current_seconds;
    GLfloat speed = 1.0f;
    GLfloat last_position = 0.0f;


    bool isPressed = false;

    while (!glfwWindowShouldClose(g_window)) {
        static double previous_seconds = glfwGetTime();
        double current_seconds = glfwGetTime();
        double elapsed_seconds = current_seconds - previous_seconds;
        previous_seconds = current_seconds;

        _update_fps_counter(g_window);
        // wipe the drawing surface clear
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);

        glUseProgram(shader_programme);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glCullFace(GL_FRONT);    // cull back face
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(vao_TOP);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(vao_RIGHT);
        glDrawArrays(GL_TRIANGLES, 0, 6);




        glCullFace(GL_BACK);    // cull back face
        glBindVertexArray(vao_LEFT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(vao_BACK);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(vao_BOTTOM);
        glDrawArrays(GL_TRIANGLES, 0, 6);





        // update other events like input handling
        glfwPollEvents();

        //-----------------------------move camera here-------------------------------
        // control keys
        bool cam_moved = false;
        if (glfwGetKey(g_window, GLFW_KEY_A)) {
            cam_pos[0] -= cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_D)) {
            cam_pos[0] += cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_PAGE_UP)) {
            cam_pos[1] += cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_PAGE_DOWN)) {
            cam_pos[1] -= cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_W)) {
            cam_pos[2] -= cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_S)) {
            cam_pos[2] += cam_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_LEFT)) {
            cam_yaw += cam_yaw_speed * elapsed_seconds;
            cam_moved = true;
        }
        if (glfwGetKey(g_window, GLFW_KEY_RIGHT)) {
            cam_yaw -= cam_yaw_speed * elapsed_seconds;
            cam_moved = true;
        }


        // update view matrix 
        if (cam_moved) {
            mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1],
                -cam_pos[2])); // cam translation
            mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);     //
            mat4 view_mat = R * T;
            glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
        }

        GLdouble time = glfwGetTime();

        if (glfwGetKey(g_window, GLFW_KEY_P)) {
            isPressed = !isPressed;
        }

        if (!isPressed)
        {

            //felle[13] = (int(time) % 3)-1;
            felle[13] = elapsed_seconds * speed + last_position;
            last_position = felle[13];
            if (fabs(last_position) > 1.0) { speed = -speed; }

            forgas[0] = cos(glfwGetTime());
            forgas[2] = -sin(glfwGetTime());
            forgas[8] = sin(glfwGetTime());
            forgas[10] = cos(glfwGetTime());

            glUniformMatrix4fv(felle_location, 1, GL_FALSE, felle);
            glUniformMatrix4fv(forgas_location, 1, GL_FALSE, forgas);

        }



        if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) { glfwSetWindowShouldClose(g_window, 1); }
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(g_window);
    }

    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}



/**/