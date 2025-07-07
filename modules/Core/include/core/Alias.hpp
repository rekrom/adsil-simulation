#pragma once

#include <memory>
#include <vector>

template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T>
using SharedVec = std::vector<std::shared_ptr<T>>;
