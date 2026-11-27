#include "build_info.h"

#include <string>

#include "build_info_data.h"
#include "version_info.h"

namespace
{
std::string MakeBuildID()
    {
    std::string id { PRODUCT_VERSION_FULL_STR "+" PTUSA_GIT_SHA };
    if ( PTUSA_GIT_DIRTY )
        {
        id += "-dirty";
        }
    return id;
    }
}

const char* build_info::GetBuildID()
    {
    static const std::string id { MakeBuildID() };
    return id.c_str();
    }

std::string build_info::GetVersionText()
    {
    std::string text { PTUSA_PROJECT_NAME " version " };
    text += GetBuildID();
    text += " (" PTUSA_BUILD_DATE ")\n";

    if ( '\0' != PTUSA_INFO_URL [ 0 ] )
        {
        text += PTUSA_INFO_URL;
        text += '\n';
        }
    return text;
    }

const char* build_info::GetBuildDate()
    {
    return PTUSA_BUILD_DATE;
    }

const char* build_info::GetInfoURL()
    {
    return PTUSA_INFO_URL;
    }

const char* build_info::GetGitSHA()
    {
    return PTUSA_GIT_SHA;
    }

bool build_info::IsDirtyBuild()
    {
    return PTUSA_GIT_DIRTY != 0;
    }
