#pragma once
#include "EngineUtils.hpp"
#include "Material.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <stdio.h>

typedef struct ObjVertex {
    glm::vec3 positition;
    glm::vec2 texture;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
} ObjVertex;

typedef struct ObjMesh {
    std::string materialName;
    std::vector<ObjVertex> vertices;
    unsigned int VAO, VBO;
} ObjMesh;

class ObjModel {
public:
    char* path;

	//constuctor
	ObjModel(char *path) {
        this->path = path;

        std::fstream newfile;
        int uv_index = 0;

        newfile.open(path, std::ios::in);
        if (newfile.is_open()) {
            std::string line;
            while (getline(newfile, line)) {
                if (line.size() >= 2) {
                    if (strncmp(line.c_str(), "vn ", 3) == 0) {
                        //pushNormal();
                    }
                    if (strncmp(line.c_str(), "vt ", 3) == 0) {
                        pushTexCoord(line, uv_index);
                        uv_index++;
                    }
                    if (strncmp(line.c_str(), "v ", 2) == 0) {
                        pushPositionCoord(line);
                    }
                    if (strncmp(line.c_str(), "f ", 2) == 0) {
                        pushFace(line);
                    }
                    if (strncmp(line.c_str(), "mtllib", 6) == 0) {
                        createMaterial(line);
                    }
                    if (strncmp(line.c_str(), "usemtl", 6) == 0) {
                        //when we hit a new material
                        meshes.push_back(ObjMesh());
                        meshes.back().materialName = line.substr(7, line.size() - 7);
                        /*
                        Want to keep drawing from pos and tex coords
                        and make sure that the ObjVertex vector is
                        correct so always use vector.back().vertices etc
                        */
                    }
                }
            }
            newfile.close();
        }

        /* TODO: Add in those sweet sweet textures */
        /*
        Create a texture class
        Later create a material class for embeding several textures 
        */

        setup();
	}

    void setup() {

        for (ObjMesh &mesh : meshes) {
            glGenVertexArrays(1, &mesh.VAO);
            glGenBuffers(1, &mesh.VBO);

            glBindVertexArray(mesh.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ObjVertex) * mesh.vertices.size(), &mesh.vertices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (void*)offsetof(ObjVertex, texture));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (void*)offsetof(ObjVertex, normal));

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (void*)offsetof(ObjVertex, tangent));

            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (void*)offsetof(ObjVertex, bitangent));

            glBindVertexArray(0);
        }
    }

    void Draw(Shader shaderProgram) {
        //might want to move the texture binding into the texture class
        shaderProgram.use();
        for (ObjMesh &mesh : meshes) {
            Mtl meshMaterial = modelMaterial.getMtl(mesh.materialName);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, meshMaterial.diffuse.texID);
            glUniform1i(glGetUniformLocation(shaderProgram.ID, "diffuseTex"), 0);

            glBindVertexArray(mesh.VAO);
            //shaderProgram.use();
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
            glBindVertexArray(0);
        }
    }

private:
    //variables
    std::vector<glm::vec3> pos_coords;
    std::vector<glm::vec2> tex_coords;
    std::vector<ObjMesh> meshes;
    Material modelMaterial;

    void createMaterial(std::string line) {
        std::string directory, materialPath, filename;
        int line_length = line.size();

        filename = line.substr(7, line_length - 7);

        directory = getDirectory(path);

        if (isDoubleSlashPath(directory)) {
            materialPath = directory + std::string("\\") + filename;
        }
        else {
            materialPath = directory + std::string("/") + filename;
        }
        modelMaterial = Material(materialPath.c_str());

        /*
        CURRENTLY getting material path, going to the material file with material name (different from the material path file name)
        And then getting the diffuse texture
        */
    }


    //functions
    void pushNormal(std::string line) { }

    void pushTexCoord(std::string line, int uv_index) {
        int pos = 3;
        float u, v;

        u = readFloat(line, &pos);
        v = readFloat(line, &pos);

        tex_coords.push_back(glm::vec2(u, v));
    }

    void pushPositionCoord(std::string line) {
        int pos = 2;
        float x, y, z;

        x = readFloat(line, &pos);
        y = readFloat(line, &pos);
        z = readFloat(line, &pos);

        pos_coords.push_back(glm::vec3(x, y, z));
    }

    float readFloat(std::string str, int *pos) {
        int end, range;
        float result;
        bool neg;

        end = *pos;
        neg = str.at(*pos) == '-';
        range = neg ? 9 : 8; //how many characters comprise the float, one extra ('-') if negative
        
        result = std::stof(str.substr(*pos, range));
        *pos = *pos + range + 1; //move past the space
        return result;   
    }

    void pushFace(std::string line) {
        int pos = 0;
        ObjVertex vert;

        int p1, t1, p2, t2, p3, t3;
        p1 = readInt(line, &pos)-1;
        t1 = readInt(line, &pos)-1;
        readInt(line, &pos); //skip normals
        p2 = readInt(line, &pos)-1;
        t2 = readInt(line, &pos)-1;
        readInt(line, &pos); //skip normals
        p3 = readInt(line, &pos)-1;
        t3 = readInt(line, &pos)-1;
        readInt(line, &pos); //skip normals

        if (p1 < pos_coords.size() && t1 < tex_coords.size()) {
            vert.positition = pos_coords.at(p1);
            vert.texture = tex_coords.at(t1);
            meshes.back().vertices.push_back(vert);
        }
        else {
            printf("");
        }

        if (p2 < pos_coords.size() && t2 < tex_coords.size()) {
            vert.positition = pos_coords.at(p2);
            vert.texture = tex_coords.at(t2);
            meshes.back().vertices.push_back(vert);
        }
        else {
            printf("");
        }

        if (p3 < pos_coords.size() && t3 < tex_coords.size()) {
            vert.positition = pos_coords.at(p3);
            vert.texture = tex_coords.at(t3);
            meshes.back().vertices.push_back(vert);
        }
        else {
            printf("");
        }
    }

    int readInt(std::string str, int* pos) {
        int start = *pos, end, result;
        
        //increment the position past all non int characters
        while (!in(str.at(*pos), "0123456789" )) {
            (*pos)++;
        }

        end = *pos;

        //read all the int characters
        do {
            end++;
        } while (end < str.length() && in(str.at(end), "0123456789"));

        result = std::stoi(str.substr(*pos, end - *pos));
        *pos = end + 1;
        return result;
    }
};