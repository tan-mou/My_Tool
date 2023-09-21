#pragma once
namespace shader
{
    char vCanvasShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec3 vPosition;  \n"
        "layout(location = 1) in vec2 in_texCoords;\n"
        "out vec2 texCoords;                       \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vec4(vPosition,1.0);    \n"
        "   texCoords=in_texCoords;               \n"
        "}                                        \n";

    char fCanvasShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "in vec2 texCoords;                           \n"
        "out vec4 fragColor;                          \n"
        "uniform sampler2D tex;                      \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = texture(tex,texCoords);  \n"
        "}                                            \n";

    char vLineShaderStr[] =
        "#version 300 es                          \n"
        "uniform mat4 trans;                       \n"
        "layout(location = 0) in vec3 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = trans*vec4(vPosition,1.0);\n"
        "}                                        \n";

    char fLineShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 1.0, 1.0, 0.0, 1.0 );  \n"
        "}                                            \n";

    char vfontShaderStr[] =
        "#version 300 es                          \n"
        "uniform mat4 projection;                 \n"
        "layout(location = 0) in vec4 vertex;     \n"
        "out vec2 texCoords;                      \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = projection*vec4(vertex.xy,0.0,1.0);\n"
        "   texCoords=vertex.zw;                  \n"
        "}                                        \n";

    char ffontShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "in vec2 texCoords;                           \n"
        "out vec4 fragColor;                          \n"
        "uniform sampler2D text;                      \n"
        "uniform vec3 textColor;                     \n"
        "void main()                                  \n"
        "{                                            \n"
        "   vec4 sampled=vec4(1.0,1.0,1.0,texture(text,texCoords).r);\n"
        "   fragColor = vec4(textColor, texture(text,texCoords).r);  \n"
        "}                                            \n";
    char vCircleShaderStr[] =
        "#version 300 es                          \n"
        "uniform mat4 projection;                 \n"
        "uniform mat4 trans;                      \n"
        "layout(location = 0) in vec3 vertex;     \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = projection*trans*vec4(vertex,1.0);\n"
        "}                                        \n";
    char fCircleShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "uniform vec3 color;                          \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( color, 1.0 );          \n"
        "}                                            \n";
    char vBirdViewShaderStr[]=
        "#version 300 es                          \n"
        "uniform mat4 projection;                 \n"
        "uniform mat4 trans;                      \n"
        "layout(location = 0) in vec3 vertex;     \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = projection*trans*vec4(vertex,1.0);\n"
        "}                                        \n";
    char fBirdViewShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "uniform vec3 color;                          \n"
        "uniform int width;                           \n"
        "uniform int height;                          \n"
        "in vec2 texCoords;                           \n"
        "uniform sampler2D uvTexture                  \n"
        "uniform sampler2D "
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( color, 1.0 );          \n"
        "}                                            \n";
} // namespace
