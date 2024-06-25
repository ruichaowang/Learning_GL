// 要优先于glfw3.h，否则编译不通过，原因未知
// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>

#include "BowlModel.h"
#include <iostream>
#include <learnopengl/camera.h>
#include <learnopengl/shader_m.h>
#include <unistd.h>
#include <unordered_set>

// function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), -0.0f,
              89.0f); //-90
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* 函数用于读取CSV文件并返回一个整数的二维向量 */
std::vector<std::vector<int>> read_csv(const std::string &filename) {
    std::vector<std::vector<int>> data;

    // 打开文件
    std::ifstream infile(filename);
    std::string line;

    // 按行读取
    while (std::getline(infile, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector<int> rowData;

        // 以逗号分隔每个字段
        while (getline(lineStream, cell, ',')) {
            // 将字符串转换为整数
            rowData.push_back(std::stoi(cell));
        }

        data.push_back(rowData);
    }

    return data;
}

std::vector<glm::vec3> obstacle_position_;

/* 这个数据是模型参数 */
const auto SCALE_FACTOR = 1;
const float VOXEL_SIZE = 1.024f / SCALE_FACTOR;
const auto IMAGE_WIDTH = 1600.0;
const auto IMAGE_HEIGHT = 900.0;
const int DRAW_ONCE = 1;
const int CAMERA_COUNTS = 6;

/* 把车挪到整个模型的中心,调节地面的基准 -2 是推测值 */
const auto VOXEL_OFFSET = glm::vec3(-49.0f, -49.0f, -3.0f); //?-35
/* 外参的坐标系和车辆坐标系的变化，这个数为推测出来的 */
const auto ExtrinsicOffset = glm::vec3(0.0, 1.425, 0.0f);

const auto bwol_vs = "../shaders/bowl.vs";
const auto bowl_fs = "../shaders/bowl.fs";
const auto texture_path = "../assets/container2.png";
const auto VOXEL_COORDINATE_PATH = "../assets/cordinate/slice_";
const auto VOXEL_COORDINATE_3X_PATH = "../assets/cordinate/slice_3x_";

struct ModelPart {
    std::vector<glm::vec3> vertices;
    GLuint VBO = 0, VAO = 0, texture = 0;
};

/**
 * @brief 生成障碍物的地图，
 * 为了方便检测障碍物信息，要删除掉 drivable surface ，把立体信息去除
 * glm::vec3 temp_positon(x * 1.0f, y * 1.0f, z * 1.0f);
 *
 * 为了我们做运算，我们只取出来中心值范围+-10m的数据
 *
 * @param path
 * @param obstacles
 * @param offset
 * @param scale
 */
void GenerateObstaclePosition(const std::string &path,
                              std::vector<glm::vec3> &obstacles,
                              glm::vec3 offset, int scale) {
    auto height = static_cast<int>(100 * scale);
    auto width = static_cast<int>(100 * scale);
    auto floor = static_cast<int>(2 * scale);
    auto roof = static_cast<int>(8 * scale);

    std::unordered_set<glm::vec3> temp_obstacles;

    const auto voxel_center = glm::vec2(width / 2, height / 2);
    const int y_min = voxel_center.y - 10 * scale;
    const int y_max = voxel_center.y + 10 * scale;
    const int x_min = voxel_center.x - 10 * scale;
    const int x_max = voxel_center.x + 10 * scale;

    /* 加载 坐标,  */
    for (int z = floor; z < roof; ++z) {
        const std::string filename = path + std::to_string(z) + ".csv";
        std::vector<std::vector<int>> data = read_csv(filename);

        for (int y = y_min; y < y_max; ++y) {
            for (int x = x_min; x < x_max; ++x) {
                /* 删除掉高度 */
                glm::vec3 temp_positon(x * 1.0f, y * 1.0f, 0 * 1.0f);

                /* 添加 voxels */
                if ((data[y][x] != 11) && (data[y][x] != 17) &&
                    (data[y][x] > 0) && (data[y][x] < 20)) {
                    temp_obstacles.emplace(temp_positon);
                }
            }
        }
    }

    // 去重完毕，返回 vector
    obstacles.insert(obstacles.end(), temp_obstacles.begin(),
                     temp_obstacles.end());

    /* cube z 轴旋转 -90 度 */
    float rotationAngleDegrees = -90.0f;
    float rotationAngleRadians = glm::radians(rotationAngleDegrees);
    glm::mat4 rotationMatrix = glm::rotate(
        glm::mat4(1.0f), rotationAngleRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    for (auto &obstacle : obstacles) {
        obstacle = glm::vec3(rotationMatrix * glm::vec4(obstacle, 1.0f));
    }

    /* 对y轴反转 */
    for (auto &obstacle : obstacles) {
        obstacle.y = -obstacle.y;
    }

    /* 先放大缩小后，再cube 整体移动，以及转化到真实世界坐标 */
    for (auto &obstacle : obstacles) {
        obstacle *= VOXEL_SIZE;
    }
    for (auto &obstacle : obstacles) {
        obstacle += offset;
    }
};

void InitBuffers(ModelPart &part) {
    glGenVertexArrays(1, &part.VAO);
    glGenBuffers(1, &part.VBO);

    glBindVertexArray(part.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, part.VBO);
    glBufferData(GL_ARRAY_BUFFER, part.vertices.size() * sizeof(glm::vec3),
                 part.vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief 此工程是为了绘制 OCC 生成的预测结果图像
 */
int main() {
    /* 生成障碍物点云 */
    if (SCALE_FACTOR == 1) {
        GenerateObstaclePosition(VOXEL_COORDINATE_PATH, obstacle_position_,
                                 VOXEL_OFFSET, SCALE_FACTOR);
    } else {
        GenerateObstaclePosition(VOXEL_COORDINATE_3X_PATH, obstacle_position_,
                                 VOXEL_OFFSET, SCALE_FACTOR);
    }

    camera.Position = glm::vec3(0, 0, 5); /* 相机放到前摄位置 */

    /* glfw & glad: initialize and configure */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                                          "VoxelRender", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    if (DRAW_ONCE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glPointSize(10.0f);
    }

    Shader bowl_program_(bwol_vs, bowl_fs);
    unsigned int bowl_vbo_, bowl_vao_;
    glGenVertexArrays(1, &bowl_vao_);
    glGenBuffers(1, &bowl_vbo_);

    glBindBuffer(GL_ARRAY_BUFFER, bowl_vbo_);
    glBufferData(GL_ARRAY_BUFFER, obstacle_position_.size() * sizeof(glm::vec3),
                 obstacle_position_.data(), GL_STATIC_DRAW);

    glBindVertexArray(bowl_vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // shader configuration

    bowl_program_.use();
    bowl_program_.setInt("camera_texture", 0);
    bowl_program_.setInt("debug_mesh", DRAW_ONCE);

    // render loop -----------
    while (!glfwWindowShouldClose(window)) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // -----
        processInput(window);

        // render ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bowl_program_.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        bowl_program_.setMat4("projection", projection);
        bowl_program_.setMat4("view", view);

        /* 实例渲染，instance rendering */
        bowl_program_.setInt("camera_texture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(bowl_vao_);

        if (DRAW_ONCE == 1) {
            for (auto i = 0; i < 1; i++) {
                glDrawArrays(GL_TRIANGLES, 0, obstacle_position_.size());
            }
        } else {
            for (auto i = 0; i < CAMERA_COUNTS; i++) {
                glDrawArrays(GL_TRIANGLES, 0, obstacle_position_.size());
            }
        }

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameDuration =
            std::chrono::duration_cast<std::chrono::microseconds>(frameEnd -
                                                                  frameStart)
                .count();
        // std::cout << "Frame time: " << frameDuration << " microseconds ("
        //           << 1.0 / (frameDuration * 1e-6) << " FPS)" << std::endl;

        // glfw: swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &bowl_vao_);
    glDeleteBuffers(1, &bowl_vbo_);

    glfwTerminate(); // clearing all previously allocated GLFW resources.

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        /* 归位 */
        camera.Position = glm::vec3(0, 0, 5);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina
    // displays.
    printf("width: %d, height: %d\n", width, height);
    glViewport(0, 0, width, height);
}

/* reversed since y-coordinates go from bottom to top */
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    // std::cout << "x,y = " << xposIn << "," << yposIn << std::endl;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = -1.0 * (xpos - lastX);
    float yoffset = -1.0 * (ypos - lastY);
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}