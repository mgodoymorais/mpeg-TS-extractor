#include "TSStructures.hpp"
#include "TSDefines.hpp"

namespace Extractor
{
    namespace TS
    {

        class TSPacket
        {
            // member vars
        private:
            const char *_bufferPtr;
            int _dataLength;

        public:
            char Sync;
            int TEI;
            int PUSI;
            int Priority;
            int PID;
            int TSC;
            int Counter;

            std::unique_ptr<AdaptationFieldControl> AFC;
            std::unique_ptr<PacketizedElementaryStream> PES;

            // class methods
        private:
            const char *ProcessPacketizedElementaryStream(const char *rawData);
            const char *ProcessAdaptationFieldControl(const char *rawData);

        public:
            TSPacket(const char *rawData);

            void DebugHeaders();

            inline const int GetDataLength() const { return _dataLength; };
            inline const char *GetDataPtr() const { return _bufferPtr; };

            inline const bool IsAudioPacket() const { return (PES != nullptr) && IS_PES_AUDIO(PES->StreamId); };
            inline const bool IsVideoPacket() const { return (PES != nullptr) && IS_PES_VIDEO(PES->StreamId); };
        };

    }
}