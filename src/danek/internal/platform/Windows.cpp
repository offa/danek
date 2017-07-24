
#include "danek/internal/platform/Platform.h"
#include <array>
#include <windows.h>

namespace danek
{
    namespace platform
    {

        std::string execCmd(const std::string& cmd)
        {
            (void) cmd;
            throw "Not implemented yet";
        }

        bool isCmdInDir(const std::string& cmd, const std::string& dir)
        {
            std::array<std::string, 3> extensions{{"", ".exe", ".bat"}};

            for( const auto& ext : extensions )
            {
                const std::string fileName = dir + pathSeparator() + cmd + ext;
                DWORD fileAttr = GetFileAttributes(fileName.c_str());

                if (fileAttr != 0xFFFFFFFF)
                {
                    return true;
                }
            }
            return false;
        }

    }
}


