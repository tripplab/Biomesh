#include <gtest/gtest.h>
#include "biomesh/HexMesh.hpp"
#include "biomesh/GiDExporter.hpp"
#include "biomesh/STLExporter.hpp"
#include "biomesh/MeshExporter.hpp"
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>

using namespace biomesh;

namespace {

HexMesh makeSingleHexMesh() {
    HexMesh mesh;
    mesh.nodes = {
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {1.0, 1.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
        {1.0, 0.0, 1.0},
        {1.0, 1.0, 1.0},
        {0.0, 1.0, 1.0},
    };
    mesh.elements.push_back({0, 1, 2, 3, 4, 5, 6, 7});
    return mesh;
}

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

int countOccurrences(const std::string& text, const std::string& needle) {
    int count = 0;
    std::string::size_type pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

} // namespace

TEST(ExporterTest, GiDExporterWritesExpectedSections) {
    HexMesh mesh = makeSingleHexMesh();
    const std::string file = "test_single_hex.msh";

    ASSERT_TRUE(GiDExporter::exportToGiD(mesh, file));

    std::string contents = readFile(file);
    EXPECT_NE(contents.find("MESH dimension 3 ElemType Hexahedra Nnode 8"), std::string::npos);
    EXPECT_NE(contents.find("Coordinates"), std::string::npos);
    EXPECT_NE(contents.find("Elements"), std::string::npos);

    std::remove(file.c_str());
}

TEST(ExporterTest, STLExporterWritesExpectedFacetCountForSingleHex) {
    HexMesh mesh = makeSingleHexMesh();
    const std::string file = "test_single_hex.stl";

    ASSERT_TRUE(STLExporter::exportToSTL(mesh, file));

    std::string contents = readFile(file);
    EXPECT_NE(contents.find("solid biomesh"), std::string::npos);
    EXPECT_EQ(countOccurrences(contents, "facet normal"), 12);

    std::remove(file.c_str());
}

TEST(ExporterTest, MeshExporterDispatchesByFormatAndRejectsUnsupported) {
    HexMesh mesh = makeSingleHexMesh();

    const std::string gidFile = "dispatch_gid.msh";
    const std::string stlFile = "dispatch_stl.stl";

    EXPECT_TRUE(MeshExporter::exportMesh(mesh, gidFile, "gid"));
    EXPECT_TRUE(MeshExporter::exportMesh(mesh, stlFile, "stl"));
    EXPECT_TRUE(MeshExporter::exportMesh(mesh, gidFile, "GiD"));
    EXPECT_TRUE(MeshExporter::exportMesh(mesh, stlFile, "STL"));
    EXPECT_FALSE(MeshExporter::exportMesh(mesh, "bad.out", "json"));

    std::remove(gidFile.c_str());
    std::remove(stlFile.c_str());
}
