#include "./material_base.hpp"

namespace builtin_scene
{
  using namespace std;

  void Material::SetGlobalDefines(const string &define)
  {
    if (find(GlobalDefines.begin(), GlobalDefines.end(), define) == GlobalDefines.end())
      GlobalDefines.push_back(define);
  }

  void Material::UnsetGlobalDefines(const string &define)
  {
    auto it = find(GlobalDefines.begin(), GlobalDefines.end(), define);
    if (it != GlobalDefines.end())
      GlobalDefines.erase(it);
  }

  const vector<string> Material::mixDefines(const vector<string> &baseDefines,
                                            const vector<string> &definesToAdd) const
  {
    vector<string> result = definesToAdd;

    // Ignore duplicates.
    for (const auto &define : baseDefines)
    {
      if (find(result.begin(), result.end(), define) == result.end())
        result.push_back(define);
    }
    return result;
  }
}
