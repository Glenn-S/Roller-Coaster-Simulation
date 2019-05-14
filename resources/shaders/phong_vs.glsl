// Borrowed from Andrew Owens boilerplate code
#version 330

layout( location = 0 ) in vec3 vertex_modelSpace;
layout( location = 1 ) in vec3 normal_modelSpace;
layout( location = 2 ) in vec3 color;

uniform mat4 MVP;
uniform mat4 M;
uniform vec3 COLOUR;

out VertexData
{
    vec3 position_worldSpace;
    vec3 normal_worldSpace;
    vec3 color;
} vertexData;

void main()
{
   vertexData.position_worldSpace = vertex_modelSpace;
   vertexData.normal_worldSpace = normal_modelSpace * transpose(inverse(mat3(M)));
   vertexData.color = color;
   //vertexData.color = COLOUR;

   gl_Position = MVP * vec4( vertex_modelSpace, 1.0 );
}
