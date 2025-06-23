#include <iostream>
#include <vector>

/// third Party libraries
#include <SDL.h>
#include <glad/glad.h>


/// Global Variables
constexpr auto SCREEN_WIDTH = 640;
constexpr auto SCREEN_HEIGHT = 480;

SDL_Window* window = nullptr;
SDL_GLContext glcontext = nullptr;

bool gQuit = false; // if true, we quit

// Vertex Array Object
GLuint VAO = 0;
// Bertex Buffer Object
GLuint VBO = 0;

// Program Object (for our shaders)
GLuint GraphicsPipelineShaderProgram = 0;

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
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL2 coud not initialize video subsystem" << std::endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("OPENGL Window", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "SDL Window could not be created" << std::endl;
		exit(1);
	}

	glcontext = SDL_GL_CreateContext(window);
	if (glcontext == nullptr)
	{
		std::cout << "OpenGL Context not available" << std::endl;
		exit(1);
	}

	// initialize the Glad Library
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		std::cout << "Glad was not initialized" << std::endl;
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
		-0.8f, -0.8f, 0.0f, // vertex 1
		0.8f, -0.8f, 0.0f,  // vertex 2
		0.0f, 0.8f, 0.0f    // vertex 3
	};

	// We start setting things up
	// on the GPU
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Start generating our Vertex Buffer Object
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

	SDL_Quit();
}



int main(int argc, char* argv[])
{
	InitializeProgram();

	VertexSpecification();

	CreateGraphicPipeline();

	MainLoop();

	CleanUp();

	return 0;
}