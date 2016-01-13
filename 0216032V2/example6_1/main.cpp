#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "glew.h"
#include "glut.h"
#include "mesh.h"
#include "FreeImage.h"
#include "Vector3D.h"
#include "ShaderLoader.h"
#include "vec3.h"
#include "vec2.h"


#define NUM_TEXTURE 100
#define		TEX_NUM 3	  //the number of textures you use.
unsigned int texObject[NUM_TEXTURE];
using namespace std;
using namespace MathTool;

const string localPath = "C:\\Users\\XDEX-pc\\Desktop\\0216032V1\\HW3\\";
const char* viewfile  = "C:\\Users\\XDEX-pc\\Desktop\\0216032V1\\HW3\\Scene3.view";
const char* lightfile = "C:\\Users\\XDEX-pc\\Desktop\\0216032V1\\HW3\\Scene3.light";
const char* scenefile = "C:\\Users\\XDEX-pc\\Desktop\\0216032V1\\HW3\\Scene3.scene";

mesh *objects[1000];
char buf[5010];

int objSum = 0 , windowSize[2] , oldX, oldY, selected = 0 , mouseDown , angle = 0 , ttt , DEL = 10;
double NewX[1001] = { 0 }, NewY[1000] = { 0 } , deltX[1000] = { 0 }, deltY[1000] = { 0 };
double R[3];
bool mohu = false;
char* files[10000];
GLuint	objtext[TEX_NUM];
GLhandleARB	YourShader;


void light();
void display();
void reshape(GLsizei, GLsizei);
struct View {
	float eye[3], vat[3], vup[3], fovy, dnear, dfar, viewport[4];
}view;
struct Light {
	float pos[3], ambient[3], diffuse[3], specular[3];
};
struct Ambient {
	float data[3];
};
struct Scene {
	static const int no = -1 , single = 0, multi = 1, cube = 2 , shader = 3;
	int textType , beginID;
	string fileName;
	float scale[3], angle, angles[3], transfer[3];
};

vector<string> textPath;
vector< Light > lights;
vector< Ambient > ambients;
vector< Scene > scenes;
vector<string> shader;

struct ReadView {
	ReadView(const char* path, View &view) {
		ifstream fin(path);
		string OP;
		while (fin >> OP) {
			if (OP == "eye") {
				for (int i = 0; i < 3; i++)
					fin >> view.eye[i];
			}
			else if (OP == "vat") {
				for (int i = 0; i < 3; i++)
					fin >> view.vat[i];
			}
			else if (OP == "vup") {
				for (int i = 0; i < 3; i++)
					fin >> view.vup[i];
			}
			else if (OP == "fovy") {
				fin >> view.fovy;
			}
			else if (OP == "dnear") {
				fin >> view.dnear;
			}
			else if (OP == "dfar") {
				fin >> view.dfar;
			}
			else if (OP == "viewport") {
				for (int i = 0; i < 4; i++) {
					fin >> view.viewport[i];
				}
			}

		}
		fin.close();
	}
};
struct ReadLight {
	ReadLight(const char* path, vector<Light> &lightVec, vector<Ambient> &ambientVec) {
		ifstream fin(path);
		string OP;
		
		cout << fin.eof();

		while (fin >> OP) {
			if (OP == "light") {
				Light light;
				for (int i = 0; i < 3; i++)
					fin >> light.pos[i];
				for (int i = 0; i < 3; i++)
					fin >> light.ambient[i];
				for (int i = 0; i < 3; i++)
					fin >> light.diffuse[i];
				for (int i = 0; i < 3; i++)
					fin >> light.specular[i];
				lightVec.push_back(light);
			}
			else if (OP == "ambient") {
				Ambient ambient;
				for (int i = 0; i < 3; i++)
					fin >> ambient.data[i];
				ambientVec.push_back(ambient);
			}

		}
		fin.close();

	}
};
struct ReadScene {
	ReadScene(const char* path, vector<Scene> &sceneVec) {
		ifstream fin(path);	
		Scene s;
		string OP , tmp2;
		int textType = s.no , beginID = 0;

		while (fin >> OP) {
			if (OP == "model") {
				Scene scene;
				fin >> scene.fileName;
				string tmp = scene.fileName;
				files[objSum] = new char[101];
				strcpy(files[objSum++], tmp.c_str());
				for (int i = 0; i < 3; i++)
					fin >> scene.scale[i];
				fin >> scene.angle;
				for (int i = 0; i < 3; i++)
					fin >> scene.angles[i];
				for (int i = 0; i < 3; i++)
					fin >> scene.transfer[i];

				scene.beginID = beginID;
				scene.textType = textType;
				sceneVec.push_back(scene);
			}


			else if (OP == "no-texture") {
				textType = s.no;
			}
			else if (OP == "single-texture") {
				fin >> tmp2;
				tmp2 = localPath + tmp2;
				textType = s.single;

				beginID = textPath.size();
				textPath.push_back(tmp2);
			}
			else if (OP == "multi-texture") {
				
				textType = s.multi;
				
				char t1[101], t2[101];
				fin.getline(buf, 5000);
				beginID = textPath.size();
				sscanf(buf, "%s %s", t1, t2);
				textPath.push_back(localPath + string(t1));
				textPath.push_back(localPath + string(t2));
				
			}
			else if (OP == "cube-map") {

				textType = s.cube;
				
				beginID = textPath.size();
				for (int i = 0; i < 6; i++) {
					fin >> tmp2;
					textPath.push_back(localPath + tmp2);

				}

			}
			else if (OP == "shader_textures") {

				textType = s.shader;
				beginID = textPath.size();
				for (int i = 0; i < 3; i++) {
					fin >> tmp2;
					textPath.push_back(localPath + tmp2);
				
				}
				
			}
		}
		fin.close();

	}

};

void LoadTexture( Scene &scene  , int id){
	
	cout << "!!" << scene.textType << endl;

	int endID = 0;
	if (scene.textType == scene.no)
		endID = scene.beginID-1;
	else if (scene.textType == scene.single)
		endID = scene.beginID ;
	else if (scene.textType == scene.multi)
		endID = scene.beginID + 1;
	else if (scene.textType == scene.shader)
		endID = scene.beginID + 2;
	else 
		endID = scene.beginID + 5;
	
	
	for (int i = scene.beginID , delta = 0; i <= endID; i++ , delta++) {

		const char* pFilename = textPath[i].c_str();

	

		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
		FIBITMAP* bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
		int bitmapWidth = FreeImage_GetWidth(bitmap32);
		int bitmapHeight = FreeImage_GetHeight(bitmap32);



		int textureType = GL_TEXTURE_2D;
		int textureMode = GL_TEXTURE_2D;
		/*
		if (scene.textType == scene.cube) {
			textureType = GL_TEXTURE_CUBE_MAP;
			switch (delta) {
				case 0:
					textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
					break;
				case 1:
					textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
					break;
				case 2:
					textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
					break;
				case 3:
					textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
					break;
				case 4:
					textureMode = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
					break;
				case 5:
					textureMode = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
					break;
			}

		}
		*/
		
		cout << "%%%" << id << " " << texObject[id] << endl;
		glBindTexture(textureType, texObject[id]);
		glTexImage2D(textureMode, 0, GL_RGBA8, bitmapWidth, bitmapHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap32));
		//glGenerateMipmap(textureType);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//float largest;
		//glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
		//glTexParameterf(textureType, GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, largest);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		FreeImage_Unload(bitmap32);
		FreeImage_Unload(bitmap);

		if (scene.textType == scene.multi || scene.textType == scene.shader) id++;
	}
}

void Refresh(int sel) {
	deltX[sel] += NewX[sel];
	NewX[sel] = 0;
	deltY[sel] += NewY[sel];
	NewY[sel] = 0;
}
void drawF();
void zoom(int val) {
	val *= -1;
	float tmp[3], length = 0;
	float delta[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		tmp[i] = view.eye[i] - view.vat[i];
		length += tmp[i] * tmp[i];
	}
	length = sqrt(length);
	for (int i = 0; i < 3; i++)
		view.eye[i] += tmp[i] / length * val;
}
void rotate(int val) {
	angle += val;
}
void keyboard(unsigned char key, int x, int y)
{
	cout << " key : " << key << endl;
	if (key <= '9' && '1' <= key) {
		int i = key - '1';
		Refresh(selected);
		if (i < objSum) {
			selected = i;
		}
	}
	
	switch (key){
	case 'r':
		mohu = !mohu;
		break;
	case 'w':
		zoom(1);
		glutPostRedisplay();
		break;
	case 's':
		zoom(-1);
		glutPostRedisplay();
		break;
	case 'a':
		rotate(-DEL);
		glutPostRedisplay();
		break;
	case 'd':
		rotate(DEL);
		glutPostRedisplay();
		break;
	}
}
void mouseClicks(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN){
			mouseDown = 1;
			oldX = x, oldY = y;
		}
		else {
			mouseDown = 0;
			Refresh(selected);
		}
		glutPostRedisplay();
		break;
	};
}
void mouseMove(int x, int y) {

	NewX[selected] = (x - oldX) / 100.0;
	NewY[selected] = -(y - oldY) / 100.0;

	glutPostRedisplay();
}

void LoadShaders();




int main(int argc, char** argv){

	ReadView viewReader(viewfile, view);
	ReadLight lightReader(lightfile, lights, ambients);
	ReadScene sceneReader(scenefile, scenes);


	for (int i = 0; i < 3; i++)
		R[i] = view.eye[i] - view.vat[i];
	for (int i = 0; i < objSum; i++)
		objects[i] = new mesh(files[i]);


	glutInit(&argc, argv);
	glutInitWindowSize(view.viewport[2], view.viewport[3]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutCreateWindow("CGHW3");

	glewInit();

	FreeImage_Initialise();

	glGenTextures(NUM_TEXTURE, texObject);
	for (int i = 0; i < scenes.size(); i++) {
		if ( (i > 0 && !( scenes[i].beginID == scenes[i-1].beginID && scenes[i].textType == scenes[i-1].textType )) || i == 0 )
		LoadTexture( scenes[i] ,  scenes[i].beginID);
		
	}
	
	FreeImage_DeInitialise();

	LoadShaders();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutMouseFunc(mouseClicks);
	//glutMotionFunc(mouseMove);
	glutMainLoop();

	return 0;
}

void LoadShaders(){
	YourShader = glCreateProgram();
	bool result;
	if (YourShader != 0){
		result = ShaderLoad(YourShader, "C:\\Users\\XDEX-pc\\Desktop\\0216032V1\\HW3\\PhongShading.vs", GL_VERTEX_SHADER);
		
		result = ShaderLoad(YourShader, "C:\\Users\\XDEX-pc\\Desktop\\0216032V1\\HW3\\PhongShading.fs", GL_FRAGMENT_SHADER);
		cout << "$$$$" << result << endl;
		cout << "Get phongshading.fs" << endl;
	}
}


void light(){
	glShadeModel(GL_SMOOTH);
	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting 
	glEnable(GL_LIGHTING);
	// set light property
	for (int i = 0; i < lights.size(); i++) {
		Light light = lights[i];
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light.pos);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light.diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light.specular);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light.ambient);
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambients[0].data);
}



void display(){

	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

													   // viewport transformation
													   //	glViewport(0, 0, windowSize[0], windowSize[1]);
	glUseProgram(YourShader);

	glViewport(view.viewport[0], view.viewport[1], view.viewport[2], view.viewport[3]);
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view.fovy, (GLfloat)view.viewport[2] / (GLfloat)view.viewport[3], view.dnear, view.dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(view.eye[0], view.eye[1], view.eye[2],// eye
		view.vat[0], view.vat[1], view.vat[2],     // center
		view.vup[0], view.vup[1], view.vup[2]);    // up
	glRotatef(angle, view.vup[0], view.vup[1], view.vup[2]);
	cout << angle << " " << view.vup[0] << " " << view.vup[1] << " " << view.vup[2] << endl;
	
	
		light();
		drawF();

	glutSwapBuffers();
}

void drawF() {

	int lastMaterial = -1;

	for (int k = 0; k < objSum; k++) {

		bool en = false;
		mesh* object = objects[k];
		glPushMatrix();//儲存現在的矩陣 (目前是modelview)
		int ptr = k;

		Scene scene = scenes[ptr];

		glTranslatef(scene.transfer[0], scene.transfer[1], scene.transfer[2]);
		glRotatef(scene.angle, scene.angles[0], scene.angles[1], scene.angles[2]);
		glScalef(scene.scale[0], scene.scale[1], scene.scale[2]);
		string s[4] = { "colorTexture_N" , "colorTexture_D" , "colorTexture_S" };
		switch (scene.textType) {
			case scene.shader:

			for (int multi = 0; multi < 3; multi++) {
				glActiveTexture(GL_TEXTURE0 + multi);
				//glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texObject[scene.beginID + multi]);
				cout << YourShader << " " << s[multi] << endl;
				GLint location = glGetUniformLocation(YourShader, s[multi].c_str() );
				cout << "location " << multi << " " << location << endl;
				glUniform1i(location, multi);
			}
			break;
	
		}


		for (size_t i = 0; i < object->fTotal; ++i) {
			if (1) {

				lastMaterial = (int)object->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, object->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, object->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, object->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &object->mList[lastMaterial].Ns);
			}
			const vec3<float> P1 = objects[0]->vList[objects[0]->faceList[i][0].v].ptr;
			const vec3<float> P2 = objects[0]->vList[objects[0]->faceList[i][1].v].ptr;
			const vec3<float> P3 = objects[0]->vList[objects[0]->faceList[i][2].v].ptr;
			const vec2<float> UV1 = objects[0]->tList[objects[0]->faceList[i][0].t].ptr;
			const vec2<float> UV2 = objects[0]->tList[objects[0]->faceList[i][1].t].ptr;
			const vec2<float> UV3 = objects[0]->tList[objects[0]->faceList[i][2].t].ptr;

			vec3<float> Edge1 = P2 - P1;
			vec3<float> Edge2 = P3 - P1;
			vec2<float> Edge1uv = UV2 - UV1;
			vec2<float> Edge2uv = UV3 - UV1;
			vec3<float> tangent, bitangent;

			float cp = Edge1uv.x * Edge2uv.y - Edge2uv.x * Edge1uv.y;

			if ( fabs(cp - 0) > 1e-6 ){
				float mul = 1.0f / cp;
				tangent = (Edge1 * Edge2uv.y + Edge2 * -Edge1uv.y) * mul;
				bitangent = (Edge1 * -Edge2uv.x + Edge2 * Edge1uv.x) * mul;
				
			}
	//		cout << "!!! " << tangent.x << " " << tangent.y << " " << tangent.z  << endl;
			
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j < 3; ++j) {
			//	cout << "########   "<<scene.textType << " " << j << endl;
				if (scene.textType == scene.shader ){
					glMultiTexCoord2fv(GL_TEXTURE0, object->tList[object->faceList[i][j].t].ptr);
					glMultiTexCoord3f( GL_TEXTURE1, tangent.x , tangent.y, tangent.z);
					glMultiTexCoord3f(GL_TEXTURE2, bitangent.x, bitangent.y , bitangent.z);
				}

				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);

				float tmp[3];
				for (int a = 0; a < 3; a++)
					tmp[a] = object->vList[object->faceList[i][j].v].ptr[a];
				tmp[0] += deltX[k] + NewX[k];
				tmp[1] += deltY[k] + NewY[k];

				glVertex3fv(tmp);
			}
			glEnd();
		}
		if (scene.textType == scene.shader) {
			for (int mul = 2; mul >= 0; mul--) {
				glActiveTexture(GL_TEXTURE0 + mul);
				glBindTexture(GL_TEXTURE_2D, 0);
				glDisable(GL_TEXTURE_2D);
			}
		}

		glPopMatrix();

	}
	
}


void reshape(GLsizei w, GLsizei h){
	windowSize[0] = w;
	windowSize[1] = h;
}

