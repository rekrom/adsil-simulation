#include <adapter/implementations/CarJsonAdapter.hpp>
#include <iostream>
namespace adapter
{

    CarJsonAdapter::CarJsonAdapter()
        : pointAdapter_(), vectorAdapter_(), deviceAdapter_()
    {
    }

    nlohmann::json CarJsonAdapter::toJson(const Car &car) const
    {
        nlohmann::json j;
        j["position"] = pointAdapter_.toJson(car.getPosition());
        j["orientation"] = vectorAdapter_.toJson(car.getOrientation());

        // transmitters dizisi
        nlohmann::json txArr = nlohmann::json::array();
        for (const auto &tx : car.getTransmitters())
        {
            txArr.push_back(deviceAdapter_.toJson(*tx));
        }
        j["transmitters"] = txArr;

        // receivers dizisi
        nlohmann::json rxArr = nlohmann::json::array();
        for (const auto &rx : car.getReceivers())
        {
            rxArr.push_back(deviceAdapter_.toJson(*rx));
        }
        j["receivers"] = rxArr;

        return j;
    }

    Car CarJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        Point p = pointAdapter_.fromJson(j.at("position"));
        Vector v = vectorAdapter_.fromJson(j.at("orientation"));
        Vector v_rad(RotationUtils::deg2rad(v.x()), RotationUtils::deg2rad(v.y()), RotationUtils::deg2rad(v.z()));

        // std::cout << "[CAR] " << v.toString() << std::endl;
        auto node = std::make_shared<spatial::TransformNode>(Transform(p, v_rad));

        std::vector<std::shared_ptr<Device>> tx;
        for (const auto &txJson : j.at("transmitters"))
        {
            std::cout << "hello" << std::endl;
            Device d = deviceAdapter_.fromJson(txJson);
            tx.push_back(std::make_shared<Device>(std::move(d)));
        }

        std::vector<std::shared_ptr<Device>> rx;
        for (const auto &rxJson : j.at("receivers"))
        {
            Device d = deviceAdapter_.fromJson(rxJson);
            rx.push_back(std::make_shared<Device>(std::move(d)));
        }

        CarConfig config(node, tx, rx);

        return Car(config);
    }

} // namespace adapter
