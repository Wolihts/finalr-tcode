#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <cstdio>
#include <cstdlib>
#include <cmath>                   // For math functions
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

static char* load_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open: %s\n", path);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = (char*)malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

static std::vector<std::string> split(const char *src, char delimiter = '\n')
{
    std::stringstream ss(src);
    std::vector<std::string> strings;
    std::string line;

    while (std::getline(ss, line, delimiter))
    {
        strings.push_back(line);
    }

    return strings;
}

#endif
