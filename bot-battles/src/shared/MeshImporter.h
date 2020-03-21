#ifndef __MESH_IMPORTER_H__
#define __MESH_IMPORTER_H__

namespace sand {

//----------------------------------------------------------------------------------------------------
class MeshImporter {
public:
    void Load(const std::array<Vertex, 4>& vertices, const std::array<U32, 6>& indices, U32& VAO, U32& VBO, U32& EBO) const;
    void ReLoad(const std::array<Vertex, 4>& vertices, U32 VBO) const;
    void UnLoad(U32& VAO, U32& VBO, U32& EBO) const;
};
}

#endif
