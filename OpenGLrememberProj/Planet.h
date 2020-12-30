#include "Render.h"




#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"
class Planet
{
public:
	ObjFile Model;
	Texture Texture;
	double Selfspeed;
	double Speed;
	double SelfSpeedCounter;
	double SpeedCounter;
	double Center[3];
	double Coords[3];
	double LastCoords[3];
	double r;
	double radius=1;

public: void Draw()
{
	glPushMatrix();
	Texture.bindTexture();
	glTranslated(Center[0], Center[1], 0);
	glRotated(90, 1, 0, 0);
	Texture.bindTexture();
	Model.DrawObj();
	glPopMatrix();
}





public: void Rotate(double x0 = 0, double y0 = 0)
{

	glPushMatrix();
	
		Coords[0] =r * cos(SpeedCounter * M_PI / 180.0) + x0;
		Coords[1] = r * sin(SpeedCounter * M_PI / 180.0) + y0;
		glTranslated(Coords[0], Coords[1], 0);
		SelfSpeedCounter += Selfspeed;
		glRotated(SelfSpeedCounter, 0, 0, 1);
		SpeedCounter += Speed;
		
		
		glRotated(90,1,0,0);
		Texture.bindTexture();
		Model.DrawObj();
		glPopMatrix();
		LastCoords[0] = Coords[0];
		LastCoords[1] = Coords[1];
}

public: void SetUpCenter(double x, double y, double x0 = 0, double y0 = 0)
	{
		Center[0] = x;
		Center[1] = y;
		Center[2] = 0;
		LastCoords[0] = x;
		LastCoords[1] = y;
		LastCoords[2] = 0;
		Selfspeed = Selfspeed*10;
		Speed =( 1 / Speed)/10;
		r = sqrt(pow(LastCoords[0] - x0, 2) + pow(LastCoords[1] - y0, 2));
		SelfSpeedCounter = 0;
		SpeedCounter = 0;
	}
};