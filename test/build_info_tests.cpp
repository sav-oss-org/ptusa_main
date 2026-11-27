#include <cstring>
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

TEST( build_info, version_text_gh_style )
    {
    // First line: "<name> version <full version> (<date>)".
    const std::string first_line {
        std::string( "ptusa_main version " ) + build_info::GetBuildID()
        + " (" + build_info::GetBuildDate() + ")\n" };
    EXPECT_EQ( 10u, std::strlen( build_info::GetBuildDate() ) );

    const std::string text { build_info::GetVersionText() };
    ASSERT_EQ( 0u, text.compare( 0, first_line.size(), first_line ) );

    // Optional URL line: release page for a tagged build, commit
    // page otherwise, absent when no repository is configured.
    const std::string url { build_info::GetInfoURL() };
    if ( !url.empty() )
        {
        EXPECT_EQ( first_line + url + "\n", text );
        }
    else
        {
        EXPECT_EQ( first_line, text );
        }
    }
