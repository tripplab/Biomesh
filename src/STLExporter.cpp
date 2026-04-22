#include "biomesh/STLExporter.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

namespace biomesh {
namespace {

struct Vec3 {
    double x;
    double y;
    double z;
};

Vec3 sub(const Point3D& a, const Point3D& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
}

void normalize(Vec3& v) {
    const double n = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (n > 1e-16) {
        v.x /= n;
        v.y /= n;
        v.z /= n;
    } else {
        v = {0.0, 0.0, 0.0};
    }
}

struct FaceKey {
    std::array<int, 4> nodes;

    bool operator==(const FaceKey& other) const {
        return nodes == other.nodes;
    }
};

struct FaceKeyHash {
    std::size_t operator()(const FaceKey& key) const {
        std::size_t h = 0;
        for (int n : key.nodes) {
            h ^= std::hash<int>{}(n) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

FaceKey makeCanonicalFaceKey(const std::array<int, 4>& face) {
    std::array<int, 4> sorted = face;
    std::sort(sorted.begin(), sorted.end());
    return {sorted};
}

} // namespace

bool STLExporter::exportToSTL(const HexMesh& mesh, const std::string& filename) {
    if (mesh.nodes.empty() || mesh.elements.empty()) {
        std::cerr << "Error: Cannot export empty mesh to STL format\n";
        return false;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << "\n";
        return false;
    }

    static const std::array<std::array<int, 4>, 6> hexFaces = {{
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {0, 1, 5, 4},
        {1, 2, 6, 5},
        {2, 3, 7, 6},
        {3, 0, 4, 7},
    }};

    std::unordered_map<FaceKey, std::array<int, 4>, FaceKeyHash> boundaryFaces;

    for (const auto& element : mesh.elements) {
        for (const auto& localFace : hexFaces) {
            std::array<int, 4> globalFace = {
                element[localFace[0]],
                element[localFace[1]],
                element[localFace[2]],
                element[localFace[3]],
            };

            FaceKey key = makeCanonicalFaceKey(globalFace);
            auto it = boundaryFaces.find(key);
            if (it == boundaryFaces.end()) {
                boundaryFaces.emplace(key, globalFace);
            } else {
                boundaryFaces.erase(it);
            }
        }
    }

    outFile << "solid biomesh\n";
    outFile << std::fixed << std::setprecision(6);

    for (const auto& entry : boundaryFaces) {
        const auto& quad = entry.second;

        const Point3D& p0 = mesh.nodes[quad[0]];
        const Point3D& p1 = mesh.nodes[quad[1]];
        const Point3D& p2 = mesh.nodes[quad[2]];
        const Point3D& p3 = mesh.nodes[quad[3]];

        const std::array<std::array<const Point3D*, 3>, 2> tris = {{
            {{&p0, &p1, &p2}},
            {{&p0, &p2, &p3}},
        }};

        for (const auto& tri : tris) {
            Vec3 u = sub(*tri[1], *tri[0]);
            Vec3 v = sub(*tri[2], *tri[0]);
            Vec3 n = cross(u, v);
            normalize(n);

            outFile << "  facet normal " << n.x << " " << n.y << " " << n.z << "\n";
            outFile << "    outer loop\n";
            outFile << "      vertex " << tri[0]->x << " " << tri[0]->y << " " << tri[0]->z << "\n";
            outFile << "      vertex " << tri[1]->x << " " << tri[1]->y << " " << tri[1]->z << "\n";
            outFile << "      vertex " << tri[2]->x << " " << tri[2]->y << " " << tri[2]->z << "\n";
            outFile << "    endloop\n";
            outFile << "  endfacet\n";
        }
    }

    outFile << "endsolid biomesh\n";
    outFile.close();

    return true;
}

} // namespace biomesh
