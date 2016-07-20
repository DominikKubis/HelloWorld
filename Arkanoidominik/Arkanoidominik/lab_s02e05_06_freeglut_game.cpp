#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#define NDEBUG
#include <GL/freeglut.h>

#include "Rectangle.h"
#include "Circle.h"

double znakopglX = 1, openglX, openglY;

const int GAME_LOGIC_REFRESH_TIME = 10;

MF::Circle ball(0.6, 1.0, 0.0, 0.0);
MF::Rectangle paddle(10, 1, 0.0, 1.0, 0.0);

std::vector<MF::Rectangle> walls;
std::vector<MF::Rectangle> blocks;

/* GLUT callback Handlers */
void resize(int width, int height)
{
	const float ar = (float)width / (float)height;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
	gluLookAt(0, 0, 45, 0, 0, 0, 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void zmianaRuchu()
{
		znakopglX = -1;
		openglX = -openglX;
}

void gameLogic(int value)
{
	int var = 0;;
	
	if (value == 0)
	{
		ball.Ustaw(glutGet(GLUT_ELAPSED_TIME), 3e-2, 60, 9.81*1E-6, -90);
	}
	else
	{
		// Check collisions - TODO!
		for (auto &w : walls)
		{
			//ball.Kolizja(w);
		}
		for (int i = 0; i < walls.size(); i++)
		{
			if (ball.Kolizja(walls[i]) && i == 1)
			{
				std::cout << "Game Over!";
			}
			else if (ball.Kolizja(paddle))
			{
			}
			for (int i = 0; i < blocks.size(); i++)
			{
				if (ball.Kolizja(blocks[i]))
				{
					for (auto itr = blocks.begin(); itr != blocks.end();)
					{
						if (ball.Kolizja(*itr))
						{
							srand(time(NULL));
							var = rand() % 2;
							itr = blocks.erase(itr);
							if (var == 1)
							{
								zmianaRuchu();
								paddle.ChangeColour(1, 0, 0);
							}
		
						}
						else
							itr++;
					}
					if (blocks.size() == 0)
					{
						std::cout << "WYGRALES!!!";
						exit(0);
					}
				}

			}
		}
		// calculate new ball position
		ball.Update(glutGet(GLUT_ELAPSED_TIME));
	}

	glutTimerFunc(GAME_LOGIC_REFRESH_TIME, gameLogic, 1);
}

void idle()
{
	glutPostRedisplay();
}

void display()
{
	// clear the scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	ball.Draw();
	paddle.Draw();

	for (auto itr = walls.begin(); itr != walls.end(); itr++)
	{
		itr->Draw();
	}

	for (auto itr = blocks.begin(); itr != blocks.end(); itr++)
	{
		itr->Draw();
	}

	glPopMatrix();

	glutSwapBuffers();
}

void passiveMouseMotion(int mouse_x, int mouse_y)
{
	double windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	double windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	openglX = znakopglX*((double)mouse_x - 600) / 1200.0 * 40;
	paddle.SetPosition(openglX, -15);
	// MOUSE - TODO!
}

void keyboard(unsigned char key_pressed, int mouse_x, int mouse_y)
{

	switch (key_pressed)
	{
		case 'a':
			{
				paddle.Move(-2.0, 0.0);
			}
			break;
		case 'd':
			{
				paddle.Move(2.0, 0.0);
			}
			break;
	}
}

/* helper functions for settings options and parameters */
void InitGLUTScene(char* window_name)
{
	glutInitWindowSize(1200, 1000);
	glutInitWindowPosition(40, 40);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

	glutCreateWindow(window_name);

	glClearColor(1, 1, 1, 1);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
}

void SetCallbackFunctions()
{
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(passiveMouseMotion);
	glutTimerFunc(GAME_LOGIC_REFRESH_TIME, gameLogic, 0);
}

void SetObjectsPositions()
{
	//pi³ka
	ball.SetPosition(0, -12);

	//paddle
	paddle.SetPosition(0, -15);

	//blocks
	for (int l = 2, n = 0; l < 6; l++, n++)
	{
		for (int i = -16; i < 20; i += 4)
		{
			double red = (double)(rand() % 1000) / 1000.0;
			double green = (double)(rand() % 1000) / 1000.0;
			double blue = (double)(rand() % 1000) / 1000.0;
			MF::Rectangle newBlock(4, 1, red, green, blue);
			newBlock.SetPosition(i, l);
			blocks.push_back(newBlock);
		}
	}

	// walls
	{
		MF::Rectangle wall(68, 2, 0.5, 0.5, 0.5);
		wall.SetPosition(0, 22);
		walls.push_back(wall);
	}
	{
		MF::Rectangle wall(68, 2, 0.5, 0.5, 0.5);
		wall.SetPosition(0, -22);
		walls.push_back(wall);
	}
	{
		MF::Rectangle wall(4, 60, 0.5, 0.5, 0.5);
		wall.SetPosition(21, 0);
		walls.push_back(wall);
	}
	{
		MF::Rectangle wall(4, 60, 0.5, 0.5, 0.5);
		wall.SetPosition(-21, 0);
		walls.push_back(wall);
	}
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	glutInit(&argc, argv);
	InitGLUTScene("Arkanoid");

	SetCallbackFunctions();
	SetObjectsPositions();

	glutMainLoop();
	
	return 0;
}