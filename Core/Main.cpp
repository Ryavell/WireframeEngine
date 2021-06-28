#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "glew.h"
#include "freeglut.h"
#include "WireframeObject.h"
#include "ObjectLoader.h"
#include "Keyboard.h"
#include "Camera.h"
#define WINDOW_TITLE_PREFIX "Application"

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

unsigned FrameCount = 0;

unsigned char keys[256];

GLfloat hPos = 0.0f;
GLfloat vPos = 0.0f;
GLfloat scale = 1.0f;

GLfloat rotX = 0.0f;
GLfloat rotY = 0.0f;

vector<GLfloat> Vertices = {
      -0.8f,  0.8f, 0.0f, 1.0f,
       0.8f,  0.8f, 0.0f, 1.0f,
      0.8f, -0.8f, 0.0f, 1.0f,
       -0.8f, -0.8f, 0.0f, 1.0f
};

vector<GLfloat> Colors = {
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  1.0f, 1.0f, 1.0f, 1.0f
};

WireframeObject* object;
WireframeObject* testObject;
ObjectLoader* objectLoader;

ShaderProgram* mainProgram;
Camera* camera;

void Initialize(int, char* []);
void InitWindow(int, char* []);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void KeyboardFunction(unsigned char, int, int);
void KeyboardUpFunction(unsigned char, int, int);
void Cleanup(void);

int main(int argc, char* argv[])
{
    Initialize(argc, argv);

    glutMainLoop();

    exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
    GLenum GlewInitResult;
    
    glewExperimental = GL_TRUE;
    InitWindow(argc, argv);
    
    GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult) {
        fprintf(
            stderr,
            "ERROR: %s\n",
            glewGetErrorString(GlewInitResult)
        );
        exit(EXIT_FAILURE);
    }

    fprintf(
        stdout,
        "INFO: OpenGL Version: %s\n",
        glGetString(GL_VERSION)
    );
    

    mainProgram = new ShaderProgram("Shaders/vertex.glsl", "Shaders/fragment.glsl");
    mainProgram->Use();

    camera = new Camera(60.0f, CurrentWidth, CurrentHeight, 0.2f, 1000.0f);
    camera->UpdateProjection(mainProgram);

    vector<Vector4> totalVertices;

    objectLoader = new ObjectLoader;
    ObjectInfo info = objectLoader->Load("Objects/cube.3d");

    for (Face f : info.faces)
    {
        for (int i = 0; i < 3; i++)
        {
            int vertexIndex = f.vertexIndices[i];
            Vertex v = info.vertices[vertexIndex - 1];
            Vector4 vertexVector = Vector4(v.x, v.y, v.z);
            
            totalVertices.push_back(vertexVector);
        }
    }
    vector<GLfloat> testVertices = {
        -0.8f, 0.8f, 0.0f, 1.0f,
        0.8f, 0.8f, 0.0f, 1.0f,
        0.8f, -0.8f, 0.0f, 1.0f,
        -0.8f, -0.8f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        -0.1f, 0.1f, 1.9f, 1.0f,
        0.1f, 0.1f, 1.9f, 1.0f,
        0.1f, -0.1f, 1.9f, 1.0f,
        -0.1f, -0.1f, 1.9f, 1.0f,
    };

    Vector4 whiteColor = Vector4(1.0f, 1.0f, 1.0f);

    object = new WireframeObject(totalVertices, whiteColor);
    //testObject = new WireframeObject(testVertices, Colors);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0f);
}

void InitWindow(int argc, char* argv[])
{
    glutInit(&argc, argv);

    glutInitContextVersion(4, 0);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

    glutInitWindowSize(CurrentWidth, CurrentHeight);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

    if (WindowHandle < 1) {
        fprintf(
            stderr,
            "ERROR: Could not create a new rendering window.\n"
        );
        exit(EXIT_FAILURE);
    }

    glutReshapeFunc(ResizeFunction);
    glutDisplayFunc(RenderFunction);
    glutKeyboardFunc(Keyboard::KeyboardDown);
    glutKeyboardUpFunc(Keyboard::KeyboardUp);
    glutIgnoreKeyRepeat(GL_TRUE);
    glutIdleFunc(IdleFunction);
    glutTimerFunc(0, TimerFunction, 0);
    glutCloseFunc(Cleanup);
}

void ResizeFunction(int Width, int Height)
{
    CurrentWidth = Width;
    CurrentHeight = Height;
    glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void RenderFunction(void)
{
    GLenum ErrorCheckValue = glGetError();

    ++FrameCount;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    object->Render(mainProgram);
    object->SetPosition(vPos, hPos, 0.0f);
    object->SetScale(scale, scale, scale);
    object->SetRotation(rotX, rotY, 0.0f);
    if (Keyboard::KeyPressed(LKEY_W))
        hPos += 0.001;
    if (Keyboard::KeyPressed(LKEY_S))
        hPos -= 0.001;
    if (Keyboard::KeyPressed(LKEY_D))
        vPos += 0.001;
    if (Keyboard::KeyPressed(LKEY_A))
        vPos -= 0.001;
    if (Keyboard::KeyPressed(KEY_PLUS))
        scale += 0.001;
    if (Keyboard::KeyPressed(KEY_MINUS))
        scale -= 0.001;
    if (Keyboard::KeyPressed(KEY_8))
        rotX += 0.1;
    if (Keyboard::KeyPressed(KEY_2))
        rotX -= 0.1;
    if (Keyboard::KeyPressed(KEY_4))
        rotY -= 0.1;
    if (Keyboard::KeyPressed(KEY_6))
        rotY += 0.1;
    glutSwapBuffers();
}

void IdleFunction(void)
{
    glutPostRedisplay();
}

void TimerFunction(int Value)
{
    if (0 != Value) {
        int size = 512 + strlen(WINDOW_TITLE_PREFIX);
        char* TempString = (char*)
            malloc(size);
        sprintf_s(
            TempString,
            size,
            "%s: %d Frames Per Second @ %d x %d",
            WINDOW_TITLE_PREFIX,
            FrameCount * 4,
            CurrentWidth,
            CurrentHeight
        );
        
        
        glutSetWindowTitle(TempString);
        free(TempString);
    }

    FrameCount = 0;
    glutTimerFunc(250, TimerFunction, 1);
}

void Cleanup(void)
{
    delete object;
}
