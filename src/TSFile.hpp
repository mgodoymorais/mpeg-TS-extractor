#pragma once

#include <fstream>
#include <string>

namespace Extractor
{
    namespace TS
    {
        class TSFile
        {
        private:
            std::ifstream _sourceFile;
            const bool _debugHeaders;

        public:
            TSFile(const std::string&, bool = false);
            bool Extract();
        };
    }
}