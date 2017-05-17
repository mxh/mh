#include <memory>
#include <string>

#include "mh/3d/mesh.h"

#include "mh/ext/tinyobjloader/tiny_obj_loader.h"

namespace mh
{

std::vector<std::shared_ptr<Mesh> > loadMeshesFromOBJ(const std::string & path);
std::vector<std::shared_ptr<Mesh> > loadMeshesFromBin(const std::string & path);
void saveMeshesToBin(const std::string & filename, const std::vector<std::shared_ptr<Mesh>> & meshes);

} // namespace mh
