#pragma once

#include "SimulationScene.hpp"
#include <math/math.hpp>
#include <geometry/implementations/Device.hpp>
#include <core/Alias.hpp>

#include <vector>
#include <tuple>
#include <memory>
#include <simulation/interfaces/IFrameObserver.hpp>
#include <simulation/interfaces/ISolver.hpp>

namespace simulation
{
    class SignalSolver2 : public ISolver
    {
    public:
        explicit SignalSolver2(std::shared_ptr<SimulationScene> scene);

        // Runs the solver and returns closest points for each (Tx, Rx) pair
        std::shared_ptr<math::PointCloud> solve() override;

    private:
        static constexpr size_t REQUIRED_RECEIVER_COUNT = 4;
        static constexpr float EPSILON = 1e-6f;

        struct TofMatrix
        {
            std::vector<std::vector<float>> values;
            size_t txCount;
            size_t rxCount;

            TofMatrix(size_t tx_size, size_t rx_size)
                : values(tx_size, std::vector<float>(rx_size, 0.0f)), txCount(tx_size), rxCount(rx_size) {}

            float &operator()(size_t tx, size_t rx) { return values[tx][rx]; }
            const float &operator()(size_t tx, size_t rx) const { return values[tx][rx]; }
        };

        // Helpers (optimized variants)
        inline math::Point findClosestPointInSet(const std::shared_ptr<math::PointCloud> &points,
                                                 const math::Point &txPos,
                                                 const math::Point &rxPos) const;

        bool isValidTofRow(const TofMatrix &tofMatrix, size_t txIndex) const;
        std::pair<math::Point, math::Point> calculateAdsilPositions(
            const TofMatrix &tofMatrix,
            size_t txIndex,
            const SharedVec<Device> &receivers) const;

        std::shared_ptr<math::PointCloud> solveAdsilTrilateration(const TofMatrix &tofMatrix);

        std::shared_ptr<SimulationScene> scene_;
        size_t solveCount_ = 0;
    };
} // namespace simulation
