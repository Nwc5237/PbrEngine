#pragma once

class ObjTexture {
public:
	const char* path; //XXX is this going to cause problems or is this pointer on the heap? Coems from a std::string.c_str() call 
	int width, height, nrChannels;
	unsigned int texID;
	
	ObjTexture(const char* path) {
		//do stb_image stuff here and texture binding
		this->path = path;
		unsigned char* data;
		data = stbi_load(path, &width, &height, &nrChannels, 0);
		
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_BYTE, data); //TODO check filetype. png is RGBA but jpeg is just rgb I believe
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			printf("Failed to load obj texture at path: %s\n", path);
			exit(0);
		}

		stbi_image_free(data);
	}

	ObjTexture(){}
};