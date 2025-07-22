#include <simulation/interfaces/ISimulationScene.hpp>

class PointCloudScene : public ISimulationScene
{
public:
    explicit PointCloudScene(const std::string &jsonPath);
    std::shared_ptr<math::PointCloud> getMergedPointCloud(int quality = 2048) const override;
    double getTimestamp() const override;

private:
    std::shared_ptr<math::PointCloud> cloud_;
    double timestamp_;
};
