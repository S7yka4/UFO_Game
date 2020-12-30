
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


	

class UFO
{
public:
    double Coords[3];
	ObjFile BlinModel,CabModel;
	Texture BlinTexture,CabTexture;
	double Speed;
	double AngleCounter;
	double k;
	double radius = 1;

public: void Init()
	{
		glActiveTexture(GL_TEXTURE0);
		loadModel("models\\blin.obj_m", &BlinModel);
		BlinTexture.loadTextureFromFile("textures//blin.bmp");
		BlinTexture.bindTexture();
		Coords[0] = 30;
		Coords[1] = 0;
		Coords[2] = 0;
		AngleCounter = 0;
		Speed = 0.1;
		k = 0.4;
		glActiveTexture(GL_TEXTURE0);
		loadModel("models\\Sharik.obj_m", &CabModel);
		CabTexture.loadTextureFromFile("textures//Cab.bmp");
		CabTexture.bindTexture();
	}

public: void DrawInPoint( char axe = 0, double Angle = 0)
{

	glPushMatrix();
	glTranslated(Coords[0], Coords[1], Coords[2]);
	glScaled(0.1, 0.1, 0.1);

	//glRotated(Angle);
	glRotated(AngleCounter, 0, 0, 1);
	AngleCounter += k;
	glRotated(90, 1, 0, 0);
	BlinTexture.bindTexture();
	BlinModel.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(Coords[0], Coords[1], Coords[2]);
	glScaled(0.1, 0.1, 0.1);
	//glRotated(Angle);
	glRotated(90, 1, 0, 0);
	CabTexture.bindTexture();
	CabModel.DrawObj();
	glPopMatrix();
}

public: bool CollisionCheck(Planet Earth )
{
	/*//double Range=sqrt(pow(Coords[0]-Earth.Coords[0],2)+ pow(Coords[1] - Earth.Coords[1], 2)+ pow(Coords[0] - Earth.Coords[0], 2));
	if (sqrt(pow(Coords[0] - Earth.Coords[0], 2) + pow(Coords[1] - Earth.Coords[1], 2) + pow(Coords[0] - Earth.Coords[0], 2)) < (Earth.radius + radius))
		return true;
	else
		return false;*/
	return false;
}

public: void MoveToStart()
{
	
		Coords[0] = 30;
		Coords[1] = 0;
		Coords[2] = 0;
}

public: void MoveForward()
{
	Coords[0] = Coords[0] - Speed;
	DrawInPoint();
}

public: void MoveBackward()
{
	Coords[0] = Coords[0] + Speed;
	DrawInPoint();
}

public: void MoveLeft()
{
		Coords[1] = Coords[1]- Speed;
	DrawInPoint();
}

public: void MoveRight()
{
	
		Coords[1] = Coords[1] + Speed;

	DrawInPoint();
}

public: void MoveUp()
{
	Coords[2] = Coords[2] + Speed;
	DrawInPoint();
}

public: void MoveDown()
{
	Coords[2] = Coords[2] - Speed;
	DrawInPoint();
}
};
