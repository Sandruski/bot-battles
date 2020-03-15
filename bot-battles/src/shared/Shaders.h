#ifndef __SHADERS_H__
#define __SHADERS_H__

static const char* vertexShaderSource = "#version 330 core\n"
                                        "layout(location = 0) in vec3 aPos;\n"
                                        "layout(location = 1) in vec2 aTexCoord;\n"

                                        "out vec2 TexCoord;\n"

                                        "uniform mat4 transform;\n"

                                        "void main()\n"
                                        "{\n"
                                        "gl_Position = transform * vec4(aPos, 1.0);\n"
                                        "TexCoord = aTexCoord;\n"
                                        "}\0";

static const char* fragmentShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"

                                          "in vec2 TexCoord;\n"

                                          "uniform sampler2D ourTexture;\n"

                                          "void main()\n"
                                          "{\n"
                                          "FragColor = texture(ourTexture, TexCoord);\n"
                                          "}\0";
#endif
