#include "PtusaMainCmpnt.hpp"
#include "Arp/Plc/Commons/Esm/ProgramComponentBase.hpp"
#include "PtusaPLCnextEngineerLibrary.hpp"


namespace PtusaPLCnextEngineer
    {
#if ARP_ABI_VERSION_MAJOR < 2
    PtusaMainCmpnt::PtusaMainCmpnt(
        IApplication& application, const String& name ) :
        ComponentBase( application,
            ::PtusaPLCnextEngineer::PtusaPLCnextEngineerLibrary::GetInstance(),
            name, ComponentCategory::Custom ),
        programProvider( *this ),
        ProgramComponentBase(
            ::PtusaPLCnextEngineer::PtusaPLCnextEngineerLibrary::GetInstance()
                .GetNamespace(),
            programProvider )
#else
    PtusaMainCmpnt::PtusaMainCmpnt( ILibrary& library, const String& name ) :
        ComponentBase(
            library, name, ComponentCategory::Custom, GetDefaultStartOrder() ),
        programProvider( *this ),
        ProgramComponentBase(
            ::PtusaPLCnextEngineer::PtusaPLCnextEngineerLibrary::GetInstance()
                .GetNamespace(),
            programProvider )
#endif
        {
        }

    void PtusaMainCmpnt::Initialize()
        {
        // never remove next line
        ProgramComponentBase::Initialize();

        // subscribe events from the event system (Nm) here
        }

    void PtusaMainCmpnt::LoadConfig()
        {
        // load project config here
        }

    void PtusaMainCmpnt::SetupConfig()
        {
        // never remove next line
        ProgramComponentBase::SetupConfig();

        // setup project config here
        }

    void PtusaMainCmpnt::ResetConfig()
        {
        // never remove next line
        ProgramComponentBase::ResetConfig();

        // implement this inverse to SetupConfig() and LoadConfig()
        }
    } // end of namespace PtusaPLCnextEngineer
