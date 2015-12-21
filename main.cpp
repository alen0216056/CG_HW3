#include "scene.h"
#include "light.h"
#include "view.h"

#include "FreeImage.h"
#include "glew.h"
#include "glut.h"

void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouse_motion(int x, int y);

int width = 1024, height = 768;
scenes* s;
lights* l;
view* v;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ACCUM | GLUT_DEPTH | GLUT_STENCIL);
	glutCreateWindow("HW2");

	l = new lights("Scene3.light");
	v = new view("Scene3.view");
	s = new scenes("Scene3.scene");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_motion);
	glutMainLoop();
	return 0;
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_ACCUM_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearDepth(1.0f);
	
	//pass1
	v->viewing();
	l->environment_lighting();
	s->scene_setting();
	//pass2
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glStencilFunc(GL_ALWAYS, 0, 255);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	s->shadow_setting(l->light_list[0].position);
	glDisable(GL_CULL_FACE);
	//pass3
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glStencilFunc(GL_ALWAYS, 0, 255);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	s->shadow_setting(l->light_list[0].position);
	glDisable(GL_CULL_FACE);
	//pass4
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glStencilFunc(GL_EQUAL, 0, 255);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	l->lighting();
	s->scene_setting();

	glAccum(GL_ACCUM, 1);
	glDisable(GL_STENCIL_TEST);
	
	/*
	for (int i = -5; i < 5; i++)
	{
		for (int j = -5; j < 5; j++)
		{
			glEnable(GL_STENCIL_TEST);
			glClearStencil(0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearDepth(1.0f);

			//pass1
			v->viewing(i / 10.0, j / 10.0);
			l->environment_lighting();
			s->scene_setting();
			//pass2
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glStencilFunc(GL_ALWAYS, 0, 255);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			s->shadow_setting(l->light_list[0].position);
			glDisable(GL_CULL_FACE);
			//pass3
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glStencilFunc(GL_ALWAYS, 0, 255);
			glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
			s->shadow_setting(l->light_list[0].position);
			glDisable(GL_CULL_FACE);
			//pass4
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glClear(GL_DEPTH_BUFFER_BIT);
			glStencilFunc(GL_EQUAL, 0, 255);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			l->lighting();
			s->scene_setting();

			glAccum(GL_ACCUM, 0.01);
			glDisable(GL_STENCIL_TEST);
		}
	}
	
	*/
	glAccum(GL_RETURN, 1.0);
	glutSwapBuffers();

	glDisable(GL_DEPTH_TEST);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':
			v->zoom(1);
		break;
		case 'a':
			v->rotate(1);
		break;
		case 's':
			v->zoom(0);
		break;
		case 'd':
			v->rotate(0);
		break;
		default:
			if (key >= '0' && key <= '9')
				s->select(key - '0');
		break;
	}
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state)
				s->set_x_y(-1, -1);
			else
				s->set_x_y(x, y);
		break;
		case GLUT_RIGHT_BUTTON :
			if (state)
				s->set_x_y(-1, -1);
			else
				s->set_x_y(x, y);
		break;
	}
}

void mouse_motion(int x, int y)
{
	s->move(x, y, v->viewport[2], v->viewport[3]);
	s->set_x_y(x, y);
	glutPostRedisplay();
}
