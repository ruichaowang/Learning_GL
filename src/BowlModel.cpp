#include "BowlModel.h"
#include <iostream>

static void CircleCenter(const glm::vec3 &pt1, const glm::vec3 &pt2, double R,
                         glm::vec3 &pt) {
    double c1 =
        (pt2.y * pt2.y - pt1.y * pt1.y + pt2.z * pt2.z - pt1.z * pt1.z) /
        (2 * (pt2.y - pt1.y));
    double c2 = (pt2.z - pt1.z) / (pt2.y - pt1.y); // 斜率
    double A = (c2 * c2 + 1);
    double B = (2 * pt1.y * c2 - 2 * c1 * c2 - 2 * pt1.z);
    double C = pt1.y * pt1.y - 2 * pt1.y * c1 + c1 * c1 + pt1.z * pt1.z - R * R;
    pt.z = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
    pt.y = c1 - c2 * pt.z;
}

static double len(const glm::vec3 &pt1, const glm::vec3 &pt2) {
    return sqrt(pow(pt2.x - pt1.x, 2) + pow(pt2.y - pt1.y, 2) +
                pow(pt2.z - pt1.z, 2));
}

static double calRad(const glm::vec3 &center, const glm::vec3 &pt) {
    return atan2(pt.z - center.z, pt.y - center.y);
}

static void AdjustVerticesBasedOnAngle(std::vector<glm::vec3> &vertices,
                                           const Obstacles &obstacles,
                                           float radius) {
    for (auto &vertex : vertices) {
        // 获取当前顶点的角度
        float angle = atan2(vertex.y, vertex.x);
        // 获取当前顶点半径距离
        float distance = glm::length(glm::vec2(vertex.y, vertex.x));

        // 根据方向调整半径,假设我们当前对某些条件的进行收缩比如0.3
        if ((angle < M_PI * 1 / 3) && (angle > -M_PI * 1 / 3)) {
            const auto scale_factor = 0.5;

            vertex.y = scale_factor * vertex.y;
            vertex.x = scale_factor * vertex.x;
        }

        if ((angle < -M_PI * 2 / 3) || (angle > M_PI * 2 / 3)) {
            const auto scale_factor = 0.5;

            vertex.y = scale_factor * vertex.y;
            vertex.x = scale_factor * vertex.x;
        }
    }
}

BowlModel::BowlModel() {
    glm::vec3 pt1(0, BOWL_BOTTOM_RADIUS, 0);
    glm::vec3 pt2(0, MAX_RADIUS, 3583.06);

    double radius = len(pt1, pt2);
    glm::vec3 center(0, 0, 0);
    CircleCenter(pt1, pt2, radius, center);
    m_RadiusEdge.clear();
    double rad = calRad(center, pt1);
    m_RadiusEdge.push_back(pt1);
    double steps = M_PI / 3 / 30;
    for (int i = 1; i <= 30; i++) {
        float posy = center.y + radius * cos(rad + steps * i);
        float posz = center.z + radius * sin(rad + steps * i);
        m_RadiusEdge.push_back({0.0f, posy, posz});
    }

    // 初始化障碍物默认值
    obstacles_ = {
        float(BOWL_BOTTOM_RADIUS * 0.3), float(BOWL_BOTTOM_RADIUS * 0.2),
        float(BOWL_BOTTOM_RADIUS * 1.0), float(BOWL_BOTTOM_RADIUS * 0.5)};
}

BowlModel::~BowlModel() { m_RadiusEdge.clear(); }

void BowlModel::initModel() {
    int cameraCount = SVM_CAMERA_COUNT;
    m_Model.resize(cameraCount);
    for (int i = 0; i < cameraCount; i++) {
        init3DModel(i);
    }
}

void BowlModel::init3DModel(int index) {
    m_Model[index].clear();
    m_Model[index] = buildCylinderVertices(index);

    // 调整顶点位置基于障碍物, 此方法为测试使用，
    //AdjustVerticesBasedOnAngle(m_Model[index], obstacles_, MAX_RADIUS);
}

int BowlModel::getPointSize(int index) {
    if (index < 0 || index >= m_Model.size()) {
        std::cout << "Error Bow index:" << index << std::endl;
        return 0;
    }
    return m_Model[index].size();
}

std::vector<glm::vec3>
BowlModel::getUnitCircleVertices(int index, double radius, double z) {
    float sectorStep = M_PI / sectorCount; // 应该是笔误？ 是半个圆再分开
    float startAngle = 0.0f;
    switch (index) {
    case 1:
        startAngle = M_PI;
        break;
    case 2:
        startAngle = M_PI_2;
        break;
    case 3:
        startAngle = M_PI + M_PI_2;
        break;
    default:
        break;
    }
    glm::vec3 position;
    std::vector<glm::vec3> unitCircleVertices;
    for (int i = 0; i <= sectorCount; ++i) {
        float sectorAngle = startAngle + i * sectorStep;
        position.x = radius * cos(sectorAngle);
        position.y = radius * sin(sectorAngle);
        position.z = z;
        unitCircleVertices.push_back(position);
    }
    return unitCircleVertices;
}

std::vector<glm::vec3> BowlModel::buildCylinderVertices(int index) {
    std::vector<glm::vec3> vertices;
    // 获取上、下圆周点数组
    glm::vec3 pos = m_RadiusEdge[0];
    std::vector<glm::vec3> vctBot = getUnitCircleVertices(index, pos.y, pos.z);
    std::vector<glm::vec3> vctTop;

    // 底部圆形
    for (int i = 0; i < vctBot.size() - 1; ++i) {
        glm::vec3 position = vctBot[i + 1];
        vertices.push_back(position);

        position = glm::vec3(0.0f, 0.0f, 0.0f);
        vertices.push_back(position);

        position = vctBot[i];
        vertices.push_back(position);
    }

    // 圆弧过渡
    for (int j = 1; j <= 30; j++) {
        pos = m_RadiusEdge[j];
        vctTop.clear();
        vctTop = getUnitCircleVertices(index, pos.y, pos.z);
        // 圆弧侧面
        for (int i = 0; i < vctTop.size() - 1; ++i) {
            // 左三角形
            vertices.push_back(vctBot[i]);
            vertices.push_back(vctTop[i]);

            vertices.push_back(vctBot[i + 1]);

            // 右三角形
            vertices.push_back(vctBot[i + 1]);
            vertices.push_back(vctTop[i]);

            vertices.push_back(vctTop[i + 1]);
        }
        vctBot.clear();
        vctBot = vctTop;
    }

    // 圆台侧面
    vctTop = getUnitCircleVertices(
        index,
        m_RadiusEdge[30].y +
            (BOWL_BOTTOM_RADIUS - m_RadiusEdge[30].z) / sqrt(3),
        BOWL_HEIGHT);
    for (int i = 0; i < vctTop.size() - 1; ++i) {
        // 左三角形
        vertices.push_back(vctBot[i]);
        vertices.push_back(vctTop[i]);
        vertices.push_back(vctBot[i + 1]);

        // 右三角形：
        vertices.push_back(vctBot[i + 1]);
        vertices.push_back(vctTop[i]);
        vertices.push_back(vctTop[i + 1]);
    }

    assert(vctTop.size() >= 2);
    assert(vctBot.size() >= 2);

    return vertices;
}
