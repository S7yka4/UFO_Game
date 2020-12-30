#include "Render.h"




#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"
#include "Planet.h"
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include <sstream>
#include "MyShaders.h"
#include "UFO.h"
#include "ObjLoader.h"
#include "GUItextRectangle.h"
#include "Goal.h"
#include "Texture.h"

GuiTextRectangle rec;

bool textureMode = true;
bool lightMode = true;
int LevelNumb = 0;
Texture monkeyTex, TGalaxy/*, TEarth, TUpiter, TUranus, TSun, TMoon, TNeptun, TSaturn, TMercury, TVenera, TYupiter,TMars,TUran*/;
Planet Earth, Moon, Upiter, Uranus, Sun, Neptun, Saturn, Mercury, Venera, Yupiter, Mars, Uran;
Goal Goals[5];

//небольшой дефайн для упрощения кода
#define POP glPopMatrix()
#define PUSH glPushMatrix()


ObjFile *model;
double pointCounter=0;
Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[12];  //массивчик для десяти шейдеров
Shader frac;
Shader cassini;
UFO UFO1;



//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 20;//15
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	virtual void SetUpCamera()
	{

		
		//lookPoint.setCoords(0,0,0);
		/*pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));*/
		lookPoint.setCoords(UFO1.Coords[0] + 1, UFO1.Coords[1] + 1, UFO1.Coords[2] + 1);//
		pos.setCoords(UFO1.Coords[0] + 7,
			UFO1.Coords[1] ,
			UFO1.Coords[2]+2 );

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//класс недоделан!
class WASDcamera :public CustomCamera
{
public:
		
	float camSpeed;

	WASDcamera()
	{
		camSpeed = 0.4;
		pos.setCoords(5, 5, 5);
		lookPoint.setCoords(0, 0, 0);
		normal.setCoords(0, 0, 1);
	}

	virtual void SetUpCamera()
	{

		if (OpenGL::isKeyPressed('W'))
		{
			Vector3 forward = (lookPoint - pos).normolize()*camSpeed;
			pos = pos + forward;
			lookPoint = lookPoint + forward;
			
		}
		if (OpenGL::isKeyPressed('S'))
		{
			Vector3 forward = (lookPoint - pos).normolize()*(-camSpeed);
			pos = pos + forward;
			lookPoint = lookPoint + forward;
			
		}

		LookAt();
	}

} WASDcam;

Texture TSun;
//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(UFO1.Coords[0], UFO1.Coords[1], UFO1.Coords[0]+15);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		
		glActiveTexture(GL_TEXTURE0);
		//glScaled(50,50,50);
		//glBindTexture();
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		//s.pos = pos;
		s.pos = Vector3(UFO1.Coords[0], UFO1.Coords[1], UFO1.Coords[2] + 15);
		s.scale = s.scale*0.08;
		s.Show();

		/*if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
				glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}*/
		/*()
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света



//старые координаты мыши
int mouseX = 0, mouseY = 0;




float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;
bool rotateflag=false;

void  NewLVL()
{
	double tmp[3];
	double r;
	double angle;
	for (int i = 0; i < 5; i++)
	{
		
		r = (rand() % 20 + 2.5);
		angle = (rand() % 360) * M_PI / 180.0;
		tmp[0] = r * cos(angle);
		tmp[1] = r * sin(angle);
		tmp[2] = 0;
		Goals[i].SetGoal(0.5, tmp);
	}
}
//обработчик движения мыши
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}


	
	//двигаем свет по плоскости, в точку где мышь
/*	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}*/

	
}

//обработчик вращения колеса  мыши
void mouseWheelEvent(OpenGL *ogl, int delta)
{


	float _tmpZ = delta*0.003;
	if (ogl->isKeyPressed('Z'))
		_tmpZ *= 10;
	zoom += 0.2*zoom*_tmpZ;


	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;
}

//обработчик нажатия кнопок клавиатуры
void keyDownEvent(OpenGL *ogl, int key)
{
	if ((OpenGL::isKeyPressed('M')))
	{
		rotateflag = true;
		srand(LevelNumb);
		UFO1.MoveToStart();
		NewLVL();


		LevelNumb++;
	

	}
//	else
		//if ((OpenGL::isKeyPressed('M')) && (rotateflag == true))
			//rotateflag = false;
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}	   

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'S')
	{
		frac.LoadShaderFromFile();
		frac.Compile();

		s[0].LoadShaderFromFile();
		s[0].Compile();

		cassini.LoadShaderFromFile();
		cassini.Compile();
	}

	if (key == 'Q')
		Time = 0;
}

void keyUpEvent(OpenGL *ogl, int key)
{

}


void DrawQuad()
{
	double A[] = { 0,0 };
	double B[] = { 1,0 };
	double C[] = { 1,1 };
	double D[] = { 0,1 };
	glBegin(GL_QUADS);
	glColor3d(.5, 0, 0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);
	glEnd();
}


ObjFile objModel,monkey,Galaxy/*,Earth,Moon,Upiter,Uranus,Sun,Neptun,Saturn,Mercury,Venera,Yupiter,Mars,Uran*/;


//выполняется перед первым рендером

OpenGL* ogl1;
void initRender(OpenGL *ogl)
{
	ogl1 = ogl;
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	
	


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   загрузка текстуры из файла
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	frac.FshaderFileName = "shaders\\frac.frag"; //имя файла фрагментного шейдера
	frac.LoadShaderFromFile(); //загружаем шейдеры из файла
	frac.Compile(); //компилируем

	cassini.VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	cassini.FshaderFileName = "shaders\\cassini.frag"; //имя файла фрагментного шейдера
	cassini.LoadShaderFromFile(); //загружаем шейдеры из файла
	cassini.Compile(); //компилируем
	

	s[0].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[0].FshaderFileName = "shaders\\light.frag"; //имя файла фрагментного шейдера
	s[0].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[0].Compile(); //компилируем

	s[1].VshaderFileName = "shaders\\v.vert"; //имя файла вершинного шейдер
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //имя файла фрагментного шейдера
	s[1].LoadShaderFromFile(); //загружаем шейдеры из файла
	s[1].Compile(); //компилируем

	

	 //так как гит игнорит модели *.obj файлы, так как они совпадают по расширению с объектными файлами, 
	 // создающимися во время компиляции, я переименовал модели в *.obj_m
	//loadModel("models\\lpgun6.obj_m", &objModel);


	/*glActiveTexture(GL_TEXTURE0);
	loadModel("models\\monkey.obj_m", &monkey);
	monkeyTex.loadTextureFromFile("textures//tex.bmp");
	monkeyTex.bindTexture();*/

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Sun.obj_m", &Sun.Model);

	Sun.Texture.loadTextureFromFile("textures//Sun.bmp");
	Sun.Texture.bindTexture();
	Sun.SetUpCenter(0, 0);
	TSun.loadTextureFromFile("textures//Sun.bmp");
	TSun.bindTexture();

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Earth — копия.obj_m", &Earth.Model);
	Earth.Texture.loadTextureFromFile("textures//Earth.bmp");
	Earth.Texture.bindTexture();
	Earth.Speed = 1;
	Earth.Selfspeed = 1.0/15.0;
	Earth.SetUpCenter(8.5, 0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\LunaMoon.obj_m", &Moon.Model);
	Moon.Texture.loadTextureFromFile("textures//Moon.bmp");
	Moon.Texture.bindTexture();
	Moon.Speed =   1.0 / 13.37;

	Moon.SetUpCenter(9.4, 0,8.5,0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Mars.obj_m", &Mars.Model);
	Mars.Texture.loadTextureFromFile("textures//Mars.bmp");
	Mars.Texture.bindTexture();
	Mars.Speed = 1.0 / 0.53129;
	Mars.Selfspeed = 1 / 15;
	Mars.SetUpCenter(11, 0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Mercury.obj_m", &Mercury.Model);
	Mercury.Texture.loadTextureFromFile("textures//Merc.bmp");
	Mercury.Texture.bindTexture();
	Mercury.Speed = 1.0 / 4.1477;
	Mercury.Selfspeed = 1.0 / (15 * 58.5);
	Mercury.SetUpCenter(5, 0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Neptun.obj_m", &Neptun.Model);
	Neptun.Texture.loadTextureFromFile("textures//Neptun.bmp");
	Neptun.Texture.bindTexture();
	Neptun.Speed = 165;
	Neptun.Selfspeed = 1.0 / (15 / 1.5);
	Neptun.SetUpCenter(25, 0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Saturn.obj_m", &Saturn.Model);
	Saturn.Texture.loadTextureFromFile("textures//Saturn.bmp");
	Saturn.Texture.bindTexture();
	Saturn.Speed = 29.5;
	Saturn.Selfspeed = 1.0 / (15.0 / 2.285);
	Saturn.SetUpCenter(18, 0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Uran2.obj_m", &Uran.Model);
	Uran.Texture.loadTextureFromFile("textures//UrAnus.bmp");
	Uran.Texture.bindTexture();
	Uran.Speed = 84;
	Uran.Selfspeed = 1.0 / (15.0 / 1.41176471);
	Uran.SetUpCenter(20,0);

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Venera.obj_m", &Venera.Model);
	Venera.Texture.loadTextureFromFile("textures//Venera.bmp");
	Venera.Texture.bindTexture();
	Venera.Speed = 10.0 / 1.62222;
	Venera.Selfspeed = 1.0/ (30.0 * 15 * 8);
	Venera.SetUpCenter(7,0);
	
	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\Yputer.obj_m", &Yupiter.Model);
	Yupiter.Texture.loadTextureFromFile("textures//Upiter.bmp");
	Yupiter.Texture.bindTexture();
	Yupiter.Speed = 12;
	Yupiter.Selfspeed = 1.0 / 1.5;
	Yupiter.SetUpCenter(15, 0);


	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\GalaxyLMAO.obj_m", &Galaxy);
	TGalaxy.loadTextureFromFile("textures//Sky.bmp");
	TGalaxy.bindTexture();

	for (int i= 0; i < 5; i++)
	{
		glActiveTexture(GL_TEXTURE0);
		loadModel("models\\Goal.obj_m", &Goals[i].Model);
		Goals[i].texture.loadTextureFromFile("textures//Goal.bmp");
		Goals[i].texture.bindTexture();
	}

	UFO1.Init();
	


	tick_n = GetTickCount();
	tick_o = tick_n;
	rec.setSize(300, 100);
	rec.setPosition(10, ogl->getHeight() - 100-10);
	char* Interface = " W-вперед\n S-назад\n A-лево\n D-право\n M-начать игру\n	Кол-во очков - ";
	rec.setText(Interface);
	
	
}




void Render(OpenGL *ogl)
{   
	
	light.DrawLightGhismo();
	tick_o = tick_n;
	tick_n = GetTickCount();
	Time += (tick_n - tick_o) / 1000.0;

	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//Прогать тут  
	if (OpenGL::isKeyPressed('W'))
	{
		UFO1.MoveForward();
	}
	else
		if (OpenGL::isKeyPressed('S'))
		{
			UFO1.MoveBackward();
		}
		else
			if (OpenGL::isKeyPressed('A'))
			{
				UFO1.MoveLeft();
			}
			else
				if (OpenGL::isKeyPressed('D'))
				{
					UFO1.MoveRight();
				}
				else
					if (OpenGL::isKeyPressed('F'))
					{
						UFO1.MoveDown();
					}
					else
						if (OpenGL::isKeyPressed('R'))
						{
							UFO1.MoveUp();
						}
						else
							UFO1.DrawInPoint();
	
	if ((sqrt(pow(UFO1.Coords[0] - Earth.Coords[0], 2) + pow(UFO1.Coords[1] - Earth.Coords[1], 2) + pow(UFO1.Coords[2] - Earth.Coords[2], 2)) < (Earth.radius + UFO1.radius)))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Mars.Coords[0], 2) + pow(UFO1.Coords[1] - Mars.Coords[1], 2) + pow(UFO1.Coords[2] - Mars.Coords[2], 2)) < (Mars.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Mercury.Coords[0], 2) + pow(UFO1.Coords[1] - Mercury.Coords[1], 2) + pow(UFO1.Coords[2] - Mercury.Coords[2], 2)) < (Mercury.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Moon.Coords[0], 2) + pow(UFO1.Coords[1] - Moon.Coords[1], 2) + pow(UFO1.Coords[2] - Moon.Coords[2], 2)) < (Moon.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Neptun.Coords[0], 2) + pow(UFO1.Coords[1] - Neptun.Coords[1], 2) + pow(UFO1.Coords[2] - Neptun.Coords[2], 2)) < (Neptun.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Saturn.Coords[0], 2) + pow(UFO1.Coords[1] - Saturn.Coords[1], 2) + pow(UFO1.Coords[2] - Saturn.Coords[2], 2)) < (Saturn.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Sun.Coords[0], 2) + pow(UFO1.Coords[1] - Sun.Coords[1], 2) + pow(UFO1.Coords[2] - Sun.Coords[2], 2)) < (Sun.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Yupiter.Coords[0], 2) + pow(UFO1.Coords[1] - Yupiter.Coords[1], 2) + pow(UFO1.Coords[2] - Yupiter.Coords[2], 2)) < (Yupiter.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Uran.Coords[0], 2) + pow(UFO1.Coords[1] - Uran.Coords[1], 2) + pow(UFO1.Coords[2] - Uran.Coords[2], 2)) < (Uran.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (sqrt(pow(UFO1.Coords[0] - Venera.Coords[0], 2) + pow(UFO1.Coords[1] - Venera.Coords[1], 2) + pow(UFO1.Coords[2] - Venera.Coords[2], 2)) < (Venera.radius + UFO1.radius))
	{
		UFO1.MoveToStart();
		pointCounter -= 1000;
		for (int i = 0; i < 5; i++)
			Goals[i].Goaled = false;
	}
	if (rotateflag != true)
	{
		glPushMatrix();
		glTranslated(UFO1.Coords[0], UFO1.Coords[1], UFO1.Coords[2]);
		glScaled(0.1, 0.1, 0.1);
		TGalaxy.bindTexture();
		Galaxy.DrawObj();
		glPopMatrix();

		
		UFO1.DrawInPoint();

		s[1].UseShader();
		int l = glGetUniformLocationARB(s[1].program, "tex");
		glUniform1iARB(l, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Earth.Draw();

		s[2].UseShader();
		int l2 = glGetUniformLocationARB(s[2].program, "tex");
		glUniform1iARB(l2, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Moon.Draw();

		s[3].UseShader();
		int l3 = glGetUniformLocationARB(s[3].program, "tex");
		glUniform1iARB(l3, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Mars.Draw();

		s[4].UseShader();
		int l4 = glGetUniformLocationARB(s[4].program, "tex");
		glUniform1iARB(l4, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Mercury.Draw();

		s[5].UseShader();
		int l5 = glGetUniformLocationARB(s[5].program, "tex");
		glUniform1iARB(l5, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Neptun.Draw();

		s[6].UseShader();
		int l6 = glGetUniformLocationARB(s[6].program, "tex");
		glUniform1iARB(l6, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Saturn.Draw();


		s[8].UseShader();
		int l8 = glGetUniformLocationARB(s[8].program, "tex");
		glUniform1iARB(l8, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Sun.Draw();

		s[9].UseShader();
		int l9 = glGetUniformLocationARB(s[9].program, "tex");
		glUniform1iARB(l9, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Uran.Draw();

		s[10].UseShader();
		int l10 = glGetUniformLocationARB(s[10].program, "tex");
		glUniform1iARB(l10, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Venera.Draw();

		s[11].UseShader();
		int l11 = glGetUniformLocationARB(s[11].program, "tex");
		glUniform1iARB(l11, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0

		Yupiter.Draw();
	}
	else
	{
		glPushMatrix();
		glTranslated(UFO1.Coords[0], UFO1.Coords[1], UFO1.Coords[2]);
		glScaled(0.1, 0.1, 0.1);
		TGalaxy.bindTexture();
		Galaxy.DrawObj();
		glPopMatrix();


		Sun.Draw();
		Moon.Rotate(Earth.LastCoords[0], Earth.LastCoords[1]);
		Earth.Rotate();
		
		Mars.Rotate();
		Mercury.Rotate();
		Neptun.Rotate();
		Saturn.Rotate();
		Uran.Rotate();
		Venera.Rotate();
		Yupiter.Rotate();
		for (int i = 0; i < 5; i++)
		{
			if( ((UFO1.radius * UFO1.radius * 0.25) > (pow((Goals[i].Center[0] - UFO1.Coords[0]), 2) + pow((Goals[i].Center[1] - UFO1.Coords[1]), 2) + pow((Goals[i].Center[2] - UFO1.Coords[2]), 2)))&&(Goals[i].Goaled==false))
			{
				Goals[i].Goaled = true;
				pointCounter += 500;
			}
				Goals[i].DrawGoal();
		}
		
	}


	//





	s[0].UseShader();

	//передача параметров в шейдер.  Шаг один - ищем адрес uniform переменной по ее имени. 
	int location = glGetUniformLocationARB(s[0].program, "light_pos");
	//Шаг 2 - передаем ей значение
	glUniform3fARB(location, light.pos.X(), light.pos.Y(),light.pos.Z());

	location = glGetUniformLocationARB(s[0].program, "Ia");
	glUniform3fARB(location, 0.2, 0.2, 0.2);

	location = glGetUniformLocationARB(s[0].program, "Id");
	glUniform3fARB(location, 1.0, 1.0, 1.0);

	location = glGetUniformLocationARB(s[0].program, "Is");
	glUniform3fARB(location, .7, .7, .7);


	location = glGetUniformLocationARB(s[0].program, "ma");
	glUniform3fARB(location, 0.2, 0.2, 0.1);

	location = glGetUniformLocationARB(s[0].program, "md");
	glUniform3fARB(location, 0.4, 0.65, 0.5);

	location = glGetUniformLocationARB(s[0].program, "ms");
	glUniform4fARB(location, 0.9, 0.8, 0.3, 25.6);

	location = glGetUniformLocationARB(s[0].program, "camera");
	glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());

	//первый пистолет
/*	objModel.DrawObj();


	Shader::DontUseShaders();
	
	//второй, без шейдеров
	glPushMatrix();
		glTranslated(-5,15,0);
		//glScaled(-1.0,1.0,1.0);
		objModel.DrawObj();
	glPopMatrix();



	//обезьяна

	s[1].UseShader();
	int l = glGetUniformLocationARB(s[1].program,"tex"); 
	glUniform1iARB(l, 0);     //так как когда мы загружали текстуру грузили на GL_TEXTURE0
	glPushMatrix();
	glRotated(-90, 0, 0, 1);
	monkeyTex.bindTexture();
	monkey.DrawObj();
	glPopMatrix();*/



	
	

	//////Рисование фрактала

	
	/*
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		frac.UseShader();

		int location = glGetUniformLocationARB(frac.program, "size");
		glUniform2fARB(location, (GLfloat)ogl->getWidth(), (GLfloat)ogl->getHeight());

		location = glGetUniformLocationARB(frac.program, "uOffset");
		glUniform2fARB(location, offsetX, offsetY);

		location = glGetUniformLocationARB(frac.program, "uZoom");
		glUniform1fARB(location, zoom);

		location = glGetUniformLocationARB(frac.program, "Time");
		glUniform1fARB(location, Time);

		DrawQuad();

	}
	*/
	
	
	//////Овал Кассини
	
	/*
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		cassini.UseShader();

		int location = glGetUniformLocationARB(cassini.program, "size");
		glUniform2fARB(location, (GLfloat)ogl->getWidth(), (GLfloat)ogl->getHeight());


		location = glGetUniformLocationARB(cassini.program, "Time");
		glUniform1fARB(location, Time);

		DrawQuad();
	}

	*/

	
	
	

	
	Shader::DontUseShaders();

	
	
}   //конец тела функции


bool gui_init = false;

//рисует интерфейс, вызывется после обычного рендера
void RenderGUI(OpenGL *ogl)
{
	
	/*Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	

	glActiveTexture(GL_TEXTURE0);
	char buffer[20];
	//char* tmp = itoa(pointCounter, buffer, 10);
	char* Interface = " W-вперед\n S-назад\n A-лево\n D-право\n M-начать игру\n	Кол-во очков - ";
	//rec.setText(Interface);
	rec.Draw();


		
	Shader::DontUseShaders(); */

	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
									//(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 250);
	rec.setPosition(10, ogl->getHeight() - 250 - 10);


	std::stringstream ss;
	ss << "W - вперед" << std::endl;
	ss << "S - назад" << std::endl;
	ss << "A - влево"  << std::endl;
	ss << "D - вправо" << std::endl;
	ss << "R - вверх" << std::endl;
	ss << "F - вниз"  << std::endl;
	ss << "M - следующий уровень" << std::endl;
	ss << "Номер уровня - " << LevelNumb << std::endl;
	ss << "Количество очков - "<< pointCounter<< std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	Shader::DontUseShaders();




	
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

