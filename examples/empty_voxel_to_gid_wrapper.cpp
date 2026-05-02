#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <process.h>
#define EXECVP _execvp
#else
#include <unistd.h>
#define EXECVP execvp
#endif

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <pdb_file> <voxel_size> <output_file> [padding] [inflate_factor] [format]\n";
        return 1;
    }

    std::string padding = (argc > 4) ? argv[4] : "2.0";
    std::string inflateFactor = (argc > 5) ? argv[5] : "1.0";
    std::string format = (argc > 6) ? argv[6] : "gid";

    std::filesystem::path biomeshPath = std::filesystem::path(argv[0]).parent_path() / "biomesh";

    std::vector<std::string> args = {
        biomeshPath.string(),
        argv[1],
        argv[2],
        "--mesh", "empty",
        "--output", argv[3],
        "--padding", padding,
        "--inflate-factor", inflateFactor,
        "--format", format
    };

    std::vector<char*> execArgs;
    execArgs.reserve(args.size() + 1);
    for (auto& arg : args) {
        execArgs.push_back(const_cast<char*>(arg.c_str()));
    }
    execArgs.push_back(nullptr);

    std::cerr << "[compat] empty_voxel_to_gid is deprecated; forwarding to biomesh.\n";
    int result = EXECVP(execArgs[0], execArgs.data());
    std::cerr << "Error: Failed to execute compat biomesh wrapper at: " << biomeshPath << "\n";
    return (result == -1) ? 1 : result;
}
