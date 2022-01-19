#version 330 core
out vec4 FragColor;

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Light
{
	vec3 position;
	vec3 color;
};

uniform Light lights[4];
uniform vec3 camPos;
uniform sampler2D normalTex;
uniform sampler2D diffuseTex;
uniform sampler2D roughnessTex;
uniform sampler2D metalnessTex;
uniform bool useTex;

float DistributionGGX(vec3 N, vec3 H, float a)
{
	a = a * a; //adding because apparently roughness should be squared according to disney and epic
	float a2, NdotH, NdotH2, num, denom;
	a2 = a * a;
	NdotH = max(dot(N, H), 0.0f);
	NdotH2 = NdotH * NdotH;

	num = a2;
	denom = 3.14159 * pow((NdotH2 * (a2 - 1.0f) + 1.0f), 2.0f);
	return num / denom;
}

float DistributionGGX1(vec3 N, vec3 H, float a)
{
	a = a * a; //adding because apparently roughness should be squared according to disney and epic
	float a2, NdotH, NdotH2, num, denom;
	a2 = a * a;
	NdotH = max(dot(N, H), 0.0f);
	NdotH2 = NdotH * NdotH;

	num = a2;
	denom = 3.14159 * pow((pow(NdotH2, 2.0f) * (a2 - 1.0f) + 1.0f), 2.0f);
	return num / denom;
}

//normal, view, and roughness alpha
float GeometrySchlickGGX(vec3 N, vec3 v, float a)
{
	a = a * a; //adding because apparently roughness should be squared according to disney and epic
	float k, NdotV;
	k = pow(a + 1, 2) / 8.0f; //remaps differently if we do IBL --- becomes (a^2)/2
	NdotV = max(dot(N, v), 0.0f);

	return NdotV / (NdotV * (1 - k) + k);
}

//normal, view, light (to/from surface) and alpha
float GeometrySmith(vec3 N, vec3 v, vec3 l, float a)
{
	a = a * a; //adding because apparently roughness should be squared according to disney and epic
	return GeometrySchlickGGX(N, v, a) * GeometrySchlickGGX(N, l, a);
}

//incidence angle (from dot product of normal and halfway vector, index of refraction
float FresnelSchlick(float VdotH, float ior)
{
	float F0 = pow((ior - 1) / (ior + 1), 2.0f);
	//float F0 = .04;
	return F0 + (1.0 - F0) * pow(clamp(1.0 - VdotH, 0.0f, 1.0f), 5.0);
}

float FresnelSchlick1(float VdotH, float ior)
{
	float F0 = pow((ior - 1) / (ior + 1), 2.0f);
	//float F0 = .04;
	return F0 + (1.0 - F0) * pow(2.0f, (-5.55473*VdotH-6.98316)*VdotH);
}

float Attenuate(vec3 lightPos, vec3 surface)
{
	return 1.0f / pow(length(lightPos-surface), 2.0f);
}

float Radiance(vec3 surface, vec3 lightPos, vec3 N)
{
	vec3 wi = normalize(lightPos - surface);
	float cosTheta = max(dot(N, wi), 0.0f);
	float attenuation = Attenuate(lightPos, surface);
	return attenuation * cosTheta;
}

vec3 mapNormals()
{
	vec3 normal = texture(normalTex, TexCoords).rgb;
	normal = normal * 2.0f - 1.0f;
	return normal;
}

void main()
{
	vec3 v = normalize(camPos - FragPos);
	vec3 l = normalize(lights[0].position - FragPos);
	vec3 H = normalize(l + v);
	float roughness = texture(roughnessTex, TexCoords).x;
	float metalness = texture(metalnessTex, TexCoords).x;

	vec3 N = normalize(Normal);

	float fader = 250.0f;
	float pi = 3.14159;
	vec3 num;
	float D, G, f, denom;
	
	
	D = DistributionGGX(N, H, roughness);
	G = GeometrySmith(N, v, l, roughness);

	//confused here. learnopengl said use H (which seems to fix problem) but textbook uses view direction. Very strange 
	f = FresnelSchlick(max(dot(H, N), 0.0f), metalness); //NO LONGER TRUE: dir and normal from textbook, learnopengl also uses halfway in place of dir and I think that's wrong


	vec3 F = vec3(f);
	num = D * G * F;
	denom = 4.0f * max(dot(N, v), 0.0f) * max(dot(N, l), 0.0f) + .0001f; //adding teeny term at the end so we don't divide by zero
	vec3 spec = num / denom;

	vec3 kS, kD;
	kS = F;
	kD = vec3(1.0f) - kS;

	vec3 radiance = fader * lights[0].color * Attenuate(lights[0].position, FragPos);
	float NdotL = max(dot(N, l), 0.0f);

	vec3 Lo = (kD * vec3(texture(diffuseTex, TexCoords)) / pi + spec) * radiance * NdotL;
	
	//vec3 Lo = (kD * vec3(1.0f, 0.0f, .2f) / pi + spec) * radiance * NdotL;

	FragColor = vec4(Lo, 1.0f);

	if(useTex)
	{
		D = DistributionGGX1(N, H, roughness);
		f = FresnelSchlick1(max(dot(H, N), 0.0f), metalness);
		G = 1.0f / pow(max(dot(l, H), .00001f), 2.0f);
		F = vec3(f);

		spec = (D * G * F) / 4.0f;
		Lo = (kD * vec3(texture(diffuseTex, TexCoords)) / pi + spec) * radiance * NdotL;
		FragColor = vec4(Lo, 1.0f);
	}

	//FragColor = vec4(texture(normalTex, TexCoords));
}