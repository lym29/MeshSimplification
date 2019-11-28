#include <iostream>  
#include <math.h>
#include <Windows.h>
#include <string>
#include "mesh_simplification.h"
#include "GL/freeglut.h"

int mousetate = 0; 
GLfloat Oldx = 0.0; 
GLfloat Oldy = 0.0;

float xRotate = 0.0f;
float yRotate = 0.0f;
float ty = 0.0f;
float scale = 1;

MyMesh mesh;
MyMesh backup_mesh;
MyMesh orig_mesh;
MeshSimplification mesh_simp(mesh);
bool is_drawing_orig = true;

const std::string file_1 = "../meshes/Bunny.obj";
const std::string file_2 = "../meshes/Bunny_head.obj";
const std::string file_3 = "../meshes/Balls.obj";
int currentfile = 1;

GLuint showFaceList, showWireList;
int showstate = 1;
bool showFace = true;
bool showWire = false;
bool showFlatlines = false;
  
void initGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(2.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); 
	/*<--Lighting-->*/  
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); 
	/*<--Display List-->*/

	showFaceList = glGenLists(1);
	showWireList = glGenLists(1);
	int temp = mesh.n_edges();

	/*<--Draw Wire-->*/
	glNewList(showWireList, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINES);
	for (auto he : mesh.halfedges()) 
	{
		glVertex3fv(mesh.point(mesh.from_vertex_handle(he)).data());
		glVertex3fv(mesh.point(mesh.to_vertex_handle(he)).data());
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glEndList();

	glNewList(showFaceList, GL_COMPILE);
	for (auto f : mesh.faces()) 
	{
		glBegin(GL_TRIANGLES);
		for (auto fv : mesh.fv_range(f)) 
		{
			glNormal3fv(mesh.normal(fv).data());
			glVertex3fv(mesh.point(fv).data());
		}
		glEnd();
	}
	glEndList();
}

void myReshape(GLint w, GLint h)
{
	glViewport(0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w > h)
		glOrtho(-static_cast<GLdouble>(w) / h, static_cast<GLdouble>(w) / h, -1.0, 1.0, -100.0, 100.0);
	else
		glOrtho(-1.0, 1.0, -static_cast<GLdouble>(h) / w, static_cast<GLdouble>(h) / w, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void readfile(std::string file) {
	mesh.request_vertex_normals();
	if (!mesh.has_vertex_normals())
	{
		std::cout << "ERROR：ATTRIBUTE NORMAL DOES NOT EXIST !" << std::endl;
		return;
	}
	OpenMesh::IO::Options opt;
	if (!OpenMesh::IO::read_mesh(mesh, file, opt))
	{
		std::cout << "Fail to read file:" << file << std::endl;
		return;
	}
	else std::cout << "Successfully read file:" << file << std::endl;

	std::cout << "Current number of vertice:" << mesh.n_vertices() << std::endl;
	
	Eigen::Vector3f bb_min(FLT_MAX, FLT_MAX, FLT_MAX);
	Eigen::Vector3f bb_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (auto v : mesh.vertices())
	{
		Eigen::Vector3f p = mesh.point(v);
		bb_min = bb_min.cwiseMin(p);
		bb_max = bb_max.cwiseMax(p);
	}

	if ((bb_min - bb_max).norm() > 1e-3)
	{
		for (auto v : mesh.vertices())
		{
			mesh.point(v) = (mesh.point(v) - bb_min).cwiseQuotient(bb_max - bb_min);
			mesh.point(v) -= Eigen::Vector3f(0.5f, 0.5f, 0.5f);
			//mesh.point(*it) *= 2;
		}
	}
	
	if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	{
		mesh.request_face_normals();
		mesh.update_normals();
		mesh.release_face_normals();
	}
}

void mySpecial(int key, int x, int y) 
{
	int target_num = -1;
	switch (key) 
	{
	case GLUT_KEY_F1:
		std::cout << "Reading file:" << file_1 << "......" << std::endl;
		readfile(file_1);
		orig_mesh = mesh;
		currentfile = 1;
		initGL();
		break;
	case GLUT_KEY_F2:
		std::cout << "Reading file:" << file_2 << "......" << std::endl;
		readfile(file_2);
		orig_mesh = mesh;
		currentfile = 2;
		initGL();
		break;
	case GLUT_KEY_F3:
		std::cout << "Reading file:" << file_3 << "......" << std::endl;
		readfile(file_3);
		orig_mesh = mesh;
		currentfile = 3;
		initGL();
		break;
	case GLUT_KEY_F4:
		if (showFace == true) 
		{
			showFace = false;
			showWire = true;
			std::cout << "Change Display Mode：WireFrame" << std::endl;
		}
		else if (showWire == true)
		{
			showWire = false;
			showFlatlines = true;
			std::cout << "Change Display Mode：Flatlines" << std::endl;
		}
		else if (showFlatlines == true) 
		{
			showFlatlines = false;
			showFace = true;
			std::cout << "Change Display Mode：Flat" << std::endl;
		}
		break;
	case GLUT_KEY_F5:
		is_drawing_orig = !is_drawing_orig;
		if (is_drawing_orig)
		{
			mesh.assign(orig_mesh);
			mesh.request_face_normals();
			mesh.update_normals();
			mesh.release_face_normals();
		}
		else
		{
			mesh.assign(backup_mesh);
			mesh.request_face_normals();
			mesh.update_normals();
			mesh.release_face_normals();
		}
		initGL();
		break;
	case GLUT_KEY_SHIFT_L:
	case GLUT_KEY_SHIFT_R:
		target_num = -1;
		std::cout << "Simplification: input the target number of vertice\n";
		std::cin >> target_num;
		if (target_num <= 0)
			std::cout << "ERROR: TARGET NUMBER SHOULD BE POSITIVE\n";
		else
		{
			mesh_simp.run_simp(target_num);
			mesh_simp.get_mesh(mesh);
			backup_mesh = mesh;
			std::cout << "Finished\n";
		}
		initGL();
		break;
	case GLUT_KEY_UP:
		ty += 0.01;
		break;
	case GLUT_KEY_DOWN:
		ty -= 0.01;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		mousetate = 1;
		Oldx = x;
		Oldy = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mousetate = 0;

	if (state == GLUT_UP && button == 3 /*Mouse Whell Rolling Down*/) 
	{
		scale -= 0.1;
	}
	if (state == GLUT_UP && button == 4 /*Mouse Whell Rolling Up*/) 
	{
		scale += 0.1;
	}
	glutPostRedisplay();
}

void onMouseMove(int x, int y) 
{
	if (mousetate) {
		yRotate += x - Oldx;
		glutPostRedisplay();
		Oldx = x;
		xRotate += y - Oldy;
		glutPostRedisplay();
		Oldy = y;
	}
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, ty);
	glScalef(scale, scale, scale);

	if (showFace)
		glCallList(showFaceList);
	if (showFlatlines) {
		glCallList(showFaceList);
		glCallList(showWireList);
	}
	if (showWire)
		glCallList(showWireList);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // GLUT_Double 表示使用双缓存而非单缓存
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 500);
	glutCreateWindow("Mesh Viewer");
	
	readfile(file_1);
	orig_mesh.assign(mesh);
	initGL();
	glutMouseFunc(myMouse);
	glutMotionFunc(onMouseMove);
	glutSpecialFunc(&mySpecial);
	glutReshapeFunc(&myReshape);
	glutDisplayFunc(&myDisplay);

	glutMainLoop();
	return 0;
}