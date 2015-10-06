#ifndef MESHIO_H
#define MESHIO_H 

#include <string>

#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "mh/3d/mesh.h"

namespace mh
{

Mesh                                convertFromAssimp (const aiMesh * inputMesh);
std::vector<std::shared_ptr<Mesh> > loadMeshesFromOBJ (std::string path);

} // namespace mh
#endif /* MESHIO_H */
