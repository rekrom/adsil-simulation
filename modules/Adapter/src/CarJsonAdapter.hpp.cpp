#include <adapter/CarJsonAdapter.hpp>

namespace adapter
{
    CarJsonAdapter::CarJsonAdapter()
        : pointAdapter_(), vectorAdapter_()
    {
    }

    nlohmann::json CarJsonAdapter::toJson(const Car &car) const
    {
        nlohmann::json j;
        j["position"] = pointAdapter_.toJson(car.getPosition());
        j["orientation"] = vectorAdapter_.toJson(car.getOrientation());

        // TODO: transmitters ve receivers adapter ile serialize edilecek
        // Şimdilik boş dizi olarak bırakalım
        j["transmitters"] = nlohmann::json::array();
        j["receivers"] = nlohmann::json::array();

        return j;
    }

    Car CarJsonAdapter::fromJson(const nlohmann::json &j) const
    {
        Point p = pointAdapter_.fromJson(j.at("position"));
        Vector v = vectorAdapter_.fromJson(j.at("orientation"));

        // TODO: transmitters ve receivers için uygun adapter/factory kullanılarak nesneler oluşturulacak
        std::vector<std::shared_ptr<Device>> tx;
        std::vector<std::shared_ptr<Device>> rx;

        return Car(p, v, tx, rx);
    }
} // namespace adapter