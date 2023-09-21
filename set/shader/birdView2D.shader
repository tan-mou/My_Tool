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
uniform sampler2D forntTexture;        
uniform sampler2D mapFrontTexture;
uniform sampler2D mapDecimalFrontTexture;

uniform sampler2D leftTexture;
uniform sampler2D mapLeftTexture;
uniform sampler2D mapDecimalLeftTexture;

uniform sampler2D rightTexture;
uniform sampler2D mapRightTexture;
uniform sampler2D mapDecimalRightTexture;


uniform sampler2D rearTexture;
uniform sampler2D mapRearTexture;
uniform sampler2D mapDecimalRearTexture;

vec4 caculateUV(sampler2D map,sampler2D mapDecimal)
{
    vec4 mapUV=texture(map,texCoords);
    vec4 mapUVDecimal=texture(mapDecimal,texCoords);
    mapUVDecimal.x/=255.0f;
    mapUVDecimal.y/=255.0f;
    mapUVDecimal.w=0.0f;
    vec4 result=vec4(vec3(mapUV.xyz+mapUVDecimal.xyz),mapUV.w);
    return mapUV+mapUVDecimal;
}
void main()                                 
{        
    vec4 frontUV=caculateUV(mapFrontTexture,mapDecimalFrontTexture);
    vec4 leftUV=caculateUV(mapLeftTexture,mapDecimalLeftTexture);
    vec4 rightUV=caculateUV(mapRightTexture,mapDecimalRightTexture);
    vec4 rearUV=caculateUV(mapRearTexture,mapDecimalRearTexture);

    vec4 F_Color=texture(forntTexture,frontUV.xy);
    vec4 L_Color=texture(leftTexture,leftUV.xy);
    vec4 Right_Color=texture(rightTexture,rightUV.xy);
    vec4 Rear_Color=texture(rearTexture,rearUV.xy);

    vec3 result=frontUV.w*F_Color.xyz+leftUV.w*L_Color.xyz+rightUV.w*Right_Color.xyz+rearUV.w*Rear_Color.xyz;
    // vec3 result=frontUV.w*F_Color.xyz+leftUV.w*L_Color.xyz;

    fragColor = vec4(result,1.0f);
}                  