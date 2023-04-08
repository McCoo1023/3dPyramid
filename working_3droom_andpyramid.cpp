#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnOpengl/camera.h>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Camera and lighting
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLuint loadShaderFromMemory(const char* vertexSource, const char* fragmentSource) {
    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }



    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

GLfloat lightCubeVertices[] = {
    // Positions
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

GLfloat roomVertices[] = {
    // Positions          // Normals           // Texture Coords
    // Back face
    -10.0f, -10.0f, -10.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
    10.0f, -10.0f, -10.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
    10.0f,  10.0f, -10.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
    10.0f,  10.0f, -10.0f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
    -10.0f,  10.0f, -10.0f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

    // Front face
    -10.0f, -10.0f,  10.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,
    10.0f, -10.0f,  10.0f,  0.0f, 0.0f,  1.0f,  1.0f, 0.0f,
    10.0f,  10.0f,  10.0f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f,
    10.0f,  10.0f,  10.0f,  0.0f, 0.0f,  1.0f,  1.0f, 1.0f,
    -10.0f,  10.0f,  10.0f,  0.0f, 0.0f,  1.0f,  0.0f, 1.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,

    // Left face
    -10.0f,  10.0f,  10.0f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
    -10.0f,  10.0f, -10.0f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
    -10.0f, -10.0f, -10.0f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    -10.0f, -10.0f, -10.0f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    -10.0f, -10.0f,  10.0f, -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
    -10.0f,  10.0f,  10.0f, -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

    // Right face
    10.0f,  10.0f,  10.0f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,
    10.0f,  10.0f, -10.0f,  1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
    10.0f, -10.0f, -10.0f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    10.0f, -10.0f, -10.0f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
    10.0f, -10.0f,  10.0f,  1.0f, 0.0f,  0.0f,  0.0f, 0.0f,
    10.0f,  10.0f,  10.0f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

    // Bottom face
    -10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
    10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
    10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    // Top face
    -10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
    10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -10.0f,  10.0f,  10.0f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -10.0f,  10.0f, -10.0f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
};


GLfloat pyramidVertices[] = {
    // Positions          // Normals           // Texture Coords
    // Front face
     0.0f,   5.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f, 1.0f,
    -5.0f,  -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     5.0f,  -5.0f,  5.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,

     // Right face
      0.0f,   5.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f, 1.0f,
      5.0f,  -5.0f,  5.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
      5.0f,  -5.0f, -5.0f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

      // Back face
       0.0f,   5.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f, 1.0f,
       5.0f,  -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      -5.0f,  -5.0f, -5.0f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

      // Left face
       0.0f,   5.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.5f, 1.0f,
      -5.0f,  -5.0f, -5.0f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
      -5.0f,  -5.0f,  5.0f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f
};

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);

       // Vertex shader source
    const char* vertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoords;

        out vec3 FragPos;
        out vec3 Normal;
        out vec2 TexCoords;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main()
        {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoords = aTexCoords;

            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )glsl";

    // Fragment shader source
    const char* fragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec3 FragPos;
        in vec3 Normal;
        in vec2 TexCoords;

        uniform sampler2D texture_diffuse;

        uniform vec3 lightPos;
        uniform vec3 viewPos;

        void main()
        {
            vec3 color = texture(texture_diffuse, TexCoords).rgb;

            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            vec3 ambient = 0.2 * color;

            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * color;

            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = spec * lightColor;

            FragColor = vec4(ambient + diffuse + specular, 1.0);
        }
    )glsl";

    const char* lightCubeVertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

    const char* lightCubeFragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
    }
)";

    unsigned int lightCubeVertexShader, lightCubeFragmentShader;
    lightCubeVertexShader = glCreateShader(GL_VERTEX_SHADER);
    lightCubeFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(lightCubeVertexShader, 1, &lightCubeVertexShaderSource, NULL);
    glShaderSource(lightCubeFragmentShader, 1, &lightCubeFragmentShaderSource, NULL);

    glCompileShader(lightCubeVertexShader);
    glCompileShader(lightCubeFragmentShader);

    unsigned int lightCubeShaderProgram = glCreateProgram();
    glAttachShader(lightCubeShaderProgram, lightCubeVertexShader);
    glAttachShader(lightCubeShaderProgram, lightCubeFragmentShader);
    glLinkProgram(lightCubeShaderProgram);

    // Load and compile shaders, create shader programs
    GLuint shaderProgram = loadShaderFromMemory(vertexShaderSource, fragmentShaderSource);

    // Load and compile shaders, create shader programs
    GLuint lightShaderProgram = loadShaderFromMemory(vertexShaderSource, fragmentShaderSource);

    // Set up vertex data, buffers, and configure vertex attributes
    GLuint roomVAO, roomVBO;
    glGenVertexArrays(1, &roomVAO);
    glGenBuffers(1, &roomVBO);
    glBindVertexArray(roomVAO);
    glBindBuffer(GL_ARRAY_BUFFER, roomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roomVertices), roomVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    GLuint pyramidVAO, pyramidVBO;
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    unsigned int lightCubeVAO, lightCubeVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lightCubeVBO);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVertices), lightCubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Load textures
    GLuint pyramidTexture = loadTexture("C:\pyramid_texture.jpg");
    GLuint roomTexture = loadTexture("C:\cylinder_texture.jpg");

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up transformation matrices and camera
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        // Render room
        glUseProgram(shaderProgram);
        glBindVertexArray(roomVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, roomTexture);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render pyramid
        glBindVertexArray(pyramidVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pyramidTexture);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // Render light cubes
        glUseProgram(lightCubeShaderProgram);
        glBindVertexArray(lightCubeVAO);

        // Light cube 1: above the apex of the pyramid
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 8.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Light cube 2: 2 y positions above the first light cube, and 4 to the right
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(6.0f, 4.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Clean up resources
    glDeleteVertexArrays(1, &roomVAO);
    glDeleteBuffers(1, &roomVBO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(lightShaderProgram);

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
