

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

static int g_windowWidth{ 800 };
static int g_windowHeight{ 600 };


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

    // Set the initial context size
    // Can be smaller than window size if we want to put stuff in the outer frames
    glViewport(0, 0, g_windowWidth, g_windowHeight);

    // Register a callback to handle window resizing
    // We can listen for other events like user input
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Create a render loop so the context stays open
    while (!glfwWindowShouldClose(window))
    {
        // handle user input
        processInput(window);

        // Do rendering
        // Typically you clear the previous frame to avoid artifacts
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Move the fully rendered frame from the back buffer to the front buffer
        // This keeps the frame out of sight until it's finished
        glfwSwapBuffers(window);

        // Check for other events
        glfwPollEvents();
    }

    // Exit normally
    glfwTerminate();
    return 0;
}


