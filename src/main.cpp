

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static const int g_windowWidth{ 800 };
static const int g_windowHeight{ 600 };

// Check if the shader compilation was successful
void checkShaderCompilation(unsigned int vertexShader) { 
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// Check if the program compilation was successful
void checkProgramCompilation(unsigned int shaderProgram) { 
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// Input is a 3D vector specified via "in" variable
// Output is a 4D vector that we assign to built-in variable gl_Position
static const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Output is a 4D vector that determines the final color of the pixel
// The format is RGBA (Red, Green, Blue, Alpha) with values between 0 and 1
static const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";


unsigned int createShaderProgram() {
        // Create a vertex shader object
    // Note how we store the ID of the shader in an unsigned int (all OpenGL objects have unique IDs)
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader);

    // Create a fragment shader object
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader);

    // Link the shaders into a shader program that will run them
    // sequentially in the rendering pipeline (will get error for input-output mismatch)
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramCompilation(shaderProgram);

    // These shaders are no longer needed after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


// OpenGL converts 3D coordinates into 2D screen-space coordinates with various attributes like color, alpha, etc.
// Rendering pipeline is a series of sequential steps where output from one step becomes input for the next
// Important phases are vertex shader and fragment shader (applying color to pixels given lighting, shadows, etc.)
void configureWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


int main() {
    configureWindow();

    GLFWwindow* window = glfwCreateWindow(g_windowWidth, g_windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << '\n';
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // Must go before any OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    unsigned int shaderProgram = createShaderProgram();

    // Note that real data isn't normalized like this, so you have to do it yourself
    // in order for vertices to fit within the visible region of the window (-1.0 to 1.0)
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
       -0.5f,  0.5f, 0.0f   // top left 
   };
   unsigned int indices[] = {
       0, 1, 3,  // first Triangle
       1, 2, 3   // second Triangle
   };

    // Vertex Buffer Object (VBO) represents the GPU memory we're storing vertices in
    // Element Buffer Object (EBO) stores references to vertices so we can reuse them
    // Vertex Array Object (VAO) stores the configuration of vertex attributes
    // In other words, VBO passes the data and VAO tells OpenGL how to interpret it
    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    // Bind VAO first so all subsequent VBOs, EBOs, and vertex configs are stored in it
    // This makes it easy to reuse the configuration to draw multiple objects
    glBindVertexArray(VAO);

    // Bind VBO to the pre-defined object GL_ARRAY_BUFFER   
    // GL_STATIC_DRAW: the data will not change (since our triangle stays in same spot)
    // GL_DYNAMIC_DRAW: the data will change frequently (GPU should put in memory that allows faster writes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Store indices referencing the vertices we stored in VBO
    // These are like pointers that cut down on the amount of data we need to send to the GPU 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Tell OpenGL how to interpret the vertex data (this is associated with VAO while bound)
    // Relevant here is the type, vertex size (3 float), stride (space b/t consecutive vertices), and offset (where the vertices start in input list)
    // The first zero corresponds to the "location" we set in the vertex shader, a hint that we're defining vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    // glVertexAttribPointer() registered VBO as the vertex attribute's input object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // do NOT unbind the EBO while a VAO is active as the bound object IS stored in the VAO
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Set the initial context size
    // Can be smaller than window size if we want to put stuff in the outer frames
    glViewport(0, 0, g_windowWidth, g_windowHeight);

    // Register a callback to handle window resizing
    // We can listen for other events like user input
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // uncomment this to draw in wireframe mode and see how triangles are combined into other shapes
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 

    // Create a render loop so the context stays open
    while (!glfwWindowShouldClose(window))
    {
        // handle user input
        processInput(window);

        // Do rendering
        // Typically you clear the previous frame to avoid artifacts
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // affects all subsequent draw calls
        glUseProgram(shaderProgram);

        // only have a single VAO there's no need to bind it every time; just for clarity
        glBindVertexArray(VAO);

        // Draws primitive using the current active shader
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Move the fully rendered frame from the back buffer to the front buffer
        // This keeps the frame out of sight until it's finished
        glfwSwapBuffers(window);

        // Check for other events
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Exit normally
    glfwTerminate();
    return 0;
}
