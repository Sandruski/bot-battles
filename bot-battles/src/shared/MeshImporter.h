#ifndef __MESH_IMPORTER_H__
#define __MESH_IMPORTER_H__

#include "MeshResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MeshImporter {
public:
    void Load(const std::vector<MeshResource::Vertex>& vertices, const std::vector<U32>& indices, U32& VAO, U32& VBO, U32& EBO) const;
    void ReLoad(const std::vector<MeshResource::Vertex>& vertices, U32 VBO) const;
    void UnLoad(U32& VAO, U32& VBO, U32& EBO) const;
};
}

#endif
