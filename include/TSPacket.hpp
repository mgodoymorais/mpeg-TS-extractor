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
        int			_dataLength;

    public:

        char Sync;
        int  TEI;
        int  PUSI;
        int  Priority;
        int  PID;
        int  TSC;
        int  Counter;

        AdaptationFieldControl *	 AFC;
        PacketizedElementaryStream * PES;

    // class methods
    private:

        const char * ProcessPacketizedElementaryStream(const char *rawData);
        const char * ProcessAdaptationFieldControl(const char *rawData);

    public:

        TSPacket(const char *rawData);
        virtual ~TSPacket();

        void				DebugHeaders();

        inline int			GetDataLength() { return _dataLength; }
        inline const char *	GetDataPtr()	{ return _bufferPtr;  }

        inline bool			IsAudioPacket() { return (PES != nullptr) && IS_PES_AUDIO(PES->StreamId); }
        inline bool			IsVideoPacket() { return (PES != nullptr) && IS_PES_VIDEO(PES->StreamId); }
    };

}
}