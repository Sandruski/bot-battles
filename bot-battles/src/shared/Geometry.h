#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

namespace sand {
struct Vertex {

    Vertex()
        : m_position(0.0f, 0.0f)
        , m_textureCoords(0.0f, 0.0f)
    {
    }

    glm::vec2 m_position;
    glm::vec2 m_textureCoords;
};
}

#endif
