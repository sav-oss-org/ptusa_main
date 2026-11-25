#pragma once
#include "Arp/System/Core/Arp.h"
#if ARP_ABI_VERSION_MAJOR < 2
#include "Arp/System/Acf/ComponentBase.hpp"
#include "Arp/System/Acf/IApplication.hpp"
#else
#include "Arp/Base/Acf/Commons/ComponentBase.hpp"
#endif
#include "Arp/Plc/Commons/Esm/ProgramComponentBase.hpp"
#include "PtusaMainCmpntProgramProvider.hpp"
#include "Arp/Plc/Commons/Meta/MetaLibraryBase.hpp"
#include "Arp/System/Commons/Logging.h"

#include "log.h"

namespace PtusaPLCnextEngineer
    {
    using namespace Arp;
#if ARP_ABI_VERSION_MAJOR < 2
    using namespace Arp::System::Acf;
#else
    using namespace Arp::Base::Acf::Commons;
#endif
    using namespace Arp::Plc::Commons::Esm;
    using namespace Arp::Plc::Commons::Meta;

    //#component
    class PtusaMainCmpnt : public ComponentBase,
                           public ProgramComponentBase,
                           private Loggable<PtusaMainCmpnt>
        {
        public:
            // typedefs

        public:
            void Start( void )
                {
                sprintf( G_LOG->msg, "Start command" );
                G_LOG->write_log( i_log::P_INFO );

                running = 1;
                };

            void Stop( void )
                {
                sprintf( G_LOG->msg, "Stop command" );
                G_LOG->write_log( i_log::P_INFO );

                running = 0;
                };

                // construction/destruction
#if ARP_ABI_VERSION_MAJOR < 2
            PtusaMainCmpnt( IApplication& application, const String& name );
            virtual ~PtusaMainCmpnt() = default;
#else
            PtusaMainCmpnt( ILibrary& library, const String& name );
#endif

        public:
            // IComponent operations
            void Initialize() override;
            void LoadConfig() override;
            void SetupConfig() override;
            void ResetConfig() override;

        public:
            // ProgramComponentBase operations
            void RegisterComponentPorts() override;

        private:
#if ARP_ABI_VERSION_MAJOR < 2
            PtusaMainCmpnt( const PtusaMainCmpnt& arg ) = delete;
            PtusaMainCmpnt& operator=( const PtusaMainCmpnt& arg ) = delete;

        public: // static factory operations
            static IComponent::Ptr Create(
                Arp::System::Acf::IApplication& application,
                const String& name );
#endif


        private:
            // fields
            PtusaMainCmpntProgramProvider programProvider;

        public:
            int running{ 0 };
            bool init_flag{ true };
            bool error_flag{ false };

        public:
            /* Ports
         =====
         Component ports are defined in the following way:
         //#port
         //#name(NameOfPort)
         boolean portField;

         The name comment defines the name of the port and is optional. Default is the name of the field.
         Attributes which are defined for a component port are IGNORED. If component ports with attributes
         are necessary, define a single structure port where attributes can be defined foreach field of the
         structure.
         */
        };

        ///////////////////////////////////////////////////////////////////////////////
        // inline methods of class PtusaMainCmpnt
#if ARP_ABI_VERSION_MAJOR < 2
    inline IComponent::Ptr PtusaMainCmpnt::Create(
        Arp::System::Acf::IApplication& application, const String& name )
        {
        return IComponent::Ptr( new PtusaMainCmpnt( application, name ) );
        }
#endif
    } // end of namespace PtusaPLCnextEngineer
