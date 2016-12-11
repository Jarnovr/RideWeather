
#include "StravaType.h"
#include <cstdio>
#include <iostream>
#include <boost/filesystem.hpp>
using namespace std;
using namespace boost::filesystem;

void ReadAthlete(const char *fn)
{
    FILE *fid;
    fid=fopen(fn, "rb");
    if (fid == NULL)
    {fprintf(stderr, "Error opening athlete file.\n");exit(1);}
    
    // get filesize
    size_t fs = file_size(fn);
    char *json = new char[fs];
    size_t read = fread(json, 1, fs, fid);
    if (read != fs)
        {fprintf(stderr, "Error reading athlete file. Read %zu of %zu\n.", read, fs);exit(1);}
    fclose(fid);
    
    
    // Create Athlete
    
};



void ReadActivity(const char *fn)
{
    
};
