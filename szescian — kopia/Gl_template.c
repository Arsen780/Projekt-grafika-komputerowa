// Gl_template.c
//Wyłšczanie błędów przed "fopen"
#define  _CRT_SECURE_NO_WARNINGS
#define PI 3.14


float x;


// Ładowanie bibliotek:

#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif




// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif
#include <windows.h>            // Window defines
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#include <math.h>				// Define for sqrt
#include <stdio.h>
#include "resource.h"           // About box resource identifiers.

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14

// Color Palette handle
HPALETTE hPalette = NULL;

// Application name and instance storeage
static LPCTSTR lpszAppName = "GL Template";
static HINSTANCE hInstance;

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;
static GLfloat kat = 3.14f / 4.0f;


static GLfloat Camx = 15;
static GLfloat Camy = 15;
static GLfloat Camz = 15;

static GLfloat pos_z = 0;
static GLfloat pos_x = 0;




static GLsizei lastHeight;
static GLsizei lastWidth;

// Opis tekstury
BITMAPINFOHEADER	bitmapInfoHeader;	// nagłówek obrazu
unsigned char* bitmapData;			// dane tekstury
unsigned int		texture[2];			// obiekt tekstury


// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam);

// Dialog procedure for about box
BOOL APIENTRY AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);



// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}



// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 750.0f;
	GLfloat fAspect;
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	lastWidth = w;
	lastHeight = h;

	fAspect = (GLfloat)w / (GLfloat)h;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange * h / w, nRange * h / w, -nRange, nRange);
	else
		glOrtho(-nRange * w / h, nRange * w / h, -nRange, nRange, -nRange, nRange);

	// Establish perspective: 
	/*
	gluPerspective(60.0f,fAspect,1.0,400);
	*/

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
{
	// Light values and coordinates
	//GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	//GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	//GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	//GLfloat	 lightPos[] = { 0.0f, 150.0f, 150.0f, 1.0f };
	//GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	//glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

	// Enable lighting
	//glEnable(GL_LIGHTING);

	// Setup and enable light 0
	//glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	//glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	//glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
	//glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	//glEnable(GL_LIGHT0);

	// Enable color tracking
	//glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a high shine
	//glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	//glMateriali(GL_FRONT,GL_SHININESS,128);


	// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black brush
	glColor3f(0.0, 0.0, 0.0);
}

void skrzynka(void)
{
	glColor3d(0.8, 0.7, 0.3);


	glEnable(GL_TEXTURE_2D); // Włącz teksturowanie

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, 25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, -25, 25);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, 25, -25);
	glEnd();

	glDisable(GL_TEXTURE_2D); // Wyłącz teksturowanie



	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glVertex3d(25, 25, -25);
	glVertex3d(25, -25, -25);
	glVertex3d(-25, -25, -25);
	glVertex3d(-25, 25, -25);

	glNormal3d(-1, 0, 0);
	glVertex3d(-25, 25, -25);
	glVertex3d(-25, -25, -25);
	glVertex3d(-25, -25, 25);
	glVertex3d(-25, 25, 25);

	glNormal3d(0, 1, 0);
	glVertex3d(25, 25, 25);
	glVertex3d(25, 25, -25);
	glVertex3d(-25, 25, -25);
	glVertex3d(-25, 25, 25);

	glNormal3d(0, -1, 0);
	glVertex3d(25, -25, 25);
	glVertex3d(-25, -25, 25);
	glVertex3d(-25, -25, -25);
	glVertex3d(25, -25, -25);
	glEnd();
}

void walec01(void)
{
	GLUquadricObj* obj;
	obj = gluNewQuadric();
	gluQuadricNormals(obj, GLU_SMOOTH);
	glColor3d(1, 0, 0);
	glPushMatrix();
	gluCylinder(obj, 20, 20, 30, 15, 7);
	gluCylinder(obj, 0, 20, 0, 15, 7);
	glTranslated(0, 0, 60);
	glRotated(180.0, 0, 1, 0);
	gluCylinder(obj, 0, 20, 30, 15, 7);
	glPopMatrix();
}

void kula(void)
{
	GLUquadricObj* obj;
	obj = gluNewQuadric();
	gluQuadricTexture(obj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3d(1.0, 0.8, 0.8);
	glEnable(GL_TEXTURE_2D);
	gluSphere(obj, 40, 15, 7);
	glDisable(GL_TEXTURE_2D);
}




// LoadBitmapFile
// opis: ładuje mapę bitową z pliku i zwraca jej adres.
//       Wypełnia strukturę nagłówka.
//	 Nie obsługuje map 8-bitowych.
unsigned char* LoadBitmapFile(char* filename, BITMAPINFOHEADER* bitmapInfoHeader)
{
	FILE* filePtr;							// wskaźnik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nagłówek pliku
	unsigned char* bitmapImage;			// dane obrazu
	int					imageIdx = 0;		// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany składowych

	// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// wczytuje nagłówek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// wczytuje nagłówek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// ustawia wskaźnik pozycji pliku na początku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// przydziela pamięć buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// sprawdza, czy udało się przydzielić pamięć
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// sprawdza, czy dane zostały wczytane
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// zamienia miejscami składowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// zamyka plik i zwraca wskaźnik bufora zawierającego wczytany obraz
	fclose(filePtr);
	return bitmapImage;

}

void diament1()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { 425.0f,25.0f,425.0f };
		GLfloat sb[3] = { 450.0f,25.0f,425.0f };
		GLfloat sc[3] = { 450.0f,50.0f,425.0f };
		GLfloat sd[3] = { 425.0f,50.0f,425.0f };
		GLfloat se[3] = { 425.0f,25.0f,425.0f };
		GLfloat sf[3] = { 450.0f,25.0f,450.0f };
		GLfloat sg[3] = { 450.0f,50.0f,450.0f };
		GLfloat sh[3] = { 425.0f,50.0f,450.0f };


		// Sciany skladowe
		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sc);
		glVertex3fv(sd);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(sg);
		glVertex3fv(sc);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sf);
		glVertex3fv(se);
		glVertex3fv(sh);
		glVertex3fv(sg);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(se);
		glVertex3fv(sa);
		glVertex3fv(sd);
		glVertex3fv(sh);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sd);
		glVertex3fv(sc);
		glVertex3fv(sg);
		glVertex3fv(sh);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(se);
		glEnd();
	}
}

void diament2()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { -625.0f,25.0f,-625.0f };
		GLfloat sb[3] = { -650.0f,25.0f,-625.0f };
		GLfloat sc[3] = { -650.0f,50.0f,-625.0f };
		GLfloat sd[3] = { -625.0f,50.0f,-625.0f };
		GLfloat se[3] = { -625.0f,25.0f,-625.0f };
		GLfloat sf[3] = { -650.0f,25.0f,-650.0f };
		GLfloat sg[3] = { -650.0f,50.0f,-650.0f };
		GLfloat sh[3] = { -625.0f,50.0f,-650.0f };


		// Sciany skladowe
		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sc);
		glVertex3fv(sd);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(sg);
		glVertex3fv(sc);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sf);
		glVertex3fv(se);
		glVertex3fv(sh);
		glVertex3fv(sg);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(se);
		glVertex3fv(sa);
		glVertex3fv(sd);
		glVertex3fv(sh);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sd);
		glVertex3fv(sc);
		glVertex3fv(sg);
		glVertex3fv(sh);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(se);
		glEnd();
	}
}

void diament3()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { 25.0f,25.0f,-425.0f };
		GLfloat sb[3] = { 50.0f,25.0f,-425.0f };
		GLfloat sc[3] = { 50.0f,50.0f,-425.0f };
		GLfloat sd[3] = { 25.0f,50.0f,-425.0f };
		GLfloat se[3] = { 25.0f,25.0f,-425.0f };
		GLfloat sf[3] = { 50.0f,25.0f,-450.0f };
		GLfloat sg[3] = { 50.0f,50.0f,-450.0f };
		GLfloat sh[3] = { 25.0f,50.0f,-450.0f };


		// Sciany skladowe
		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sc);
		glVertex3fv(sd);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(sg);
		glVertex3fv(sc);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sf);
		glVertex3fv(se);
		glVertex3fv(sh);
		glVertex3fv(sg);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(se);
		glVertex3fv(sa);
		glVertex3fv(sd);
		glVertex3fv(sh);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sd);
		glVertex3fv(sc);
		glVertex3fv(sg);
		glVertex3fv(sh);
		glEnd();

		glColor3f(0.56f, 1.0f, 0.76f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(se);
		glEnd();
	}
}


void galezie()
{//GALEZIE 1
	GLfloat a1[3] = { 580.0f,38.0f,-238.0f };
	GLfloat a2[3] = { 580.0f,188.0f,-198.0f };
	GLfloat a3[3] = { 580.0f,38.0f,-158.0f };

	glEnable(GL_TEXTURE_2D); // Włącz teksturowanie

	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(a1);
	glTexCoord2d(0.0, 1.0); glVertex3fv(a2);
	glTexCoord2d(0.0, 0.0); glVertex3fv(a3);
	glEnd();

	GLfloat a4[3] = { 540.0f,38.0f,-198.0f };
	GLfloat a5[3] = { 580.0f,188.0f,-198.0f };
	GLfloat a6[3] = { 620.0f,38.0f,-198.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(a4);
	glTexCoord2d(0.0, 1.0); glVertex3fv(a5);
	glTexCoord2d(0.0, 0.0); glVertex3fv(a6);
	glEnd();

	GLfloat a7[3] = { 608.0f,38.0f,-174.0f };
	GLfloat a8[3] = { 580.0f,188.0f,-198.0f };
	GLfloat a9[3] = { 552.0f,38.0f,-222.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(a7);
	glTexCoord2d(0.0, 1.0); glVertex3fv(a8);
	glTexCoord2d(0.0, 0.0); glVertex3fv(a9);
	glEnd();

	GLfloat a11[3] = { 562.0f,38.0f,-226.0f };
	GLfloat a12[3] = { 580.0f,188.0f,-198.0f };
	GLfloat a13[3] = { 608.0f,38.0f,-170.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(a11);
	glTexCoord2d(0.0, 1.0); glVertex3fv(a12);
	glTexCoord2d(0.0, 0.0); glVertex3fv(a13);
	glEnd();

	//GALEZIE 2
	GLfloat ab1[3] = { 580.0f - 982.0f,38.0f,-238.0f - 430.0f };
	GLfloat ab2[3] = { 580.0f - 982.0f,188.0f,-198.0f - 430.0f };
	GLfloat ab3[3] = { 580.0f - 982.0f,38.0f,-158.0f - 430.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(ab1);
	glTexCoord2d(0.0, 1.0); glVertex3fv(ab2);
	glTexCoord2d(0.0, 0.0); glVertex3fv(ab3);
	glEnd();

	GLfloat ab4[3] = { 540.0f - 982.0f,38.0f,-198.0f - 430.0f };
	GLfloat ab5[3] = { 580.0f - 982.0f,188.0f,-198.0f - 430.0f };
	GLfloat ab6[3] = { 620.0f - 982.0f,38.0f,-198.0f - 430.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(ab4);
	glTexCoord2d(0.0, 1.0); glVertex3fv(ab5);
	glTexCoord2d(0.0, 0.0); glVertex3fv(ab6);

	GLfloat ab7[3] = { 608.0f - 982.0f,38.0f,-174.0f - 430.0f };
	GLfloat ab8[3] = { 580.0f - 982.0f,188.0f,-198.0f - 430.0f };
	GLfloat ab9[3] = { 552.0f - 982.0f,38.0f,-222.0f - 430.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(ab7);
	glTexCoord2d(0.0, 1.0); glVertex3fv(ab8);
	glTexCoord2d(0.0, 0.0); glVertex3fv(ab9);
	glEnd();

	GLfloat ab11[3] = { 562.0f - 982.0f,38.0f,-226.0f - 430.0f };
	GLfloat ab12[3] = { 580.0f - 982.0f,188.0f,-198.0f - 430.0f };
	GLfloat ab13[3] = { 608.0f - 982.0f,38.0f,-170.0f - 430.0f };

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3fv(ab11);
	glTexCoord2d(0.0, 1.0); glVertex3fv(ab12);
	glTexCoord2d(0.0, 0.0); glVertex3fv(ab13);
	glEnd();
	glDisable(GL_TEXTURE_2D); // Wyłącz teksturowanie
}

void pnie()
{
	double xo, yo, alphao, PIo = 3.14;
	double ra = 8, ha = 38;
	//PIEŃ1
	glPushMatrix();
	glTranslatef(580.0f, 38.0f, -198.0f);
	glRotatef(90.0f, 90.0f, 0.0f, 0.0f); // Obrót o 90 stopni w osi Y
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.647059, 0.164706, 0.164706);
	glVertex3d(0, 0, 0);
	for (alphao = 0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alphao = 0.0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, 0);
		glVertex3d(xo, yo, ha);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, ha);
	for (alphao = 0; alphao >= -2 * PIo; alphao -= PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, ha);
	}
	glEnd();
	glPopMatrix();

	//PIEN2

	glPushMatrix();
	glTranslatef(-398.0f, 38.0f, -628.0f);
	glRotatef(90.0f, 90.0f, 0.0f, 0.0f); // Obrót o 90 stopni w osi Y
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.647059, 0.164706, 0.164706);
	glVertex3d(0, 0, 0);
	for (alphao = 0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alphao = 0.0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, 0);
		glVertex3d(xo, yo, ha);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, ha);
	for (alphao = 0; alphao >= -2 * PIo; alphao -= PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, ha);
	}
	glEnd();
	glPopMatrix();

}


void kamien()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { 300.0f,5.0f,300.0f };
		GLfloat sb[3] = { 350.0f,5.0f,300.0f };
		GLfloat sc[3] = { 350.0f,55.0f,300.0f };
		GLfloat sd[3] = { 300.0f,55.0f,300.0f };
		GLfloat se[3] = { 300.0f,5.0f,350.0f };
		GLfloat sf[3] = { 350.0f,5.0f,350.0f };
		GLfloat sg[3] = { 350.0f,55.0f,350.0f };
		GLfloat sh[3] = { 300.0f,55.0f,350.0f };

		glEnable(GL_TEXTURE_2D); // Włącz teksturowanie

		// Sciany skladowe
		glColor3f(0.5f, 0.5f, 0.5f);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glTexCoord2d(1.0, 1.0); glVertex3fv(sa);
		glTexCoord2d(0.0, 1.0); glVertex3fv(sb);
		glTexCoord2d(0.0, 0.0); glVertex3fv(sc);
		glTexCoord2d(1.0, 0.0); glVertex3fv(sd);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glTexCoord2d(1.0, 1.0); glVertex3fv(sb);
		glTexCoord2d(0.0, 1.0); glVertex3fv(sf);
		glTexCoord2d(0.0, 0.0); glVertex3fv(sg);
		glTexCoord2d(1.0, 0.0); glVertex3fv(sc);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glTexCoord2d(1.0, 1.0); glVertex3fv(sf);
		glTexCoord2d(0.0, 1.0); glVertex3fv(se);
		glTexCoord2d(0.0, 0.0); glVertex3fv(sh);
		glTexCoord2d(1.0, 0.0); glVertex3fv(sg);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glTexCoord2d(1.0, 1.0); glVertex3fv(se);
		glTexCoord2d(0.0, 1.0); glVertex3fv(sa);
		glTexCoord2d(0.0, 0.0); glVertex3fv(sd);
		glTexCoord2d(1.0, 0.0); glVertex3fv(sh);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glTexCoord2d(1.0, 1.0); glVertex3fv(sd);
		glTexCoord2d(0.0, 1.0); glVertex3fv(sc);
		glTexCoord2d(0.0, 0.0); glVertex3fv(sg);
		glTexCoord2d(1.0, 0.0); glVertex3fv(sh);
		glEnd();

		glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glTexCoord2d(1.0, 1.0); glVertex3fv(sa);
		glTexCoord2d(0.0, 1.0); glVertex3fv(sb);
		glTexCoord2d(0.0, 0.0); glVertex3fv(sf);
		glTexCoord2d(1.0, 0.0); glVertex3fv(se);
		glEnd();
		glDisable(GL_TEXTURE_2D); // Wyłącz teksturowanie
	}
}

void lazik(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { -10.0f,5.0f,0.0f };
		GLfloat sb[3] = { 90.0f,5.0f,0.0f };
		GLfloat sc[3] = { 90.0f,15.0f,0.0f };
		GLfloat sd[3] = { -10.0f,15.0f,0.0f };
		GLfloat se[3] = { -10.0f,5.0f,-40.0f };
		GLfloat sf[3] = { 90.0f,5.0f,-40.0f };
		GLfloat sg[3] = { 90.0f,15.0f,-40.0f };
		GLfloat sh[3] = { -10.0f,15.0f,-40.0f };

		// Sciany skladowe
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sc);
		glVertex3fv(sd);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(sg);
		glVertex3fv(sc);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sf);
		glVertex3fv(se);
		glVertex3fv(sh);
		glVertex3fv(sg);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(se);
		glVertex3fv(sa);
		glVertex3fv(sd);
		glVertex3fv(sh);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sd);
		glVertex3fv(sc);
		glVertex3fv(sg);
		glVertex3fv(sh);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(se);
		glEnd();
	}

	//skrzynia
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat xa[3] = { -10.0f,15.0f,0.0f };
		GLfloat xb[3] = { 40.0f,15.0f,0.0f };
		GLfloat xc[3] = { 40.0f,45.0f,0.0f };
		GLfloat xd[3] = { -10.0f,45.0f,0.0f };
		GLfloat xe[3] = { -10.0f,15.0f,-40.0f };
		GLfloat xf[3] = { 40.0f,15.0f,-40.0f };
		GLfloat xg[3] = { 40.0f,45.0f,-40.0f };
		GLfloat xh[3] = { -10.0f,45.0f,-40.0f };

		// Sciany skladowe
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(xa);
		glVertex3fv(xb);
		glVertex3fv(xc);
		glVertex3fv(xd);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(xb);
		glVertex3fv(xf);
		glVertex3fv(xg);
		glVertex3fv(xc);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(xf);
		glVertex3fv(xe);
		glVertex3fv(xh);
		glVertex3fv(xg);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(xe);
		glVertex3fv(xa);
		glVertex3fv(xd);
		glVertex3fv(xh);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(xd);
		glVertex3fv(xc);
		glVertex3fv(xg);
		glVertex3fv(xh);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(xa);
		glVertex3fv(xb);
		glVertex3fv(xf);
		glVertex3fv(xe);
		glEnd();
	}

	int r = 10;
	int h = 10;

	// koło 1
	double x, y, alpha;
	glPushMatrix();
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.8, 0.0);
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();

	//oś1
	double xo, yo, alphao, PIo = 3.14;
	double ro = 2, ho = 62;

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -51.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.2, 0.7, 0.5);
	glVertex3d(0, 0, 0);
	for (alphao = 0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alphao = 0.0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, 0);
		glVertex3d(xo, yo, ho);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, ho);
	for (alphao = 0; alphao >= -2 * PIo; alphao -= PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, ho);
	}
	glEnd();
	glPopMatrix();

	//oś2
	glPushMatrix();
	glTranslatef(40.0f, 0.0f, -51.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.2, 0.7, 0.5);
	glVertex3d(0, 0, 0);
	for (alphao = 0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alphao = 0.0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, 0);
		glVertex3d(xo, yo, ho);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, ho);
	for (alphao = 0; alphao >= -2 * PIo; alphao -= PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, ho);
	}
	glEnd();
	glPopMatrix();

	//oś3
	glPushMatrix();
	glTranslatef(80.0f, 0.0f, -51.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.2, 0.7, 0.5);
	glVertex3d(0, 0, 0);
	for (alphao = 0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alphao = 0.0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, 0);
		glVertex3d(xo, yo, ho);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, ho);
	for (alphao = 0; alphao >= -2 * PIo; alphao -= PIo / 8.0)
	{
		xo = ro * sin(alphao);
		yo = ro * cos(alphao);
		glVertex3d(xo, yo, ho);
	}
	glEnd();
	glPopMatrix();

	double ha = -45, ra = 1;
	//antena
	glPushMatrix();
	glTranslatef(80.0f, 15.0f, -20.0f);
	glRotatef(90.0f, 90.0f, 0.0f, 0.0f); // Obrót o 90 stopni w osi Y
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.2, 0.7, 0.5);
	glVertex3d(0, 0, 0);
	for (alphao = 0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alphao = 0.0; alphao <= 2 * PIo; alphao += PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, 0);
		glVertex3d(xo, yo, ha);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, ha);
	for (alphao = 0; alphao >= -2 * PIo; alphao -= PIo / 8.0)
	{
		xo = ra * sin(alphao);
		yo = ra * cos(alphao);
		glVertex3d(xo, yo, ha);
	}
	glEnd();
	glPopMatrix();

	//kolo4
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -50.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.8, 0.0);
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
	glPopMatrix();

	//kolo5
	glPushMatrix();
	glTranslatef(80.0f, 0.0f, -50.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.8, 0.0);
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
	glPopMatrix();

	//kolo6
	glPushMatrix();
	glTranslatef(40.0f, 0.0f, -50.0f);
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.8, 0.0);
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
	glPopMatrix();

	//koło 2
	glTranslatef(80.0f, 0.0f, 0.0f); // Przesuń na nowe miejsce

	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.8, 0.0); // Inny kolor
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
	glPopMatrix();


	//koło 3
	glTranslatef(40.0f, 0.0f, 0.0f); // Przesuń na nowe miejsce
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.8, 0.0); // Inny kolor
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
	glPopMatrix();


}

void szescian(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	{
		// Parametry wierzcholkow

		GLfloat sa[3] = { 0.0f,0.0f,0.0f };
		GLfloat sb[3] = { 80.0f,0.0f,0.0f };
		GLfloat sc[3] = { 80.0f,25.0f,0.0f };
		GLfloat sd[3] = { 0.0f,25.0f,0.0f };
		GLfloat se[3] = { 0.0f,0.0f,-10.0f };
		GLfloat sf[3] = { 80.0f,0.0f,-10.0f };
		GLfloat sg[3] = { 80.0f,25.0f,-10.0f };
		GLfloat sh[3] = { 0.0f,25.0f,-10.0f };

		GLfloat sa2[3] = { 0.0f,0.0f,0.0f };
		GLfloat sb2[3] = { 10.0f * cos(x),0.0f,10.0f * sin(x) };
		GLfloat sc2[3] = { 10.0f * cos(x),10.0f,10.0f * sin(x) };
		GLfloat sd2[3] = { 0.0f,10.0f,0.0f };


		// Sciany skladowe
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sc);
		glVertex3fv(sd);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(sg);
		glVertex3fv(sc);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sf);
		glVertex3fv(se);
		glVertex3fv(sh);
		glVertex3fv(sg);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(se);
		glVertex3fv(sa);
		glVertex3fv(sd);
		glVertex3fv(sh);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sd);
		glVertex3fv(sc);
		glVertex3fv(sg);
		glVertex3fv(sh);
		glEnd();

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POLYGON);
		glVertex3fv(sa);
		glVertex3fv(sb);
		glVertex3fv(sf);
		glVertex3fv(se);
		glEnd();
	}
}

void walec(double r, double h)
{
	double x, y, alpha;
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.0, 0);
	glVertex3d(0, 0, 0);
	for (alpha = 0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0.0; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -2 * PI; alpha -= PI / 8.0)
	{
		x = r * sin(alpha);
		y = r * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
}
void ramie(double r1, double r2, double h, double d)
{
	double alpha, x, y;
	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.0, 0);
	glVertex3d(0, 0, 0);
	for (alpha = PI; alpha <= 2 * PI; alpha += PI / 8.0)
	{
		x = r1 * sin(alpha);
		y = r1 * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (alpha = 0; alpha >= -PI; alpha -= PI / 8.0)
	{
		x = r1 * sin(alpha);
		y = r1 * cos(alpha);
		glVertex3d(x, y, h);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3d(0, 0, h);
	for (alpha = 0; alpha >= -PI; alpha -= PI / 8.0)
	{
		x = r1 * sin(alpha);
		y = r1 * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3d(0.8, 0.0, 0);
	//glVertex3d(d,r2,0);
	//glVertex3d(d, r2, h);
	for (alpha = 0; alpha <= PI; alpha += PI / 8.0)
	{
		x = d + r2 * sin(alpha);
		y = d + r2 * cos(alpha);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	//glVertex3d(d, r2, 0);
	for (alpha = 0; alpha <= PI; alpha += PI / 8.0)
	{
		x = d + r2 * sin(alpha);
		y = d + r2 * cos(alpha);
		glVertex3d(x, y, h);
		glVertex3d(x, y, 0);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	//glVertex3d(d, r2, h);
	for (alpha = 0; alpha <= PI; alpha += PI / 8.0)
	{
		x = d + r2 * sin(alpha);
		y = d + r2 * cos(alpha);
		glVertex3d(x, y, h);
	}
	glEnd();
}
// Called to draw scene
void RenderScene(void)
{
	//float normal[3];	// Storeage for calculated surface normal

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();
	//glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	//glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	gluLookAt(Camx, Camy, Camz, Camx - 30, 0, Camz, 0, 1, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// PODŁOGA//

	GLfloat z1[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z2[3] = { 0.0f,-10.0f,800.0f };
	GLfloat z3[3] = { 800.0f,-10.0f,800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z1);
	glVertex3fv(z2);
	glVertex3fv(z3);
	glEnd();

	GLfloat z4[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z5[3] = { 800.0f,-10.0f,0.0f };
	GLfloat z6[3] = { 800.0f,-10.0f,800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z4);
	glVertex3fv(z5);
	glVertex3fv(z6);
	glEnd();

	GLfloat z7[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z8[3] = { -800.0f,-10.0f,0.0f };
	GLfloat z9[3] = { -800.0f,-10.0f,-800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z7);
	glVertex3fv(z8);
	glVertex3fv(z9);
	glEnd();

	GLfloat z10[3] = { 0.0f,-10.0f,-800.0f };
	GLfloat z11[3] = { 800.0f,-10.0f,-800.0f };
	GLfloat z12[3] = { 0.0f,-10.0f,0.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z10);
	glVertex3fv(z11);
	glVertex3fv(z12);
	glEnd();

	GLfloat z13[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z14[3] = { 800.0f,-10.0f,0.0f };
	GLfloat z15[3] = { 800.0f,-10.0f,-800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z13);
	glVertex3fv(z14);
	glVertex3fv(z15);
	glEnd();

	GLfloat z16[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z17[3] = { 0.0f,-10.0f,-800.0f };
	GLfloat z18[3] = { -800.0f,-10.0f,-800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z16);
	glVertex3fv(z17);
	glVertex3fv(z18);
	glEnd();

	GLfloat z19[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z20[3] = { 0.0f,-10.0f,800.0f };
	GLfloat z21[3] = { -800.0f,-10.0f,800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z19);
	glVertex3fv(z20);
	glVertex3fv(z21);
	glEnd();

	GLfloat z22[3] = { 0.0f,-10.0f,0.0f };
	GLfloat z23[3] = { -800.0f,-10.0f,0.0f };
	GLfloat z24[3] = { -800.0f,-10.0f,800.0f };

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_POLYGON);
	glVertex3fv(z22);
	glVertex3fv(z23);
	glVertex3fv(z24);
	glEnd();

	//Sposób na odróźnienie "przedniej" i "tylniej" ściany wielokąta:
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	galezie();
	pnie();
	kamien();
	diament1();
	diament2();
	diament3();


	glTranslatef(pos_x, 0.0, pos_z);
	glRotatef(zRot, 0.0f, 1.0f, 0.0f);
	glTranslatef(-50.f, 0, -25.5f);


	lazik();


	//glPopMatrix();
	//Uzyskanie siatki:
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//Wyrysowanie prostokata:
	//glRectd(-10.0,-10.0,20.0,20.0);

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Flush drawing commands
	glFlush();
}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
		1,                                                              // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		24,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}



// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE* pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange;
	// Range for each color entry (7,7,and 3)


// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;

	// Loop through all the palette entries
	for (i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}


	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE       hInst,
	HINSTANCE       hPrevInstance,
	LPSTR           lpCmdLine,
	int                     nCmdShow)
{
	MSG                     msg;            // Windows message structure
	WNDCLASS        wc;                     // Windows class structure
	HWND            hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;

	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName = lpszAppName;

	// Register the window class
	if (RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,

		// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

		// Window position and size
		50, 50,
		400, 400,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}




// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
{
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context

	switch (message)
	{
		// Window creation, setup for OpenGL
	case WM_CREATE:
		// Store the device context
		hDC = GetDC(hWnd);

		// Select the pixel format
		SetDCPixelFormat(hDC);

		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);

		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		glGenTextures(2, &texture[0]);                  // tworzy obiekt tekstury			

		// ładuje pierwszy obraz tekstury:
		bitmapData = LoadBitmapFile("kamien_1.bmp", &bitmapInfoHeader);

		glBindTexture(GL_TEXTURE_2D, texture[0]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ładuje drugi obraz tekstury:
		bitmapData = LoadBitmapFile("igly_2.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[1]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		bitmapData = LoadBitmapFile("pien_1.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[2]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ustalenie sposobu mieszania tekstury z tłem
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;

		// Window is being destroyed, cleanup
	case WM_DESTROY:
		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);

		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;


		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;


		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT:
	{
		// Call OpenGL drawing code
		RenderScene();

		SwapBuffers(hDC);

		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
	}
	break;

	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette)
		{
			int nRet;

			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);

			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		}
		break;


		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd))
		{
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries to system palette
			RealizePalette(hDC);

			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		}
		break;

		// Key press, check for arrow keys to do cube rotation.
	case WM_KEYDOWN:
	{
		if (wParam == VK_DOWN) {
			Camx -= cos((zRot / 360) * PI * 2) * 20.f;
			Camz += sin((zRot / 360) * PI * 2) * 20.f;
			pos_x -= cos((zRot / 360) * PI * 2) * 20.f;
			pos_z += sin((zRot / 360) * PI * 2) * 20.f;
		}


		else if (wParam == VK_UP) {
			Camx += cos((zRot / 360) * PI * 2) * 20.f;
			Camz -= sin((zRot / 360) * PI * 2) * 20.f;
			pos_x += cos((zRot / 360) * PI * 2) * 20.f;
			pos_z -= sin((zRot / 360) * PI * 2) * 20.f;

		}

		else if (wParam == VK_LEFT) {
			zRot += 5.f;
		}

		else if (wParam == VK_RIGHT) {
			zRot -= 5.f;
		}
		if (wParam = VK_NUMPAD8) {
			Camx -= 20.f;
		}
		if (wParam = VK_NUMPAD2) {
			Camx += 20.f;
		}
		if (wParam = VK_NUMPAD4) {
			Camz += 20.f;
		}
		if (wParam = VK_NUMPAD6) {
			Camz -= 20.f;
		}

		if (wParam == VK_NUMPAD1) {
			if (kat < 3.14f * 0.5f) {
				kat += 3.14f * 0.125f;
			}
		}

		xRot = (const int)xRot % 360;
		yRot = (const int)yRot % 360;
		zRot = (const int)zRot % 360;
		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;

	// A menu command
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			// Exit the program
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;

			// Display the about box
		case ID_HELP_ABOUT:
			DialogBox(hInstance,
				MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
				hWnd,
				AboutDlgProc);
			break;
		}
	}
	break;


	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));

	}

	return (0L);
}




// Dialog procedure.
BOOL APIENTRY AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam)
{

	switch (message)
	{
		// Initialize the dialog box
	case WM_INITDIALOG:
	{
		int i;
		GLenum glError;

		// glGetString demo
		SetDlgItemText(hDlg, IDC_OPENGL_VENDOR, glGetString(GL_VENDOR));
		SetDlgItemText(hDlg, IDC_OPENGL_RENDERER, glGetString(GL_RENDERER));
		SetDlgItemText(hDlg, IDC_OPENGL_VERSION, glGetString(GL_VERSION));
		SetDlgItemText(hDlg, IDC_OPENGL_EXTENSIONS, glGetString(GL_EXTENSIONS));

		// gluGetString demo
		SetDlgItemText(hDlg, IDC_GLU_VERSION, gluGetString(GLU_VERSION));
		SetDlgItemText(hDlg, IDC_GLU_EXTENSIONS, gluGetString(GLU_EXTENSIONS));


		// Display any recent error messages
		i = 0;
		do {
			glError = glGetError();
			SetDlgItemText(hDlg, IDC_ERROR1 + i, gluErrorString(glError));
			i++;
		} while (i < 6 && glError != GL_NO_ERROR);


		return (TRUE);
	}
	break;

	// Process command messages
	case WM_COMMAND:
	{
		// Validate and Make the changes
		if (LOWORD(wParam) == IDOK)
			EndDialog(hDlg, TRUE);
	}
	break;

	// Closed from sysbox
	case WM_CLOSE:
		EndDialog(hDlg, TRUE);
		break;
	}

	return FALSE;
}
