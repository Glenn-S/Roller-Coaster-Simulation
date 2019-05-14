// Borrowed from Andrew Owens boilerplate code

#version 330 core

uniform vec3 lightPosition_worldSpace;
uniform vec3 cameraPosition_worldSpace;
uniform int shade;

in VertexData {
    vec3 position_worldSpace;
    vec3 normal_worldSpace;
    vec3 color;
} vertexData;

out vec4 color;

void main() {
    if (shade == 1) {
        vec3 ambient = 0.6 * vertexData.color;

        vec3 l = normalize(lightPosition_worldSpace - vertexData.position_worldSpace);
        vec3 n = normalize(vertexData.normal_worldSpace);

        float diff = max(dot(l, n), 0.0);
        vec3 diffuse = diff * vertexData.color;

        vec3 e = normalize(cameraPosition_worldSpace - vertexData.position_worldSpace);
        vec3 h = normalize(l + e);
        float spec = pow(max(dot(n, h), 0.0), 32.0);
        vec3 specular = vec3(0.3) * spec;

        color = vec4(ambient + diffuse + specular, 1.0);
    } else {
        color = vec4(vertexData.color, 1.0);
    }

}
