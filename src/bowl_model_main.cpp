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

/* 这个数据是模型参数 */
std::vector<glm::vec3> obstacle_position_;
const auto SCALE_FACTOR = 3;
const float VOXEL_SIZE = 1.024f / SCALE_FACTOR;
const auto IMAGE_WIDTH = 1600.0;
const auto IMAGE_HEIGHT = 900.0;
const int DRAW_ONCE = 1;

// settings
const unsigned int SCREEN_WIDTH = 1920;
const unsigned int SCREEN_HEIGHT = 1080;

// camera
const auto camera_pos = glm::vec3(0.0f, 0.0f, 5.0f);
const auto world_up = glm::vec3(0.0f, 0.0f, 1.0f);
Camera camera(camera_pos, world_up, -0.0f, 89.0f);

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* 把车挪到整个模型的中心,调节地面的基准 -2 是推测值 */
const auto VOXEL_OFFSET = glm::vec3(-49.0f, -49.0f, -3.0f); //?-35
/* 外参的坐标系和车辆坐标系的变化，这个数为推测出来的 */
const auto ExtrinsicOffset = glm::vec3(0.0, 1.425, 0.0f);

const auto bwol_vs = "../shaders/bowl.vs";
const auto bowl_fs = "../shaders/bowl.fs";
const auto texture_path = "../assets/container2.png";
const auto VOXEL_COORDINATE_PATH = "../assets/cordinate/slice_";
const auto VOXEL_COORDINATE_3X_PATH = "../assets/cordinate/slice_3x_";
const auto cam_front_path =
    "../assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_FRONT__1538984245412460.jpg";
const auto cam_back_path =
    "../assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_BACK__1538984245437525.jpg";
const auto cam_front_left_path =
    "../assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_FRONT_LEFT__1538984245404844.jpg";
const auto cam_front_right_path =
    "../assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_FRONT_RIGHT__1538984245420339.jpg";
const auto cam_back_left_path =
    "../assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_BACK_LEFT__1538984245447423.jpg";
const auto cam_back_right_path =
    "../assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_BACK_RIGHT__1538984245427893.jpg";

const auto intrinsics_front =
    glm::mat3(1266.417203046554, 0.0, 816.2670197447984, 0.0, 1266.417203046554,
              491.50706579294757, 0.0, 0.0, 1.0);
const auto quaternion_front =
    glm::quat(0.4998015430569128, -0.5030316162024876, 0.4997798114386805,
              -0.49737083824542755);
const auto translation_vectors_front =
    glm::vec3(1.70079118954, 0.0159456324149, 1.51095763913);

const auto intrinsics_rear =
    glm::mat3(809.2209905677063, 0.0, 829.2196003259838, 0.0, 809.2209905677063,
              481.77842384512485, 0.0, 0.0, 1.0);
const auto quaternion_rear = glm::quat(0.5037872666382278, -0.49740249788611096,
                                       -0.4941850223835201, 0.5045496097725578);
const auto translation_vectors_rear =
    glm::vec3(0.0283260309358, 0.00345136761476, 1.57910346144);

const auto intrinsics_front_left =
    glm::mat3(1272.5979470598488, 0.0, 826.6154927353808, 0.0,
              1272.5979470598488, 479.75165386361925, 0.0, 0.0, 1.0);
const auto quaternion_front_left =
    glm::quat(0.6757265034669446, -0.6736266522251881, 0.21214015046209478,
              -0.21122827103904068);
const auto translation_vectors_front_left =
    glm::vec3(1.52387798135, 0.494631336551, 1.50932822144);

const auto intrinsics_front_right =
    glm::mat3(1260.8474446004698, 0.0, 807.968244525554, 0.0,
              1260.8474446004698, 495.3344268742088, 0.0, 0.0, 1.0);
const auto quaternion_front_right =
    glm::quat(0.2060347966337182, -0.2026940577919598, 0.6824507824531167,
              -0.6713610884174485);
const auto translation_vectors_front_right =
    glm::vec3(1.5508477543, -0.493404796419, 1.49574800619);

const auto intrinsics_back_left =
    glm::mat3(1256.7414812095406, 0.0, 792.1125740759628, 0.0,
              1256.7414812095406, 492.7757465151356, 0.0, 0.0, 1.0);
const auto quaternion_back_left =
    glm::quat(0.6924185592174665, -0.7031619420114925, -0.11648342771943819,
              0.11203317912370753);
const auto translation_vectors_back_left =
    glm::vec3(1.0148780988, -0.480568219723, 1.56239545128);

const auto intrinsics_back_right =
    glm::mat3(1259.5137405846733, 0.0, 807.2529053838625, 0.0,
              1259.5137405846733, 501.19579884916527, 0.0, 0.0, 1.0);
const auto quaternion_back_right =
    glm::quat(0.12280980120078765, -0.132400842670559, -0.7004305821388234,
              0.690496031265798);
const auto translation_vectors_back_right =
    glm::vec3(1.0148780988, -0.480568219723, 1.56239545128);

/* 内外参部分 */
const int CAMERA_COUNTS = 6;
std::array<glm::mat3, CAMERA_COUNTS> intrinsics_ = {
    intrinsics_front,       intrinsics_rear,      intrinsics_front_left,
    intrinsics_front_right, intrinsics_back_left, intrinsics_back_right};
std::array<glm::quat, CAMERA_COUNTS> quaternions_{
    quaternion_front,       quaternion_rear,      quaternion_front_left,
    quaternion_front_right, quaternion_back_left, quaternion_back_right};
std::array<glm::vec3, CAMERA_COUNTS> translation_vectors_ = {
    translation_vectors_front,      translation_vectors_rear,
    translation_vectors_front_left, translation_vectors_front_right,
    translation_vectors_back_left,  translation_vectors_back_right};
std::array<glm::vec3, CAMERA_COUNTS> t2_;
std::array<glm::mat4, CAMERA_COUNTS> model_mat_;
std::array<unsigned int, CAMERA_COUNTS> camera_textures;

/** 生成需要的内外参，先用一个相机的来测试流程
 *  旋转向量，在 solve pnp 中是 from the model coordinate system to the
 * camera coordinate system. 在此，将四元数转换为旋转矩阵，看起来是 camera
 * to world， 结合旋转矩阵和平移矩阵得到外参矩阵,从世界坐标系到相机坐标系,
 * 负号是因为要反过来求变化方向
 */
void GenerateModelMat(glm::quat &quaternion, glm::vec3 &translationVector,
                      glm::mat4 &model_mat, glm::vec3 &t2_,
                      const glm::vec3 &ExtrinsicOffset) {
    glm::quat rotation_final;
    {
        float angle_x_degrees = 0.0f;
        float angle_y_degrees = 0.0f;
        float angle_z_degrees = 90.0f;

        glm::quat rotation_x =
            glm::angleAxis(glm::radians(angle_x_degrees), glm::vec3(1, 0, 0));
        glm::quat rotation_y =
            glm::angleAxis(glm::radians(angle_y_degrees), glm::vec3(0, 1, 0));
        glm::quat rotation_z =
            glm::angleAxis(glm::radians(angle_z_degrees), glm::vec3(0, 0, 1));

        rotation_final = rotation_z * rotation_y * rotation_x;
    }

    quaternion = rotation_final * quaternion;
    auto temp_trans = translationVector + ExtrinsicOffset;
    glm::mat3 rotation_matrix_c2w = glm::mat3_cast(quaternion);
    t2_ = rotation_final * temp_trans;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            model_mat[i][j] = rotation_matrix_c2w[i][j];
        }
    }
    model_mat[3][0] = t2_[0];
    model_mat[3][1] = t2_[1];
    model_mat[3][2] = t2_[2];
    model_mat[3][3] = 1.0f;
    model_mat = glm::inverse(model_mat);
}

void Save2DArrayToFile(const std::vector<std::vector<bool>> &array,
                       const std::string &filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open file for writing: " << filename
                  << std::endl;
        return;
    }

    for (const auto &row : array) {
        for (size_t col = 0; col < row.size(); ++col) {
            file << row[col];
            if (col < row.size() - 1) {
                file << ", ";
            }
        }
        file << '\n';
    }

    file.close();
}

void ConvertToMeters(std::vector<glm::vec3> &vertices) {
    for (auto &vertex : vertices) {
        vertex /= 1000.0f; // 将坐标转换为米
    }
}

struct ModelPart {
    std::vector<glm::vec3> vertices;
    GLuint VBO = 0, VAO = 0, texture = 0;
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

std::vector<glm::vec3>
MergeModels(const std::shared_ptr<BowlModel> &bowlModel) {
    const auto &modelParts = bowlModel->getModel();
    std::vector<glm::vec3> mergedVertices;

    for (const auto &part : modelParts) {
        mergedVertices.insert(mergedVertices.end(), part.begin(), part.end());
    }

    // 测试只取一部分

    // mergedVertices = modelParts[0];

    return mergedVertices;
}

// 障碍物都在2D 平面进行统计，grid 的维度大小是 100 * 100
// 我们关心的是 中心10m之内的数据
struct VoxelGrid {
    std::vector<std::vector<bool>> grid;
    glm::vec3 offset;
    float resolution;
    int width, height, depth;
    float scale = 1.0f;
};

VoxelGrid GenerateObstaclePosition(const std::string &path,
                                   std::vector<glm::vec3> &obstacles,
                                   glm::vec3 offset, int scale) {
    auto height = static_cast<int>(100 * scale);
    auto width = static_cast<int>(100 * scale);
    auto floor = static_cast<int>(2 * scale);
    auto roof = static_cast<int>(8 * scale);

    /* 生成voxels */
    VoxelGrid voxels;
    voxels.resolution = VOXEL_SIZE;
    voxels.width = width;
    voxels.height = height;
    voxels.depth = roof - floor; // 暂时不需要这个参数
    voxels.scale = scale;

    const auto voxel_center = glm::vec2(width / 2, height / 2);
    const int y_min = voxel_center.y - 10 * scale;
    const int y_max = voxel_center.y + 10 * scale;
    const int x_min = voxel_center.x - 10 * scale;
    const int x_max = voxel_center.x + 10 * scale;

    /* 初始化 grid */
    voxels.grid = std::vector<std::vector<bool>>(
        2 * 10 * scale, std::vector<bool>(2 * 10 * scale, false));
    voxels.offset = offset;

    std::unordered_set<glm::vec3> temp_obstacles;

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

    /* obstacles in glm to 3d vectors*/
    for (const auto &obstacle : obstacles) {
        const int x = static_cast<int>(obstacle.x);
        const int y = static_cast<int>(obstacle.y);
        if (x >= (voxels.width / 2 - 10 * scale) &&
            x < (voxels.width / 2 + 10 * scale) &&
            y >= (voxels.height / 2 - 10 * scale) &&
            y < (voxels.height / 2 + 10 * scale)) {
            int index_x = x - (voxels.width / 2 - 10 * scale);
            int index_y = y - (voxels.height / 2 - 10 * scale);

            voxels.grid[index_y][index_x] = true;
        }
    }

    return voxels;
};

float DetectObstacleDistance(const VoxelGrid &voxels, glm::vec2 &direction) {
    int steps = 0;
    float distance = std::numeric_limits<float>::infinity(); // 初始值设为无穷大

    while (true) {
        // 计算当前检测的位置
        glm::vec2 current_pos =
             10 * voxels.scale + direction * static_cast<float>(steps) * voxels.resolution;

        // 对应位置的 voxel 索引
        int x = static_cast<int>(current_pos.x / voxels.resolution);
        int y = static_cast<int>(current_pos.y / voxels.resolution);

        if (x < 0 || x >= (10 * voxels.scale * 2) || y < 0 || y >= (10 * voxels.scale* 2)) {
            // 当前位置超出范围
            break;
        }

        // 当检测到障碍物的时候
        if (voxels.grid[y][x]) {
            distance = steps * voxels.resolution;
            break;
        }

        steps++;
    }

    return distance;
}

void AdjustVerticesBasedOnVoxels(std::vector<glm::vec3> &vertices,
                                 const VoxelGrid &voxels, float radius) {
    for (auto &vertex : vertices) {
        float angle = atan2(vertex.y, vertex.x);
        float distanceFromCenter = glm::length(glm::vec2(vertex.x, vertex.y));

        glm::vec2 direction(cos(angle), sin(angle));
        float obstacleDistance =
            DetectObstacleDistance(voxels, direction);

        float scale = std::min(1.0f, obstacleDistance / radius);
        distanceFromCenter *= scale;

        vertex.x = distanceFromCenter * cos(angle);
        vertex.y = distanceFromCenter * sin(angle);
    }
}

void SaveVerticesToCSV(const std::vector<glm::vec3> &vertices,
                       const std::string &filename) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open file for writing: " << filename
                  << std::endl;
        return;
    }

    // 写入数据
    for (const auto &vertex : vertices) {
        file << vertex.x << ", " << vertex.y << ", " << vertex.z << '\n';
    }

    file.close();
}

/**
 * @brief 绘制 bowlmodel + camera projection，不用voxels, 我们先绘制碗的一部分
 */
int main() {
    /* 生成障碍物点云 */
    VoxelGrid voxels;
    if (SCALE_FACTOR == 1) {
        voxels =
            GenerateObstaclePosition(VOXEL_COORDINATE_PATH, obstacle_position_,
                                     VOXEL_OFFSET, SCALE_FACTOR);
    } else {
        voxels = GenerateObstaclePosition(VOXEL_COORDINATE_3X_PATH,
                                          obstacle_position_, VOXEL_OFFSET,
                                          SCALE_FACTOR);
    }

    Save2DArrayToFile(voxels.grid, "output.csv");

    /* 获取碗模型，为方便验证，先合并到一起再转化到 m，再进行调整 */
    std::shared_ptr<BowlModel> mBowlModel = BowlModel::create();
    mBowlModel->initModel();

    // 为了方便直接选择一片vao vbo
    std::vector<glm::vec3> vertices_merged = MergeModels(mBowlModel);
    ConvertToMeters(vertices_merged);

    SaveVerticesToCSV(vertices_merged, "vertices_raw.csv");
    AdjustVerticesBasedOnVoxels(vertices_merged, voxels, 10.0f);
    SaveVerticesToCSV(vertices_merged, "vertices_scale.csv");

    for (auto i = 0; i < 6; i++) {
        GenerateModelMat(quaternions_[i], translation_vectors_[i],
                         model_mat_[i], t2_[i], ExtrinsicOffset);
    }
    camera.Position = t2_[0]; /* 相机放到前摄位置 */

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
        // 线框模式进行debug GL_LINE， 填充  GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(30.0f);
    }

    Shader bowl_program_(bwol_vs, bowl_fs);
    unsigned int bowl_vbo_, bowl_vao_;
    glGenVertexArrays(1, &bowl_vao_);
    glGenBuffers(1, &bowl_vbo_);

    glBindBuffer(GL_ARRAY_BUFFER, bowl_vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertices_merged.size() * sizeof(glm::vec3),
                 vertices_merged.data(), GL_STATIC_DRAW);

    glBindVertexArray(bowl_vao_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);

    /* load textures */
    camera_textures[0] = loadTexture(cam_front_path);
    camera_textures[1] = loadTexture(cam_back_path);
    camera_textures[2] = loadTexture(cam_front_left_path);
    camera_textures[3] = loadTexture(cam_front_right_path);
    camera_textures[4] = loadTexture(cam_back_left_path);
    camera_textures[5] = loadTexture(cam_back_right_path);

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

        /* 添加上相机内外参 0.791511 1.40713, 0.510167 0.546119 */
        glm::vec2 focal_length = glm::vec2(intrinsics_[0][0][0] / IMAGE_WIDTH,
                                           intrinsics_[0][1][1] / IMAGE_HEIGHT);
        glm::vec2 principal_point =
            glm::vec2(intrinsics_[0][0][2] / IMAGE_WIDTH,
                      intrinsics_[0][1][2] / IMAGE_HEIGHT);

        bowl_program_.setVec2("focal_lengths", focal_length);
        bowl_program_.setVec2("camera_principal_point", principal_point);

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
                glBindTexture(GL_TEXTURE_2D, camera_textures[i]);
                bowl_program_.setMat4("extrinsic_matrix", model_mat_[i]);
                glDrawArrays(GL_TRIANGLES, 0, vertices_merged.size());
            }
        } else {
            for (auto i = 0; i < CAMERA_COUNTS; i++) {
                glBindTexture(GL_TEXTURE_2D, camera_textures[i]);
                bowl_program_.setMat4("extrinsic_matrix", model_mat_[i]);
                glDrawArrays(GL_TRIANGLES, 0, vertices_merged.size());
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
        camera.Position = t2_[0];
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