#pragma once

#ifndef build_info_h_
#define build_info_h_

#include <string>

// Build identification (issue #1647): ties a binary or an installed
// artifact to the exact repository state it was compiled from.

namespace build_info
{
    // Full one-line version string "<version>+<git sha>[-dirty]";
    // the sha is "unknown" for builds without the repository
    // information. --version shows it, and PAC_info reports the
    // same string to the supervisor in its VERSION tag.
    const char* GetBuildID();

    // Full --version text in the gh CLI style:
    // "<name> version <full version> (<date>)\n<url>\n". The URL is
    // the release page of an exact tagged build, the commit page
    // otherwise, and the whole line is omitted when the build has
    // no repository address.
    std::string GetVersionText();

    // Date shown in parentheses next to the version, "YYYY-MM-DD":
    // the commit date, or the configure date without git.
    const char* GetBuildDate();

    // Release or commit page of this build, "" when no repository
    // address is configured.
    const char* GetInfoURL();

    // Short git commit hash the build was configured from, or
    // "unknown".
    const char* GetGitSHA();

    // True when the tree carried uncommitted changes at configure
    // time.
    bool IsDirtyBuild();
}

#endif
