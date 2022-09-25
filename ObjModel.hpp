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

        newfile.open(path, std::ios::in);
        if (newfile.is_open()) {
            std::string line;
            while (getline(newfile, line)) {
                if (line.size() >= 2) {
                    if (strncmp(line.c_str(), "vn ", 3) == 0) {
                        pushNormal(line);
                    }
                    if (strncmp(line.c_str(), "vt ", 3) == 0) {
                        pushTexCoord(line);
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
                        //when we hit a different material, we create a new mesh
                        meshes.push_back(ObjMesh());
                        meshes.back().materialName = line.substr(7, line.size() - 7);
                    }
                }
            }
            newfile.close();
        }

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
    std::vector<glm::vec3> pos_coords, norm_coords;
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
    }


    //functions
    void pushNormal(std::string line) {
        int pos = 3;
        float x, y, z;

        x = readFloat(line, &pos);
        y = readFloat(line, &pos);
        z = readFloat(line, &pos);

        norm_coords.push_back(glm::vec3(x, y, z));
    }

    void pushTexCoord(std::string line) {
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

    void pushFace(std::string line) {
        int pos = 2; //past the 'f' command character and the space after
        ObjVertex vert;


        /*To really make this robust, we need to make this count how many componenet and groups there are. (also, should be optional, so require
        a read of '/' to move to the next one.)*/
        int p1, t1, n1, p2, t2, n2, p3, t3, n3;
        p1 = readInt(line, &pos)-1;
        pos++; //past the slash
        t1 = readInt(line, &pos)-1;
        pos++; //past the slash again
        n1 = readInt(line, &pos)-1;

        p2 = readInt(line, &pos)-1;
        pos++; //past the slash
        t2 = readInt(line, &pos)-1;
        pos++; //past the slash
        n2 = readInt(line, &pos)-1;

        p3 = readInt(line, &pos)-1;
        pos++; //past the slash
        t3 = readInt(line, &pos)-1;
        pos++; //past the slash
        n3 = readInt(line, &pos)-1;

        if (p1 < pos_coords.size() && t1 < tex_coords.size() && n1 < norm_coords.size()) {
            vert.positition =  pos_coords.at(p1);
            vert.texture    =  tex_coords.at(t1);
            vert.normal     = norm_coords.at(n1);
            meshes.back().vertices.push_back(vert);
        }
        else
            goto COORD_ERROR; //literally do not care about using gotos. They're great.

        if (p2 < pos_coords.size() && t2 < tex_coords.size() && n2 < norm_coords.size()) {
            vert.positition =  pos_coords.at(p2);
            vert.texture    =  tex_coords.at(t2);
            vert.normal     = norm_coords.at(n2);
            meshes.back().vertices.push_back(vert);
        }
        else
            goto COORD_ERROR;

        if (p3 < pos_coords.size() && t3 < tex_coords.size() && n3 < norm_coords.size()) {
            vert.positition =  pos_coords.at(p3);
            vert.texture    =  tex_coords.at(t3);
            vert.normal     = norm_coords.at(n3);
            meshes.back().vertices.push_back(vert);
        }
        else
            goto COORD_ERROR;

        return;

    COORD_ERROR:
        printf("Coordinate error in ObjModel.hpp::pushFace. Set breakpoints on the goto statements there to find where its coming from\n");
        exit(0);
    }
};