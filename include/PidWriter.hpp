#pragma once

#include <fstream>

namespace Extractor
{
namespace Util
{
    const int MaxBuffer = 1024 * 1024;

    class PidWriter 
    {
    private:

        std::ofstream _file;
        char          _buffer[MaxBuffer];
        int           _bufferIndex;

    public:
        PidWriter(const char *filename);
        virtual ~PidWriter();

        void Add(const char *data, int byteCount);
    };

}
}
