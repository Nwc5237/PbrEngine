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
    std::string path;

    Material () {}

    Material(const char* path) {
        this->path = std::string(path);
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
                    materials.back().diffuse = getTexture(getTexturePath(line, 7));
                }
                if (line.find("map_Bump ") != std::string::npos) {
                    materials.back().normal = getTexture(getTexturePath(line, 9));
                }
                if (line.find("map_Ns ") != std::string::npos) {
                    materials.back().roughness = getTexture(getTexturePath(line, 7));
                }
                if (line.find("refl ") != std::string::npos) {
                    materials.back().metalness = getTexture(getTexturePath(line, 5));
                }
            }
            newfile.close();
        }
    }

    /*get texture path - If we recognize that it has the full path, use it, otherwise do the appending stuff*/
    std::string getTexturePath(std::string line, int offset) {
        std::string file_name, directory;

        //check whether we have like d:\\ or c:/
        if (line.find(":") != std::string::npos)
            return line.substr(offset, line.length());

        //otherwise the path needs to be relative to the model's directory
        directory = getDirectory(path);
        directory += '/';
        file_name = line.substr(offset, line.length());
        directory.append(file_name);
        return toSingleSlash(directory);
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