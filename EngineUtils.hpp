#pragma once
#include <string>

//checks whether a character is in a string
bool in(char a, char* b) {
    int i = 0;
    char cur;
    while ((cur = b[i]) != '\0') {
        if (a == cur)
            return true;
        i++;
    }
    return false;
}

//takes in a file path and returns the directory it lives in
std::string getDirectory(std::string path) {
    size_t pos;
    if ((pos = path.find_last_of("\\")) != std::string::npos) {
        return path.substr(0, pos);
    }
    else if ((pos = path.find_last_of("/")) != std::string::npos) {

    }
    else {
        printf("Could not find directory parent for filepath: %s\n", path.c_str());
        exit(0);
    }

    return path.substr(0, pos);
}

bool isDoubleSlashPath(std::string path) {
    if (path.find_last_of("\\") != std::string::npos)
        return true;
    if (path.find_last_of("/") != std::string::npos)
        return false;
    printf("No slashes found in path: %s\n", path.c_str());
    exit(0);
}