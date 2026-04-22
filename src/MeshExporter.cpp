#include "biomesh/MeshExporter.hpp"
#include "biomesh/GiDExporter.hpp"
#include "biomesh/STLExporter.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace biomesh {

bool MeshExporter::exportMesh(const HexMesh& mesh, const std::string& filename, const std::string& format) {
    std::string normalized = format;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (normalized == "gid") {
        return GiDExporter::exportToGiD(mesh, filename);
    }
    if (normalized == "stl") {
        return STLExporter::exportToSTL(mesh, filename);
    }

    std::cerr << "Error: Unsupported export format: " << format << "\n";
    return false;
}

} // namespace biomesh
