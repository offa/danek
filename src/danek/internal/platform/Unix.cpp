
#include "danek/internal/platform/Platform.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
            const std::string fileName = dir + pathSeparator() + cmd;
            struct stat sb;
            return ( stat(fileName.c_str(), &sb) == 0 );
        }

    }
}

