#pragma once
#include "EngineUtils.hpp"
#include "ObjTexture.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <stdio.h>

typedef struct Mtl {
    std::string name;
    ObjTexture diffuse, normal, roughness, metalness;
};

class Material {
public:
    std::vector<Mtl> materials;
    ObjTexture diffuse;
    //on the line newmtl, read for this and add it as the material name
    Material(const char* path) {
        std::fstream newfile;
        int uv_index = 0;

        newfile.open(path, std::ios::in);
        if (newfile.is_open()) {
            std::string line;
            while (getline(newfile, line)) {
                if (line.find("map_Kd ") != std::string::npos) {
                    std::string a;
                    a = getDiffuseTexturePath(line);
                    this->diffuse = getTexture(getDiffuseTexturePath(line));
                }
            }
            newfile.close();
        }
    }

    std::string getDiffuseTexturePath(std::string line) {
        std::string a;
        a = line.substr(7, line.size() - 7);
        return line.substr(7, line.size()-7);
    }

    ObjTexture getTexture(std::string path) {
        return ObjTexture(path.c_str());
    }
};