#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 directionToSun = normalize(vec3(1.0, 2.0, 3.0));
out vec4 FragColor;

/*
layout(location = 0) in vec3 inPosition;   // Vertex position
layout(location = 1) in vec3 inNormal;     // Vertex normal
layout(location = 2) in vec2 inTexCoord;   // Texture coordinates
*/

//uniform sampler2D texture0;

void main()
{
  //FragColor = 1.25 * colDiffuse * fragColor * vec4(texture(texture0, fragTexCoord).xyz * (0.5 + 0.5 * fragNormal.y), 1.0);
  float lightK = (0.75 + 0.5 * dot(fragNormal, directionToSun));
  FragColor = colDiffuse * fragColor * vec4(lightK * texture(texture0, fragTexCoord).xyz, 1.0);
}