#pragma once

#include "biomesh/HexMesh.hpp"
#include <string>

namespace biomesh {

/**
 * @brief ASCII STL exporter for hexahedral meshes.
 *
 * Exports only the boundary surface as triangulated facets.
 */
class STLExporter {
public:
    /**
     * @brief Export hexahedral mesh boundary to ASCII STL format.
     * @param mesh The hexahedral mesh to export.
     * @param filename Output file path (should end with .stl).
     * @return true if export was successful, false otherwise.
     */
    static bool exportToSTL(const HexMesh& mesh, const std::string& filename);
};

} // namespace biomesh
