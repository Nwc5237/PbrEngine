#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout (location = 3) out vec4 gRoughMetal;
layout (location = 4) out vec4 gAmbient;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 camPos;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D roughnessTex;
uniform sampler2D metalnessTex;

uniform bool useTex;
uniform bool usePBR;

//returns the normal for this fragment
vec3 mapNormal()
{
	vec3 normal = texture(normalTex, TexCoords).rgb;
	normal = normal * 2.0f - 1.0f;

	return normalize(TBN * normal);
}

//for when the texture compression throws away the blue channel and it needs to be reconstructed
vec3 mapNormalNoBlue()
{
	vec3 normal = texture(normalTex, TexCoords).rgb;
	normal.b = sqrt(1.0f - pow(texture(normalTex, TexCoords).r, 2.0f) - pow(texture(normalTex, TexCoords).g, 2.0f));
	normal = normal * 2.0f - 1.0f;
	
	return normalize(TBN * normal);
}

//can pass in text coords and get white noise texture.
float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

void main()
{
	float roughness, metalness;

	roughness = texture(roughnessTex, TexCoords).x;
	metalness = 1.0f - texture(metalnessTex, TexCoords).x;
	
	gPosition = vec4(FragPos, 1.0f); //should be able to add roughness and metalness in alpha channel but it's being weird so we're doing this
	gNormal = vec4(mapNormal(), 1.0f);
	gAlbedo = vec4(texture(diffuseTex, TexCoords).rgb, 1.0f);
	gRoughMetal = vec4(roughness, metalness, 0.0f, 1.0f); //could probably use third channel for ambient

	//throw this into a new texture since this value just needs to get added
	//this value needs to be added on its own since it's local to the object
	//we can't get this info from anywhere else
	gAmbient = vec4(vec3(.03) * gAlbedo.rgb, 1.0f);

	//highlighting
	if(!useTex)
	{
		gAmbient = vec4(1.0f, 0.2f, 0.0f, 1.0f);
	}
}

//Note: For some blender models the normals are exported incorrectly, so export as .obj and uncheck "write normals"
//in the geometry dropdown. Assimp is setup to create normals, tangents, and bitangents in this project


//quick phong: texture(diffuseTex, TexCoords).rgb * fader * max(0, dot(N, l)) + lights[i].color * fader * pow(max(0, dot(N, H)), 100);