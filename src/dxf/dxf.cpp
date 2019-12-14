#include <dxf/dxf.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <dxflib/dl_dxf.h>
#include <dxflib/dl_creationadapter.h>
#include <dxf/DXFParse_Class.h>
#include <json/json.h>

std::string DXF::readDXF(const char* file)
{
    // Load DXF file into memory:
    //std::cout << "Reading file " << file << "...\n";
    DXFParse_Class* creationClass = new DXFParse_Class();
    DL_Dxf* dxf = new DL_Dxf();
    if (!dxf->in(file, creationClass)) { // if file open failed
        std::cerr << file << " could not be opened.\n";
        return "{}";
    }
    delete dxf;
    //std::cout << creationClass->dxfJSON.dump() << std::endl;
    std::string ret = creationClass->dxfJSON.dump();
    delete creationClass;
    return ret;
}