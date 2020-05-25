#ifndef __SHADERS_H__
#define __SHADERS_H__

static const char* defaultVertexShaderSource = "#version 330 core\n"
                                               "layout(location = 0) in vec2 position;\n"
                                               "layout(location = 1) in vec2 textureCoords;\n"

                                               "layout(location = 2) in vec2 spriteCoords[4];\n"
                                               "layout(location = 6) in mat4 model;\n"

                                               "out vec2 TextureCoords;\n"

                                               "uniform mat4 projection;\n"

                                               "void main()\n"
                                               "{\n"
                                               "gl_Position = projection * model * vec4(position, 0.0, 1.0);\n"
                                               "if (position.x == -0.5 && position.y == 0.5)\n"
                                               "{\n"
                                               "TextureCoords = spriteCoords[0];\n"
                                               "}\n"
                                               "else if (position.x == 0.5 && position.y == 0.5)\n"
                                               "{\n"
                                               "TextureCoords = spriteCoords[1];\n"
                                               "}\n"
                                               "else if (position.x == -0.5 && position.y == -0.5)\n"
                                               "{\n"
                                               "TextureCoords = spriteCoords[2];\n"
                                               "}\n"
                                               "else if (position.x == 0.5 && position.y == -0.5)\n"
                                               "{\n"
                                               "TextureCoords = spriteCoords[3];\n"
                                               "}\n"
                                               "else\n"
                                               "{\n"
                                               "TextureCoords = textureCoords;\n"
                                               "}\n"
                                               "}\0";

static const char* defaultFragmentShaderSource = "#version 330 core\n"
                                                 "out vec4 FragColor;\n"

                                                 "in vec2 TextureCoords;\n"
                                                 "uniform sampler2D sprite;\n"
                                                 "uniform vec4 color;\n"
                                                 "uniform float pct;\n"

                                                 "void main()\n"
                                                 "{\n"
                                                 "FragColor = vec4(1.0,0.0,0.0,1.0);\n"
                                                 "FragColor = mix(texture(sprite, TextureCoords), color, pct);\n"
                                                 "if (FragColor.a == 0.0)\n"
                                                 "{\n"
                                                 "discard;\n"
                                                 "}\n"
                                                 "}\0";
#endif
