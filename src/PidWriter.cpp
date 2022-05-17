#include <cstring>

#include "PidWriter.hpp"

namespace Extractor
{
namespace Util
{

    PidWriter::PidWriter(const std::string& filename)
    : _bufferIndex(0)
    {
        _file.open(filename, std::ios_base::binary);
    }

    PidWriter::~PidWriter()
    {
        if (_bufferIndex > 0)
        {
            _file.write((const char *)_buffer, _bufferIndex);
        }
    }

    void PidWriter::Add(const char *data, int byteCount)
    {
        while( byteCount > 0)
        {
            if (_bufferIndex + byteCount < MaxBuffer)
            {
                std::memcpy(&_buffer[_bufferIndex], data, byteCount);
                _bufferIndex += byteCount;
                byteCount = 0;
            }
            else
            {
                int diff = MaxBuffer - _bufferIndex;
                std::memcpy(&_buffer[_bufferIndex], data, diff);
                _bufferIndex += diff;
                _file.write((const char *)_buffer, _bufferIndex);
                data += diff;
                byteCount -= diff;
                _bufferIndex = 0;
            }
        }
    }

}
}
