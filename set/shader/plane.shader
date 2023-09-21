@Vertex
#version 300 es                          
layout(location = 0) in vec3 vPosition;  
layout(location = 1) in vec2 in_texCoords;
out vec2 texCoords;                       
void main()                              
    {                                        
        gl_Position = vec4(vPosition,1.0);   
        texCoords=in_texCoords;          
    }                         












@Fragment
#version 300 es                              
precision mediump float;                     
in vec2 texCoords;                          
out vec4 fragColor;                          
uniform sampler2D tex;                      
void main()                                 
{                                            
    fragColor = texture(tex,texCoords);  
}                                            