#include <string>
#include <vector>
#include <filesystem>
#include "../../../src/object.hpp"

extern "C"
{
void __current_path();
extern std::vector<std::string> _FUNCTIONS;
}

namespace anole
{
class PathObject : public Object, public std::enable_shared_from_this<PathObject>
{
  public:
    PathObject(std::filesystem::path path);
    Address load_member(const std::string &name) override;
    std::string to_str() override;

    std::filesystem::path &path();

  private:
    std::filesystem::path path_;
};
}