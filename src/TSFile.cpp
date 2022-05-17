#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "TSFile.hpp"
#include "TSPacket.hpp"
#include "PidWriter.hpp"

namespace Extractor
{
    namespace TS
    {

        TSFile::TSFile(const std::string &filename, bool debugHeaders)
            : _debugHeaders(debugHeaders)
        {
            _sourceFile.open(filename, std::ios_base::binary);
            if (!_sourceFile.is_open())
            {
                std::cerr << "Failed to open file " << filename << std::endl;
            }
        }

        bool TSFile::Extract()
        {
            if (!_sourceFile.is_open())
            {
                return false;
            }

            char buffer[188];

            std::map<int, std::unique_ptr<Extractor::Util::PidWriter>> pidHandleList;

            int counter = 0;
            while (!_sourceFile.eof())
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

                        auto writer = std::make_unique<Extractor::Util::PidWriter>(filename.str());
                        writer->Add(packet.GetDataPtr(), packet.GetDataLength());
                        pidHandleList.emplace(packet.PID, std::move(writer));
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
            return true;
        }
    }
}
