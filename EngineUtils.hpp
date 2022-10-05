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

/*
replaces all / with \\ TODO add error handling in case there's a bad path, and add bounds checking.
*/
std::string toSingleSlash(std::string src) {
    std::string replaced;
    int i = 0;
    char cur = src.at(i);
    while(i < src.length()) {
        cur = src.at(i);
        if (cur == '\\') {

            //when double slash
            if (i + 1 < src.length() && src.at(i + 1) == '\\')
                i++;
            replaced += '/';
            //i++;
        }
        else {
            replaced += cur;
        }
        i++;
    }

    return replaced;
}

void readWhiteSpace(std::string str, int* pos) {
    while (*pos < str.length() && in(str.at(*pos), " \n\t"))
        (*pos)++;
}

int readInt(std::string str, int* pos) {
    int start, end;

    readWhiteSpace(str, pos);
    start = *pos;
    end = *pos;

    if (str.at(end) == '-')
        end++;

    while (end < str.length() && in(str.at(end), "0123456789"))
        end++;

    *pos = end;
    return std::stoi(str.substr(start, end - start));
}

float readFloat(std::string str, int* pos) {
    int start, end;
    
    readWhiteSpace(str, pos);
    start = *pos;
    end = *pos;

    if (str.at(end) == '-')
        end++;

    while (end < str.length() && in(str.at(end), "0123456789"))
        end++;

    if (str.at(end) == '.')
        end++;
    else
        goto CREATE_FLOAT;

    while (end < str.length() && in(str.at(end), "0123456789"))
        end++;

CREATE_FLOAT:
    *pos = end;
    return std::stof(str.substr(start, end - start));
}

// mapping a value i from the range[a, b] to the range[x, y]
float map_val(float i, float a, float b, float x, float y)
{
    float percent_of_ab = (i - a) / (b - a);
    return percent_of_ab * (y - x) + x;
}

