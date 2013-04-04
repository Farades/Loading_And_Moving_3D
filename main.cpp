#include<windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <iostream>
#include <cmath>
#define TEX_IMAGE_NAME "D:/workspace/Loading_And_Moving_3D/Test.jpg"
#define WORLDFILE = "D:/workspace/Loading_And_Moving_3D/World.txt";

float WinWidth       = 1400.0;
float WinHeight      = 900.0;

const float piover180 = 0.0174532925f;
float heading;
float xpos;
float zpos;

GLfloat	yrot;				// Y Rotation
GLfloat walkbias = 0;
GLfloat walkbiasangle = 0;
GLfloat lookupdown = 0.0f;
GLfloat	z = 0.0f;

GLuint	filter = 0;
GLuint	texture[3];

typedef struct tagVERTEX
{
    float x, y, z;
    float u, v;
} VERTEX;

typedef struct tagTRIANGLE
{
    VERTEX vertex[3];
} TRIANGLE;

typedef struct tagSECTOR
{
    int numtriangles;
    TRIANGLE* triangle;
} SECTOR;

SECTOR sector1;

//-----------------------------------------------------------------------------

static void timer_rotation(int value = 0)
{

  /* send redisplay event */
  glutPostRedisplay();

  /* call this function again in 30 milliseconds */
  glutTimerFunc(10, timer_rotation, 0);
}

void readstr(FILE *f,char *string)
{
    do
    {
        fgets(string, 255, f);
    } while ((string[0] == '/') || (string[0] == '\n'));
    return;
}

//-----------------------------------------------------------------------------

void SetupWorld()
{
    float x, y, z, u, v;
    int numtriangles;
    FILE *filein;
    char oneline[255];
    filein = fopen("D:/workspace/Loading_And_Moving_3D/World.txt", "rt");

    readstr(filein,oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

    sector1.triangle = new TRIANGLE[numtriangles];
    sector1.numtriangles = numtriangles;
    for (int loop = 0; loop < numtriangles; loop++)
    {
        for (int vert = 0; vert < 3; vert++)
        {
            readstr(filein,oneline);
            sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
            sector1.triangle[loop].vertex[vert].x = x;
            sector1.triangle[loop].vertex[vert].y = y;
            sector1.triangle[loop].vertex[vert].z = z;
            sector1.triangle[loop].vertex[vert].u = u;
            sector1.triangle[loop].vertex[vert].v = v;
        }
    }
    fclose(filein);
    return;
}

//-----------------------------------------------------------------------------
//Функция загрузки изображения текстуры
void Load_Tex_Image()
{
    int width, height, bpp;

    ilLoad(IL_JPG, reinterpret_cast<const ILstring>(TEX_IMAGE_NAME));
    int err = ilGetError();                          // Считывание кода ошибки
    if (err != IL_NO_ERROR)
    {
        const char* strError = iluErrorString(err);  // Считываем строку ошибки
        std::cout << "Error load texture image: " << strError << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Load texture image completed!" << std::endl;
        width  = ilGetInteger(IL_IMAGE_WIDTH);
        height = ilGetInteger(IL_IMAGE_HEIGHT);
        bpp    = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
        std::cout << "width:  "<< width << std::endl << "height: "
                  << height << std::endl << "bpp:    " << bpp << std::endl;
    }

    unsigned char* data = ilGetData();
    unsigned int type;

    switch (bpp) {
    case 1:
      type  = GL_RGB8;
      break;
    case 3:
      type = GL_RGB;
      break;
    case 4:
      type = GL_RGBA;
      break;
    }
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
    GL_RGB, GL_UNSIGNED_BYTE, data);
}


void KeyboardEvent(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'q' :
        lookupdown -= 2.0f;
        break;
    case 'e' :
        lookupdown += 2.0f;
        break;
    case 'a' :
        heading += 2.0f;
        yrot = heading;              // Вращать сцену влево
        break;
    case 'd' :
        heading -= 2.0f;
        yrot = heading;              // Вращать сцену влево
        break;
    case 'w' :
        xpos -= (float)sin(heading * piover180) * 0.05f;
        zpos -= (float)cos(heading * piover180) * 0.05f;
        if (walkbiasangle >= 359.0f)
            walkbiasangle = 0.0f;
        else
            walkbiasangle+= 10;
        // Иммитация походки человека
        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
        break;
    case 's' :
        xpos += (float)sin(heading * piover180) * 0.05f;
        zpos += (float)cos(heading * piover180) * 0.05f;
        if (walkbiasangle <= 1.0f)
            walkbiasangle = 359.0f;
        else                          // В противном случае
            walkbiasangle-= 10;
        // Иммитация походки человека
        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
        break;
    }
}


//-----------------------------------------------------------------------------
//Функция инициализации
void init()
{
    //Инициализация DevIL
    ilInit();
    iluInit();
    ilutInit();

    Load_Tex_Image();

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport( 0, 0, WinWidth, WinHeight );
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45.0f, (GLfloat)WinWidth/(GLfloat)WinHeight, 0.1f, 100.0f );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    SetupWorld();
}


//-----------------------------------------------------------------------------
//Функция рисования сцены
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xtrans = -xpos;
    GLfloat ztrans = -zpos;
    GLfloat ytrans = -walkbias-0.25f;
    GLfloat sceneroty = 360.0f - yrot;

    int numtriangles;

    glRotatef(lookupdown,1.0f,0,0);
    glRotatef(sceneroty,0,1.0f,0);

    glTranslatef(xtrans, ytrans, ztrans);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    numtriangles = sector1.numtriangles;

    for (int loop_m = 0; loop_m < numtriangles; loop_m++)
    {
        glBegin(GL_TRIANGLES);
            glNormal3f( 0.0f, 0.0f, 1.0f);
            x_m = sector1.triangle[loop_m].vertex[0].x;
            y_m = sector1.triangle[loop_m].vertex[0].y;
            z_m = sector1.triangle[loop_m].vertex[0].z;
            u_m = sector1.triangle[loop_m].vertex[0].u;
            v_m = sector1.triangle[loop_m].vertex[0].v;
            glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

            x_m = sector1.triangle[loop_m].vertex[1].x;
            y_m = sector1.triangle[loop_m].vertex[1].y;
            z_m = sector1.triangle[loop_m].vertex[1].z;
            u_m = sector1.triangle[loop_m].vertex[1].u;
            v_m = sector1.triangle[loop_m].vertex[1].v;
            glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

            x_m = sector1.triangle[loop_m].vertex[2].x;
            y_m = sector1.triangle[loop_m].vertex[2].y;
            z_m = sector1.triangle[loop_m].vertex[2].z;
            u_m = sector1.triangle[loop_m].vertex[2].u;
            v_m = sector1.triangle[loop_m].vertex[2].v;
            glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);
        glEnd();
    }
    glFlush();
}

int main(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WinWidth, WinHeight);
    glutInitWindowPosition(100, 70);
    glutCreateWindow("Texture_test");
    glutDisplayFunc(display);
    init();
    glutKeyboardFunc(KeyboardEvent);
    timer_rotation();
    glutMainLoop();
}
