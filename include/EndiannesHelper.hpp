#pragma once

#include <cstdint>

namespace Extractor
{
namespace Util
{

    class EndiannessHelper
    {
    private:
        bool			 _isBigEndian;

    private:
        EndiannessHelper()
        {
            union
            {
                unsigned int i;
                char c[sizeof(unsigned int)];
            } u;
            u.i = 1;
            _isBigEndian = !u.c[0];	
        }

    public:

        static EndiannessHelper& GetInstance()
        {
            static EndiannessHelper instance; 
            return instance;
        }

        inline std::uint32_t Rotate32(std::uint32_t x)
        {
            if(!_isBigEndian)
            {
                return (x >> 24) |
                    ((x >> 8) & 0x0000ff00) |
                    ((x << 8) & 0x00ff0000) |
                    (x << 24);
            }
            return x;
        }

        inline std::uint16_t Rotate16(std::uint16_t x)
        {
            if(!_isBigEndian)
            {
                x = (x >> 8) | (x << 8);
            }
            return x;
        }
    };

}
}
