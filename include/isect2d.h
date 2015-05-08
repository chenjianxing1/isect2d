#pragma once

#include <cmath>
#include <algorithm>
#include <limits>
#include <set>
#include <iostream>

enum Dimension {
    X, Y, MAX_DIM
};

#include "aabb.h"
#include "vec2.h"
#include "sap.h"
#include "obb.h"

inline static std::set<std::pair<int, int>> intersect(const std::vector<isect2d::AABB>& _aabbs) {
    std::set<std::pair<int, int>> pairs;

    if (_aabbs.size() == 0) {
        return pairs;
    }
    
    for (size_t i = 0; i < _aabbs.size(); ++i) {
        for (size_t j = i + 1; j < _aabbs.size(); ++j) {
            if (_aabbs[i].intersect(_aabbs[j])) {
                pairs.insert({ i, j });
            }
        }
    }

    return pairs;
}

