#include "HexCoord.h"
#include <algorithm>

// Direction vectors for axial coordinates
const HexCoord HexCoord::DIRECTIONS[6] = {
    HexCoord(1, 0),   // East
    HexCoord(1, -1),  // Northeast
    HexCoord(0, -1),  // Northwest
    HexCoord(-1, 0),  // West
    HexCoord(-1, 1),  // Southwest
    HexCoord(0, 1)    // Southeast
};

HexCoord::HexCoord() : q(0), r(0) {}

HexCoord::HexCoord(int q, int r) : q(q), r(r) {}

bool HexCoord::operator==(const HexCoord& other) const {
    return q == other.q && r == other.r;
}

bool HexCoord::operator!=(const HexCoord& other) const {
    return !(*this == other);
}

HexCoord HexCoord::operator+(const HexCoord& other) const {
    return HexCoord(q + other.q, r + other.r);
}

HexCoord HexCoord::operator-(const HexCoord& other) const {
    return HexCoord(q - other.q, r - other.r);
}

HexCoord HexCoord::operator*(int scale) const {
    return HexCoord(q * scale, r * scale);
}

int HexCoord::s() const {
    return -q - r;
}

int HexCoord::distance(const HexCoord& other) const {
    return (std::abs(q - other.q) + std::abs(r - other.r) + std::abs(s() - other.s())) / 2;
}

std::vector<HexCoord> HexCoord::neighbors() const {
    std::vector<HexCoord> result;
    result.reserve(6);
    for (int i = 0; i < 6; ++i) {
        result.push_back(*this + DIRECTIONS[i]);
    }
    return result;
}

HexCoord HexCoord::neighbor(int direction) const {
    if (direction < 0 || direction >= 6) {
        return *this;
    }
    return *this + DIRECTIONS[direction];
}

bool HexCoord::isValid(int gridRadius) const {
    return std::abs(q) <= gridRadius && 
           std::abs(r) <= gridRadius && 
           std::abs(s()) <= gridRadius;
}

std::vector<HexCoord> HexCoord::hexesInRange(int range) const {
    std::vector<HexCoord> results;
    
    for (int dq = -range; dq <= range; ++dq) {
        for (int dr = std::max(-range, -dq - range); 
             dr <= std::min(range, -dq + range); ++dr) {
            results.push_back(HexCoord(q + dq, r + dr));
        }
    }
    
    return results;
}

nlohmann::json HexCoord::toJson() const {
    return nlohmann::json{{"q", q}, {"r", r}};
}

HexCoord HexCoord::fromJson(const nlohmann::json& j) {
    return HexCoord(j["q"].get<int>(), j["r"].get<int>());
}
