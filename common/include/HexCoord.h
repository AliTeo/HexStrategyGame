#pragma once

#include <cmath>
#include <vector>
#include <nlohmann/json.hpp>

/**
 * Axial coordinate system for hexagonal grid
 * Uses (q, r) coordinates where:
 * - q: column (increases right)
 * - r: row (increases down-right)
 * - s: implicit third coordinate (s = -q - r)
 */
class HexCoord {
public:
    int q; // Column
    int r; // Row

    HexCoord();
    HexCoord(int q, int r);

    // Coordinate operations
    bool operator==(const HexCoord& other) const;
    bool operator!=(const HexCoord& other) const;
    HexCoord operator+(const HexCoord& other) const;
    HexCoord operator-(const HexCoord& other) const;
    HexCoord operator*(int scale) const;

    // Get the implicit s coordinate
    int s() const;

    // Manhattan distance between hexes
    int distance(const HexCoord& other) const;

    // Get all 6 neighbors
    std::vector<HexCoord> neighbors() const;

    // Get neighbor in specific direction (0-5)
    HexCoord neighbor(int direction) const;

    // Check if coordinate is within bounds
    bool isValid(int gridRadius) const;

    // Get all hexes within range
    std::vector<HexCoord> hexesInRange(int range) const;

    // Serialization
    nlohmann::json toJson() const;
    static HexCoord fromJson(const nlohmann::json& j);

    // Direction vectors (6 directions)
    static const HexCoord DIRECTIONS[6];
};

// Hash function for use in unordered_map
namespace std {
    template<>
    struct hash<HexCoord> {
        size_t operator()(const HexCoord& coord) const {
            return hash<int>()(coord.q) ^ (hash<int>()(coord.r) << 1);
        }
    };
}
