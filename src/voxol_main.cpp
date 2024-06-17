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

#include <iostream>
#include <learnopengl/camera.h>
#include <learnopengl/shader_m.h>

#include <unistd.h>

// function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f), -0.0f,
              89.0f); //-90
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 光源属性结构体
struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

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
const float voxel_size = 1.024f;
const auto image_width = 1600.0;
const auto image_height = 900.0;

/* lighting， 我们认为y轴正向为我们需要的方向放置一个标志 */
glm::vec3 lightPos(0.0f, 10.0f, 5.0f);
glm::vec3 light_intensity(2.0f);
/* 把车挪到整个模型的中心,调节地面的基准 -2 是推测值 */
const auto cube_offset = glm::vec3(-50.0f, -50.0f, -2.0f);
/* 外参的坐标系和车辆坐标系的变化，这个数为推测出来的 */
const auto ExtrinsicOffset = glm::vec3(0.0, 1.5, 2.5);

const auto color_vs = "shaders/colors.vs";
const auto color_fs = "shaders/colors.fs";
const auto light_cube_vs = "shaders/light_cube.vs";
const auto light_cube_fs = "shaders/light_cube.fs";
const auto texture_path = "assets/container2.png";
const auto cordinate_path = "assets/cordinate/slice_";
const auto cam_front_path =
    "assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_FRONT__1538984245412460.jpg";
const auto cam_back_path =
    "assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_BACK__1538984245437525.jpg";
const auto cam_front_left_path =
    "assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_FRONT_LEFT__1538984245404844.jpg";
const auto cam_front_right_path =
    "assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_FRONT_RIGHT__1538984245420339.jpg";
const auto cam_back_left_path =
    "assets/camera/"
    "n015-2018-10-08-15-36-50+0800__CAM_BACK_LEFT__1538984245447423.jpg";
const auto cam_back_right_path =
    "assets/camera/"
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

/* 立方体定点数据，应该需要转化 */
const float original_ertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
    -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
    0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
    0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
    0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
    -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
    0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
    0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
    -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
    0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
    -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
    -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
    1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
    -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
    1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

/* 内外参部分 */
const int camera_count = 6;
std::array<glm::mat3, camera_count> intrinsics_ = {
    intrinsics_front,       intrinsics_rear,      intrinsics_front_left,
    intrinsics_front_right, intrinsics_back_left, intrinsics_back_right};
std::array<glm::quat, camera_count> quaternions_{
    quaternion_front,       quaternion_rear,      quaternion_front_left,
    quaternion_front_right, quaternion_back_left, quaternion_back_right};
std::array<glm::vec3, camera_count> translation_vectors_ = {
    translation_vectors_front,      translation_vectors_rear,
    translation_vectors_front_left, translation_vectors_front_right,
    translation_vectors_back_left,  translation_vectors_back_right};
std::array<glm::vec3, camera_count> t2_;
std::array<glm::mat4, camera_count> model_mat_;

/* 定义立方体的位置 */
std::vector<glm::vec3> cube_positions_ = {};

/**
 * @brief 定义生成 cube 位置的方法
 *
 * @param cordinate_path 模型加载位置
 * @param cube_positions 输出的立方体位置
 * @param offset 基准值，把车移动到中心，且移动了地面的高度
 */
void GenCubePosition(const std::string &cordinate_path,
                     std::vector<glm::vec3> cube_positions, glm::vec3 offset) {
    auto depth = 30;
    auto height = 100;
    auto width = 100;
    /* 加载 cube 坐标,  */
    for (int z = 2; z < 8; ++z) {
        const std::string filename =
            cordinate_path + std::to_string(z) + ".csv";
        std::vector<std::vector<int>> data = read_csv(filename);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                /* 填充地面 */
                if (z == 2) {
                    glm::vec3 temp_positon(x * 1.0f, y * 1.0f, z * 1.0f);
                    cube_positions_.push_back(temp_positon);
                }

                /* 添加边缘和立面 */
                if (y == 0 || y == 99 || x == 0 || x == 99) {
                    glm::vec3 temp_positon(x * 1.0f, y * 1.0f, z * 1.0f);
                    cube_positions_.push_back(temp_positon);
                }

                /* 添加 voxels */
                if (data[y][x] != 17) {
                    glm::vec3 temp_positon(x * 1.0f, y * 1.0f, z * 1.0f);
                    cube_positions_.push_back(temp_positon);
                }
            }
        }
    }

    std::vector<glm::vec3> wallPositions;
    // 生成四周墙壁
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            // 左右墙壁
            wallPositions.push_back(glm::vec3(0, y, z));
            wallPositions.push_back(glm::vec3(width - 1, y, z));
        }
        for (int x = 0; x < width; ++x) {
            // 前后墙壁
            wallPositions.push_back(glm::vec3(x, 0, z));
            wallPositions.push_back(glm::vec3(x, height - 1, z));
        }
    }
    cube_positions_.insert(cube_positions_.end(), wallPositions.begin(),
                           wallPositions.end());

    // 移除重复的顶点
    cube_positions_.erase(
        std::unique(cube_positions_.begin(), cube_positions_.end()),
        cube_positions_.end());

    /* cube 整体移动，以及转化到真实世界坐标 */
    for (auto &position : cube_positions_) {
        position += offset;
    }
    for (auto &position : cube_positions_) {
        position *= voxel_size;
    }

    /* cube z 轴旋转 -90 度 */
    float rotationAngleDegrees = -90.0f;
    float rotationAngleRadians = glm::radians(rotationAngleDegrees);
    glm::mat4 rotationMatrix = glm::rotate(
        glm::mat4(1.0f), rotationAngleRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    for (auto &position : cube_positions_) {
        position = glm::vec3(rotationMatrix * glm::vec4(position, 1.0f));
    }

    /* 对y轴反转 */
    for (auto &position : cube_positions_) {
        position.y = -position.y;
    }
};

/* 生成外参: modelmat 的方法 */
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
    translationVector += ExtrinsicOffset;
    glm::mat3 rotation_matrix_c2w = glm::mat3_cast(quaternion);
    t2_ = rotation_final * translationVector;

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

/**
 * @brief 绘制 Voxel + camera projection
 */
int main() {
    /** 生成需要的内外参，先用一个相机的来测试流程
     *  旋转向量，在 solve pnp 中是 from the model coordinate system to the
     * camera coordinate system. 在此，将四元数转换为旋转矩阵，看起来是 camera
     * to world， 结合旋转矩阵和平移矩阵得到外参矩阵,从世界坐标系到相机坐标系,
     * 负号是因为要反过来求变化方向
     */

    for (auto i = 0; i < 6; i++) {
        GenerateModelMat(quaternions_[i], translation_vectors_[i],
                         model_mat_[i], t2_[i], ExtrinsicOffset);
    }

    camera.Position = t2_[0]; /* 相机放到前摄位置 */

    /* 生成立方体 */
    GenCubePosition(cordinate_path, cube_positions_, cube_offset);

    /* 设置光源 */
    Light light;
    light.position = glm::vec3(1.2f, 1.0f, 2.0f) * light_intensity;
    light.ambient = glm::vec3(0.2f, 0.2f, 0.2f) * light_intensity;
    light.diffuse = glm::vec3(0.5f, 0.5f, 0.5f) * light_intensity;
    light.specular = glm::vec3(1.0f, 1.0f, 1.0f) * light_intensity;

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
    GLFWwindow *window =
        glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "VoxelRender", NULL, NULL);
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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram

    // ------------------------------------
    Shader lightingShader(color_vs, color_fs);
    Shader lightCubeShader(light_cube_vs, light_cube_fs);
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    /* 缩放顶点数据 */
    float scaledVertices[sizeof(original_ertices) /
                         sizeof(float)]; // 创建一个新数组来存储缩放后的顶点数据
    for (size_t i = 0; i < sizeof(original_ertices) / sizeof(float); i += 8) {
        scaledVertices[i] = original_ertices[i] * voxel_size;         // x坐标
        scaledVertices[i + 1] = original_ertices[i + 1] * voxel_size; // y坐标
        scaledVertices[i + 2] = original_ertices[i + 2] * voxel_size; // z坐标
        // 法线和纹理坐标保持不变
        for (int j = 3; j < 8; ++j) {
            scaledVertices[i + j] = original_ertices[i + j];
        }
    }

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(scaledVertices), scaledVertices,
                 GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, cube_positions_.size() * sizeof(glm::vec3),
                 &cube_positions_[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glVertexAttribDivisor(3, 1);

    // second, configure the light's VAO (VBO stays the same; the vertices are
    // the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer),
    // no need to fill it; the VBO's data already contains all we need (it's
    // already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the
    // updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    /* load textures */
    unsigned int cam_front_tex = loadTexture(cam_front_path);
    unsigned int cam_back_tex = loadTexture(cam_back_path);
    unsigned int cam_front_left_tex = loadTexture(cam_front_left_path);
    unsigned int cam_front_right_tex = loadTexture(cam_front_right_path);
    unsigned int cam_back_left_tex = loadTexture(cam_back_left_path);
    unsigned int cam_back_right_tex = loadTexture(cam_back_right_path);

    // shader configuration
    lightingShader.use();
    lightingShader.setInt("camera_texture", 0);
    // light properties
    lightingShader.setVec3("light.ambient", light.ambient);
    lightingShader.setVec3("light.diffuse", light.diffuse);
    lightingShader.setVec3("light.specular", light.specular);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        /* 添加上相机内外参 0.791511 1.40713, 0.510167 0.546119 */
        glm::vec2 focal_length = glm::vec2(intrinsics_[0][0][0] / image_width,
                                           intrinsics_[0][1][1] / image_height);
        glm::vec2 principal_point =
            glm::vec2(intrinsics_[0][0][2] / image_width,
                      intrinsics_[0][1][2] / image_height);

        lightingShader.setVec2("focal_lengths", focal_length);
        lightingShader.setVec2("cammera_principal_point", principal_point);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        /* 实例渲染，instance rendering */
        lightingShader.setInt("camera_texture", 0);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, cam_front_tex);
        lightingShader.setMat4("extrinsic_matrix", model_mat_[0]);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_positions_.size());

        glBindTexture(GL_TEXTURE_2D, cam_back_tex);
        lightingShader.setMat4("extrinsic_matrix", model_mat_[1]);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_positions_.size());

        glBindTexture(GL_TEXTURE_2D, cam_front_left_tex);
        lightingShader.setMat4("extrinsic_matrix", model_mat_[2]);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_positions_.size());

        glBindTexture(GL_TEXTURE_2D, cam_front_right_tex);
        lightingShader.setMat4("extrinsic_matrix", model_mat_[3]);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_positions_.size());

        glBindTexture(GL_TEXTURE_2D, cam_back_left_tex);
        lightingShader.setMat4("extrinsic_matrix", model_mat_[4]);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_positions_.size());

        glBindTexture(GL_TEXTURE_2D, cam_back_right_tex);
        lightingShader.setMat4("extrinsic_matrix", model_mat_[5]);
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, cube_positions_.size());

        /* 实例渲染结束 */
        if (0) {
            /* 绘制一次 */
            lightingShader.setMat4("model", glm::mat4(1.0f));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            /* 直接绘制多次 */
            glBindVertexArray(cubeVAO);
            for (unsigned int i = 0; i < cube_positions_.size(); i++) {
                lightingShader.setVec3("position", cube_positions_[i]);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        /* draw the a smaller cube lamp object */
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model;
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);

    glfwTerminate(); // clearing all previously allocated GLFW resources.

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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