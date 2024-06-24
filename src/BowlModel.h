#ifndef BOW_MODEL_H
#define BOW_MODEL_H

#include <glm/glm.hpp>
#include <vector>

//  变更方式：删除 AppConfig， 删除 namespace，使用glm 取代 filavec3,
//  先用4个摄像头
// todo 尺寸缩减到1/1000

const auto SVM_CAMERA_COUNT = 4;
class BowlModel;
using BowlModelPtr = std::shared_ptr<BowlModel>;

class BowlModel {
  public:
    static BowlModelPtr create() {
        return std::shared_ptr<BowlModel>(new BowlModel);
    }

    ~BowlModel();
    BowlModel(const BowlModel &) = delete;
    BowlModel &operator=(const BowlModel &) = delete;
    
    void initModel();

    const std::vector<std::vector<glm::vec3>> &getModel() { return m_Model; }

    int getPointSize(int index);

  private:
    const int sectorCount = 36;
    std::vector<glm::vec3> getUnitCircleVertices(int index, double radius,
                                                 double z);
    std::vector<glm::vec3> buildCylinderVertices(int index);

    std::vector<std::vector<glm::vec3>> m_Model;
    std::vector<glm::vec3> m_RadiusEdge;

    BowlModel();
    void init3DModel(int index);
};
#endif
