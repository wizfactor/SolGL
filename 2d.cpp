//#include <GL/glew.h>
#include <GL/glfw.h>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

struct Vtx {
	GLfloat x, y;
	GLuint color;
};

enum { ATTRIB_POS, ATTRIB_COLOR };

const GLfloat PI = 3.14159265f;
const GLuint cPoints = 300; //number of vertices to represents one circle
GLuint totalPoints = (int) 10 * cPoints + 4; //1 sun + 8 planets + 1 moon + 1 axis

//Colors are in BGR order
const GLuint RED =  0x0000FF;
const GLuint BLUE = 0xFF0000;
const GLuint DARK_BLUE = 0x990000;
const GLuint GREEN = 0x00FF00;
const GLuint YELLOW = 0x00FFFF;
const GLuint ORANGE = 0x3366FF;
const GLuint PURPLE = 0x660066;
const GLuint WHITE = 0xFFFFFF;
const GLuint GRAY = 0x666666;
const GLuint GOLD = 0x6699;
const GLuint PALE_YELLOW = 0x99FFFF;
const GLuint LIGHT_TORQUOISE = 0xFFFFF33;
const GLuint DARK_TORQUOISE = 0x999900;

const GLfloat radius = 0.1;

GLuint MAT_ID;
GLuint TEX_ID;

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

void createCircle(Vtx t[], GLuint start, GLuint end, GLuint color1, GLuint color2) {
	GLfloat rad = 0, theta = 360 * (PI/180) / (cPoints); 

	t[start].x = 0;
	t[start].y = 0;
	t[start].color = color2;

	cout << color1 << " " << color2 << endl;

	//bool alternate = true;

	//for(int i = start+1; i < end-1; ++i) { //USE THIS TO SHOW OBJETS ARE ROTATING
	for(int i = start+1; i < end; ++i) {
		t[i].x = (radius * cos(rad));
		t[i].y = (radius * sin(rad));
		t[i].color = color1;

		//cout << t[i].x << " " << t[i].y << " " << t[i].color << endl;

		rad += theta;
	}

	t[end].x =(radius * cos(0));
	t[end].y =(radius * sin(0));
	t[end].color = color1;

	cout << endl;
}

void createAxes(Vtx t[]) {
	t[cPoints * 10].x = 0;
	t[cPoints * 10].y = 0;
	t[cPoints * 10].color = GREEN;

	t[cPoints * 10 + 1].x = 0.5;
	t[cPoints * 10 + 1].y = 0;
	t[cPoints * 10 + 1].color = GREEN;

	t[cPoints * 10 + 2].x = 0;
	t[cPoints * 10 + 2].y = 0;
	t[cPoints * 10 + 2].color = GREEN;

	t[cPoints * 10 + 3].x = 0;
	t[cPoints * 10 + 3].y = 0.5;
	t[cPoints * 10 + 3].color = GREEN;
}

GLMatrix3 draw(GLMatrix3 baseTransform, float sc, float rot, float tran, float rev, GLuint ind, bool axes) {
	GLMatrix3 transform, scale, rotate, revolve, translate, rotateInverse;
	
	scale.setIdentity();
	scale.scale(sc,sc);
	rotate.setIdentity();
	rotate.setRotation(0,0,rot);
	translate.setIdentity();
	translate.setTranslation(tran,0);
	revolve.setIdentity();
	revolve.setRotation(0,0,rev);

	//Counteract the extra rotation caused by revolution
	rotateInverse = revolve;
	rotateInverse.transpose();

	transform.setIdentity();
	transform = baseTransform * revolve * translate * rotateInverse * rotate * scale;
	glUniformMatrix3fv(MAT_ID, 1, false, transform.mat);
	glDrawArrays(GL_TRIANGLE_FAN, cPoints * ind, cPoints);

	//Code block for showing axes
	if(axes) {
		glDrawArrays(GL_LINES, cPoints*10, 2);	
		glDrawArrays(GL_LINES, cPoints*10 + 2, 2);	
	}	

	//This returned matrix is to be used exclusively for moon-related transformations
	transform = baseTransform * revolve * translate * rotateInverse;
	return transform;
}

int main() {
	if ( !glfwInit() ) {
		cerr << "Unable to initialize OpenGL!\n";
		return -1;
	}
	if ( !glfwOpenWindow(850,850,
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

	Vtx vertices[totalPoints];

	//Sun
	createCircle(vertices, 0, cPoints-1, YELLOW, YELLOW);
	//Mercury
	createCircle(vertices, cPoints, (cPoints*2)-1, WHITE, GRAY);
	//Venus
	createCircle(vertices, (cPoints*2), (cPoints*3)-1, YELLOW, GOLD);
	//Earth
	createCircle(vertices, (cPoints*3), (cPoints*4)-1,BLUE, GREEN);
	//Moon
	createCircle(vertices, (cPoints*4), (cPoints*5)-1, WHITE, WHITE);
	//Mars
	createCircle(vertices, (cPoints*5), (cPoints*6)-1, RED, RED);
	//Jupiter
	createCircle(vertices, (cPoints*6), (cPoints*7)-1,ORANGE, RED);
	//Saturn
	createCircle(vertices, (cPoints*7), (cPoints*8)-1,YELLOW, GOLD);
	//Uranus
	createCircle(vertices, (cPoints*8), (cPoints*9)-1, LIGHT_TORQUOISE, DARK_TORQUOISE);
	//Neptune
	createCircle(vertices, (cPoints*9), (cPoints*10)-1,BLUE, DARK_BLUE);

	createAxes(vertices);

	glEnableVertexAttribArray(ATTRIB_POS);
	glEnableVertexAttribArray(ATTRIB_COLOR);

	glVertexAttribPointer(ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, sizeof(Vtx), &vertices[0].x);
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vtx), &vertices[0].color);
	MAT_ID = glGetUniformLocation(program,"modelTransform");
	TEX_ID = glGetUniformLocation(program,"texUnit0");

	glUseProgram(program);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	GLfloat t = 0;	
	GLfloat moveY = 0, moveX = 0, scaleX = 0.4, scaleY = 0.4, rotT = 0, delta = 5;
	bool pause = false, toggle = false;

	do {
		glClear(GL_COLOR_BUFFER_BIT);

		if(glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS) {
			if ( glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS ) {
				scaleX += 0.01;
				scaleY += 0.01;
			} 
			if ( glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS ) {
				if(scaleX <= 0 || scaleY <= 0) { //Scale should NOT be negative
					scaleX = 0;
					scaleY = 0;
				}
				else {
					scaleX -= 0.01;
					scaleY -= 0.01;
				}
			} 
			if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
				rotT += 0.01;
			} 
			if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
				rotT -= 0.01;
			}
		}
		else if (glfwGetKey(GLFW_KEY_LALT) == GLFW_PRESS) {
			if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
				if(delta <= 0) { //Time should NOT be negative
					delta = 0;
				} else {
					delta -= 0.01;
				}
			} 
			else if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
				delta += 0.01;
			}

		}
		else if (glfwGetKey(GLFW_KEY_RALT) == GLFW_PRESS) {
			if(glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
				toggle = false;
			} else if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
				toggle = true;
			} else if (glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS) {
				pause = false;
			} else if (glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS) {
				pause = true;
			}
		}
		else {
			if ( glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS ) {
				moveY -= 0.01;
			} 
			if ( glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS ) {
				moveY += 0.01;
			} 
			if (glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS) {
				moveX += 0.01;
			} 
			if (glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS) {
				moveX -= 0.01;
			}
		}  
		if( glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) { //The Reset Button
			moveY = 0, moveX = 0, scaleX = 0.4, scaleY = 0.4, rotT = 0, t = 0;
		}

		GLMatrix3 baseTransform, sunRot, moonTransform;

		//The Sun
		baseTransform.setIdentity();
		baseTransform.scale(scaleX,scaleY);
		baseTransform.translate(moveX,moveY);
		sunRot.setIdentity();
		sunRot.setRotation(0,0,rotT);

		baseTransform = sunRot * baseTransform;

		glUniformMatrix3fv(MAT_ID, 1, false, baseTransform.mat);
		glDrawArrays(GL_TRIANGLE_FAN, 0, cPoints);	

		//Mercury
		draw(baseTransform, 0.15, t*(0.017), 0.3, t*(0.011), 1, toggle);

		//Venus
		draw(baseTransform, 0.25, t*(-0.004), 0.4, t * 0.0046, 2, toggle);

		//Earth
		moonTransform = draw(baseTransform, 0.27, t, 0.55, t * 0.0027, 3, toggle);

		//Moon
		draw(moonTransform, 0.05, t*0.036, 0.07, t * 0.033, 4, toggle);		

		//Mars
		draw(baseTransform, 0.19, t*(1.03), 0.72, t * 0.00146, 5, toggle);

		//Jupiter
		draw(baseTransform, 0.75, t*(2.41), 1.1, t * 0.00023, 6, toggle);

		//Saturn
		draw(baseTransform, 0.70, t*(2.25), 1.4, t * 0.000092, 7, toggle);

		//Uranus
		draw(baseTransform, 0.62, t*(-1.39), 1.65, t * 0.000033, 8, toggle);

		//Neputune
		draw(baseTransform, 0.58, t*(1.486), 2, t * 0.000017, 9, toggle);
		
		glfwSwapBuffers();

		if(!pause) {
			t += delta;
		}
	} while ( glfwGetKey(GLFW_KEY_ESC) != GLFW_PRESS && glfwGetWindowParam(GLFW_OPENED) );
	glfwTerminate();
	
	return 0;
}
