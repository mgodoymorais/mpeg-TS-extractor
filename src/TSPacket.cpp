#include <iostream>
#include <cstdint>
#include <cstring>

#include "EndiannesHelper.hpp"
#include "TSPacket.hpp"
#include "TSDefines.hpp"

namespace Extractor
{
namespace TS
{

    TSPacket::TSPacket(const char *rawData)
    : _bufferPtr(nullptr)
    , AFC(nullptr)
    , PES(nullptr)
    {
        _bufferPtr = rawData;

        std::uint32_t tsHeader;

        std::memcpy(&tsHeader, rawData, sizeof(tsHeader));
        _bufferPtr += sizeof(tsHeader);

        tsHeader = Extractor::Util::EndiannessHelper::GetInstance().Rotate32(tsHeader);

        Sync     = TSPACKET_SYNC(tsHeader);
        TEI      = TSPACKET_TEI(tsHeader);
        PUSI     = TSPACKET_PUSI(tsHeader);
        Priority = TSPACKET_PRIORITY(tsHeader);
        PID      = TSPACKET_PID(tsHeader);
        TSC      = TSPACKET_TSC(tsHeader);
        Counter  = TSPACKET_CCOUNTER(tsHeader);
    
        auto adaptationFieldControl = TSPACKET_AFCONTROL(tsHeader);

        if (adaptationFieldControl > 1 )
        {
            _bufferPtr = ProcessAdaptationFieldControl(_bufferPtr);
        }

        if (adaptationFieldControl & 0x01)
        {
            _bufferPtr = ProcessPacketizedElementaryStream(_bufferPtr);
        }
    
        _dataLength = 188 - (_bufferPtr - rawData);
    }


    TSPacket::~TSPacket()
    {
        if (AFC != nullptr)
        {
            delete AFC;
        }
        if (PES != nullptr)
        {
            if (PES->OptionalHeader != nullptr)
            {
                delete PES->OptionalHeader;
            }

            delete PES;
        }
    }

    const char * TSPacket::ProcessPacketizedElementaryStream(const char *rawData)
    {
        std::uint32_t startCode;
        std::uint16_t length;

        std::memcpy(&startCode, rawData, sizeof(startCode));

        std::memcpy(&length, rawData + sizeof(startCode), sizeof(length));

        startCode = Extractor::Util::EndiannessHelper::GetInstance().Rotate32(startCode);
        length    = Extractor::Util::EndiannessHelper::GetInstance().Rotate16(length   );
        if ((PES_STARTCODE(startCode) == 1)
        {
            rawData += sizeof(startCode);
            rawData += sizeof(length);

            std::uint8_t streamId = startCode & 0xff;

            if (IS_PES_AUDIO(streamId) || IS_PES_VIDEO(streamId))
            {
                PES = new PacketizedElementaryStream;

                PES->StreamId = streamId;
                PES->Length   = length;

                std::uint16_t optionalPESHeader;
                std::memcpy((void*)&optionalPESHeader, rawData, sizeof(optionalPESHeader));

                optionalPESHeader = Extractor::Util::EndiannessHelper::GetInstance().Rotate16(optionalPESHeader);
                if (PES_OPTHEADER(optionalPESHeader) == 0x2)
                {
                    rawData += sizeof(optionalPESHeader);
    
                    PES->OptionalHeader = new PESOptionalHeader;

                    PES->OptionalHeader->Scrambling         = PES_OPTHEADER_SCRAMBLING(optionalPESHeader);
                    PES->OptionalHeader->Priority           = PES_OPTHEADER_PRIORITY(optionalPESHeader);
                    PES->OptionalHeader->DataAlignment      = PES_OPTHEADER_ALIGNMENT(optionalPESHeader);
                    PES->OptionalHeader->Copyright          = PES_OPTHEADER_COPYRIGHT(optionalPESHeader);
                    PES->OptionalHeader->Original           = PES_OPTHEADER_ORIGINAL(optionalPESHeader);
                    PES->OptionalHeader->PTSDTS             = PES_OPTHEADER_PTSDTS(optionalPESHeader);
                    PES->OptionalHeader->ESCR               = PES_OPTHEADER_ESCR(optionalPESHeader);
                    PES->OptionalHeader->ES                 = PES_OPTHEADER_ES(optionalPESHeader);
                    PES->OptionalHeader->DSMTrickMode       = PES_OPTHEADER_DSMTRICK(optionalPESHeader);
                    PES->OptionalHeader->AdditionalCopyInfo = PES_OPTHEADER_COPYINFO(optionalPESHeader);
                    PES->OptionalHeader->CRC                = PES_OPTHEADER_CRC(optionalPESHeader);
                    PES->OptionalHeader->Extension          = PES_OPTHEADER_EXTENSION(optionalPESHeader);

                    std::uint8_t pesHeaderLength;
                    std::memcpy(&pesHeaderLength, rawData, 1);
                    rawData += sizeof(pesHeaderLength);
                    PES->OptionalHeader->PESHeaderLength = pesHeaderLength;

                    rawData += pesHeaderLength;
                }
            }
        }
        return rawData;
    }

    const char * TSPacket::ProcessAdaptationFieldControl(const char *bufferPtr)
    {
        std::uint8_t length;
        std::uint8_t bitFlags;

        std::memcpy(&length, _bufferPtr, sizeof(length));
        bufferPtr += sizeof(length);

        std::memcpy(&bitFlags, bufferPtr, sizeof(bitFlags));

        AFC = new AdaptationFieldControl;

        AFC->Length                 = length;
        AFC->Discontinuity          = AFC_DISCONTINUITY(bitFlags);
        AFC->RandomAccess           = AFC_RANDOMACCESS(bitFlags);
        AFC->ESPriority             = AFC_ESPRIORITY(bitFlags);
        AFC->PCR                    = AFC_PCR(bitFlags);
        AFC->OPCR                   = AFC_OPCR(bitFlags);
        AFC->SplicingPoint          = AFC_SPLICINGPOINT(bitFlags);
        AFC->TransportPrivateData   = AFC_PRIVATEDATA(bitFlags);
        AFC->AdaptionFieldExtension = AFC_AFEXT(bitFlags);

        return bufferPtr + AFC->Length;
    }


    void TSPacket::DebugHeaders()
    {
        printf("TS Header\n");
        printf("Sync                     : %x\n"  , Sync     );
        printf("TEI                      : %x\n"  , TEI      );
        printf("PUSI                     : %x\n"  , PUSI     );
        printf("Transport Priority       : %x\n"  , Priority );
        printf("PID                      : %d\n"  , PID      );
        printf("TSC                      : %x\n"  , TSC      );
        printf("Counter                  : %d\n\n", Counter  );

        if (AFC != nullptr)
        {
            printf("Adaptation field\n");
            printf("Length                   : %d\n"  , AFC->Length                 );
            printf("Discontinuity indicator  : %d\n"  , AFC->Discontinuity          );
            printf("Random Access indicator  : %d\n"  , AFC->RandomAccess           );
            printf("ES priority indicaor     : %d\n"  , AFC->ESPriority             );
            printf("PCR                      : %d\n"  , AFC->PCR                    );
            printf("OPCR                     : %d\n"  , AFC->OPCR                   );
            printf("Splicing point           : %d\n"  , AFC->SplicingPoint          );
            printf("Transport private data   : %d\n"  , AFC->TransportPrivateData   );
            printf("Adaptation field ext     : %d\n\n", AFC->AdaptionFieldExtension );
        }

        if (PES != nullptr)
        {
            printf("PES header found : audio packet %d Length %d\n", PES->StreamId, PES->Length);
            if (PES->OptionalHeader)
            {
                printf("PES Scrambling           : %d\n"  , PES->OptionalHeader->Scrambling         );
                printf("PES Priority             : %d\n"  , PES->OptionalHeader->Priority           );
                printf("Data Alignment           : %d\n"  , PES->OptionalHeader->DataAlignment      );
                printf("Copyright                : %d\n"  , PES->OptionalHeader->Copyright          );
                printf("Original                 : %d\n"  , PES->OptionalHeader->Original           );
                printf("PTS DTS                  : %d\n"  , PES->OptionalHeader->PTSDTS             );
                printf("ESCR                     : %d\n"  , PES->OptionalHeader->ESCR               );
                printf("ES                       : %d\n"  , PES->OptionalHeader->ES                 );
                printf("DSM Trick Mode           : %d\n"  , PES->OptionalHeader->DSMTrickMode       );
                printf("Additional Copy Info     : %d\n"  , PES->OptionalHeader->AdditionalCopyInfo );
                printf("CRC                      : %d\n"  , PES->OptionalHeader->CRC                );
                printf("Extension                : %d\n\n", PES->OptionalHeader->Extension          );
            }
        }
        std::cout << std::endl;
    }

}
}
