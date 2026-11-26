#pragma once

#ifndef build_info_h_
#define build_info_h_

// Build identification (issue #1647): ties a binary or an installed
// artifact to the exact repository state it was compiled from.

namespace build_info
{
    // One-line identifier "<version>+<git sha>[-dirty]"; the sha is
    // "unknown" for builds without the repository information.
    const char* GetBuildID();

    // Short git commit hash the build was configured from, or
    // "unknown".
    const char* GetGitSHA();

    // True when the tree carried uncommitted changes at configure
    // time.
    bool IsDirtyBuild();
}

#endif
