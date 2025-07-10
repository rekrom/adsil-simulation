#include <adapter/implementations/CarJsonAdapter.hpp>
#include <iostream>
namespace adapter
{

    CarJsonAdapter::CarJsonAdapter()
        : pointAdapter_(), vectorAdapter_(), deviceAdapter_()
    {
    }

    nlohmann::json CarJsonAdapter::toJson(const std::shared_ptr<Car> &car) const
    {
        nlohmann::json j;
        j["origin"] = pointAdapter_.toJson(car->getPosition());
        j["orientation"] = vectorAdapter_.toJson(car->getOrientation());

        // transmitters dizisi
        nlohmann::json txArr = nlohmann::json::array();
        for (const auto &tx : car->getTransmitters())
        {
            txArr.push_back(deviceAdapter_.toJson(*tx));
        }
        j["transmitters"] = txArr;

        // receivers dizisi
        nlohmann::json rxArr = nlohmann::json::array();
        for (const auto &rx : car->getReceivers())
        {
            rxArr.push_back(deviceAdapter_.toJson(*rx));
        }
        j["receivers"] = rxArr;

        CarDimension dim = car->getDimension();
        j["dimension"] = {
            {"length", dim.length},
            {"width", dim.width},
            {"height", dim.height}};

        return j;
    }

    std::shared_ptr<Car> CarJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        Point p = pointAdapter_.fromJson(j.at("origin"));
        Vector v = vectorAdapter_.fromJson(j.at("orientation"));
        Vector v_rad(RotationUtils::deg2rad(v.x()), RotationUtils::deg2rad(v.y()), RotationUtils::deg2rad(v.z()));

        CarDimension dim(0, 0, 0);
        auto dims = j.at("dimension");
        dim.length = dims.at("length");
        dim.width = dims.at("width");
        dim.height = dims.at("height");

        // std::cout << "[CAR] " << v.toString() << std::endl;
        auto node = std::make_shared<spatial::TransformNode>(spatial::Transform(p, v_rad));

        SharedVec<Device> tx;
        for (const auto &txJson : j.at("transmitters"))
        {
            Device d = deviceAdapter_.fromJson(txJson);
            tx.push_back(std::make_shared<Device>(std::move(d)));
        }

        SharedVec<Device> rx;
        for (const auto &rxJson : j.at("receivers"))
        {
            Device d = deviceAdapter_.fromJson(rxJson);
            rx.push_back(std::make_shared<Device>(std::move(d)));
        }

        CarConfig config(node, tx, rx, dim);

        return std::make_shared<Car>(config);
    }

} // namespace adapter
