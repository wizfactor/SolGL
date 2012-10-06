//#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstdio>
#include <cmath>
#include <iostream>

using namespace std;

struct Vtx {
	GLfloat x, y;
	GLuint color;
};

enum { ATTRIB_POS, ATTRIB_COLOR };

struct GLMatrix3 {
	GLfloat mat[9];
	
	void setIdentity() {
		mat[0] = 1, mat[3] = 0, mat[6] = 0;
		mat[1] = 0, mat[4] = 1, mat[7] = 0;
		mat[2] = 0, mat[5] = 0, mat[8] = 1;
	}
	
	void setRotation(GLfloat x, GLfloat y, GLfloat theta) {
		const GLfloat c = cos(theta), s = sin(theta);
		mat[0] = c, mat[3] = -s, mat[6] = -c * x + s * y + x;
		mat[1] = s, mat[4] = c,  mat[7] = -s * x - c * y + y;
		mat[2] = 0, mat[5] = 0,  mat[8] = 1;
	}
	
	void setTranslation(GLfloat x, GLfloat y) {
		mat[0] = 1, mat[3] = 0, mat[6] = x;
		mat[1] = 0, mat[4] = 1, mat[7] = y;
		mat[2] = 0, mat[5] = 0, mat[8] = 1;
	}

	void translate(GLfloat x, GLfloat y) {
		mat[6] += x;
		mat[7] += y;
	}
	
	void scale(GLfloat sx, GLfloat sy) {
		mat[0] *= sx;
		mat[3] *= sx;
		mat[6] *= sx;
		
		mat[1] *= sy;
		mat[4] *= sy;
		mat[7] *= sy;
	}
	
	void transpose() {
		swap(mat[3],mat[1]);
		swap(mat[6],mat[2]);
		swap(mat[7],mat[5]);
	}
	
	GLMatrix3& operator=(const GLMatrix3 &rhs) {
		memcpy(mat, rhs.mat, sizeof(mat));
		return *this;
	}
	
	GLMatrix3 operator*(const GLMatrix3 &rhs) const {
		GLMatrix3 ret;
		for ( int i = 0; i < 9; ++i ) {
			const int a = i % 3, b = (i / 3) * 3;
			ret.mat[i] = mat[a]*rhs.mat[b] + mat[a+3]*rhs.mat[b+1] + mat[a+6]*rhs.mat[b+2];
		}
		return ret;
	}
	
	GLMatrix3& operator*=(const GLMatrix3 &rhs) {
		GLMatrix3 tmp;
		for ( int i = 0; i < 9; ++i ) {
			const int a = i % 3, b = (i / 3) * 3;
			tmp.mat[i] = mat[a]*rhs.mat[b] + mat[a+3]*rhs.mat[b+1] + mat[a+6]*rhs.mat[b+2];
		}
		memcpy(mat, tmp.mat, sizeof(mat));
		return *this;
	}
};

bool loadShaderSource(GLuint shader, const char *filePath) {
	FILE *f = fopen(filePath, "r");
	if ( !f ) {
		cerr << "Cannot find file: " << filePath << '\n';
		return false;
	}
	fseek(f, 0, SEEK_END);
	const size_t sz = ftell(f) + 1;
	fseek(f, 0, SEEK_SET);
	
	GLchar *buffer = new GLchar[sz];
	fread(buffer, 1, sz, f);
	fclose(f);
	buffer[sz-1] = 0;
	glShaderSource(shader, 1, (const GLchar**) &buffer, NULL);
	
	glCompileShader(shader);
	delete [] buffer;
	
	GLint logLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
	if ( logLength > 0 ) {
		GLchar *log = new GLchar[logLength];
		glGetShaderInfoLog(shader, logLength, &logLength, log);
		cout << "Shader Compile Log:\n" << log << endl;
		delete [] log;
	}
	
	return true;
}

int main() {
	if ( !glfwInit() ) {
		cerr << "Unable to initialize OpenGL!\n";
		return -1;
	}
	if ( !glfwOpenWindow(640,640,
						 8,8,8,8,
						 0,0,
						 GLFW_WINDOW) ) {
		cerr << "Unable to create OpenGL window.\n";
		glfwTerminate();
		return -1;
	}
	/*
	if ( glewInit() != GLEW_OK ) {
		cerr << "Unable to hook OpenGL extensions!\n";
		return -1;
	}
	*/
	glfwSetWindowTitle("2D Transformations");
	
	glfwEnable(GLFW_STICKY_KEYS);
	glfwSwapInterval(1);
	
	GLuint vtxShader = glCreateShader(GL_VERTEX_SHADER),
	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if ( !loadShaderSource(vtxShader, "2d.vsh") ) return -1;
	if ( !loadShaderSource(fragShader, "2d.fsh") ) return -1;
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vtxShader);
	glAttachShader(program, fragShader);
	
	glBindAttribLocation(program, ATTRIB_POS, "pos");
	glBindAttribLocation(program, ATTRIB_COLOR, "color");
	
	glLinkProgram(program);
	
	{
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		if ( logLength > 0 ) {
			GLchar *log = new GLchar[logLength];
			glGetProgramInfoLog(program, logLength, &logLength, log);
			cout << "Program Compile Log:\n" << log << endl;
			delete [] log;
		}
	}
	glDeleteShader(fragShader);
	glDeleteShader(vtxShader);
	fragShader = 0;
	vtxShader = 0;
	
	const GLfloat rectWidth = 0.2, rectHeight = 0.2;
	Vtx vertices[] = { //Colors are in BGR order
		{-rectWidth, rectHeight,0x0000FF},  //Upper Left is Red
		{-rectWidth, -rectHeight,0x00FF00}, //Lower Left is Green
		{rectWidth, rectHeight,0xFF0000},	//Upper Right is Blue
		{rectWidth, -rectHeight,0x00FFFF}	//Lower Right is Yellow
	};	
	
	glEnableVertexAttribArray(ATTRIB_POS);
	glEnableVertexAttribArray(ATTRIB_COLOR);

	glVertexAttribPointer(ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof(Vtx), &vertices[0].x);
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vtx), &vertices[0].color);
	GLuint MAT_ID = glGetUniformLocation(program,"modelTransform");

	glUseProgram(program);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glColorPointer(4, GL_UNSIGNED_BYTE, 8, vertices);
	
	GLfloat t = 0;	
	const GLfloat x_off = .4, y_off = .4;

	do {
		glClear(GL_COLOR_BUFFER_BIT);
		
		GLMatrix3 transform, rotate, revolve, translation;

		GLfloat t2 = t / 4;

		transform.setIdentity();
		translation.setTranslation(0.4,0.4);
		rotate.setRotation(0,0,t);
		revolve.setRotation(0,0,t2);

		
		transform = revolve * translation * rotate;
		

		glUniformMatrix3fv(MAT_ID, 1, false, transform.mat);

		//Place transformation code here.

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glfwSwapBuffers();
		t += 0.06f;
	} while ( glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED) );
	glfwTerminate();
	
	return 0;
}