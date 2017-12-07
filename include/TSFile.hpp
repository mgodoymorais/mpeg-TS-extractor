#pragma once

#include <fstream>

namespace Extractor
{
namespace TS
{

    class TSFile
    {
    private:
        std::ifstream _sourceFile;
        bool _debugHeaders;

    public:
        TSFile(const char *, bool = false);
        virtual ~TSFile();
    
        void Extract();

    };

}
}