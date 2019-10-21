#include "ArgParser.h"

#include <cstring>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

ArgParser::ArgParser(int argc, const char *argv[]) 
{
    defaultValues();

    for (int i = 1; i < argc; i++) {
        // rendering output
        if (!strcmp(argv[i], "-input")) {
            i++; assert (i < argc); 
            input_file = argv[i];
        } else if (!strcmp(argv[i], "-output")) {
            i++; assert (i < argc); 
            output_file = argv[i];
        } else if (!strcmp(argv[i], "-normals")) {
            i++; assert (i < argc); 
            normals_file = argv[i];
        } else if (!strcmp(argv[i], "-size")) {
            i++; assert (i < argc); 
            width = atoi(argv[i]);
            i++; assert (i < argc); 
            height = atoi(argv[i]);
        } 

        // rendering options
        else if (!strcmp(argv[i], "-depth")) {
            i++; assert (i < argc); 
            depth_min = (float)atof(argv[i]);
            i++; assert (i < argc); 
            depth_max = (float)atof(argv[i]);
            i++; assert (i < argc); 
            depth_file = argv[i];
        } else if (!strcmp(argv[i], "-bounces")) {
            i++; assert (i < argc); 
            bounces = atoi(argv[i]);
        } else if (!strcmp(argv[i], "-shadows")) {
            shadows = true;
        }

        // supersampling
        else if (strcmp(argv[i], "-jitter") == 0) {
            jitter = true;
        } else if(strcmp(argv[i], "-filter") == 0) {
            filter = true;
        } 
        else {
            printf ("Unknown command line argument %d: '%s'\n", i, argv[i]);
            exit(1);
        }
    }

    std::cout << "Args:\n";
    std::cout << "- input: " << input_file << std::endl;
    std::cout << "- output: " << output_file << std::endl;
    std::cout << "- depth_file: " << depth_file << std::endl;
    std::cout << "- normals_file: " << normals_file << std::endl;
    std::cout << "- width: " << width << std::endl;
    std::cout << "- height: " << height << std::endl;
    std::cout << "- depth_min: " << depth_min << std::endl;
    std::cout << "- depth_max: " << depth_max << std::endl;
    std::cout << "- bounces: " << bounces << std::endl;
    std::cout << "- shadows: " << shadows << std::endl;
}

void
ArgParser::defaultValues() 
{
    // rendering output
    input_file = "";
    output_file = "";
    depth_file = "";
    normals_file = "";
    width = 100;
    height = 100;
    stats = 0;

    // rendering options
    depth_min = 0;
    depth_max = 1;
    bounces = 0;
    shadows = false;

    // sampling
    jitter = false;
    filter = false;
}
