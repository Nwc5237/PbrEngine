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
    //on the line newmtl, read for this and add it as the material name

    Material () {}

    Material(const char* path) {
        std::fstream newfile;
        int uv_index = 0;

        newfile.open(path, std::ios::in);
        if (newfile.is_open()) {
            std::string line;

            while (getline(newfile, line)) {
                if (line.find("newmtl") != std::string::npos) {
                    materials.push_back(Mtl());
                    materials.back().name = line.substr(7, line.size() - 7);
                }
                if (line.find("map_Kd ") != std::string::npos) {
                    materials.back().diffuse = getTexture(getDiffuseTexturePath(line));
                }
                if (line.find("map_Bump ") != std::string::npos) {
                    materials.back().normal = getTexture(getNormalTexturePath(line));
                }
            }
            newfile.close();
        }
    }

    std::string getDiffuseTexturePath(std::string line) {
        std::string a;
        int offset = 7;
        a = line.substr(offset, line.size() - offset);
        return line.substr(offset, line.size() - offset);
    }

    std::string getNormalTexturePath(std::string line) {
        std::string a;
        int offset = 9;
        a = line.substr(offset, line.size() - offset);
        return line.substr(offset, line.size() - offset);
    }

    ObjTexture getTexture(std::string path) {
        return ObjTexture(path.c_str());
    }

    Mtl getMtl(std::string name) {
        for (Mtl mat : materials) {
            if (!name.compare(mat.name)) //comparison == 0
                return mat;
        }
        printf("No matching material for: %s\n", name.c_str());
        exit(0);
    }
};