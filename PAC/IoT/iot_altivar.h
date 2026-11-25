#pragma once

#include "iot_base.h"
#include "smart_ptr.h"
#include <cstdint>
#include <map>
#include <string>
#include <array>
#include <memory>

class modbus_client;

class altivar_node: public i_iot_node
	{
	public:
		altivar_node(unsigned int id, const char* ip, unsigned int port,
            uint32_t exchangetimeout, int type, const char* name );
		~altivar_node() override = default;
		void Evaluate();
		void Enable();
		void Disable();
		bool enabled;
		unsigned long queryinterval;


		float frq_value;
		int state;
		int remote_state;
		int cmd;
		float rpm_setpoint;
		int rpm_value;
		float frq_max;
		float amperage;
		int reverse;
		char    ip_address[16];
		void set_output_in_percent( float value );
		float get_output_in_percent( );

		enum CFG_STEP
			{
			CFG_STEP_INIT_OUTPUTS = 0,
			CFG_STEP_SET_OUTPUTS,
			CFG_STEP_INIT_INPUTS,
			CFG_STEP_SET_INPUTS,
			CFG_STEP_INIT_IOSCANNER,
			CFG_STEP_SET_IOSCANNER,
			CFG_STEP_INIT_IOPROFILE,
			CFG_STEP_SET_IOPROFILE,
			CFG_STEP_INIT_REF1,
			CFG_STEP_SET_REF1,
			CFG_STEP_INIT_CMD1,
			CFG_STEP_SET_CMD1,
			CFG_STEP_INIT_FAULTRESET,
			CFG_STEP_SET_FAULTRESET,
			CFG_STEP_INIT_SAVESETTINGS,
			CFG_STEP_SET_SAVESETTINGS,
			CFG_STEP_END = 99,
			};

		enum RUN_STEP
			{
			RUN_STEP_CHECK_CONFIG = 0,
			RUN_STEP_CONFIG = 1,
			RUN_STEP_INIT_IOSCANNER,
			RUN_STEP_QUERY_IOSCANNER,
			RUN_STEP_INIT_END = 98,
			RUN_STEP_END = 99,
			};

		enum TYPES
			{
			TYPE_ATV320 = 320,
			TYPE_ATV630 = 630,
			};

        enum settings_const
            {
            FRQ_MIN_SETTING = 10,
            FRQ_MAX_SETTING = 120
            };

    private:
	    float frq_setpoint;
        std::array<char, 50 > name{};

        // Explicitly delete the copy constructors.
        altivar_node( const altivar_node& ) = delete;
        altivar_node( altivar_node&& ) = delete;
        altivar_node& operator=( const altivar_node& ) = delete;
        altivar_node& operator=( altivar_node&& ) = delete;

	protected:
        std::unique_ptr<modbus_client> mc;
		bool configure;
		int type;
		int querystep;
		int configurestep;
		uint32_t querytimer;
		uint32_t modbustimeout;
		int ismodbuserror;
	};

typedef std::map<std::string, altivar_node*> altivar_node_map;
typedef std::pair<std::string, altivar_node*> altivar_node_pair;

typedef std::map<unsigned int, altivar_node*> altivar_node_num_map;
typedef std::pair<unsigned int, altivar_node*> altivar_node_num_pair;

class altivar_manager
	{
	public:

		virtual ~altivar_manager();

		static altivar_manager* get_instance();
		void add_node(const char* IP_address, unsigned int port, unsigned int timeout,
            const char* article, const char* motor_name );
		altivar_node* get_node(const char* IP_address);
		altivar_node* get_node(unsigned int id);
		void evaluate();
	protected:
		altivar_manager();
		altivar_node_map nodes;
		altivar_node_num_map num_nodes;
		//altivar_node** nodes;
		//u_int nodes_count;

		static auto_smart_ptr< altivar_manager > instance;
		static unsigned int index;
	};



altivar_manager* G_ALTIVAR_MANAGER();

