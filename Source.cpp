/*	Author:  Nicholas Renna
    CS330 SNHU comp graphics and visualization
    Instructor: Kurt Diesch  */

#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
//Image loading library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



 // GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//includes for cylinder/sphere creation header files 
#include "cylinder.h"
#include "sphere.h"

#include <string>
#include <sstream>

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "3D Scene Project--Nicholas Renna"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 1200; //1200
    const int WINDOW_HEIGHT = 900; //900


    //Scale, rotation and translation 
    glm::mat4 scale;
    glm::mat4 rotation;
    glm::mat4 translation;
    //model, view and projection matrices 
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    //perspective and ortho
    glm::mat4 perspective;
    glm::mat4 ortho;

    //setting it true, for the switch between ortho and perspective
    bool isPerspective = true;




    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Shader programs
    GLuint gProgramId;
    GLuint gLampProgramId;
    GLuint gFillProgramId;
    GLuint gSpotProgramId;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbos[2];     // Handles for the vertex buffer objects
        GLuint nIndices;    // Number of indices of the mesh
        GLuint nVertices;   // Number of vertices of the mesh
    };
    //stores coordinates for points 
    struct GLCoord {
        GLfloat x;
        GLfloat y;
        GLfloat z;
    };
    // Triangle mesh data
    // Floor mesh
    GLMesh floorMesh;
    //GLMesh gMesh;
    GLMesh pyramidMesh;
    //CUBE mesh data
    GLMesh cubeMesh;
    //CUBE DOOR mesh dat
    GLMesh cubedoorMesh;
    //Cube book
    GLMesh bookMesh;
    

    //Camera variables
    glm::vec3 gCameraPos = glm::vec3(0.0f, -0.2f, 2.5f);
    glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f); 
    glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    //camera
    float yaw = -90.0f; // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float gLastX = 800.f / 2.0;
    float gLastY = 600.0 / 2.0;
    float fov = 45.0f;
    float sensitivity = 0.1f; //the scoll on mouse will control this
    bool gFirstMouse = true;

    //time variables for camera movement
    float gDeltaTime = 0.0f; // Time between current frame and last frame
    float gLastFrame = 0.0f;

    //Texture variables 
    GLuint deskTextureId, pyramidTextureId, cubeTextureId, doorTextureId, bookTextureId, flashlightTextureId, basketballTextureId;
    
    // Texture Id and Uv scales, GL_REPEAT
    GLuint gTextureId;
    glm::vec2 gUVScale(1.0f, 1.0f);
    //GLint gTexWrapMode = GL_REPEAT;

    //object and light color
    glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f);
    
    //First light-on top of FLASHLIGHT
    //--------------------------------
    //LIGHT position and scale, Color 
    glm::vec3 gLightPosition(-0.29f, -0.239f, 0.0f);
    glm::vec3 gLightScale(0.06f);
    glm::vec3 gLightColor(1.0f, 0.57f, 0.16f); //Candle (255, 147, 41), usign RGB values
    
    //Second Light-top of scene LAMP Light
    //--------------------------------
    // Fill position, scale, Color 
    glm::vec3 gFillPosition;
    //This creates two light locations 
    glm::vec3 pointFillPosition[] = {
        glm::vec3(3.0f, 2.0f, -1.0f),
        glm::vec3(-2.0f, 3.0f, -1.0f),
    };
    glm::vec3 gFillScale(0.1f);
    glm::vec3 gFillColor(1.0f, 1.0f, 1.0f);  //White
    
    //Third Light-in front of scene
    //--------------------------------
    //SPOT Position, scale and Color 
    glm::vec3 gSpotPosition(1.5f, -0.2f, 2.0f); //1.0 ,-0.2, 2.0
    glm::vec3 gSpotScale(0.1f);
    glm::vec3 gSpotColor(1.0f, 1.0f, 1.0f); // OVerCast Sky .78, .88, 1.0
    
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);

//Initialize mouse movement callbacks
void UMousePostionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

//Initialize function to create objects
void UCubeMesh(GLMesh& mesh); //Cube function
void UCubeDoorMesh(GLMesh& mesh); //Cubes door way
void UCreatePlaneMesh(GLMesh& mesh, GLCoord topRight, GLCoord topLeft, GLCoord bottomLeft, GLCoord bottomRight); //floor function
void UCubeBookMesh(GLMesh& mesh); //Cube book 
void UPyramidMesh(GLMesh& mesh); //Pyramid function

//Initializw inputs, destroy mesh, U render and create, destroy, Texture shader programs
void UProcessInput(GLFWwindow* window);
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void UDestroyTexture(GLuint textureId);

//Initialize function for texture loading
bool UCreateTexture(const char* filename, GLuint& textureId);




/* 3D Scene Vertex Shader Source Code*/
const GLchar* scenevertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
layout(location = 1) in vec3 normal;  // Color data from Vertex Attrib Pointer 1
layout(location = 2) in vec2 textureCoordinate; //texture data from Vertex Attribute Pointer 2

out vec3 vertexNormal; // variable to transfer color data to the fragment shader/normals to fragment shader
out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
out vec2 vertexTextureCoordinate; //variable to transfer texture coordinates to fragment shader

//Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)
    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate; //References incoming texture data
}
);


/* 3D scene Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // Variable to hold incoming color data from vertex shader/ incoming normals 
    in vec3 vertexFragmentPos; //for incoming fragment positions 
    in vec2 vertexTextureCoordinate; //variable to hold incoming texture data from the vertex shader

out vec4 fragmentColor;
out vec4 fillfragmentColor;

uniform vec3 objectColor;
uniform vec3 lightColor; //First light
uniform vec3 lightPos;
uniform vec3 fillColor; //Second light
uniform vec3 fillPos;
uniform vec3 spotColor; //third light
uniform vec3 spotPos;
uniform vec3 viewPosition;
uniform sampler2D uTexture;
uniform vec2 uvScale;

void main()
{
    //Will create the Phong lighting type for the 3D scene
    //KEY LIGHT / strength = .10 percent
    //Calculate Ambient lighting
    float ambientStrength = 0.01f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on object
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting
    float specularIntensity = 0.01f; // Set specular light strength
    float highlightSize = 32.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------
    //SPOT LIGHT Strength = 30 percent
    //The front spot light creates a diffuse lighting source to leave out the bright spots 
    float spotAmbientStrength = 0.3f; //Setting another ambient global lighting strength to add another light 
    vec3 spotAmbient = spotAmbientStrength * spotColor;
    //Calculate Diffuse lighting
    vec3 spotDirection = normalize(spotPos - vertexFragmentPos);
    float spotImpact = max(dot(norm, spotDirection), 0.0);
    vec3 spotDiffuse = spotImpact * spotColor;

    //Calculate Specular lighting
    float spotSpecularIntensity = 0.3f; // Set specular light strength
    float spotHighlightSize = 32.0f; // Set specular highlight size
    vec3 spotViewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 spotReflectDir = reflect(-spotDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float spotSpecularComponent = pow(max(dot(spotViewDir, spotReflectDir), 0.0), spotHighlightSize);
    vec3 spotSpecular = spotSpecularIntensity * spotSpecularComponent * spotColor;

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------
    //FILL LIGHT Strength= 50 percent
    // Calculate AmbientLighting 
    float fillAmbientStrength = 0.5f; // Set ambient or global lighting strength
    vec3 fillAmbient = fillAmbientStrength * fillColor; // Generate ambient light color

    //Calculate Diffuse lighting
    vec3 fillDirection = normalize(fillPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float fillImpact = max(dot(norm, fillDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 fillDiffuse = fillImpact * fillColor; // Generate diffuse light color

    //Calculate Specular lighting
    float fillSpecularIntensity = 0.5f; // Set specular light strength
    float fillHighlightSize = 32.0f; // Set specular highlight size
    vec3 fillViewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 fillReflectDir = reflect(-fillDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float fillSpecularComponent = pow(max(dot(fillViewDir, fillReflectDir), 0.0), fillHighlightSize);
    vec3 fillSpecular = fillSpecularIntensity * fillSpecularComponent * fillColor;
    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Texture holds the color to be used for all three components
    vec3 objectColor = texture(uTexture, vertexTextureCoordinate).xyz;
    vec3 keyResult = (ambient + diffuse + specular) * objectColor;
    vec3 fillResult = (fillAmbient + fillDiffuse + fillSpecular);
    vec3 spotResult = (spotAmbient + spotDiffuse + spotSpecular);
    vec3 lightingResult = keyResult + fillResult + spotResult;
    vec3 phong = (lightingResult)*objectColor;

    fragmentColor = vec4(phong, 1.0);
}
);

/* Lamp Vertex Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);

/* Lamp Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);

/* Fill Shader Source Code*/
const GLchar* fillVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);

/* Fill Fragment Shader Source Code*/
const GLchar* fillFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to soft yellow (1.0f,1.0f,0.6f) with alpha 1.0
}
);

/* Spot Shader Source Code*/
const GLchar* spotVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);

/* spot Fragment Shader Source Code*/
const GLchar* spotFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f); // Set color to soft yellow (1.0f,1.0f,0.6f) with alpha 1.0
}
);

// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}

int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    //Create mesh objects
    // Coordinates for plane
    struct GLCoord topLeft = { -4.0f, -1.0f, -4.0f };
    struct GLCoord topRight = { 4.0f, -1.0f, -4.0f };
    struct GLCoord bottomLeft = { -4.0f, -1.0f, 4.0f };
    struct GLCoord bottomRight = { 4.0f, -1.0f, 4.0f };
    //Plane
    UCreatePlaneMesh(floorMesh, topRight, topLeft, bottomLeft, bottomRight); //calls the function to create the vertex buffer object
    // Create the mesh for the PYRAMID/ROOF
    UPyramidMesh(pyramidMesh); // Calls the function to create the Vertex Buffer Object
    //Create the mesh for the CUBE/HOUSE
    UCubeMesh(cubeMesh); // Calls the function to create the Vertex Buffer Object
    //mesh for the Cube face DOOR/HOUSE
    UCubeDoorMesh(cubedoorMesh); //calls the function for the door on the cube
    //Mesh for large BOOK 
    UCubeBookMesh(bookMesh); //calls the function for the book

    
    //Shader source creation:
    // Create the shader program
    //Main shader, texture gProgramId
    if (!UCreateShaderProgram(scenevertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;
    //Lamp Light
    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
        return EXIT_FAILURE;
    //Fill Light
    if (!UCreateShaderProgram(fillVertexShaderSource, fillFragmentShaderSource, gFillProgramId))
        return EXIT_FAILURE;
    // Spot Light
    if (!UCreateShaderProgram(spotVertexShaderSource, spotFragmentShaderSource, gSpotProgramId))
        return EXIT_FAILURE;

    //generate textures/load them
    const char* texFilename = "../resources/textures/desk1.jpg";
    if (!UCreateTexture(texFilename, deskTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/roof4.jpg";
    if (!UCreateTexture(texFilename, pyramidTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/house2.jpg";
    if (!UCreateTexture(texFilename, cubeTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/door4.jpg";
    if (!UCreateTexture(texFilename, doorTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/book1.jpg";
    if (!UCreateTexture(texFilename, bookTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/flashlight.jpg";
    if (!UCreateTexture(texFilename, flashlightTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    texFilename = "../resources/textures/basketball.jpg";
    if (!UCreateTexture(texFilename, basketballTextureId))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    //assign textures to shader
    glUseProgram(gProgramId);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);
    //glUniform1i(glGetUniformLocation(gProgramId, "doorTexture"), 1);


    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {

        //will create a perspective projection, 45 degree angle
        //set perspective and ortho projections, can swtich between the two use P for Perspective and O for ortho
        perspective = glm::perspective(glm::radians(45.0f), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        ortho = glm::ortho(-2.0f, +2.0f, -2.0f, +2.0f, 0.1f, 100.0f);
        // per-frame timing
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        ////Camera-view transformation
        view = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);
        //// input
        // -----
        UProcessInput(gWindow);
        //if statement to check for perspective or ortho
        if (isPerspective)
            projection = perspective;
        else
            projection = ortho;
        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(pyramidMesh);
    UDestroyMesh(cubeMesh);
    UDestroyMesh(cubedoorMesh);
    UDestroyMesh(floorMesh);
    UDestroyMesh(bookMesh);

    // Release shader program
    UDestroyShaderProgram(gProgramId);
    UDestroyShaderProgram(gLampProgramId);
    UDestroyShaderProgram(gFillProgramId);
    UDestroyShaderProgram(gSpotProgramId);

    //Release texture
    UDestroyTexture(gTextureId);


    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    //Tell GLFW to use mouse movement
    glfwSetCursorPosCallback(*window, UMousePostionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);


    //Tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}
//glfw: Whenever the window size changed (by OS or user resize) this called function executes 
//-------------------------------------------------------------------------------------------
void UMousePostionCallback(GLFWwindow* window, double xpos, double ypos) {
    if (gFirstMouse) {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top
    gLastX = xpos;
    gLastY = ypos;

    //float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    //calculate the actual dirctions (front) using this formula from learnopengl.com
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    gCameraFront = glm::normalize(front);

}
//glfw: Whenever the mouse scroll wheel scrolls, this callback is called
//------------------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    //Idea uused from learnopengl.com camera section
    //scroll down will slow camera down, scroll up will speed the camera 
    sensitivity += 0.1f * (float)yoffset;
    //parmeters for sensitivity
    if (sensitivity > 1.0f)
        sensitivity = 1.0f;
    if (sensitivity < 0.01f)
        sensitivity = 0.01;
}

//glfw: When the mouse buttons are pressed this callback is called
// -----------------------------------------------------------------------------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    //static const float cameraSpeed = 2.5f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * gDeltaTime);
    //camera forward, backward, left, and camera right
    // W= Forward
    // S= Backwards
    // A= Left
    // D= Right
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCameraPos += cameraSpeed * gCameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCameraPos -= cameraSpeed * gCameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
    //camera up and camera down
    // Q= Upward
    // E= Downward
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCameraPos += gCameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCameraPos -= gCameraUp * cameraSpeed;
    //perspective view and otho view
    // P= Perspetive view-3D
    // O= Ortho view-2D    
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        isPerspective = true;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        isPerspective = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Functioned called to render a frame
void URender()
{
    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.50f, 0.50f, 0.50f, 1.0f); //Creates a grey window that matches the 2D image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the shader to be used
    glUseProgram(gProgramId);

    //Retrieves and pass transform matrices to the shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Reference matrix uniforms from the Cube Shader program for the cub color
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    //Light color and position
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    //Fill color and position 
    GLint fillColorLoc = glGetUniformLocation(gProgramId, "fillColor");
    GLint fillPositionLoc = glGetUniformLocation(gProgramId, "fillPos");
    //Spot color and Position
    GLint spotColorLoc = glGetUniformLocation(gProgramId, "spotColor");
    GLint spotPositionLoc = glGetUniformLocation(gProgramId, "spotPos");
    //Camera position
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");

    // Pass color, light, and camera data to the Object Shader program's corresponding uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    //uniform for the light colro and position
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);
    //Uniforms for fill color and position 
    glUniform3f(fillColorLoc, gFillColor.r, gFillColor.g, gFillColor.b);
    glUniform3f(fillPositionLoc, gFillPosition.x, gFillPosition.y, gFillPosition.z);
    //Uniforms for spot color and positions 
    glUniform3f(spotColorLoc, gSpotColor.r, gSpotColor.g, gSpotColor.b);
    glUniform3f(spotPositionLoc, gSpotPosition.x, gSpotPosition.y, gSpotPosition.z);
    //Camera data
    const glm::vec3 cameraPosition = gCameraPos;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));
    //-----------------------------------------------------------------------------------------------------------------------
    //For the FLOOR/DESK
    //-----------------------------------------------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, deskTextureId);
    glBindVertexArray(floorMesh.vao);

    // 1. Scales the object by 0.3
    scale = glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));
    // 2. Rotates shape by 20 degrees in the x axis at unit 1
    rotation = glm::rotate(glm::radians(00.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //Rotates shape 0 degress in the -y axis
    rotation = glm::rotate(glm::radians(00.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    // 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(0.0f, -0.5f, -0.8f));
    //Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //Draws the floor
    glDrawElements(GL_TRIANGLES, floorMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    //-----------------------------------------------------------------------------------------------------------------------
    //For the PYRAMID 
    //-----------------------------------------------------------------------------------------------------------------------
    // Activate the VBOs contained within the mesh's VAO
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pyramidTextureId);
    glBindVertexArray(pyramidMesh.vao);

    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.453f, 0.431f, 0.45f));
    // 2. Rotates shape by 20 degrees in the x axis at unit 1
    rotation = glm::rotate(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //Rotates shape 0 degress in the -y axis
    rotation = glm::rotate(glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(-0.75f, -0.1571f, -0.275f));
    //Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, pyramidMesh.nVertices);
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
    //-----------------------------------------------------------------------------------------------------------------------
    //For the Cubes DOOR
    //-----------------------------------------------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, doorTextureId);
    glBindVertexArray(cubedoorMesh.vao);

    // 1. Scales the object 
    scale = glm::scale(glm::vec3(0.453f, 0.420f, 0.42f));
    // 2. Rotates shape by 20 degrees in the x axis at unit 1
    //Rotates shape 0 degress in the -y axis
    rotation = glm::rotate(glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(-0.744f, -0.583f, -0.260f));
    //Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //Draws the cubes door, Image in front of a image
    glDrawElements(GL_TRIANGLES, cubedoorMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
    
    glBindVertexArray(0);
    //-----------------------------------------------------------------------------------------------------------------------
    //For CUBE of the small HOUSE
    //-----------------------------------------------------------------------------------------------------------------------
    //Activate the VBOs contained within the mesh's VAO
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTextureId);
    glBindVertexArray(cubeMesh.vao);

    // 1. Scales the object 
    scale = glm::scale(glm::vec3(0.453f, 0.452f, 0.42f));
    // 2. Rotates shape two times 
    //Rotates shape 180 degress in the y and z axis to flip the house over to match the bricks 
    //Then rotate again for scene match
    rotation = glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
    rotation = glm::rotate(rotation, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //rotation = glm::rotate(rotation, glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(-0.75f, -0.583f, -0.275f));//-0.57
    //Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //Draws the triangles for the Cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
    //Deactivates the Vertex Array Object
    glBindVertexArray(0);
    //-----------------------------------------------------------------------------------------------------------------------
    //FOR Cube BOOK
    //-----------------------------------------------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bookTextureId);
    glBindVertexArray(bookMesh.vao);

    // 1. Scales the object
    scale = glm::scale(glm::vec3(0.35f, 0.35f, 0.35f));
    // 2. Rotates shape by 20 degrees in the x axis at unit 1
    rotation = glm::rotate(glm::radians(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    //Rotates shape 0 degress in the -y axis
    rotation = glm::rotate(glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(0.0f, -0.447f, -0.8f));
    //Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    //Draws the book with custom texture coordinates 
    glDrawElements(GL_TRIANGLES, bookMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    
    //-----------------------------------------------------------------------------------------------------------------------
    //For CYLINDER/Flashlight
    //-----------------------------------------------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, flashlightTextureId);
    
    //// 1. Scales the object by 0.06
    scale = glm::scale(glm::vec3(0.06f, 0.06f, 0.06f));
    //// 2. Rotates shape by 20 degrees in the x axis at unit 1
    rotation = glm::rotate(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //// 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(-0.29f, -0.525f, 0.0f));
    ////Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glColor3f(0.0f, 0.0f, 0.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // will call the cylinder to render the flashlight                          
    static_meshes_3D::Cylinder C(1, 30, 9, true, true, true); //radius/num of slices/height 1, 20, 7
    C.render();
    glBindVertexArray(0);
    // ----------------------------------------------------------------------------------------------------------------------
    // For SPHERE/Basketball
    // ----------------------------------------------------------------------------------------------------------------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, basketballTextureId);
    
    //// 1. Scales the object by 0.06
    scale = glm::scale(glm::vec3(0.06f, 0.06f, 0.06f));
    //// 2. Rotates shape by 20 degrees in the x axis at unit 1
    rotation = glm::rotate(glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //// 3. Place object at the origin, starting at -0.5 y axis and -0.8 on the z axis
    translation = glm::translate(glm::vec3(0.8f, -0.38f, 0.0f));
    ////Model matrix: transformations are applied right-to-left-order
    model = translation * rotation * scale;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // will call the sphere to create a basketball object                       
    static_meshes_3D::Sphere S(7, 20, 20, true, true, true); //radius/num of slices/stacks 7 20 20 
    S.render();
    glBindVertexArray(0);
    //-----------------------------------------------------------------------------------------------------------------------
    //LAMP: Lamp LIGHT for FlashLight 
    //------------------------------------------------------------------------------------------------------------------------
    //Calling new shader program for LAMPS 
    glUseProgram(gLampProgramId);

    //Transform the smaller cube used as a visual que for the light source can position with ambient position and scale
    model = glm::translate(gLightPosition) * glm::scale(gLightScale);
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gLampProgramId, "model");
    viewLoc = glGetUniformLocation(gLampProgramId, "view");
    projLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //Draws the small light cylinder to Replicate a flashlight top
    static_meshes_3D::Cylinder C1(0.75, 20, 0.5, true, true, true); //radius/num of slices/height 
    C1.render();
    //------------------------------------------------------------------------------------------------------------------------
    //LAMP: Draw FILL, This will draw two light to light the scene better
    //------------------------------------------------------------------------------------------------------------------------
    //Calling new shader program for the FILL light
    glUseProgram(gFillProgramId);
    gFillPosition = pointFillPosition[0];
    model = glm::translate(gFillPosition) * glm::scale(gFillScale);
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gFillProgramId, "model");
    viewLoc = glGetUniformLocation(gFillProgramId, "view");
    projLoc = glGetUniformLocation(gFillProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    //Draws the small sphere to act as a moon or sun 
    static_meshes_3D::Sphere S1(1, 20, 20, true, true, true); //radius/num of slices/stacks 7 20 20 
    S1.render();
    glBindVertexArray(0);
    //Second -TOP light
    //------------------------------------------------------------------------------------------------------------------------
    gFillPosition = pointFillPosition[1];
    model = glm::translate(gFillPosition) * glm::scale(gFillScale);
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gFillProgramId, "model");
    viewLoc = glGetUniformLocation(gFillProgramId, "view");
    projLoc = glGetUniformLocation(gFillProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //Draws the small sphere to act as a moon or sun 
    static_meshes_3D::Sphere S2(1, 20, 20, true, true, true); //radius/num of slices/stacks 7 20 20 
    S2.render();
    glBindVertexArray(0);
    //-------------------------------------------------------------------------------------------------------------------------
    //LAMP: Draw SPOT with direction towards the 3D scene
    //-------------------------------------------------------------------------------------------------------------------------
    //Calling new shader program for the spot light 
    glUseProgram(gSpotProgramId);
    glBindVertexArray(cubedoorMesh.vao);

    model = glm::translate(gSpotPosition) * glm::scale(gSpotScale);
    // Reference matrix uniforms from the Lamp Shader program
    modelLoc = glGetUniformLocation(gSpotProgramId, "model");
    viewLoc = glGetUniformLocation(gSpotProgramId, "view");
    projLoc = glGetUniformLocation(gSpotProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    //Draws the small cube
    //Reusing a function for more organization 
    glDrawElements(GL_TRIANGLES, cubedoorMesh.nIndices, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    glUseProgram(0);
    //---------------------------------------------------------------------------------------------------------------------------------
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
}

//Implements the UCreateMesh function for the plane-----------------------------------DESK-------------------------------------------
void UCreatePlaneMesh(GLMesh& mesh, GLCoord topRight, GLCoord topLeft, GLCoord bottomLeft, GLCoord bottomRight) {
    GLfloat verts[] = {
        //x, y, z                                   //rgb                       //texture
        topLeft.x, topLeft.y, topLeft.z,              0.51f, 0.38f, 0.25f,      0.0f, 1.0f,    //top left
        topRight.x, topRight.y, topRight.z,           0.51f, 0.38f, 0.25f,      1.0f, 1.0f,   //top right
        bottomRight.x, bottomRight.y,  bottomRight.z, 0.51f, 0.38f, 0.25f,      1.0f, 0.0f,    //bottom right
        bottomLeft.x, bottomLeft.y,  bottomLeft.z,    0.51f, 0.38f, 0.25f,      0.0f, 0.0f    //bottom left
    };

    // Index data to share position data
    GLushort indices[] = {
        0, 1, 3,  //first triangle
        1, 2, 3   //second triangle

    };
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 3;
    const GLuint floatsPerUV = 2;

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(indices) / sizeof(indices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, s, t). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers for vertex, color and texture
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    //Texture
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

}

// --Implements the UCreateMesh function for the pyramid ROOF----------------------------------------------------------- 
void UPyramidMesh(GLMesh& mesh)
{
    // Position (x, y, z) and Color data (r, g, b)
    //for the roof of house or pyramid top of house (roof)
    GLfloat verts[] = {
        // Vertex Positions     //Normal vector             //Texture
        // Pyramid sides
        // Triangle 1
        0.0f,  0.5f, 0.0f,      0.0f, 0.45f, -0.89f,        0.5f, 1.0f,        // Top Vertex 0
       -0.5f, -0.5f, -0.5f,    0.0f, 0.45f, -0.89f,        0.0f, 0.0f,        // Front Bottom Left Vertex 1
        0.5f, -0.5f, -0.5f,     0.0f, 0.45f, -0.89f,        1.0f, 0.0f,        // Front Bottom Right Vertex 2
        // Triangle 2
         0.0f,  0.5f, 0.0f,      0.89f, 0.45f, 0.0f,         0.5f, 1.0f,         // Top Vertex 0
         0.5f, -0.5f, -0.5f,     0.89f, 0.45f, 0.0f,         1.0f, 0.0f,         // Front Bottom Right Vertex 2 
         0.5f, -0.5f, 0.5f,      0.89f, 0.45f, 0.0f,         0.0f, 0.0f,        // Rear Bottom Right Vertex 3
        // Triangle 3       
         0.0f,  0.5f, 0.0f,      0.0f, 0.45f, 0.89f,         0.5f, 1.0f,        // Top Vertex 0
         0.5f, -0.5f, 0.5f,      0.0f, 0.45f, 0.89f,         0.0f, 0.0f,        // Rear Bottom Right Vertex 3
        -0.5f, -0.5f, 0.5f,     0.0f, 0.45f, 0.89f,         1.0f, 0.0f,         // Rear Bottom Left Vertex 4
         // Triangle 4
        0.0f, 0.5f, 0.0f,       -0.89f, 0.45f, 0.0f,        0.5f, 1.0f,        // Top Vertex 0
       -0.5f, -0.5f, 0.5f,     -0.89f, 0.45f, 0.0f,        1.0f, 0.0f,        // Rear Bottom Left Vertex 4
       -0.5f, -0.5f, -0.5f,    -0.89f, 0.45f, 0.0f,        0.0f, 0.0f,        // Front Bottom Left Vertex 1
         // Triangle 1, base
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,           0.0f, 1.0f,      // Front Bottom Left Vertex 1
         0.5f, -0.5f, -0.5f,     0.0f, 0.0f, 1.0f,           1.0f, 1.0f,       // Front Bottom Right Vertex 2
         0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,           1.0f, 0.0f,       // Rear Bottom Right Vertex 3
        // Triangle 2, base
         0.5f, -0.5f, 0.5f,      0.0f, 0.0f, 1.0f,           1.0f, 0.0f,       // Rear Bottom Right Vertex 3
        -0.5f,  -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,           0.0f, 0.0f,        // Rear Bottom Left Vertex 4
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,           0.0f, 1.0f,      // Front Bottom Left Vertex 1


    };

    //tell how many values for vertices and colors, 3 for vertices and 4 for color
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;
    //Vertices between vertex coordinates 
    mesh.nVertices = sizeof(verts) / sizeof(verts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV);
    
    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    //this tells OpenGL how it should interpret the data, first parameter specifies the location of the postion which is at 1
    //then the next argument is the size of the vertex attribute for color or 4 unit (r,g,b,a), GL_Float is the type of data, and GL_FALSE for normalizing the data
    //Attribute pointer for the colors with loction one
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    //Texture
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

}

//Function to create the cube for the-------HOUSE----------------------------------------------------------------
void UCubeMesh(GLMesh& mesh) {

    // Position and Color data
    GLfloat Cubeverts[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    //tell how many values for vertices and colors, 3 for vertices and 4 for color
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;
    ////Vertices between vertex coordinates 
    mesh.nVertices = sizeof(Cubeverts) / sizeof(Cubeverts[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV);
    glGenVertexArrays(1, &mesh.vao); //also generate multiple VAOS or buffers at the same time
    glBindVertexArray(mesh.vao);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); //Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Cubeverts), Cubeverts, GL_STATIC_DRAW); //Sends vertex or coordinate data to the GPU

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    //this tells OpenGL how it should interpret the data, first parameter specifies the location of the postion which is at 1
    //then the next argument is the size of the vertex attribute for color or 4 unit (r,g,b,a), GL_Float is the type of data, and GL_FALSE for normalizing the data
    //Attribute pointer for the colors with loction one
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    //Texture
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

}
//Function to create the cubes --------------------DOOR---------------------front face only---------------
void UCubeDoorMesh(GLMesh& mesh) {
    GLfloat doorverts[]{
        //Front Face			//Positive Z Normal
        -0.5f, 0.5f,  0.5f,	    0.0f,  0.0f,  1.0f,  0.0f, 1.0f, //20
        -0.5f, -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,  0.0f, 0.0f, //21
        0.5f,  -0.5f,  0.5f,	0.0f,  0.0f,  1.0f,  1.0f, 0.0f, //22
        0.5f,  0.5f,  0.5f,		0.0f,  0.0f,  1.0f,  1.0f, 1.0f, //23
    };

    GLushort Doorindices[] = {
    0, 1, 3,  // Triangle 1
    1, 2, 3,   // Triangle 2
    };
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    glGenVertexArrays(1, &mesh.vao); //also generate multiple VAOS or buffers at the same time
    glBindVertexArray(mesh.vao);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); //Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(doorverts), doorverts, GL_STATIC_DRAW); //Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(Doorindices) / sizeof(Doorindices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Doorindices), Doorindices, GL_STATIC_DRAW);


    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    //this tells OpenGL how it should interpret the data, first parameter specifies the location of the postion which is at 1
    //then the next argument is the size of the vertex attribute for color or 4 unit (r,g,b,a), GL_Float is the type of data, and GL_FALSE for normalizing the data
    //Attribute pointer for the colors with loction one
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    //Texture
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

}
//Function to create the ------------------------BOOK, CUSTOM texture coordinates to replicate a book (back/left/right faces)
void UCubeBookMesh(GLMesh& mesh) {
    GLfloat bookverts[]{
        //Postions             //Color                     //With Custom Texture for the back/left/right faces 
       //Back Face				  
       0.5f, -0.5f, -0.5f,		1.0f,  1.0f,  -1.0f,      1.0f, 1.0f,   //0
       0.5f, -1.0f, -0.5f,		1.0f,  1.0f,  -1.0f,      1.0f, 1.0f,   //1
       -1.0f, -1.0f, -0.5f, 	1.0f,  1.0f,  -1.0f,      1.0f, 1.0f,   //2
       -1.0f, -0.5f, -0.5f,     1.0f,  1.0f,  -1.0f,      1.0f, 1.0f,   //3

       //Bottom Face			
       -1.0f, -1.0f, -0.5f,	    0.0f,  1.0f,  0.0f,     0.0f, 1.0f,  //4
       -1.0f, -1.0f, 2.0f,	    0.0f,  1.0f,  0.0f,     0.0f, 0.0f,  //5
       0.5f, -1.0f, 2.0f,		0.0f,  1.0f,  0.0f,     1.0f, 0.0f,  //6
       0.5f, -1.0f,  -0.5f,	    0.0f,  1.0f,  0.0f,     1.0f, 1.0f, //7

       //Left Face				
       -1.0f, -0.5f, -0.5f,	    1.0f,  0.0f,  0.0f,     0.0f, 0.0f,   //8   0, 1 
       -1.0f, -1.0f,  -0.5f,	1.0f,  0.0f,  0.0f,     0.0f, 0.0f,  //9    0, 0
       -1.0f,  -1.0f,  2.0f,	1.0f,  0.0f,  0.0f,     0.0f, 0.0f,  //10   1,0
       -1.0f,  -0.5f,  2.0f,	1.0f,  0.0f,  0.0f,     0.0f, 0.0f,  //11   1,1

       //Right Face			
       0.5f,  -0.5f,  2.0f,	    1.0f,  0.0f,  0.0f,     1.0f, 1.0f,  //12  //0,1
       0.5f,  -1.0f, 2.0f,		1.0f,  0.0f,  0.0f,     1.0f, 1.0f,  //13  //0,0
       0.5f, -1.0f, -0.5f,		1.0f,  0.0f,  0.0f,     1.0f, 1.0f,  //14  //1,0
       0.5f, -0.5f, -0.5f,		1.0f,  0.0f,  0.0f,     1.0f, 1.0f,  //15  //1,1

       //Top Face				
       -1.0f,  -0.5f, -0.5f,	0.0f,  1.0f,  0.0f,     0.0f, 1.0f, //16
       -1.0f,  -0.5f, 2.0f, 	0.0f,  1.0f,  0.0f,     0.0f, 0.0f, //17
       0.5f,  -0.5f,  2.0f,	    0.0f,  1.0f,  0.0f,     1.0f, 0.0f, //18
       0.5f,  -0.5f,  -0.5f,	0.0f,  1.0f,  0.0f,     1.0f, 1.0f, //19

       //Front Face			
       -1.0f, -0.5f,  2.0f,	    0.0f,  0.0f,  1.0f,     0.0f, 1.0f, //20
       -1.0f, -1.0f,  2.0f,	    0.0f,  0.0f,  1.0f,     0.0f, 0.0f, //21
       0.5f,  -1.0f,  2.0f,	    0.0f,  0.0f,  1.0f,     1.0f, 0.0f, //22
       0.5f,  -0.5f,  2.0f,	    0.0f,  0.0f,  1.0f,     1.0f, 1.0f, //23
    };

    GLushort bookindices[] = {
        0,1,2,
        0,3,2,
        4,5,6,
        4,7,6,
        8,9,10,
        8,11,10,
        12,13,14,
        12,15,14,
        16,17,18,
        16,19,18,
        20,21,22,
        20,23,22
    };
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;


    glGenVertexArrays(1, &mesh.vao); //also generate multiple VAOS or buffers at the same time
    glBindVertexArray(mesh.vao);
    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]); //Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(bookverts), bookverts, GL_STATIC_DRAW); //Sends vertex or coordinate data to the GPU

    mesh.nIndices = sizeof(bookindices) / sizeof(bookindices[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bookindices), bookindices, GL_STATIC_DRAW);

    // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    //this tells OpenGL how it should interpret the data, first parameter specifies the location of the postion which is at 1
    //then the next argument is the size of the vertex attribute for color or 4 unit (r,g,b,a), GL_Float is the type of data, and GL_FALSE for normalizing the data
    //Attribute pointer for the colors with loction one
    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);
    ////Texture
    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

}


//Generate and load the texture
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, mesh.vbos);
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
