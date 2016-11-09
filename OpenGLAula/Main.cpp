#include <GL/glew.h> /* include GLEW and new version of GL on Windows */ 
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>

GLfloat points[] = {
	0.2f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	0.2f, 0.5f, 0.0f,
	0.7f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f
};

GLfloat colors[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f
};

const char* vertex_shader =
"#version 330\n"
"layout(location=0) in vec3 vp;"
"layout(location=1) in vec3 vc;"
"out vec3 color;"
"void main () {"
" color = vc;"
" gl_Position = vec4 (vp, 1.0);"
"}";

const char* fragment_shader =
"#version 330\n"
"in vec3 color;"
"out vec4 frag_color;"
"void main () {"
" frag_color = vec4 (color, 1.0);"
"}";

// vari�veis globais 
double previous_seconds;
int frame_count;

void updateFPS(GLFWwindow* window) {
	double current_seconds =
		glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds
		;
	char tmp[128];
	double fps = (double)frame_count / elapsed_seconds
		;
	sprintf_s(tmp, "opengl @ fps: %.2f", fps);
	glfwSetWindowTitle(window, tmp);
	previous_seconds = current_seconds;
	frame_count
		++;
}

int main() {
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	/* Caso necess�rio, defini��es espec�ficas para SOs, p. e. Apple OSX *
	/* Definir como 3.2 para
	Apple OS X */
	/*
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	GLFWwindow *g_window = glfwCreateWindow(
		640, 480, "Teste de vers�o OpenGL", NULL, NULL);
	if (!g_window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(g_window);
	// inicia manipulador da extens�o GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	// obten��o de vers�o suportada da OpenGL e renderizador
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL (vers�o suportada) %s\n", version);


	// identifica vs e o associa com vertex_shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);
	// identifica fs e o associa com fragment_shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	// identifica do programa, adiciona partes e faz "linkagem"
	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);
	// passagem de vari�veis CPU para GPU
	GLint location = glGetUniformLocation(shader_programme, "inColor");
	glUniform4f(location, 0.0f, 0.0f, 1.0f, 1.0f);


	// Define shader_programme como o shader a ser utilizado
	glUseProgram(shader_programme);

	GLuint colorsVBO = 0;
	glGenBuffers(1, &colorsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors) * sizeof(GLfloat), colors, GL_STATIC_DRAW);

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) * sizeof(GLfloat), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // identifica vbo atual
										// habilitado primeiro atributo do vbo bound atual
	glEnableVertexAttribArray(0);
	// associa��o do vbo atual com primeiro atributo
	// 0 identifica que o primeiro atributo est� sendo definido
	// 3, GL_FLOAT identifica que dados s�o vec3 e est�o a cada 3 float.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// � poss�vel associar outros atributos, como normais, mapeamento e cores
	// lembre-se: um por v�rtice!
	glBindBuffer(GL_ARRAY_BUFFER, colorsVBO);
	// habilitado segundo atributo do vbo bound atual
	glEnableVertexAttribArray(1);
	// note que agora o atributo 1 est� definido
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CW); // GL_CCW for counter clock-wise


	while (!glfwWindowShouldClose(g_window)) {
		updateFPS(g_window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Define vao como verte array atual
		glBindVertexArray(vao);
		// desenha pontos a partir do p0 e 3 no total do VAO atual com o shader
		// atualmente em uso
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();
		glfwSwapBuffers(g_window);
	}
	// loop de desenho! Pr�ximo slide...
	// encerra contexto GL e outros recursos da GLFW
	glfwTerminate();
	return 0;
}