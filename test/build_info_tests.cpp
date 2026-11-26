#include <string>

#include "includes.h"
#include "build_info.h"
#include "version_info.h"

TEST( build_info, id_format )
    {
    const std::string sha { build_info::GetGitSHA() };
    ASSERT_FALSE( sha.empty() );

    const std::string id { build_info::GetBuildID() };
    const std::string expected {
        std::string( PRODUCT_VERSION_FULL_STR "+" ) + sha
        + ( build_info::IsDirtyBuild() ? "-dirty" : "" ) };
    EXPECT_EQ( expected, id );
    }

TEST( build_info, unknown_sha_is_not_dirty )
    {
    if ( std::string( "unknown" ) == build_info::GetGitSHA() )
        {
        EXPECT_FALSE( build_info::IsDirtyBuild() );
        EXPECT_EQ( std::string( PRODUCT_VERSION_FULL_STR "+unknown" ),
            build_info::GetBuildID() );
        }
    }
