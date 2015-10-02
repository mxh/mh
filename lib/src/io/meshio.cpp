#include "io/meshio.h"

namespace lp
{

Mesh convertFromAssimp(const aiMesh * inputMesh)
{
    std::vector<Eigen::Vector3f> vertData;
    std::vector<Eigen::Vector3f> normalData;
    std::vector<Eigen::Vector3i> faceData;

    for (size_t i = 0; i < inputMesh->mNumVertices; ++i)
    {
        vertData.push_back  (Eigen::Vector3f(inputMesh->mVertices[i].x,
                                             inputMesh->mVertices[i].y,
                                             inputMesh->mVertices[i].z));
        normalData.push_back(Eigen::Vector3f(inputMesh->mNormals[i].x,
                                             inputMesh->mNormals[i].y,
                                             inputMesh->mNormals[i].z));
    }

    for (size_t i = 0; i < inputMesh->mNumFaces; ++i)
    {
        const aiFace * face = &(inputMesh->mFaces[i]);
        faceData.push_back(Eigen::Vector3i(face->mIndices[0],
                                           face->mIndices[1],
                                           face->mIndices[2]));
    }

    Mesh mesh(vertData, normalData, faceData);

    return mesh;
}

std::vector<std::shared_ptr<Mesh> > loadMeshesFromOBJ(std::string path)
{
    std::vector<std::shared_ptr<Mesh> > meshes;
    Assimp::Importer importer;

    const aiScene * scene = importer.ReadFile(path.c_str(),
        aiProcess_JoinIdenticalVertices
    );

    LP_ASSERT(scene);

    meshes.reserve(scene->mNumMeshes);
    for (size_t i = 0; i < scene->mNumMeshes; ++i)
    {
        auto mesh = std::make_shared<Mesh>(convertFromAssimp(scene->mMeshes[i]));
        meshes.push_back(mesh);
    }

    return meshes;
}


} // namespace lp
