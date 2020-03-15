#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

static const F32 quadVertices[] = {
    // Positions // Texture coords
    0.5f, 0.5f, 1.0f, 1.0f, // top right
    0.5f, -0.5f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f, 1.0f // top left
};

static const U32 quadIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
};

#endif
