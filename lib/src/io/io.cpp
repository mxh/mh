#include "mh/io/io.h"

#include <fstream>
#include <unordered_map>

#include "mh/gpu/texture_manager.h"

#define TRANSFER_TOBJ_VERT(IDX) \
    std::shared_ptr<Vertex> vert_##IDX; \
    auto vert_##IDX##_it = vert_idx_map.find(vert_##IDX##_idx.vertex_index); \
    if (vert_##IDX##_it != vert_idx_map.end()) \
    { \
        vert_##IDX = mesh->getVerts()[vert_##IDX##_it->second]; \
    } else { \
        mesh->getVerts().push_back(std::make_shared<Vertex>(Eigen::Vector3f(attrib.vertices[3 * vert_##IDX##_idx.vertex_index + 0], \
                                                                           attrib.vertices[3 * vert_##IDX##_idx.vertex_index + 1], \
                                                                           attrib.vertices[3 * vert_##IDX##_idx.vertex_index + 2]), mesh->getVerts().size())); \
        vert_##IDX = mesh->getVerts().back(); \
        vert_idx_map[vert_##IDX##_idx.vertex_index] = mesh->getVerts().size() - 1; \
    }

namespace mh
{

std::string get_absolute_material_path(const std::string & mat_file, const std::string & obj_file)
{
    if (mat_file[0] == '/')
    {
        return mat_file;
    } else {
        std::string obj_path = obj_file.substr(0, obj_file.find_last_of("/"));
        std::string abs_mat_file = obj_path + "/" + mat_file;
        return abs_mat_file;
    }
}

std::string get_parent_dir(const std::string & file)
{
    return file.substr(0, file.find_last_of("/")) + "/";
}

std::shared_ptr<Material> tobj_to_mh_material(const tinyobj::material_t & tobj_material, const std::string & obj_filename)
{
    auto mh_material = std::make_shared<Material>();

    mh_material->setName(tobj_material.name);

    mh_material->setDiffuse(Eigen::Vector3f(tobj_material.diffuse[0],
                                            tobj_material.diffuse[1],
                                            tobj_material.diffuse[2]));
    mh_material->setSpecular(Eigen::Vector3f(tobj_material.specular[0],
                                             tobj_material.specular[1],
                                             tobj_material.specular[2]));
    mh_material->setShininess(tobj_material.shininess);

    if (tobj_material.diffuse_texname != "")
    {
        mh_material->setDiffuseTexture(TextureManager::get_instance().load_texture(get_absolute_material_path(tobj_material.diffuse_texname, obj_filename)));
    }

    if (tobj_material.specular_texname != "")
    {
        mh_material->setSpecularTexture(TextureManager::get_instance().load_texture(get_absolute_material_path(tobj_material.specular_texname, obj_filename)));
    }

    return mh_material;
}

std::vector<std::shared_ptr<Mesh> > loadMeshesFromOBJ(const std::string & path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), get_parent_dir(path).c_str());

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        return std::vector<std::shared_ptr<Mesh> >();
    }

    std::vector<std::shared_ptr<Mesh> > meshes;
    for (size_t shape_idx = 0; shape_idx < shapes.size(); ++shape_idx)
    {
        // we want to split each part so that one part has one material, i.e.
        // not have face-specific materials but part-based
        std::map<int, std::vector<size_t> > mat_to_face_map;
        for (size_t face_idx = 0; face_idx < shapes[shape_idx].mesh.num_face_vertices.size(); ++face_idx)
        {
            mat_to_face_map[shapes[shape_idx].mesh.material_ids[face_idx]].push_back(face_idx);
        }

        for (auto & mat_facelist_pair : mat_to_face_map)
        {
            std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
            mesh->setName(shapes[shape_idx].name);

            // loop over triangles
            std::map<size_t, size_t> vert_idx_map;
            for (auto face_idx : mat_facelist_pair.second) {
                tinyobj::index_t vert_0_idx = shapes[shape_idx].mesh.indices[face_idx * 3 + 0];
                tinyobj::index_t vert_1_idx = shapes[shape_idx].mesh.indices[face_idx * 3 + 1];
                tinyobj::index_t vert_2_idx = shapes[shape_idx].mesh.indices[face_idx * 3 + 2];

                TRANSFER_TOBJ_VERT(0);
                TRANSFER_TOBJ_VERT(1);
                TRANSFER_TOBJ_VERT(2);

                auto face = std::make_shared<Face>(face_idx);
                mesh->getFaces().push_back(face);

                auto he_0 = std::make_shared<HalfEdge>();
                auto he_1 = std::make_shared<HalfEdge>();
                auto he_2 = std::make_shared<HalfEdge>();
                mesh->getHalfEdges().push_back(he_0);
                mesh->getHalfEdges().push_back(he_1);
                mesh->getHalfEdges().push_back(he_2);

                he_0->setVertex(vert_0.get());
                he_1->setVertex(vert_1.get());
                he_2->setVertex(vert_2.get());

                he_0->setFace(face.get());
                he_1->setFace(face.get());
                he_2->setFace(face.get());

                he_0->setNext(he_1.get());
                he_1->setNext(he_2.get());
                he_2->setNext(he_0.get());

                vert_2->setHalfEdge(he_0.get());
                vert_0->setHalfEdge(he_1.get());
                vert_1->setHalfEdge(he_2.get());

                face->setHalfEdge(he_0.get());

                face->getWedges().resize(3);

                auto wedge_0 = std::make_shared<Wedge>(vert_0.get(), face.get());
                auto wedge_1 = std::make_shared<Wedge>(vert_1.get(), face.get());
                auto wedge_2 = std::make_shared<Wedge>(vert_2.get(), face.get());
                face->getWedges()[0] = wedge_0.get();
                face->getWedges()[1] = wedge_1.get();
                face->getWedges()[2] = wedge_2.get();
                mesh->getWedges().push_back(wedge_0);
                mesh->getWedges().push_back(wedge_1);
                mesh->getWedges().push_back(wedge_2);

                if (vert_0_idx.texcoord_index >= 0)
                {
                    face->getWedges()[0]->setTextureCoords(makeFloat2(attrib.texcoords[2 * vert_0_idx.texcoord_index + 0],
                                                                      attrib.texcoords[2 * vert_0_idx.texcoord_index + 1]));
                }
                if (vert_1_idx.texcoord_index >= 0)
                {
                    face->getWedges()[1]->setTextureCoords(makeFloat2(attrib.texcoords[2 * vert_1_idx.texcoord_index + 0],
                                                                      attrib.texcoords[2 * vert_1_idx.texcoord_index + 1]));
                }
                if (vert_2_idx.texcoord_index >= 0)
                {
                    face->getWedges()[2]->setTextureCoords(makeFloat2(attrib.texcoords[2 * vert_2_idx.texcoord_index + 0],
                                                                      attrib.texcoords[2 * vert_2_idx.texcoord_index + 1]));
                }

                if (vert_0_idx.normal_index >= 0)
                {
                    face->getWedges()[0]->setNormal(Eigen::Vector3f(attrib.normals[3 * vert_0_idx.normal_index + 0],
                                                                    attrib.normals[3 * vert_0_idx.normal_index + 1],
                                                                    attrib.normals[3 * vert_0_idx.normal_index + 2]));
                }
                if (vert_1_idx.normal_index >= 0)
                {
                    face->getWedges()[1]->setNormal(Eigen::Vector3f(attrib.normals[3 * vert_1_idx.normal_index + 0],
                                                                    attrib.normals[3 * vert_1_idx.normal_index + 1],
                                                                    attrib.normals[3 * vert_1_idx.normal_index + 2]));
                }
                if (vert_2_idx.normal_index >= 0)
                {
                    face->getWedges()[2]->setNormal(Eigen::Vector3f(attrib.normals[3 * vert_2_idx.normal_index + 0],
                                                                    attrib.normals[3 * vert_2_idx.normal_index + 1],
                                                                    attrib.normals[3 * vert_2_idx.normal_index + 2]));
                }
            }

            // convert material
            if (mat_facelist_pair.first >= 0)
            {
                std::shared_ptr<Material> mesh_material = tobj_to_mh_material(materials[mat_facelist_pair.first], path);
                mesh->setMaterial(mesh_material);
            } else {
                mesh->setMaterial(std::make_shared<Material>());
            }

            meshes.push_back(mesh);
        }
    }

    return meshes;
}


std::vector<std::shared_ptr<Mesh>> loadMeshesFromBin(const std::string & path)
{
    std::ifstream in_file;
    in_file.open(path, std::ios::binary | std::ios::in);

    int n_meshes;
    in_file.read(reinterpret_cast<char*>(&n_meshes), sizeof(int));
    std::vector<std::shared_ptr<Mesh>> meshes(n_meshes);
    for (int mesh_idx = 0; mesh_idx < n_meshes; ++mesh_idx)
    {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

        int n_verts;
        in_file.read(reinterpret_cast<char*>(&n_verts), sizeof(int));
        mesh->getVerts().resize(n_verts);
        std::unordered_map<size_t, size_t> vert_idx_map;
        for (int vert_idx = 0; vert_idx < n_verts; ++vert_idx)
        {
            int idx;
            Eigen::Vector3f pos;
            in_file.read(reinterpret_cast<char*>(&idx), sizeof(int));
            in_file.read(reinterpret_cast<char*>(&pos(0)), sizeof(float));
            in_file.read(reinterpret_cast<char*>(&pos(1)), sizeof(float));
            in_file.read(reinterpret_cast<char*>(&pos(2)), sizeof(float));
            auto vert = std::make_shared<Vertex>(pos, idx);
    
            mesh->getVerts()[vert_idx] = vert;
            vert_idx_map[idx] = vert_idx;
        }

        int n_faces;
        in_file.read(reinterpret_cast<char*>(&n_faces), sizeof(int));
        mesh->getFaces().resize(n_faces);
        for (int face_idx = 0; face_idx < n_faces; ++face_idx)
        {
            int idx;
            int vert_0_idx;
            int vert_1_idx;
            int vert_2_idx;
            in_file.read(reinterpret_cast<char*>(&idx), sizeof(int));
            in_file.read(reinterpret_cast<char*>(&vert_0_idx), sizeof(int));
            in_file.read(reinterpret_cast<char*>(&vert_1_idx), sizeof(int));
            in_file.read(reinterpret_cast<char*>(&vert_2_idx), sizeof(int));

            auto face = std::make_shared<Face>(idx);
            mesh->getFaces()[face_idx] = face;

            auto vert_0 = mesh->getVerts()[vert_idx_map[vert_0_idx]];
            auto vert_1 = mesh->getVerts()[vert_idx_map[vert_1_idx]];
            auto vert_2 = mesh->getVerts()[vert_idx_map[vert_2_idx]];

            auto he_0 = std::make_shared<HalfEdge>();
            auto he_1 = std::make_shared<HalfEdge>();
            auto he_2 = std::make_shared<HalfEdge>();
            mesh->getHalfEdges().push_back(he_0);
            mesh->getHalfEdges().push_back(he_1);
            mesh->getHalfEdges().push_back(he_2);

            he_0->setVertex(vert_0.get());
            he_1->setVertex(vert_1.get());
            he_2->setVertex(vert_2.get());

            he_0->setFace(face.get());
            he_1->setFace(face.get());
            he_2->setFace(face.get());

            he_0->setNext(he_1.get());
            he_1->setNext(he_2.get());
            he_2->setNext(he_0.get());

            vert_2->setHalfEdge(he_0.get());
            vert_0->setHalfEdge(he_1.get());
            vert_1->setHalfEdge(he_2.get());

            face->setHalfEdge(he_0.get());

            face->getWedges().resize(3);

            auto wedge_0 = std::make_shared<Wedge>(vert_0.get(), face.get());
            auto wedge_1 = std::make_shared<Wedge>(vert_1.get(), face.get());
            auto wedge_2 = std::make_shared<Wedge>(vert_2.get(), face.get());
            face->getWedges()[0] = wedge_0.get();
            face->getWedges()[1] = wedge_1.get();
            face->getWedges()[2] = wedge_2.get();
            mesh->getWedges().push_back(wedge_0);
            mesh->getWedges().push_back(wedge_1);
            mesh->getWedges().push_back(wedge_2);

        }

        mesh->setMaterial(std::make_shared<Material>());
        meshes[mesh_idx] = mesh;
    }

    return meshes;
}

void saveMeshesToBin(const std::string & filename, const std::vector<std::shared_ptr<Mesh>> & meshes)
{
    std::ofstream out_file;
    out_file.open(filename, std::ios::binary | std::ios::out);

    int n_meshes = meshes.size();
    out_file.write(reinterpret_cast<char*>(&n_meshes), sizeof(int));
    for (size_t mesh_idx = 0; mesh_idx < meshes.size(); ++mesh_idx)
    {
        int n_verts = meshes[mesh_idx]->getVerts().size();
        out_file.write(reinterpret_cast<char*>(&n_verts), sizeof(int));
        for (const auto & vert : meshes[mesh_idx]->getVerts())
        {
            int idx = vert->idx();
            Eigen::Vector3f pos = vert->getPosition();
            out_file.write(reinterpret_cast<char*>(&idx), sizeof(int));
            out_file.write(reinterpret_cast<char*>(&pos(0)), sizeof(float));
            out_file.write(reinterpret_cast<char*>(&pos(1)), sizeof(float));
            out_file.write(reinterpret_cast<char*>(&pos(2)), sizeof(float));
        }

        int n_faces = meshes[mesh_idx]->getFaces().size();
        out_file.write(reinterpret_cast<char*>(&n_faces), sizeof(int));
        for (const auto & face : meshes[mesh_idx]->getFaces())
        {
            int idx = face->idx();
            int vert_0_idx = face->getVertex(0)->idx();
            int vert_1_idx = face->getVertex(1)->idx();
            int vert_2_idx = face->getVertex(2)->idx();
            out_file.write(reinterpret_cast<char*>(&idx), sizeof(int));
            out_file.write(reinterpret_cast<char*>(&vert_0_idx), sizeof(int));
            out_file.write(reinterpret_cast<char*>(&vert_1_idx), sizeof(int));
            out_file.write(reinterpret_cast<char*>(&vert_2_idx), sizeof(int));
        }
    }

    out_file.close();
}

} // namespace mh
