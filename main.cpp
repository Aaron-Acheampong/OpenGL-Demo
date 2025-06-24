// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>

/* Compilation on Linux
* g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -lSDL2 -ldl
*/

/// Third Party Libraries
#include <SDL.h>
#include <glad/glad.h>


/* ********** Global Variables ******* */

// Screen Dimensions
constexpr auto SCREEN_WIDTH  = 640;
constexpr auto SCREEN_HEIGHT = 480;

SDL_Window* window           = nullptr;
SDL_GLContext glcontext      = nullptr;

// Main Loop Flag
bool gQuit  = false; // if true, we quit

/* ** SHADER **** The following store the unique id for the graphics pipeline*******   */
GLuint VAO                            = 0;  // Vertex Array Object (VAO) encapsulates all items needed to render an object.
GLuint VBO                            = 0;  // Vertex Buffer Object (VBO) stores information relating to vertices (e.g. positions, normals, textures). VBOs are a mechanism for arranging geometry on the GPU.
GLuint GraphicsPipelineShaderProgram  = 0;  // Program Object (for our shaders) that will be used for our OpenGL draw calls.



// Vertex Shader
// Vertex Shader executes once per vertex, and will be in charge of
// the finall position of the vertex.
const std::string VertexShaderSource =
"#version 410 core\n"
"in vec4 position;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
"}\n";

// Fragment Shader
// Fragment Shader executes once per fragment (i.e. roughly for every pixel that will be rasterized),
// and in part determines the final color that will be sent to the screen.
const std::string FragmentShaderSource =
"#version 410 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"	color = vec4(1.0f, 0.5f, 0.0f, 1.0f);\n"
"}\n";

void GetOpenGLVersionInfo()
{
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}



void InitializeProgram()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL2 coud not initialize video subsystem" << std::endl;
		exit(1);
	}

	// Setup OpenGL Context
	// Use OpenGL 4.1 core or greater
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Request a double buffer for smooth updating.
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// Create an application window using OpenGL that supports SDL
	window = SDL_CreateWindow("OpenGL Program Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr) // check if window was created
	{
		std::cout << "SDL Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	// Create OpenGL Craphics Context
	glcontext = SDL_GL_CreateContext(window);
	if (glcontext == nullptr)
	{
		std::cout << "OpenGL Context could not be created! SDL Error : " << SDL_GetError() << std::endl;
		exit(1);
	}

	// Initialize the GLAD Library
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cout << "GLAD did not initialized" << std::endl;
		exit(1);
	}

	GetOpenGLVersionInfo();

}


void VertexSpecification()
{
	// Lives on the CPU
	const std::vector<GLfloat> vertexPosition
	{
		// x     y     z
		-0.8f, -0.8f, 0.0f, // Left vertex position
		0.8f, -0.8f, 0.0f,  // Right vertex position
		0.0f, 0.8f, 0.0f    // Top vertex position
	};

	// We start setting things up on the GPU
	// 
	// Vertex Arrays Object (VAO) Setup
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertex Buffer Object (VBO) creation
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), vertexPosition.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
}

GLuint CompileShader(GLuint type, const std::string& source)
{
	GLuint shaderObject = 0;;

	if (type == GL_VERTEX_SHADER)
	{
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == GL_FRAGMENT_SHADER)
	{
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	return shaderObject;
}


GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource)
{
	GLuint programObject = glCreateProgram();

	GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
	GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

	glAttachShader(programObject, myVertexShader);
	glAttachShader(programObject, myFragmentShader);
	glLinkProgram(programObject);

	// Validate our program
	glValidateProgram(programObject);
	// glDetachShader, glDeleteShader

	return programObject;
}


void CreateGraphicPipeline()
{
	GraphicsPipelineShaderProgram = CreateShaderProgram(VertexShaderSource, FragmentShaderSource);
}



void Input()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			std::cout << "Goodbye!" << std::endl;
			gQuit = true;
		}
	}
}

// Predraw function
void Update()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glClearColor(1.f, 1.f, 0.f, 1.f);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(GraphicsPipelineShaderProgram);
}


void Draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}


void MainLoop()
{
	while (!gQuit)
	{
		Input();

		Update();

		Draw();

		// Update Screen
		SDL_GL_SwapWindow(window);
	}
}

void CleanUp()
{
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();
}

/**
* The main entry point into our C++ programs.
* 
* @return program status
*/
int main(int argc, char* argv[])
{
	// 1. Setup the graphics program
	InitializeProgram();

	// 2. Setup our geometry
	VertexSpecification();

	// 3. Create our graphics pipeline
	// - At a minimum, this means the vertexand fragment shader
	CreateGraphicPipeline();

	// 4. Call the main application loop
	MainLoop();

	// 5. Call the Cleanup function when our program terminates
	CleanUp();

	return 0;
}