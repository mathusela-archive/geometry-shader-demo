#define DEV
// #define FRAMERATE

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <gl/GL.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

const int WIDTH = 700;
const int HEIGHT = 700;
const char TITLE[] = "OpenGL";



std::vector<float> points = {
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.0f, 0.0f, 0.0f
};



void framebuffer_size_callback(GLFWwindow* window, int w, int h) {

	glViewport(0, 0, w, h);

}



unsigned int create_vao(std::vector<float> verts) {

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), &verts[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	return VAO;

}



std::string import_shader_source(std::string inputPath) {

    std::ifstream input(inputPath);
    std::string inputText = "";

    for(std::string line; getline(input, line);) {
        inputText = inputText + "\n" + line;
    }

    inputText += "\0";

    input.close();
    input.clear();

    return inputText;

}


unsigned int create_shader(std::string vertexShaderPath, std::string geometryShaderPath, std::string fragmentShaderPath) {

    std::string vertexShaderSourceString = import_shader_source(vertexShaderPath);
    const char* vertexShaderSource = vertexShaderSourceString.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    #ifdef DEV
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            std::cout << vertexShaderSource << "\n" << std::endl;
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR VERTEX SHADER COMPILATION FAILED\n" << infoLog << std::endl;
        };
    #endif


	std::string geometryShaderSourceString = import_shader_source(geometryShaderPath);
    const char* geometryShaderSource = geometryShaderSourceString.c_str();
    unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShader);

    #ifdef DEV
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            std::cout << geometryShaderSource << "\n" << std::endl;
            glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
            std::cout << "ERROR GEOMETRY SHADER COMPILATION FAILED\n" << infoLog << std::endl;
        };
    #endif


    std::string fragmentShaderSourceString = import_shader_source(fragmentShaderPath);
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    #ifdef DEV
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            std::cout << fragmentShaderSource << "\n" << std::endl;
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR FRAGMENT SHADER COMPILATION FAILED\n" << infoLog << std::endl;
        };
    #endif

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;

}



int main() {

	glfwInit();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



	unsigned int shaderProgram = create_shader("../resources/shaders/vertexShader.vert", "../resources/shaders/geometryShader.geom", "../resources/shaders/fragmentShader.frag");

	unsigned int pointsVAO = create_vao(points);	



	float segments = 10.0f;
	float radius = 0.3;

	#ifdef FRAMERATE
		float lastFrame = glfwGetTime();
		double framerateSum = 0;
		double frameCount = 0;
	#endif

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while (!glfwWindowShouldClose(window)) {

		#ifdef FRAMERATE
			float currentFrame = glfwGetTime();
			float deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			float framerate = 1/deltaTime;
			framerateSum += framerate;
			frameCount += 1;
			float averageFramerate = framerateSum/frameCount;
			std::cout << averageFramerate << std::endl;

			if (frameCount == 3500) {
				frameCount = 0;
				framerateSum = 0;
			}
		#endif

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {segments += 0.01;}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {segments -= 0.01;}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {radius += 0.0003;}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {radius -= 0.0003;}

		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(pointsVAO);
		glUseProgram(shaderProgram);

		glUniform1f(glGetUniformLocation(shaderProgram, "radius"), radius);
		glUniform1i(glGetUniformLocation(shaderProgram, "segments"), (int)segments);

		glDrawArrays(GL_POINTS, 0, points.size()/3);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	return 0;

}