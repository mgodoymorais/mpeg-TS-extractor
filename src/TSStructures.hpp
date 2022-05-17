#pragma once

#include <memory>

namespace Extractor
{
    namespace TS
    {

        // our "high-level" data structures

        struct AdaptationFieldControl
        {
            int Length;
            int Discontinuity;
            int RandomAccess;
            int ESPriority;
            int PCR;
            int OPCR;
            int SplicingPoint;
            int TransportPrivateData;
            int AdaptionFieldExtension;
        };

        struct PESOptionalHeader
        {
            int Scrambling;
            int Priority;
            int DataAlignment;
            int Copyright;
            int Original;
            int PTSDTS;
            int ESCR;
            int ES;
            int DSMTrickMode;
            int AdditionalCopyInfo;
            int CRC;
            int Extension;
            int PESHeaderLength;
        };

        struct PacketizedElementaryStream
        {
            unsigned char StreamId;
            int Length;
            std::unique_ptr<PESOptionalHeader> OptionalHeader;
        };

    }
}
