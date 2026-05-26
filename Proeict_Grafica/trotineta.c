
#include "glos.h"
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

static float scooterX = 0.0f;
static float scooterZ = 0.0f;
static float scooterYaw = 0.0f; //ține rotația trotinetei în jurul axei verticale.
static float steer = 0.0f; //directie ghidonului
static float wheelSpin = 0.0f;//unghiul de rotație al roților în jurul axei lor
static GLfloat lightPos[4] = { 2.0f, 3.0f, -2.0f, 1.0f };   //pozitia luminii si ultimul element 1.0f pentru a indica o lumină punctuală
static float moveSpeed = 0.03f;//viteza trotinetei
static float wheelRadius = 0.22f;
static const float scooterMinX = -2.5f;//limita min pe axa x pentru a nu iesi din scena
static const float scooterMaxX = 2.5f;//limita max pe axa x pentru a nu iesi din scena

static void ShadowMatrix(GLfloat m[16], const GLfloat plane[4], const GLfloat light[4])
{
    GLfloat temp = plane[0] * light[0] + plane[1] * light[1] + plane[2] * light[2] + plane[3] * light[3];

    m[0] = temp - light[0] * plane[0];  m[4] = -light[0] * plane[1];      m[8] = -light[0] * plane[2];      m[12] = -light[0] * plane[3];
    m[1] = -light[1] * plane[0];       m[5] = temp - light[1] * plane[1]; m[9] = -light[1] * plane[2];      m[13] = -light[1] * plane[3];
    m[2] = -light[2] * plane[0];       m[6] = -light[2] * plane[1];      m[10] = temp - light[2] * plane[2];  m[14] = -light[2] * plane[3];
    m[3] = -light[3] * plane[0];       m[7] = -light[3] * plane[1];      m[11] = -light[3] * plane[2];       m[15] = temp - light[3] * plane[3];
}

static void SetMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat shininess)
{
    GLfloat diffuse[] = { r, g, b, 1.0f };
    GLfloat specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat ambient[] = { r * 0.2f, g * 0.2f, b * 0.2f, 1.0f };
    GLfloat sh[] = { shininess };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, sh);
}

//pentru capetele cilindrului, adică pentru baza și partea de sus a roții 
static void DrawDisk(float r, int slices)
{
    glNormal3f(0, 0, 1);
    glBegin(GL_POLYGON);
    for (int i = 0; i < slices; i++) {
        float a = 2.0f * 3.14 * i / slices;
        glVertex3f(r * cosf(a), r * sinf(a), 0.0f);
    }
    glEnd();
}

//deseneaza un cilindru cu raza r și înălțimea h, împărțit în "slices" segmente pentru a crea o aproximare a formei cilindrice. De asemenea, adaugă capetele cilindrului folosind funcția DrawDisk pentru a închide cilindrul și a-i oferi un aspect solid.
static void DrawCylinder(float radius, float height, int slices)
{
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float a = 2.0f * 3.14 * i / slices;
        float x = radius * cosf(a);
        float y = radius * sinf(a);
        glNormal3f(cosf(a), sinf(a), 0.0f);
        glVertex3f(x, y, 0.0f);
        glVertex3f(x, y, height);
    }
    glEnd();

    glPushMatrix();
    glNormal3f(0, 0, -1);
    glRotatef(180.0f, 1, 0, 0);
    DrawDisk(radius, slices);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, height);
    glNormal3f(0, 0, 1);
    DrawDisk(radius, slices);
    glPopMatrix();
}

//deseneaza o cutie (box) cu dimensiunile specificate de lățime (w), înălțime (h) și adâncime (d). Funcția utilizează glBegin(GL_QUADS) pentru a desena fiecare față a cutiei ca un quadrilateral. Pentru fiecare față, se specifică normalul corespunzător pentru a asigura iluminarea corectă și apoi se definesc cele patru vârfuri ale feței folosind glVertex3f. Cutia este centrată în origine, iar dimensiunile sunt calculate astfel încât să se extindă de la -x la +x, de la -y la +y și de la -z la +z, unde x, y și z sunt jumătate din dimensiunile w, h și d, respectiv.
static void DrawBox(float w, float h, float d)
{
    float x = w * 0.5f, y = h * 0.5f, z = d * 0.5f;

    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1);
    glVertex3f(-x, -y, z); glVertex3f(x, -y, z); glVertex3f(x, y, z); glVertex3f(-x, y, z);

    glNormal3f(0, 0, -1);
    glVertex3f(x, -y, -z); glVertex3f(-x, -y, -z); glVertex3f(-x, y, -z); glVertex3f(x, y, -z);

    glNormal3f(1, 0, 0);
    glVertex3f(x, -y, z); glVertex3f(x, -y, -z); glVertex3f(x, y, -z); glVertex3f(x, y, z);

    glNormal3f(-1, 0, 0);
   glVertex3f(-x, -y, -z); glVertex3f(-x, -y, z); glVertex3f(-x, y, z); glVertex3f(-x, y, -z);

    glNormal3f(0, 1, 0);
    glVertex3f(-x, y, z); glVertex3f(x, y, z); glVertex3f(x, y, -z); glVertex3f(-x, y, -z);

    glNormal3f(0, -1, 0);
    glVertex3f(-x, -y, -z); glVertex3f(x, -y, -z); glVertex3f(x, -y, z); glVertex3f(-x, -y, z);

    glEnd();
}

//deseneaza o roată de trotineta. Funcția utilizează glPushMatrix și glPopMatrix pentru a salva și restaura matricea de transformare curentă, astfel încât rotațiile aplicate pentru a orienta roata să nu afecteze alte părți ale modelului. În interiorul funcției, se aplică o rotație de 90 de grade în jurul axei Y pentru a orienta roata corect, apoi se setează materialul pentru roată folosind SetMaterial și se desenează cilindrul care reprezintă roata folosind DrawCylinder.
static void DrawWheel(void)
{
    glPushMatrix();
    glRotatef(90.0f, 0, 1, 0);
    SetMaterial(0.1f, 0.1f, 0.1f, 30.0f);
    DrawCylinder(wheelRadius, 0.08f, 10);
    glPopMatrix();
}

//deseneaza modelul complet al trotinetei, compus dintr-un deck (plată) și două roți. Funcția utilizează glPushMatrix și glPopMatrix pentru a organiza transformările și a asigura că fiecare componentă a modelului este poziționată corect în spațiu. În interiorul funcției, se setează materialul pentru deck și se desenează cutia care reprezintă platforma trotinetei folosind DrawBox. Apoi, se desenează roata din spate și roata din față, aplic
static void DrawScooterModel(void)
{

	//platforma trotinetei
    glPushMatrix();
    SetMaterial(0.2f, 0.5f, 0.9f, 60.0f);
    glTranslatef(0.0f, 0.25f, 0.0f);
    DrawBox(1.6f, 0.10f, 0.35f);
    glPopMatrix();

	//muta si roteste roata din spate
    glPushMatrix();
    glTranslatef(-0.004f, wheelRadius, 0.0f);
	glRotatef(90.0f, 0, 1, 0);
    glRotatef(wheelSpin, 1, 0, 0);
    DrawWheel();
    glPopMatrix();

    //creeaza ghidonul
    glPushMatrix();
    glTranslatef(0.65f, 0.0f, 0.0f);
    glRotatef(90.0f, 0, 1, 0);
    glRotatef(steer, 0, 1, 0);

    // constante
	const float deckY = 0.25f;// inaltimea deck-ului (platformei) de la sol
	const float stemBaseY = deckY + 0.05f;// inaltimea la care se fixeaza coloana (stem) pe deck
	const float stemH = 0.95f;// inaltimea coloanei (stem)

	const float barY = stemBaseY + stemH;// inaltimea la care se fixeaza bara ghidon pe coloana
    const float barHalfW = 0.55f;     // marime ghidon (stanga/dreapta)
	const float barR = 0.020f;// grosime ghidon

	const float gripLen = 0.14f;// lungime mansoane (grips) la capetele ghidonului
	const float gripR = 0.026f;// grosime mansoane (grips)

    const float barZ = 0.07f;         // IMPORTANT!! scoate ghidonul un pic spre camera
	const float forkTopY = stemBaseY + 0.30f;// inaltimea la care se fixeaza furca pe coloana (stem)
	const float forkBottomY = wheelRadius + 0.02f;// inaltimea la care se fixeaza furca pe roata
	const float forkH = (forkTopY - forkBottomY);// inaltimea furcii (diferenta dintre cele doua puncte de fixare)

    //  Coloana (stem)
    glPushMatrix();
    glTranslatef(0.0f, stemBaseY, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);
    SetMaterial(0.90f, 0.90f, 0.90f, 80.0f);
    DrawCylinder(0.03f, stemH, 16);
    glPopMatrix();

    //  “pipa” (blocul de sus) 
    glPushMatrix();
    glTranslatef(0.0f, barY - 0.03f, barZ);
    SetMaterial(0.80f, 0.80f, 0.80f, 60.0f);
    DrawBox(0.07f, 0.06f, 0.08f);
    glPopMatrix();

    //  Bara ghidon (centrata) - desenata de la -barHalfW la +barHalfW
    glPushMatrix();
    glTranslatef(-barHalfW, barY, barZ);   // pornim din stanga
    glRotatef(90.0f, 0, 1, 0);             // cilindru pe axa X
    SetMaterial(0.90f, 0.20f, 0.20f, 80.0f);
    DrawCylinder(barR, barHalfW * 2.0f, 16);
    glPopMatrix();


    //  Roata fata
    glPushMatrix();
    glTranslatef(0.0f, wheelRadius, 4.0f);
    glRotatef(wheelSpin, 1, 0, 0);
    DrawWheel();
    glPopMatrix();

    glPopMatrix();
}
//deseneaza întreaga lume a trotinetei, adică poziționează și rotește modelul trotinetei în funcție de variabilele globale care controlează poziția și orientarea acesteia. Funcția utilizează glPushMatrix și glPopMatrix pentru a salva și restaura matricea de transformare curentă, astfel încât transformările aplicate pentru a poziționa și roti trotineta să nu afecteze alte elemente din scenă. În interiorul funcției, se aplică o translație pentru a poziționa trotineta la coordonatele specificate de scooterX și scooterZ, apoi se aplică o rotație în jurul axei Y pentru a orienta trotineta conform valorii din scooterYaw. În final, se apelează funcția DrawScooterModel pentru a desena modelul complet al trotinetei în poziția și orientarea calculate.
static void DrawScooterWorld(void)
{
    glPushMatrix();
    glTranslatef(scooterX, 0.0f, scooterZ);
    glRotatef(scooterYaw, 0, 1, 0);
    DrawScooterModel();
    glPopMatrix();
}
static void myinit(void)
{
    glClearColor(0.15f, 0.18f, 0.22f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat diffuse[] = { 0.90f, 0.90f, 0.90f, 1.0f };
    GLfloat specular[] = { 1.00f, 1.00f, 1.00f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

static void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    if (!h) return;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (GLfloat)w / (GLfloat)h, 0.1, 12.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, -0.4f, -6.0f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
}
static void CALLBACK display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Aici se setează lumina.

    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glPopMatrix();
    {
        //deseneaza solul si grila
        glDisable(GL_LIGHTING);
        glColor3f(0.85f, 0.85f, 0.85f);
        glBegin(GL_QUADS);
        glVertex3f(-12.0f, 0.0f, -12.0f);
        glVertex3f(12.0f, 0.0f, -12.0f);
        glVertex3f(12.0f, 0.0f, 12.0f);
        glVertex3f(-12.0f, 0.0f, 12.0f);
        glEnd();

        glColor3f(0.70f, 0.70f, 0.70f);
        glBegin(GL_LINES);
        for (int i = -10; i <= 10; i++) {
            glVertex3f((float)i, 0.0f, -10.0f);
            glVertex3f((float)i, 0.0f, 10.0f);
            glVertex3f(-10.0f, 0.0f, (float)i);
            glVertex3f(10.0f, 0.0f, (float)i);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }

    {
        //Becul/lumina
        glPushMatrix();

        glTranslatef(lightPos[0], lightPos[1], lightPos[2]);

        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.2f);
        DrawBox(0.12f, 0.12f, 0.12f);   // “becul” (vizibil clar)

        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
    //trotineta
    glPushMatrix();
    glColor3f(0.2f, 0.5f, 0.9f);
    DrawScooterWorld();
    glPopMatrix();

    {
        //umbra
        const GLfloat plane[4] = { 0, 1, 0, 0 };
        GLfloat sm[16];
        ShadowMatrix(sm, plane, lightPos);

        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-2.0f, -2.0f);//offset de adancime

        glColor4f(0.0f, 0.0f, 0.0f, 1.0f);

        glPushMatrix();
        glMultMatrixf(sm);//aplica matricea umbra
        glTranslatef(0.0f, 0.001f, 0.0f);   // mic “lift” sa nu se piarda pe sol
        DrawScooterWorld();

        glPopMatrix();

        glDisable(GL_POLYGON_OFFSET_FILL);
        glDepthMask(GL_TRUE);
        glEnable(GL_LIGHTING);
    }

    auxSwapBuffers();
}

static void CALLBACK IdleFunction(void)
{
    wheelSpin += 6.0f;
    if (wheelSpin >= 360.0f) wheelSpin -= 360.0f;
    display();
    Sleep(16);
}

static void CALLBACK MoveLeft(void)
{
    scooterX -= moveSpeed;
    if (scooterX < scooterMinX) scooterX = scooterMinX;
}

static void CALLBACK MoveRight(void)
{   
    scooterX += moveSpeed;
    if (scooterX > scooterMaxX) scooterX = scooterMaxX;
}
static void CALLBACK LeftWheel(void)
{
    wheelSpin += 10.0f;
    if (wheelSpin >= 360.0f) wheelSpin -= 360.0f;
}

static void CALLBACK RightWheel(void)
{
    wheelSpin += 10.0f;
    if (wheelSpin >= 360.0f) wheelSpin -= 360.0f;
}

static void CALLBACK LightLeft(void) { lightPos[0] -= 0.2f; }
static void CALLBACK LightRight(void) { lightPos[0] += 0.2f; }
static void CALLBACK LightUp(void) { lightPos[1] += 0.2f; }
static void CALLBACK LightDown(void) { 
    lightPos[1] -= 0.2f; 
if (lightPos[1] < 0.2f) lightPos[1] = 0.2f; }
static void CALLBACK LightIn(void) { lightPos[2] -= 0.2f; }
static void CALLBACK LightOut(void) { lightPos[2] += 0.2f; }


int main(int argc, char** argv)
{
    auxInitDisplayMode(AUX_DOUBLE | AUX_RGB | AUX_DEPTH16);
    auxInitPosition(10, 10, 800, 600);
    auxInitWindow("Trotineta");

    myinit();
    auxReshapeFunc(myReshape);

    auxKeyFunc('A', MoveLeft);
	auxKeyFunc('a', MoveLeft);
    auxKeyFunc('D', MoveRight);
	auxKeyFunc('d', MoveRight);

    auxKeyFunc('J', LightLeft);
	auxKeyFunc('j', LightLeft);
    auxKeyFunc('L', LightRight);
	auxKeyFunc('l', LightRight);    
    auxKeyFunc('U', LightUp);
	auxKeyFunc('u', LightUp);
    auxKeyFunc('O', LightDown);
	auxKeyFunc('o', LightDown);
    auxKeyFunc('I', LightIn);
	auxKeyFunc('i', LightIn);
    auxKeyFunc('K', LightOut);
	auxKeyFunc('k', LightOut);

    auxIdleFunc(IdleFunction);
    auxMainLoop(display);
    return 0;
}   