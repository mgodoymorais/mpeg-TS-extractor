#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "TSFile.hpp"
#include "TSPacket.hpp"
#include "PidWriter.hpp"

namespace Extractor
{
namespace TS
{

    TSFile::TSFile(const char * filename, bool debugHeaders)
    : _debugHeaders(debugHeaders)
    {
        _sourceFile.open(filename, std::ios_base::binary);
    }

    TSFile::~TSFile()
    {
        if (_sourceFile.is_open())
        {
            _sourceFile.close();
        }
    }

    void TSFile::Extract()
    {
        char buffer[188];

        auto pidHandleList = std::unordered_map<int, Extractor::Util::PidWriter* >();

        int counter = 0;
        while(!_sourceFile.eof())
        {
            counter++;
            _sourceFile.read(buffer, 188);
            auto packet = TSPacket(buffer);
            if (packet.PUSI && (packet.IsAudioPacket() || packet.IsVideoPacket()))
            {
                if (_debugHeaders)
                {
                    packet.DebugHeaders();
                }

                auto x = pidHandleList.find(packet.PID);
                if (x == pidHandleList.end())
                {
                    std::ostringstream filename;
                    filename << "extracted_pid_" << packet.PID << (packet.IsAudioPacket() ? ".aac" : ".avc1");
                
                    auto writer = new Extractor::Util::PidWriter(filename.str().c_str());
                    pidHandleList[packet.PID] = writer;
                    writer->Add(packet.GetDataPtr(), packet.GetDataLength());
                }
                else
                {
                    x->second->Add(packet.GetDataPtr(), packet.GetDataLength());
                }
            }
            else 
            {
                auto x = pidHandleList.find(packet.PID);
                if (x != pidHandleList.end())
                {
                    x->second->Add(packet.GetDataPtr(), packet.GetDataLength());
                }
            }
        }
        for(auto it : pidHandleList)
        {
            delete it.second;
        }
    }

}
}
