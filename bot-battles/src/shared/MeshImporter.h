#ifndef __MESH_IMPORTER_H__
#define __MESH_IMPORTER_H__

#include "MeshResource.h"

namespace sand {

//----------------------------------------------------------------------------------------------------
class MeshImporter {
public:
    void Load(U32& VAO, U32& VBO, U32& instanceVBO) const;
    void ReLoadVertices(const std::vector<MeshResource::Vertex>& vertices, U32 VBO, bool isStatic) const;
    void ReLoadInstances(const std::vector<MeshResource::Instance>& instances, U32 instanceVBO, bool isStatic) const;
    void UnLoad(U32& VAO, U32& VBO, U32& instanceVBO) const;
};
}

#endif
