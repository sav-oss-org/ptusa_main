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

const char* build_info::GetGitSHA()
    {
    return PTUSA_GIT_SHA;
    }

bool build_info::IsDirtyBuild()
    {
    return PTUSA_GIT_DIRTY != 0;
    }
