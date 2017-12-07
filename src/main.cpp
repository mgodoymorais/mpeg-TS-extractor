
#include <iostream>
#include <fstream>
#include <cstring>

#include "TSFile.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Incorrect number of parameters" << std::endl;
        std::cout << "Usage: " << argv[0] << " <filname> [--debug-headers]" << std::endl;
        return -1;
    }
    bool debugHeaders = false;
    for(int index = 2; index < argc; index++)
    {
        if (strcmp(argv[index], "--debug-headers") == 0)
        {
            debugHeaders = true;
        }
    }

    Extractor::TS::TSFile ts(argv[1], debugHeaders); 
    ts.Extract();
  
    return 0;
}
