#pragma once

namespace adapter
{

    // Ortak polymorphic base sınıf — RTTI için gerekli
    class IJsonAdapterBase
    {
    public:
        virtual ~IJsonAdapterBase() = default;
    };

} // namespace adapter
