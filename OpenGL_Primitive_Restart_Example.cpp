#include <gmtl\gmtl.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW\glfw3.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "glfw3.lib")

using namespace std; // std::vector -> vector

// GLOBAL VARIABLES
gmtl::Matrix44f M; // The matrix we'll wind up sending to the shaders
gmtl::Matrix44f W_C; // The matrix representing the pose of our cube
float transDelta = 0.1f; // How much any translation operation will translate by
float rotDelta = gmtl::Math::PI_OVER_4 / 2.0; // How much any rotation operation rotates by

// Matrices that we can reuse so we don't have to declare them every time. They will be set to identity in main			
gmtl::Matrix44f xTrans;		
gmtl::Matrix44f zRot;
gmtl::Matrix44f yRot;

// Setup lists for our vertices and indices
vector<GLfloat> arrowVertexBufferData;
vector<GLuint> arrowIndexBufferData;

vector<GLfloat> cubeVertexBufferData;
vector<GLuint> cubeIndexBufferData;


// Handle for our Vertex array object
GLuint arrowVertexArrayID;
GLuint cubeVertexArrayID;


// Handles for our buffer objects
GLuint arrowVertexBuffer;
GLuint arrowIndexBuffer;
GLuint arrowColorBuffer;
GLuint cubeVertexBuffer;
GLuint cubeIndexBuffer;
GLuint cubeColorBuffer;


// Shader attribute handles
GLuint vertex_position, vertex_color;

// RGB triples for each coordinate in vertex list data.
vector<GLfloat> cubeColorBufferData;
vector<GLfloat> arrowColorBufferData;




/* Setup handles for our Vertex Array Objects and Buffer Objects */
void setupDrawObject(GLuint &vertexArrayID, GLuint &vertexBuffer, vector<GLfloat> &vertexBufferData, GLuint &indexBuffer, vector<GLuint> &indexBufferData, GLuint &ColorBuffer, vector<GLfloat> &ColorBufferData) {
	/* Assign ID for a Vertex Array Object to our handle */
	glGenVertexArrays(1, &vertexArrayID);
	/* Bind our Vertex Array Object as the current used object. In OpenGL, you typically will bind a VAO, and then all gl operations after this will be applied to that VAO.*/
	glBindVertexArray(vertexArrayID);

	/* Assign ID for Vertex Buffer Object to our handle */
	glGenBuffers(1, &vertexBuffer);
	/* Bind our VBO as being the active buffer and storing vertex attributes (coordinates) */
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	/* Copy the vertex data from mesh to our buffer */
	/* num_verts * 3 * sizeof(GLfloat) is the size of the mesh list, since */
	/* it contains num_verts * 3 GLfloat values */
	glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(GLfloat), &vertexBufferData[0], GL_STATIC_DRAW);

	/* Assign ID for Index Buffer Object to our handle */
	glGenBuffers(1, &indexBuffer);
	/* Copy the index data from indices to our buffer */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	/* num_indices * sizeof(GLuint) is the size of the indices list, since it contains num_indices GLuint values */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint), &indexBufferData[0], GL_STATIC_DRAW);

	/* Specify that our coordinate data is going into attribute index 0 (SHADER_POSITION_INDEX), and contains three floats per vertex */
	glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, 0, ((void*)0));
	/* Enable attribute index 0 (SHADER_POSITION_INDEX) as being used.*/
	glEnableVertexAttribArray(vertex_position);

	/* Assign ID for Color Buffer Object to our handle */
	glGenBuffers(1, &ColorBuffer);
	/* Copy the color data from colors to our buffer */
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	/* Copy the color data from colors to our buffer */
	/* num_verts * 3 * sizeof(GLfloat) is the size of the color list, since */
	/* it contains num_verts * 3 GLfloat values */
	glBufferData(GL_ARRAY_BUFFER, ColorBufferData.size() * sizeof(GLfloat), &ColorBufferData[0], GL_STATIC_DRAW);
	/* Specify that our color data is going into attribute index 1 (SHADER_COLOR_INDEX), and contains three floats per vertex */
	glVertexAttribPointer(vertex_color, 3, GL_FLOAT, GL_FALSE, 0, ((void*)0));
	/* Enable attribute index 1 (SHADER_COLOR_INDEX) as being used */
	glEnableVertexAttribArray(vertex_color);
}



/* A simple function that will read a file into an allocated char pointer buffer */
char* filetobuf(char *file)
{
	FILE *fptr;
	long length;
	char *buf;

	fopen_s(&fptr, file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length + 1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */

	return buf; /* Return the buffer */
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (action == GLFW_PRESS) {
		switch (mods)
		{
		default: // Do local operations
			switch (key)
			{
			case GLFW_KEY_LEFT: // Local negative x trans
				// This is basically the same as world x trans, only we apply xTrans to the right of M
				xTrans[0][3] = -transDelta;
				xTrans.setState(gmtl::Matrix44f::TRANS);
				M = M * xTrans;
				break;
			case GLFW_KEY_RIGHT: // Local positive x trans
				xTrans[0][3] = transDelta;
				xTrans.setState(gmtl::Matrix44f::TRANS);
				M = M * xTrans;
				break;
			case GLFW_KEY_UP: // Local positive z rot
				zRot[0][0] = gmtl::Math::cos(rotDelta);
				zRot[0][1] = -gmtl::Math::sin(rotDelta);
				zRot[1][0] = gmtl::Math::sin(rotDelta);
				zRot[1][1] = gmtl::Math::cos(rotDelta);
				zRot.setState(gmtl::Matrix44f::ORTHOGONAL);
				M = M * zRot;
				break;
			}
			break;
		}
	}
}

/** This function sets up our window, OpenGL context, etc. For assignments, you don't need to know how it works. */
GLFWwindow* setupWindow()
{
	GLFWwindow *window;
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(700, 700, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	//gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glEnable(GL_DEPTH_TEST);

	return window;
}

/** This function sets up shaders on the graphics card. For assignments, you don't need to know how it works. */
GLuint setupShaderProgram()
{
	GLuint vertex_shader, fragment_shader, shader_program;
	int IsCompiled_VS, IsCompiled_FS, IsLinked, max_length;
	char *vertex_shader_log;
	char *fragment_shader_log;
	char *shader_program_log;

	/* Read our shaders into the appropriate buffers */
	char* vertex_source = filetobuf((char*)"OpenGL_Example.vert");
	char* fragment_source = filetobuf((char*)"OpenGL_Example.frag");

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_source, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &IsCompiled_VS);
	if (IsCompiled_VS == GL_FALSE)
	{
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

		/* The max_length includes the NULL character */
		vertex_shader_log = (char *)malloc(max_length);

		glGetShaderInfoLog(vertex_shader, max_length, &max_length, vertex_shader_log);
		printf("Error: %s", vertex_shader_log);
		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		free(vertex_shader_log);
		free(vertex_source);
		return 0;
	}

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &IsCompiled_FS);
	if (IsCompiled_FS == GL_FALSE)
	{
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

		/* The max_length includes the NULL character */
		fragment_shader_log = (char *)malloc(max_length);

		glGetShaderInfoLog(fragment_shader, max_length, &max_length, fragment_shader_log);
		printf("Error: %s", fragment_shader_log);
		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		free(fragment_shader_log);
		free(vertex_source);
		free(fragment_source);
		return 0;
	}

	/* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
	/* We must link them together to make a GL shader program */
	/* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
	/* Assign our program handle a "name" */
	shader_program = glCreateProgram();

	/* Attach our shaders to our program */
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	/* Link our program */
	/* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
	/* The binary code is uploaded to the GPU, if there is no error. */
	glLinkProgram(shader_program);

	/* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
	/* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
	/* too many texel fetch instructions or too many interpolators or dynamic loops. */

	glGetProgramiv(shader_program, GL_LINK_STATUS, (int *)&IsLinked);
	if (IsLinked == GL_FALSE)
	{
		/* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);

		/* The max_length includes the NULL character */
		shader_program_log = (char *)malloc(max_length);

		/* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
		glGetProgramInfoLog(shader_program, max_length, &max_length, shader_program_log);
		printf("Error: %s", shader_program_log);
		/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
		/* In this simple program, we'll just leave */
		free(shader_program_log);
		free(vertex_source);
		free(fragment_source);
		return 0;
	}
	//	glBindAttribLocation(shader_program, SHADER_POSITION_INDEX, "in_position");
	//	glBindAttribLocation(shader_program, SHADER_COLOR_INDEX, "in_color");

	free(vertex_source);
	free(fragment_source);

	return shader_program;

}

/**
This function gets attribute and uniform locations from shaders. For _this_ assignment (A2),
you don't need to know how it works. Do not modify the shader in any way for this assignment.
*/
void init(GLuint shader_program, GLuint &pos_loc_out, GLuint &color_loc_out, GLuint &m_loc_out)
{
	pos_loc_out = glGetAttribLocation(shader_program, "in_position");
	color_loc_out = glGetAttribLocation(shader_program, "in_color");
	m_loc_out = glGetUniformLocation(shader_program, "M");
}

/**
This function sets up a mesh with a crude diamond base and an arrowhead. You should change the vertex and index lists to generate sphere/cylinder geometry as described in
the assignment handout and in class.
*/
void setupDrawnObjects()
{

	const int numVerts = 25; // Number of verts in the final arrow

	// Base of the arrow shape
	GLfloat baseVerts[5][3] = {
		{0, 0, 0},
		{0, -.25f, 0},
		{.4f, -.25f, 0},
		{.4f, -.5f, 0},
		{.6f, 0, 0}
	};

	// ---------------- REVOLUTION ALGORITHM --------------------------
	const float theta = gmtl::Math::TWO_PI / 4.0; // We place a new vertex after this many radians of revolution
	float currentTheta; // Set in the loop
	int vertIndex = 0; // index counter for accessing vertex array
	GLfloat *basePoint; // A quick pointer to the base vertex we'll be basing the generated vertex off of

	for (int i = 0; i <= 4; i++) {
		for (int j = 0; j < 5; j++) {
			basePoint = baseVerts[j];
			currentTheta = theta * i;
			// Make the new point based off the base vertex
			arrowVertexBufferData.push_back(basePoint[0]); // x always stays the same, cause we're revolving around x axis
			arrowVertexBufferData.push_back(basePoint[1] * gmtl::Math::cos(currentTheta));
			arrowVertexBufferData.push_back(basePoint[1] * gmtl::Math::sin(currentTheta));
			// Give vertex a random color
			arrowColorBufferData.push_back((rand() % 255) / 255.0f);
			arrowColorBufferData.push_back((rand() % 255) / 255.0f);
			arrowColorBufferData.push_back((rand() % 255) / 255.0f);
			vertIndex++;
		}
	}


	// ----------------- INDEX LIST ALGORITHM --------------------------
	const int numIndices = 44; // Size of the index. Think of it as 2 * area of the vertex grid just created, + one row for primitive restarts
	arrowIndexBufferData.resize(numIndices);// Size of the index. Think of it as 2 * area of the vertex grid just created, + one row for primitive restarts

	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	int indexIndex = 0; // Which index of the index list we're modifying

	// Traverse through the nm * np grid. First we grab one vertex, then we grab the one on top of it, then move right and repeat til we end the row, then do primitive restart.
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 5; j++) {
			arrowIndexBufferData[indexIndex++] = i * 5 + j;
			arrowIndexBufferData[indexIndex++] = (i + 1) * 5 + j;
		}
		/* Every time GL reads our specified primitive-restart value from the index buffer it will start a new primitive. */
		/* This is nice, because this allows us to use one array-buffer and one element-buffer to draw separate GL_TRIANGLE_STRIPS */
		arrowIndexBufferData[indexIndex++] = 0xFFFF;
	}

	// This is a slightly modified cube.

	cubeVertexBufferData = 
	{
		-0.33f, 0.33f, -0.33f,
		-0.33f, 0.33f, 0.33f,
		-0.33f, -0.33f, -0.33f,
		-0.33f, -0.33f, 0.33f,
		0.33f, 0.33f, 0.33f,
		0.33f, -0.33f, 0.33f,
		0.33f, 0.33f, -0.33f,
		0.33f, -0.33f, -0.33f,
		-0.33f, 0.33f, -0.33f,
		0.33f, 0.33f, -0.33f,
		-0.33f, 0.33f, 0.33f,
		0.33f, 0.33f, 0.33f,
		-0.33f, -0.33f, -0.33f,
		0.33f, -0.33f, -0.33f,
		-0.33f, -0.33f, 0.33f,
		0.33f, -0.33f, 0.33f
	};


	cubeColorBufferData =
	{
		1.0f, 1.0f, 1.0f, 
		1.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f, 
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 
		1.0f, 0.0f, 1.0f, 
		1.0f, 1.0f, 0.0f, 
		1.0f, 0.0f, 1.0f, 
		0.0f, 0.0f, 1.0f, 
		1.0f, 0.5f, 0.5f, 
		0.5f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.50f,
		1.0f, 0.50f, 0.0f,
		1.0f, 0.50f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	// Indices for the triangle strips
	cubeIndexBufferData =
	{
		0, 1, 2, 3, // First triangle strip
		0xFFFF, // Primitive restart
		1, 4, 3, 5, // Second triangle strip
		0xFFFF, 
		4, 6, 5, 7, 
		0xFFFF, 
		0, 6, 2, 7, 
		0xFFFF, 
		8, 9, 10, 11, 
		0xFFFF, 
		12, 13, 14, 15
	};

}

/*
Call our function that performs opengl operations. This is where your changes for matrix and vertex
manipulation should be. 
*/
void drawSingleMesh(GLFWwindow* window, GLuint shader_program, GLuint m_location, GLuint VAO, gmtl::Matrix44f mat, vector<GLuint> &indexData)
{
	glUniformMatrix4fv(
		m_location,		// uniform location
		1,				// count
		GL_FALSE,		// transpose (no)
		mat.mData		// data
	);

	glBindVertexArray(VAO);

	// Draw the triangles 
	glDrawElements(
		GL_TRIANGLE_STRIP,		// mode
		indexData.size(),		// count, should be updated with your index list size.
		GL_UNSIGNED_INT,		// type
		(void*)0				// element array buffer offset
	);


}
// Add in note "Do not modify shader"
/* Our program's entry point */
int main(int argc, char *argv[])
{
	GLFWwindow* mainwindow = NULL;
	GLuint program = NULL, VAO = NULL;
	GLuint pos_location = NULL, color_location = NULL, m_location = NULL;

	// Make those global matrices identity
	gmtl::identity(xTrans);
	gmtl::identity(yRot);
	gmtl::identity(zRot);

	gmtl::identity(W_C);
	// Set up our cube's pose so that it is translated 0.4 units along negative x-axis.
	W_C[0][3] = -0.4;
	// Set this matrix's state for greater efficiency in gmtl
	W_C.setState(gmtl::Matrix44f::TRANS);

	/* This function sets up our window, OpenGL context, etc. For assignments, you don't need to know how it works. */
	mainwindow = setupWindow();

	/* This function sets up shaders on the graphics card. For assignments, you don't need to know how it works. */
	program = setupShaderProgram();

	/* This function gets attribute and uniform locations from shaders. For _this_ assignment, you don't need to know how it works. */
	init(program, vertex_position, vertex_color, m_location);

	/* This function sets up the vertices and attributes for our objects. You should change the vertex and index lists to generate sphere geometry as described in the assignment handout and in class.*/
	setupDrawnObjects();

	/* This function sets up a Vertex Array Object and buffers vertex and attribute data on the graphics card. Remember that a VAO is typically going to represent one "object" made of vertices and attributes.*/
	setupDrawObject(arrowVertexArrayID, arrowVertexBuffer, arrowVertexBufferData, arrowIndexBuffer, arrowIndexBufferData, arrowColorBuffer, arrowColorBufferData);
	/* Do the same for our cube*/
	setupDrawObject(cubeVertexArrayID, cubeVertexBuffer, cubeVertexBufferData, cubeIndexBuffer, cubeIndexBufferData, cubeColorBuffer, cubeColorBufferData);

	/* This is where we enable primitive restart; Note that we have only two draw objects but we're drawing multiple separate triangle strips.*/
	glEnable(GL_PRIMITIVE_RESTART);
	/* Primitive restart allows you to flag an index of a element buffer as "special". */
	glPrimitiveRestartIndex(0xFFFF);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(mainwindow))
	{
		// Call our function that performs opengl operations
		// This is where your changes for matrix and vertex manipulation should be. 
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Load the shader into the rendering pipeline */


		//One example of how to draw two meshes
		// One example of having same mesh and two poses
		glUseProgram(program);
		// Draw our arrow mesh.
		drawSingleMesh(mainwindow, program, m_location, arrowVertexArrayID, M, arrowIndexBufferData);
		// Draw our cube mesh.
		drawSingleMesh(mainwindow, program, m_location, cubeVertexArrayID, M * W_C, cubeIndexBufferData);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		/* Swap our buffers to make our changes visible */
		glfwSwapBuffers(mainwindow);
		glfwPollEvents();
	}
	/* Delete our opengl context, destroy our window, and shutdown SDL */
	glfwDestroyWindow(mainwindow);

	glUseProgram(NULL);

	glDisableVertexAttribArray(pos_location);
	glDisableVertexAttribArray(color_location);

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &VAO);

	return 0;
}