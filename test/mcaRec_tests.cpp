#include "mcaRec_tests.h"

#include <cstdio>
#include <string>

using namespace ::testing;

TEST( TMediumRecipeManager, LoadFromFile )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );

    auto recipeNo = 0;
    mngr.ResetRecipeToDefaults( recipeNo );
    auto fileName = "tmp1.txt";
    EXPECT_EQ( 0, mngr.SaveToFile( fileName ) );
    EXPECT_EQ( 0, mngr.LoadFromFile( fileName ) );
    }

TEST( TMediumRecipeManager, CopyRecipe )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );
    mngr.CopyRecipe();
    }

TEST( TMediumRecipeManager, PasteRecipe )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );
    mngr.PasteRecipe();
    }

TEST( TMediumRecipeManager, NullifyRecipe )
    {
    TMediumRecipeManager mngr( TMediumRecipeManager::MT_CAUSTIC );
    mngr.NullifyRecipe();
    }


TEST( TRecipeManager, LoadFromFile )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );

    auto recipeNo = 0;
    mngr.ResetRecipeToDefaults( recipeNo );

    auto fileName = "tmp2.txt";
    EXPECT_EQ( 0, mngr.SaveToFile( fileName ) );
    EXPECT_EQ( 0, mngr.LoadFromFile( fileName ) );
    }

TEST( TRecipeManager, CopyRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );
    mngr.CopyRecipe();
    }

TEST( TRecipeManager, PasteRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );
    mngr.PasteRecipe();
    }

TEST( TRecipeManager, NullifyRecipe )
    {
    auto lineNo = 1;
    TRecipeManager mngr( lineNo );
    mngr.NullifyRecipe();
    }

namespace
{

/// Checks that the helper rejects the name and clears output.
void ExpectRejectedRecipeName( const char* name )
    {
    std::string path{ "sentinel" };
    EXPECT_FALSE( MakeSafeRecipePath( name, path ) ) << name;
    EXPECT_TRUE( path.empty() ) << name;
    }

} // namespace

TEST( RecipePathValidation, AcceptsPlainFileNames )
    {
    std::string path{ "sentinel" };
    EXPECT_TRUE( MakeSafeRecipePath( "tmp3.bin", path ) );
    EXPECT_EQ( "tmp3.bin", path );
    EXPECT_TRUE( MakeSafeRecipePath( "line1rec.bin", path ) );
    EXPECT_EQ( "line1rec.bin", path );
    EXPECT_TRUE( MakeSafeRecipePath( "recipe 1.bin", path ) );
    EXPECT_EQ( "recipe 1.bin", path );
    }

TEST( RecipePathValidation, RejectsTraversalSeparatorsAndDrives )
    {
    const char* badNames[] { "..", "../evil.bin", "a/../../evil",
        "..\\evil.bin", "sub/evil.bin", "sub\\evil.bin",
        "/etc/passwd", "\\evil.bin", "C:\\evil.bin", "C:evil.bin",
        "C:/evil.bin", "\\\\server\\share\\evil.bin",
        "name..x", "evil/../evil.bin", "." };
    for ( const char* name : badNames )
        {
        ExpectRejectedRecipeName( name );
        }
    }

TEST( RecipePathValidation, RejectsEmptyNames )
    {
    ExpectRejectedRecipeName( "" );
    std::string path{ "sentinel" };
    EXPECT_FALSE( MakeSafeRecipePath( nullptr, path ) );
    EXPECT_TRUE( path.empty() );
    }

TEST( RecipePathValidation, ManagersRefuseUnsafeFileNames )
    {
    auto lineNo = 1;
    TRecipeManager lineMgr( lineNo );
    EXPECT_EQ( 1, lineMgr.SaveToFile( "../tmp1621_evil.bin" ) );
    EXPECT_EQ( 1, lineMgr.LoadFromFile( "../tmp1621_evil.bin" ) );
    TMediumRecipeManager mediumMgr(
        TMediumRecipeManager::MT_CAUSTIC );
    EXPECT_EQ( 1, mediumMgr.SaveToFile( "sub/tmp1621_evil.bin" ) );
    EXPECT_EQ( 1, mediumMgr.LoadFromFile( "C:/tmp1621_evil.bin" ) );
    EXPECT_EQ( nullptr, std::fopen( "../tmp1621_evil.bin", "rb" ) );
    }
