
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
class Goal {

public:
	ObjFile Model;
	Texture texture;
	double Center[3];
	double r;
	bool Goaled;

public:
	void SetGoal(double _r, double _Center[3])
	{
		for (int i = 0; i < 3; i++)
			Center[i] = _Center[i];
		r = _r;
		Goaled = false;
	}


	void DrawTriangle(double v1[], double v2[], double v3[], int flag = 0)
	{
		glBegin(GL_TRIANGLES);

		glVertex3dv(v1);

		glVertex3dv(v2);
		glVertex3dv(v3);
		glEnd();
	}

public:
	void DrawGoal()
	{
		if (!Goaled)
		{
			glPushMatrix();
			texture.bindTexture();
			glTranslated(Center[0], Center[1], 0);
			glRotated(atan(Center[1] / Center[0]) * 180.0 / M_PI, 0, 0, 1);
			texture.bindTexture();
			Model.DrawObj();
			
			
		
			
			glPopMatrix();
			/*glBegin(GL_POINTS);
			double tmp[3] = { 0,0,0 };
			tmp[0] = Center[0];
			tmp[1] = Center[1];
			glPointSize(40);
			glVertex3dv(tmp);
			glEnd();*/
			glPushMatrix();
			
			/*int n = 1000;
			double tmp1[3], tmp2[3];
			tmp1[1] = 0 + Center[1];
			tmp2[1] = 0 + Center[1];
			for (int i = 1; i <= n; i++)
			{
				tmp1[0] = 0.5 * cos((2.0 * M_PI / n) * i) + Center[0];
				tmp1[2] = 0.5 * sin((2.0 * M_PI / n) * i) + Center[2];
				tmp2[0] = 0.5 * cos((2.0 * M_PI / n) * (i + 1.0)) + Center[0];
				tmp2[2] = 0.5 * sin((2.0 * M_PI / n) * (i + 1.0)) + Center[2];
				glRotated(Center[1] / Center[0] * 180 / M_PI, 0, 0, 1);
				DrawTriangle(tmp1, tmp2, Center);
			}*/
			glBegin(GL_POINTS);
			
			glPointSize(20);
			glVertex3dv(Center);
			glEnd();
			glPopMatrix();
			
		}
	}
};
