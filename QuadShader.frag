#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D tex;

void main()
{
	FragColor = vec4(TexCoords.x, TexCoords.y, 1.0f, 1.0f);
	vec4 final = vec4(texture(tex, TexCoords).rgb, 1.0f);
	FragColor = final;

	if(TexCoords.y > .45 && TexCoords.y < .65)
		FragColor = vec4(1-final.x, 1-final.y, 1-final.z, 1.0f);
}