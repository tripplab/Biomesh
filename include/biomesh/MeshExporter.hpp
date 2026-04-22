#pragma once

#include "biomesh/HexMesh.hpp"
#include <string>

namespace biomesh {

class MeshExporter {
public:
    /**
     * @brief Export mesh to the requested format.
     * @param mesh Mesh data.
     * @param filename Output path.
     * @param format Output format (gid|stl).
     * @return true on success, false on error.
     */
    static bool exportMesh(const HexMesh& mesh, const std::string& filename, const std::string& format);
};

} // namespace biomesh
