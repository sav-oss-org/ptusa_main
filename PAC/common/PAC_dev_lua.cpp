/*
** Lua binding: PAC_dev
** Generated automatically by tolua++-1.0.92 on Wed Aug 26 12:58:59 2026.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_PAC_dev_open (lua_State* tolua_S);

#include <stdlib.h>
#include "device/base.h"
#include "device/device.h"
#include "device/manager.h"
#include "tech_def.h"
#include "cip_tech_def.h"
#include "bus_coupler_io.h"
#include "PID.h"
#include "g_device.h"
#include "g_errors.h"
#include "modbus_client.h"
#include "modbus_serv.h"
#include "params_recipe_manager.h"
#ifdef WIN_OS
#pragma warning(disable: 4800) //Warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 6011)  //dereferencing NULL pointer <name>.
#pragma warning(disable: 26812) //Prefer 'enum class' over 'enum'.
#endif // WIN_OS
 // Check for correct "uint32_t get_millisec()".
 static_assert( std::is_same_v<uint32_t, unsigned int>, "uint32_t should be unsigned int" );

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_PID (lua_State* tolua_S)
{
 PID* self = (PID*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_tm (lua_State* tolua_S)
{
 tm* self = (tm*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_timer (lua_State* tolua_S)
{
 timer* self = (timer*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_cipline_tech_object (lua_State* tolua_S)
{
 cipline_tech_object* self = (cipline_tech_object*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_modbus_client (lua_State* tolua_S)
{
 modbus_client* self = (modbus_client*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_tech_object (lua_State* tolua_S)
{
 tech_object* self = (tech_object*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"i_DO_AO_device");
 tolua_usertype(tolua_S,"step");
 tolua_usertype(tolua_S,"node_dev");
 tolua_usertype(tolua_S,"level");
 tolua_usertype(tolua_S,"siren_lights_manager");
 tolua_usertype(tolua_S,"saved_params_float");
 tolua_usertype(tolua_S,"device");
 tolua_usertype(tolua_S,"io_manager");
 tolua_usertype(tolua_S,"virtual_counter");
 tolua_usertype(tolua_S,"counter_iolink");
 tolua_usertype(tolua_S,"errors_manager");
 tolua_usertype(tolua_S,"tech_object_manager");
 tolua_usertype(tolua_S,"tech_object");
 tolua_usertype(tolua_S,"operation_state");
 tolua_usertype(tolua_S,"timer");
 tolua_usertype(tolua_S,"PID");
 tolua_usertype(tolua_S,"PAC_info");
 tolua_usertype(tolua_S,"dev_stub");
 tolua_usertype(tolua_S,"ModbusServ");
 tolua_usertype(tolua_S,"modbus_client");
 tolua_usertype(tolua_S,"action");
 tolua_usertype(tolua_S,"cipline_tech_object");
 tolua_usertype(tolua_S,"operation");
 tolua_usertype(tolua_S,"MSAPID");
 tolua_usertype(tolua_S,"tm");
 tolua_usertype(tolua_S,"i_Lua_save_device");
 tolua_usertype(tolua_S,"dev_errors_manager");
 tolua_usertype(tolua_S,"device_manager");
 tolua_usertype(tolua_S,"i_camera");
 tolua_usertype(tolua_S,"i_log");
 tolua_usertype(tolua_S,"camera");
 tolua_usertype(tolua_S,"ParamsRecipeStorage");
 tolua_usertype(tolua_S,"ParamsRecipeManager");
 tolua_usertype(tolua_S,"run_time_params_float");
 tolua_usertype(tolua_S,"run_time_params_u_int_4");
 tolua_usertype(tolua_S,"i_DI_device");
 tolua_usertype(tolua_S,"saved_params_u_int_4");
 tolua_usertype(tolua_S,"ParamsRecipeAdapter");
 tolua_usertype(tolua_S,"i_DO_device");
 tolua_usertype(tolua_S,"i_counter");
 tolua_usertype(tolua_S,"timer_manager");
 tolua_usertype(tolua_S,"i_AO_device");
 tolua_usertype(tolua_S,"i_wages");
 tolua_usertype(tolua_S,"valve");
 tolua_usertype(tolua_S,"i_AI_device");
 tolua_usertype(tolua_S,"signal_column");
 tolua_usertype(tolua_S,"io_manager::io_node");
 tolua_usertype(tolua_S,"operation_manager");
 tolua_usertype(tolua_S,"io_device");
 tolua_usertype(tolua_S,"i_motor");
}

/* method: get_state of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_get_state00
static int tolua_PAC_dev_i_DI_device_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DI_device* self = (i_DI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_active of class  i_DI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DI_device_is_active00
static int tolua_PAC_dev_i_DI_device_is_active00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DI_device* self = (i_DI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_active'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_active();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_active'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_on00
static int tolua_PAC_dev_i_DO_device_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_off00
static int tolua_PAC_dev_i_DO_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: instant_off of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_direct_off00
static int tolua_PAC_dev_i_DO_device_direct_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'instant_off'", NULL);
#endif
  {
   self->instant_off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'direct_off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: instant_off of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_instant_off00
static int tolua_PAC_dev_i_DO_device_instant_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'instant_off'", NULL);
#endif
  {
   self->instant_off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'instant_off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  i_DO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_device_set_state00
static int tolua_PAC_dev_i_DO_device_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_device* self = (i_DO_device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  i_AI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AI_device_get_value00
static int tolua_PAC_dev_i_AI_device_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AI_device* self = (i_AI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  i_AI_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AI_device_get_state00
static int tolua_PAC_dev_i_AI_device_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AI_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AI_device* self = (i_AI_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AO_device_off00
static int tolua_PAC_dev_i_AO_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AO_device* self = (i_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  i_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_AO_device_set_value00
static int tolua_PAC_dev_i_AO_device_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_AO_device* self = (i_AO_device*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_get_state00
static int tolua_PAC_dev_i_DO_AO_device_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_set_state00
static int tolua_PAC_dev_i_DO_AO_device_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_on00
static int tolua_PAC_dev_i_DO_AO_device_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_off00
static int tolua_PAC_dev_i_DO_AO_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_get_value00
static int tolua_PAC_dev_i_DO_AO_device_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_DO_AO_device_set_value00
static int tolua_PAC_dev_i_DO_AO_device_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_DO_AO_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: __i_AO_device__ of class  i_DO_AO_device */
#ifndef TOLUA_DISABLE_tolua_get_i_DO_AO_device___i_AO_device__
static int tolua_get_i_DO_AO_device___i_AO_device__(lua_State* tolua_S)
{
  i_DO_AO_device* self = (i_DO_AO_device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable '__i_AO_device__'",NULL);
#endif
#ifdef __cplusplus
   tolua_pushusertype(tolua_S,(void*)static_cast<i_AO_device*>(self), "i_AO_device");
#else
   tolua_pushusertype(tolua_S,(void*)((i_AO_device*)self), "i_AO_device");
#endif
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: pause of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_pause00
static int tolua_PAC_dev_i_counter_pause00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pause'", NULL);
#endif
  {
   self->pause();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pause'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: start of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_start00
static int tolua_PAC_dev_i_counter_start00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'start'", NULL);
#endif
  {
   self->start();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'start'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_reset00
static int tolua_PAC_dev_i_counter_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: restart of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_restart00
static int tolua_PAC_dev_i_counter_restart00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'restart'", NULL);
#endif
  {
   self->restart();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'restart'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_quantity of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_quantity00
static int tolua_PAC_dev_i_counter_get_quantity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_quantity'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_quantity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_quantity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_flow of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_flow00
static int tolua_PAC_dev_i_counter_get_flow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_flow'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_flow();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_flow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_state00
static int tolua_PAC_dev_i_counter_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_abs_quantity of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_get_abs_quantity00
static int tolua_PAC_dev_i_counter_get_abs_quantity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_abs_quantity'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_abs_quantity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_abs_quantity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: abs_reset of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_abs_reset00
static int tolua_PAC_dev_i_counter_abs_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'abs_reset'", NULL);
#endif
  {
   self->abs_reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'abs_reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pause_daily of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_pause_daily00
static int tolua_PAC_dev_i_counter_pause_daily00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
  i_counter::DAY_CTR n = ((i_counter::DAY_CTR) (int)  tolua_tonumber(tolua_S,2,i_counter::DAY_CTR::DAY_T1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pause_daily'", NULL);
#endif
  {
   self->pause_daily(n);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pause_daily'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: start_daily of class  i_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_counter_start_daily00
static int tolua_PAC_dev_i_counter_start_daily00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_counter",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_counter* self = (i_counter*)  tolua_tousertype(tolua_S,1,0);
  i_counter::DAY_CTR n = ((i_counter::DAY_CTR) (int)  tolua_tonumber(tolua_S,2,i_counter::DAY_CTR::DAY_T1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'start_daily'", NULL);
#endif
  {
   self->start_daily(n);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'start_daily'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_off00
static int tolua_PAC_dev_device_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_on00
static int tolua_PAC_dev_device_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_state00
static int tolua_PAC_dev_device_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_get_value00
static int tolua_PAC_dev_device_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_value00
static int tolua_PAC_dev_device_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_cmd00
static int tolua_PAC_dev_device_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_par of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_par00
static int tolua_PAC_dev_device_set_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  float value = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_par'", NULL);
#endif
  {
   self->set_par(idx,offset,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_rt_par of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_rt_par00
static int tolua_PAC_dev_device_set_rt_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_rt_par'", NULL);
#endif
  {
   self->set_rt_par(idx,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_rt_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_property of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_property00
static int tolua_PAC_dev_device_set_property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* field = ((const char*)  tolua_tostring(tolua_S,2,0));
  device* dev = ((device*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_property'", NULL);
#endif
  {
   self->set_property(field,dev);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_string_property of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_string_property00
static int tolua_PAC_dev_device_set_string_property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* field = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* value = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_string_property'", NULL);
#endif
  {
   self->set_string_property(field,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_string_property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_descr of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_set_descr00
static int tolua_PAC_dev_device_set_descr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  const char* description = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_descr'", NULL);
#endif
  {
   self->set_descr(description);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_descr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_type_str of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_get_type_str00
static int tolua_PAC_dev_device_get_type_str00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const device* self = (const device*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_type_str'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->get_type_str();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_type_str'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: param_emulator of class  device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_param_emulator00
static int tolua_PAC_dev_device_param_emulator00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device* self = (device*)  tolua_tousertype(tolua_S,1,0);
  float math_expec = ((float)  tolua_tonumber(tolua_S,2,0));
  float stddev = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'param_emulator'", NULL);
#endif
  {
   self->param_emulator(math_expec,stddev);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'param_emulator'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: tare of class  i_wages */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_wages_tare00
static int tolua_PAC_dev_i_wages_tare00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_wages",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_wages* self = (i_wages*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'tare'", NULL);
#endif
  {
   self->tare();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'tare'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  i_wages */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_wages_get_value00
static int tolua_PAC_dev_i_wages_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_wages",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_wages* self = (i_wages*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  i_wages */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_wages_get_state00
static int tolua_PAC_dev_i_wages_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_wages",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_wages* self = (i_wages*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reverse of class  i_motor */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_motor_reverse00
static int tolua_PAC_dev_i_motor_reverse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_motor",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_motor* self = (i_motor*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reverse'", NULL);
#endif
  {
   self->reverse();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reverse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_linear_speed of class  i_motor */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_motor_get_linear_speed00
static int tolua_PAC_dev_i_motor_get_linear_speed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const i_motor",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const i_motor* self = (const i_motor*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_linear_speed'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_linear_speed();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_linear_speed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_amperage of class  i_motor */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_motor_get_amperage00
static int tolua_PAC_dev_i_motor_get_amperage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const i_motor",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const i_motor* self = (const i_motor*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_amperage'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_amperage();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_amperage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_off00
static int tolua_PAC_dev_signal_column_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_on00
static int tolua_PAC_dev_signal_column_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_set_state00
static int tolua_PAC_dev_signal_column_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_get_value00
static int tolua_PAC_dev_signal_column_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_set_value00
static int tolua_PAC_dev_signal_column_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_off_red of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_off_red00
static int tolua_PAC_dev_signal_column_turn_off_red00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_off_red'", NULL);
#endif
  {
   self->turn_off_red();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_off_red'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_off_yellow of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_off_yellow00
static int tolua_PAC_dev_signal_column_turn_off_yellow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_off_yellow'", NULL);
#endif
  {
   self->turn_off_yellow();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_off_yellow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_off_green of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_off_green00
static int tolua_PAC_dev_signal_column_turn_off_green00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_off_green'", NULL);
#endif
  {
   self->turn_off_green();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_off_green'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_off_blue of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_off_blue00
static int tolua_PAC_dev_signal_column_turn_off_blue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_off_blue'", NULL);
#endif
  {
   self->turn_off_blue();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_off_blue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_on_red of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_on_red00
static int tolua_PAC_dev_signal_column_turn_on_red00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_on_red'", NULL);
#endif
  {
   self->turn_on_red();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_on_red'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_on_yellow of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_on_yellow00
static int tolua_PAC_dev_signal_column_turn_on_yellow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_on_yellow'", NULL);
#endif
  {
   self->turn_on_yellow();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_on_yellow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_on_green of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_on_green00
static int tolua_PAC_dev_signal_column_turn_on_green00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_on_green'", NULL);
#endif
  {
   self->turn_on_green();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_on_green'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_on_blue of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_on_blue00
static int tolua_PAC_dev_signal_column_turn_on_blue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_on_blue'", NULL);
#endif
  {
   self->turn_on_blue();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_on_blue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: normal_blink_red of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_normal_blink_red00
static int tolua_PAC_dev_signal_column_normal_blink_red00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'normal_blink_red'", NULL);
#endif
  {
   self->normal_blink_red();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'normal_blink_red'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: normal_blink_yellow of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_normal_blink_yellow00
static int tolua_PAC_dev_signal_column_normal_blink_yellow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'normal_blink_yellow'", NULL);
#endif
  {
   self->normal_blink_yellow();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'normal_blink_yellow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: normal_blink_green of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_normal_blink_green00
static int tolua_PAC_dev_signal_column_normal_blink_green00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'normal_blink_green'", NULL);
#endif
  {
   self->normal_blink_green();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'normal_blink_green'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: normal_blink_blue of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_normal_blink_blue00
static int tolua_PAC_dev_signal_column_normal_blink_blue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'normal_blink_blue'", NULL);
#endif
  {
   self->normal_blink_blue();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'normal_blink_blue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: slow_blink_red of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_slow_blink_red00
static int tolua_PAC_dev_signal_column_slow_blink_red00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'slow_blink_red'", NULL);
#endif
  {
   self->slow_blink_red();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'slow_blink_red'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: slow_blink_yellow of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_slow_blink_yellow00
static int tolua_PAC_dev_signal_column_slow_blink_yellow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'slow_blink_yellow'", NULL);
#endif
  {
   self->slow_blink_yellow();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'slow_blink_yellow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: slow_blink_green of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_slow_blink_green00
static int tolua_PAC_dev_signal_column_slow_blink_green00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'slow_blink_green'", NULL);
#endif
  {
   self->slow_blink_green();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'slow_blink_green'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: slow_blink_blue of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_slow_blink_blue00
static int tolua_PAC_dev_signal_column_slow_blink_blue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'slow_blink_blue'", NULL);
#endif
  {
   self->slow_blink_blue();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'slow_blink_blue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_on_siren of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_on_siren00
static int tolua_PAC_dev_signal_column_turn_on_siren00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_on_siren'", NULL);
#endif
  {
   self->turn_on_siren();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_on_siren'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_off_siren of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_turn_off_siren00
static int tolua_PAC_dev_signal_column_turn_off_siren00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_off_siren'", NULL);
#endif
  {
   self->turn_off_siren();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_off_siren'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_error_exists of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_error_exists00
static int tolua_PAC_dev_signal_column_show_error_exists00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_error_exists'", NULL);
#endif
  {
   self->show_error_exists();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_error_exists'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_message_exists of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_message_exists00
static int tolua_PAC_dev_signal_column_show_message_exists00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_message_exists'", NULL);
#endif
  {
   self->show_message_exists();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_message_exists'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_batch_is_not_running of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_batch_is_not_running00
static int tolua_PAC_dev_signal_column_show_batch_is_not_running00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_batch_is_not_running'", NULL);
#endif
  {
   self->show_batch_is_not_running();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_batch_is_not_running'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_batch_is_running of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_batch_is_running00
static int tolua_PAC_dev_signal_column_show_batch_is_running00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_batch_is_running'", NULL);
#endif
  {
   self->show_batch_is_running();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_batch_is_running'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_operation_is_not_running of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_operation_is_not_running00
static int tolua_PAC_dev_signal_column_show_operation_is_not_running00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_operation_is_not_running'", NULL);
#endif
  {
   self->show_operation_is_not_running();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_operation_is_not_running'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_operation_is_running of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_operation_is_running00
static int tolua_PAC_dev_signal_column_show_operation_is_running00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_operation_is_running'", NULL);
#endif
  {
   self->show_operation_is_running();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_operation_is_running'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: show_idle of class  signal_column */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_signal_column_show_idle00
static int tolua_PAC_dev_signal_column_show_idle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"signal_column",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  signal_column* self = (signal_column*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'show_idle'", NULL);
#endif
  {
   self->show_idle();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'show_idle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_io_node of class  node_dev */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_node_dev_set_io_node00
static int tolua_PAC_dev_node_dev_set_io_node00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"node_dev",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"io_manager::io_node",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  node_dev* self = (node_dev*)  tolua_tousertype(tolua_S,1,0);
  io_manager::io_node* io_node = ((io_manager::io_node*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_io_node'", NULL);
#endif
  {
   self->set_io_node(io_node);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_io_node'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: V */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_V00
static int tolua_PAC_dev_V00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   valve* tolua_ret = (valve*)  V(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"valve");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'V'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: VC */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_VC00
static int tolua_PAC_dev_VC00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AO_device* tolua_ret = (i_AO_device*)  VC(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'VC'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: M */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_M00
static int tolua_PAC_dev_M00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_motor* tolua_ret = (i_motor*)  M(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_motor");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'M'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_LS00
static int tolua_PAC_dev_LS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  LS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: FS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FS00
static int tolua_PAC_dev_FS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  FS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: AI */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_AI00
static int tolua_PAC_dev_AI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  AI(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: AO */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_AO00
static int tolua_PAC_dev_AO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AO_device* tolua_ret = (i_AO_device*)  AO(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: FQT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FQT00
static int tolua_PAC_dev_FQT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_counter* tolua_ret = (i_counter*)  FQT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_counter");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FQT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: virtual_FQT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_FQT00
static int tolua_PAC_dev_virtual_FQT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   virtual_counter* tolua_ret = (virtual_counter*)  virtual_FQT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"virtual_counter");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'virtual_FQT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: FQT_IOLINK */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_FQT_IOLINK00
static int tolua_PAC_dev_FQT_IOLINK00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   counter_iolink* tolua_ret = (counter_iolink*)  FQT_IOLINK(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"counter_iolink");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FQT_IOLINK'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: TE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_TE00
static int tolua_PAC_dev_TE00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  TE(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'TE'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: LT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_LT00
static int tolua_PAC_dev_LT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   level* tolua_ret = (level*)  LT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"level");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_GS00
static int tolua_PAC_dev_GS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  GS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: HA */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_HA00
static int tolua_PAC_dev_HA00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  HA(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HA'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: HL */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_HL00
static int tolua_PAC_dev_HL00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  HL(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HL'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: HLA */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_HLA00
static int tolua_PAC_dev_HLA00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   signal_column* tolua_ret = (signal_column*)  HLA(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"signal_column");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HLA'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: SB */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_SB00
static int tolua_PAC_dev_SB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  SB(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SB'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DI */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_DI00
static int tolua_PAC_dev_DI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  DI(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DO */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_DO00
static int tolua_PAC_dev_DO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_device* tolua_ret = (i_DO_device*)  DO(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: QT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_QT00
static int tolua_PAC_dev_QT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  QT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'QT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: WT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_WT00
static int tolua_PAC_dev_WT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_wages* tolua_ret = (i_wages*)  WT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_wages");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'WT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PT */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PT00
static int tolua_PAC_dev_PT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AI_device* tolua_ret = (i_AI_device*)  PT(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: F */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_F00
static int tolua_PAC_dev_F00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_AO_device* tolua_ret = (i_DO_AO_device*)  F(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'F'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: C */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_C00
static int tolua_PAC_dev_C00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_AO_device* tolua_ret = (i_DO_AO_device*)  C(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'C'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: CAM */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_CAM00
static int tolua_PAC_dev_CAM00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_camera* tolua_ret = (i_camera*)  CAM(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_camera");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CAM'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: PDS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PDS00
static int tolua_PAC_dev_PDS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  PDS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PDS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: TS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_TS00
static int tolua_PAC_dev_TS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  TS(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'TS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_G */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G00
static int tolua_PAC_dev_G00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DO_AO_device* tolua_ret = (i_DO_AO_device*)  get_G(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DO_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: WATCHDOG */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_WATCHDOG00
static int tolua_PAC_dev_WATCHDOG00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_DI_device* tolua_ret = (i_DI_device*)  WATCHDOG(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_DI_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'WATCHDOG'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: EY */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_EY00
static int tolua_PAC_dev_EY00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isstring(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,1,0));
  {
   i_AO_device* tolua_ret = (i_AO_device*)  EY(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_AO_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EY'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: STUB */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_STUB00
static int tolua_PAC_dev_STUB00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   dev_stub* tolua_ret = (dev_stub*)  STUB();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"dev_stub");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'STUB'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: DEVICE */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_DEVICE00
static int tolua_PAC_dev_DEVICE00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int s_number = ((int)  tolua_tonumber(tolua_S,1,0));
  {
   device* tolua_ret = (device*)  DEVICE(s_number);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DEVICE'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_get_value00
static int tolua_PAC_dev_dev_stub_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_set_value00
static int tolua_PAC_dev_dev_stub_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_active of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_is_active00
static int tolua_PAC_dev_dev_stub_is_active00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_active'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_active();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_active'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_on00
static int tolua_PAC_dev_dev_stub_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_off00
static int tolua_PAC_dev_dev_stub_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: instant_off of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_instant_off00
static int tolua_PAC_dev_dev_stub_instant_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'instant_off'", NULL);
#endif
  {
   self->instant_off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'instant_off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_set_state00
static int tolua_PAC_dev_dev_stub_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  dev_stub */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_stub_get_state00
static int tolua_PAC_dev_dev_stub_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_stub",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_stub* self = (dev_stub*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_instance of class  errors_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_errors_manager_get_instance00
static int tolua_PAC_dev_errors_manager_get_instance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"errors_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   errors_manager* tolua_ret = (errors_manager*)  errors_manager::get_instance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"errors_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_instance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  errors_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_errors_manager_set_cmd00
static int tolua_PAC_dev_errors_manager_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"errors_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  errors_manager* self = (errors_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int cmd = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int object_type = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int object_number = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int object_alarm_number = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   self->set_cmd(cmd,object_type,object_number,object_alarm_number);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_instance of class  dev_errors_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_errors_manager_get_instance00
static int tolua_PAC_dev_dev_errors_manager_get_instance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"dev_errors_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   errors_manager* tolua_ret = (errors_manager*)  dev_errors_manager::get_instance();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"errors_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_instance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  dev_errors_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_dev_errors_manager_set_cmd00
static int tolua_PAC_dev_dev_errors_manager_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"dev_errors_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  dev_errors_manager* self = (dev_errors_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int cmd = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int object_type = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int object_number = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int object_alarm_number = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   self->set_cmd(cmd,object_type,object_number,object_alarm_number);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_opened of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_is_opened00
static int tolua_PAC_dev_valve_is_opened00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_opened'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_opened();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_opened'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_closed of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_is_closed00
static int tolua_PAC_dev_valve_is_closed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_closed'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_closed();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_closed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_on00
static int tolua_PAC_dev_valve_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_off00
static int tolua_PAC_dev_valve_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: instant_off of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_direct_off00
static int tolua_PAC_dev_valve_direct_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'instant_off'", NULL);
#endif
  {
   self->instant_off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'direct_off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: instant_off of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_instant_off00
static int tolua_PAC_dev_valve_instant_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'instant_off'", NULL);
#endif
  {
   self->instant_off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'instant_off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_set_state00
static int tolua_PAC_dev_valve_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_get_state00
static int tolua_PAC_dev_valve_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_on_fb_value of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_get_on_fb_value00
static int tolua_PAC_dev_valve_get_on_fb_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_on_fb_value'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_on_fb_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_on_fb_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_off_fb_value of class  valve */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_valve_get_off_fb_value00
static int tolua_PAC_dev_valve_get_off_fb_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"valve",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  valve* self = (valve*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_off_fb_value'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_off_fb_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_off_fb_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_volume of class  level */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_level_get_volume00
static int tolua_PAC_dev_level_get_volume00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"level",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  level* self = (level*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_volume'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_volume();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_volume'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_reset00
static int tolua_PAC_dev_virtual_counter_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_quantity of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_get_quantity00
static int tolua_PAC_dev_virtual_counter_get_quantity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_quantity'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_quantity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_quantity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_flow of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_get_flow00
static int tolua_PAC_dev_virtual_counter_get_flow00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_flow'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_flow();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_flow'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_get_state00
static int tolua_PAC_dev_virtual_counter_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_abs_quantity of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_get_abs_quantity00
static int tolua_PAC_dev_virtual_counter_get_abs_quantity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_abs_quantity'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_abs_quantity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_abs_quantity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: abs_reset of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_abs_reset00
static int tolua_PAC_dev_virtual_counter_abs_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'abs_reset'", NULL);
#endif
  {
   self->abs_reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'abs_reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_set00
static int tolua_PAC_dev_virtual_counter_set00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
  unsigned int value = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int abs_value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  float flow = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set'", NULL);
#endif
  {
   self->set(value,abs_value,flow);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: eval of class  virtual_counter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_virtual_counter_eval00
static int tolua_PAC_dev_virtual_counter_eval00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"virtual_counter",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  virtual_counter* self = (virtual_counter*)  tolua_tousertype(tolua_S,1,0);
  unsigned int read_value = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int abs_read_value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  float read_flow = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'eval'", NULL);
#endif
  {
   self->eval(read_value,abs_read_value,read_flow);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'eval'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_temperature of class  counter_iolink */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_counter_iolink_get_temperature00
static int tolua_PAC_dev_counter_iolink_get_temperature00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"counter_iolink",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  counter_iolink* self = (counter_iolink*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_temperature'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_temperature();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_temperature'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_conductivity of class  counter_iolink */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_counter_iolink_get_conductivity00
static int tolua_PAC_dev_counter_iolink_get_conductivity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"counter_iolink",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  counter_iolink* self = (counter_iolink*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_conductivity'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_conductivity();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_conductivity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_set_cmd00
static int tolua_PAC_dev_tech_object_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_new00
static int tolua_PAC_dev_tech_object_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   tech_object* tolua_ret = (tech_object*)  Mtolua_new((tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_new00_local
static int tolua_PAC_dev_tech_object_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   tech_object* tolua_ret = (tech_object*)  Mtolua_new((tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_delete00
static int tolua_PAC_dev_tech_object_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_set_mode00
static int tolua_PAC_dev_tech_object_set_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int new_state = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_mode(mode,new_state);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_mode of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_mode00
static int tolua_PAC_dev_tech_object_get_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_mode(mode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_operation_state of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_operation_state00
static int tolua_PAC_dev_tech_object_get_operation_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_operation_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_operation_state(mode);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_operation_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: exec_cmd of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_exec_cmd00
static int tolua_PAC_dev_tech_object_exec_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int cmd = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'exec_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->exec_cmd(cmd);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'exec_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_modes_count of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_modes_count00
static int tolua_PAC_dev_tech_object_get_modes_count00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_modes_count'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_modes_count();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_modes_count'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: check_operation_on of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_check_operation_on00
static int tolua_PAC_dev_tech_object_check_operation_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  unsigned int operation_n = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  bool show_error = ((bool)  tolua_toboolean(tolua_S,3,true));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'check_operation_on'", NULL);
#endif
  {
   int tolua_ret = (int)  self->check_operation_on(operation_n,show_error);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'check_operation_on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_par_float
static int tolua_get_tech_object_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par_float,"saved_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_par_float
static int tolua_set_tech_object_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par_float = *((saved_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rt_par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_rt_par_float
static int tolua_get_tech_object_rt_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->rt_par_float,"run_time_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rt_par_float of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_rt_par_float
static int tolua_set_tech_object_rt_par_float(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"run_time_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rt_par_float = *((run_time_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_par_uint
static int tolua_get_tech_object_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_uint'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par_uint,"saved_params_u_int_4");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_par_uint
static int tolua_set_tech_object_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_uint'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_u_int_4",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par_uint = *((saved_params_u_int_4*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rt_par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_rt_par_uint
static int tolua_get_tech_object_rt_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_uint'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->rt_par_uint,"run_time_params_u_int_4");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rt_par_uint of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_rt_par_uint
static int tolua_set_tech_object_rt_par_uint(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_uint'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"run_time_params_u_int_4",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rt_par_uint = *((run_time_params_u_int_4*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: timers of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_get_tech_object_timers
static int tolua_get_tech_object_timers(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'timers'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->timers,"timer_manager");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: timers of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_set_tech_object_timers
static int tolua_set_tech_object_timers(lua_State* tolua_S)
{
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'timers'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"timer_manager",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->timers = *((timer_manager*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_modes_manager of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_modes_manager00
static int tolua_PAC_dev_tech_object_get_modes_manager00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_modes_manager'", NULL);
#endif
  {
   operation_manager* tolua_ret = (operation_manager*)  self->get_modes_manager();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"operation_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_modes_manager'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_idle of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_is_idle00
static int tolua_PAC_dev_tech_object_is_idle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_idle'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_idle();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_idle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_number of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_get_number00
static int tolua_PAC_dev_tech_object_get_number00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_number'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_number();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_number'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_err_msg of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_set_err_msg00
static int tolua_PAC_dev_tech_object_set_err_msg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object* self = (tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* err_msg = ((const char*)  tolua_tostring(tolua_S,2,0));
  int mode = ((int)  tolua_tonumber(tolua_S,3,0));
  int new_mode = ((int)  tolua_tonumber(tolua_S,4,0));
  tech_object::ERR_MSG_TYPES type = ((tech_object::ERR_MSG_TYPES) (int)  tolua_tonumber(tolua_S,5,tech_object::ERR_CANT_ON));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_err_msg'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_err_msg(err_msg,mode,new_mode,type);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_err_msg'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_any_message of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_is_any_message00
static int tolua_PAC_dev_tech_object_is_any_message00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_any_message'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_any_message();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_any_message'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_any_error of class  tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_is_any_error00
static int tolua_PAC_dev_tech_object_is_any_error00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const tech_object* self = (const tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_any_error'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_any_error();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_any_error'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_TECH_OBJECT_MNGR */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_TECH_OBJECT_MNGR00
static int tolua_PAC_dev_G_TECH_OBJECT_MNGR00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   tech_object_manager* tolua_ret = (tech_object_manager*)  G_TECH_OBJECT_MNGR();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_TECH_OBJECT_MNGR'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_object_with_active_mode of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00
static int tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object_manager* self = (tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int mode = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int start_idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int end_idx = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_object_with_active_mode'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_object_with_active_mode(mode,start_idx,end_idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_object_with_active_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_tech_objects of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_get_tech_objects00
static int tolua_PAC_dev_tech_object_manager_get_tech_objects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object_manager* self = (tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_tech_objects'", NULL);
#endif
  {
   tech_object* tolua_ret = (tech_object*)  self->get_tech_objects(idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_tech_objects'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: print of class  tech_object_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_tech_object_manager_print00
static int tolua_PAC_dev_tech_object_manager_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"tech_object_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  tech_object_manager* self = (tech_object_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'print'", NULL);
#endif
  {
   self->print();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluation_time of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_evaluation_time00
static int tolua_PAC_dev_operation_evaluation_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'evaluation_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->evaluation_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'evaluation_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: active_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_active_step00
static int tolua_PAC_dev_operation_active_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation* self = (const operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'active_step'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->active_step();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'active_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: active_step_evaluation_time of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_active_step_evaluation_time00
static int tolua_PAC_dev_operation_active_step_evaluation_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation* self = (const operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'active_step_evaluation_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->active_step_evaluation_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'active_step_evaluation_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_active_step_set_time of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_get_active_step_set_time00
static int tolua_PAC_dev_operation_get_active_step_set_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation* self = (const operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_active_step_set_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_active_step_set_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_active_step_set_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: to_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_to_step00
static int tolua_PAC_dev_operation_to_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  unsigned int new_step = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned long cooperative_time = ((unsigned long)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'to_step'", NULL);
#endif
  {
   self->to_step(new_step,cooperative_time);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'to_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: to_next_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_to_next_step00
static int tolua_PAC_dev_operation_to_next_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'to_next_step'", NULL);
#endif
  {
   self->to_next_step();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'to_next_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: turn_off_active_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_turn_off_active_step00
static int tolua_PAC_dev_operation_turn_off_active_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'turn_off_active_step'", NULL);
#endif
  {
   self->turn_off_active_step();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'turn_off_active_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation__geti00
static int tolua_PAC_dev_operation__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   operation_state* tolua_ret = (operation_state*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"operation_state");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on_extra_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_on_extra_step00
static int tolua_PAC_dev_operation_on_extra_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  int step = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on_extra_step'", NULL);
#endif
  {
   int tolua_ret = (int)  self->on_extra_step(step);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on_extra_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off_extra_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_off_extra_step00
static int tolua_PAC_dev_operation_off_extra_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  int step = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off_extra_step'", NULL);
#endif
  {
   int tolua_ret = (int)  self->off_extra_step(step);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off_extra_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: switch_active_extra_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_switch_active_extra_step00
static int tolua_PAC_dev_operation_switch_active_extra_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  int off_step = ((int)  tolua_tonumber(tolua_S,2,0));
  int on_step = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'switch_active_extra_step'", NULL);
#endif
  {
   int tolua_ret = (int)  self->switch_active_extra_step(off_step,on_step);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'switch_active_extra_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_active_extra_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_is_active_extra_step00
static int tolua_PAC_dev_operation_is_active_extra_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation* self = (const operation*)  tolua_tousertype(tolua_S,1,0);
  int step_idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_active_extra_step'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_active_extra_step(step_idx);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_active_extra_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_step of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_add_step00
static int tolua_PAC_dev_operation_add_step00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  int next_step_n = ((int)  tolua_tonumber(tolua_S,3,0));
  int step_duration_par_n = ((int)  tolua_tonumber(tolua_S,4,0));
  int step_max_duration_par_n = ((int)  tolua_tonumber(tolua_S,5,0));
  operation::state_idx s_idx = ((operation::state_idx) (int)  tolua_tonumber(tolua_S,6,operation::RUN));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_step'", NULL);
#endif
  {
   step* tolua_ret = (step*)  self->add_step(name,next_step_n,step_duration_par_n,step_max_duration_par_n,s_idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"step");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_step'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_name of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_get_name00
static int tolua_PAC_dev_operation_get_name00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation* self = (const operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_name'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->get_name();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_name'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_step_cooperate_time_par_n of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_set_step_cooperate_time_par_n00
static int tolua_PAC_dev_operation_set_step_cooperate_time_par_n00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation* self = (operation*)  tolua_tousertype(tolua_S,1,0);
  int step_cooperate_time_par_n = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_step_cooperate_time_par_n'", NULL);
#endif
  {
   self->set_step_cooperate_time_par_n(step_cooperate_time_par_n);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_step_cooperate_time_par_n'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  operation */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_get_state00
static int tolua_PAC_dev_operation_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation* self = (const operation*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   operation::state_idx tolua_ret = (operation::state_idx)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_mode of class  operation_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_manager_add_mode00
static int tolua_PAC_dev_operation_manager_add_mode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation_manager* self = (operation_manager*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_mode'", NULL);
#endif
  {
   operation* tolua_ret = (operation*)  self->add_mode(name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"operation");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_mode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_operation of class  operation_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_manager_add_operation00
static int tolua_PAC_dev_operation_manager_add_operation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation_manager* self = (operation_manager*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_operation'", NULL);
#endif
  {
   operation* tolua_ret = (operation*)  self->add_operation(name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"operation");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_operation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  operation_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_manager__geti00
static int tolua_PAC_dev_operation_manager__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation_manager* self = (operation_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   operation* tolua_ret = (operation*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"operation");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_idle_time_sec of class  operation_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_manager_get_idle_time_sec00
static int tolua_PAC_dev_operation_manager_get_idle_time_sec00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation_manager* self = (operation_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_idle_time_sec'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_idle_time_sec();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_idle_time_sec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_idle_time of class  operation_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_manager_get_idle_time00
static int tolua_PAC_dev_operation_manager_get_idle_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation_manager* self = (operation_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_idle_time'", NULL);
#endif
  {
   unsigned long long tolua_ret = (unsigned long long)  self->get_idle_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_idle_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  operation_state */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_state__geti00
static int tolua_PAC_dev_operation_state__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"operation_state",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  operation_state* self = (operation_state*)  tolua_tousertype(tolua_S,1,0);
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   step* tolua_ret = (step*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"step");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: steps_count of class  operation_state */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_operation_state_steps_count00
static int tolua_PAC_dev_operation_state_steps_count00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const operation_state",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const operation_state* self = (const operation_state*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'steps_count'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->steps_count();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'steps_count'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step__geti00
static int tolua_PAC_dev_step__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"step",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  step* self = (step*)  tolua_tousertype(tolua_S,1,0);
  int idx = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   action* tolua_ret = (action*)  self->operator[](idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"action");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_name of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step_get_name00
static int tolua_PAC_dev_step_get_name00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const step",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const step* self = (const step*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_name'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->get_name();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_name'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_active of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step_is_active00
static int tolua_PAC_dev_step_is_active00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const step",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const step* self = (const step*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_active'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_active();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_active'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_tag of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step_get_tag00
static int tolua_PAC_dev_step_get_tag00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const step",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const step* self = (const step*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_tag'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_tag();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_tag'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_tag of class  step */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_step_set_tag00
static int tolua_PAC_dev_step_set_tag00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"step",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  step* self = (step*)  tolua_tousertype(tolua_S,1,0);
  int new_tag = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_tag'", NULL);
#endif
  {
   self->set_tag(new_tag);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_tag'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_dev of class  action */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_action_add_dev00
static int tolua_PAC_dev_action_add_dev00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"action",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  action* self = (action*)  tolua_tousertype(tolua_S,1,0);
  device* dev = ((device*)  tolua_tousertype(tolua_S,2,0));
  unsigned int group = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int subgroup = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_dev'", NULL);
#endif
  {
   self->add_dev(dev,group,subgroup);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_dev'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_param_idx of class  action */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_action_set_param_idx00
static int tolua_PAC_dev_action_set_param_idx00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"action",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  action* self = (action*)  tolua_tousertype(tolua_S,1,0);
  unsigned int position = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int idx = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_param_idx'", NULL);
#endif
  {
   self->set_param_idx(position,idx);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_param_idx'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_bool_property of class  action */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_action_set_bool_property00
static int tolua_PAC_dev_action_set_bool_property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"action",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  action* self = (action*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  bool value = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_bool_property'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_bool_property(name,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_bool_property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int_property of class  action */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_action_set_int_property00
static int tolua_PAC_dev_action_set_int_property00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"action",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  action* self = (action*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  int value = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int_property'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_int_property(name,idx,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int_property'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_DEVICE_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_DEVICE_MANAGER00
static int tolua_PAC_dev_G_DEVICE_MANAGER00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   device_manager* tolua_ret = (device_manager*)  G_DEVICE_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_DEVICE_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_TECH_OBJECTS */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_TECH_OBJECTS00
static int tolua_PAC_dev_G_TECH_OBJECTS00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   tech_object* tolua_ret = (tech_object*)  G_TECH_OBJECTS(idx);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_TECH_OBJECTS'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: print of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_print00
static int tolua_PAC_dev_device_manager_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const device_manager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const device_manager* self = (const device_manager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'print'", NULL);
#endif
  {
   self->print();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_io_device of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_add_io_device00
static int tolua_PAC_dev_device_manager_add_io_device00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isstring(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device_manager* self = (device_manager*)  tolua_tousertype(tolua_S,1,0);
  int dev_type = ((int)  tolua_tonumber(tolua_S,2,0));
  int dev_sub_type = ((int)  tolua_tonumber(tolua_S,3,0));
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,4,0));
  char* descr = ((char*)  tolua_tostring(tolua_S,5,0));
  char* article = ((char*)  tolua_tostring(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_io_device'", NULL);
#endif
  {
   io_device* tolua_ret = (io_device*)  self->add_io_device(dev_type,dev_sub_type,dev_name,descr,article);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"io_device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_io_device'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_device of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_get_device00
static int tolua_PAC_dev_device_manager_get_device00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device_manager* self = (device_manager*)  tolua_tousertype(tolua_S,1,0);
  int dev_type = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_device'", NULL);
#endif
  {
   device* tolua_ret = (device*)  self->get_device(dev_type,dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_device'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_device of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_get_device01
static int tolua_PAC_dev_device_manager_get_device01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  device_manager* self = (device_manager*)  tolua_tousertype(tolua_S,1,0);
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_device'", NULL);
#endif
  {
   device* tolua_ret = (device*)  self->get_device(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"device");
  }
 }
 return 1;
tolua_lerror:
 return tolua_PAC_dev_device_manager_get_device00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_node of class  device_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_device_manager_get_node00
static int tolua_PAC_dev_device_manager_get_node00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"device_manager",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  device_manager* self = (device_manager*)  tolua_tousertype(tolua_S,1,0);
  const char* dev_name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_node'", NULL);
#endif
  {
   node_dev* tolua_ret = (node_dev*)  self->get_node(dev_name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"node_dev");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_node'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  io_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_device_init00
static int tolua_PAC_dev_io_device_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_device* self = (io_device*)  tolua_tousertype(tolua_S,1,0);
  int DO_count = ((int)  tolua_tonumber(tolua_S,2,0));
  int DI_count = ((int)  tolua_tonumber(tolua_S,3,0));
  int AO_count = ((int)  tolua_tonumber(tolua_S,4,0));
  int AI_count = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init'", NULL);
#endif
  {
   self->init(DO_count,DI_count,AO_count,AI_count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_channel of class  io_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_device_init_channel00
static int tolua_PAC_dev_io_device_init_channel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_device* self = (io_device*)  tolua_tousertype(tolua_S,1,0);
  int type = ((int)  tolua_tonumber(tolua_S,2,0));
  int ch_inex = ((int)  tolua_tonumber(tolua_S,3,0));
  int node = ((int)  tolua_tonumber(tolua_S,4,0));
  int offset = ((int)  tolua_tonumber(tolua_S,5,0));
  int module_offset = ((int)  tolua_tonumber(tolua_S,6,-1));
  int logical_port = ((int)  tolua_tonumber(tolua_S,7,-1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_channel'", NULL);
#endif
  {
   self->init_channel(type,ch_inex,node,offset,module_offset,logical_port);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_channel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_io_vendor of class  io_device */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_device_set_io_vendor00
static int tolua_PAC_dev_io_device_set_io_vendor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_device",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_device* self = (io_device*)  tolua_tousertype(tolua_S,1,0);
  io_device::VENDOR vendor = ((io_device::VENDOR) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_io_vendor'", NULL);
#endif
  {
   self->set_io_vendor(vendor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_io_vendor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  timer_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_manager__geti00
static int tolua_PAC_dev_timer_manager__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer_manager* self = (timer_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   timer* tolua_ret = (timer*)  self->operator[](index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"timer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: start of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_start00
static int tolua_PAC_dev_timer_start00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'start'", NULL);
#endif
  {
   self->start();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'start'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_reset00
static int tolua_PAC_dev_timer_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: pause of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_pause00
static int tolua_PAC_dev_timer_pause00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'pause'", NULL);
#endif
  {
   self->pause();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'pause'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_time_up of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_is_time_up00
static int tolua_PAC_dev_timer_is_time_up00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_time_up'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_time_up();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_time_up'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_work_time of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_get_work_time00
static int tolua_PAC_dev_timer_get_work_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_work_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_work_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_work_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_countdown_time of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_set_countdown_time00
static int tolua_PAC_dev_timer_set_countdown_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  timer* self = (timer*)  tolua_tousertype(tolua_S,1,0);
  unsigned long new_countdown_time = ((unsigned long)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_countdown_time'", NULL);
#endif
  {
   self->set_countdown_time(new_countdown_time);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_countdown_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_countdown_time of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_get_countdown_time00
static int tolua_PAC_dev_timer_get_countdown_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_countdown_time'", NULL);
#endif
  {
   unsigned long tolua_ret = (unsigned long)  self->get_countdown_time();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_countdown_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_get_state00
static int tolua_PAC_dev_timer_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const timer* self = (const timer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   timer::STATE tolua_ret = (timer::STATE)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_new00
static int tolua_PAC_dev_timer_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   timer* tolua_ret = (timer*)  Mtolua_new((timer)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"timer");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  timer */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_timer_new00_local
static int tolua_PAC_dev_timer_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"timer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   timer* tolua_ret = (timer*)  Mtolua_new((timer)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"timer");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_save00
static int tolua_PAC_dev_saved_params_float_save00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save(idx,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save_all of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_save_all00
static int tolua_PAC_dev_saved_params_float_save_all00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save_all'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save_all();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save_all'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float__seti00
static int tolua_PAC_dev_saved_params_float__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float tolua_value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float__geti00
static int tolua_PAC_dev_saved_params_float__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   float tolua_ret = (float)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  saved_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_float_reset_to_000
static int tolua_PAC_dev_saved_params_float_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_float",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_float* self = (saved_params_float*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_save00
static int tolua_PAC_dev_saved_params_u_int_4_save00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save(idx,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save_all of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_save_all00
static int tolua_PAC_dev_saved_params_u_int_4_save_all00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save_all'", NULL);
#endif
  {
   int tolua_ret = (int)  self->save_all();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save_all'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4__seti00
static int tolua_PAC_dev_saved_params_u_int_4__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int tolua_value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4__geti00
static int tolua_PAC_dev_saved_params_u_int_4__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  saved_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_saved_params_u_int_4_reset_to_000
static int tolua_PAC_dev_saved_params_u_int_4_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"saved_params_u_int_4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  saved_params_u_int_4* self = (saved_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4__seti00
static int tolua_PAC_dev_run_time_params_u_int_4__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_u_int_4* self = (run_time_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int tolua_value = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4__geti00
static int tolua_PAC_dev_run_time_params_u_int_4__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_u_int_4* self = (run_time_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  run_time_params_u_int_4 */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_u_int_4_reset_to_000
static int tolua_PAC_dev_run_time_params_u_int_4_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_u_int_4",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_u_int_4* self = (run_time_params_u_int_4*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float__seti00
static int tolua_PAC_dev_run_time_params_float__seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_float* self = (run_time_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float tolua_value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](idx) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float__geti00
static int tolua_PAC_dev_run_time_params_float__geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_float* self = (run_time_params_float*)  tolua_tousertype(tolua_S,1,0);
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   float tolua_ret = (float)  self->operator[](idx);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset_to_0 of class  run_time_params_float */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_run_time_params_float_reset_to_000
static int tolua_PAC_dev_run_time_params_float_reset_to_000(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"run_time_params_float",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  run_time_params_float* self = (run_time_params_float*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset_to_0'", NULL);
#endif
  {
   self->reset_to_0();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset_to_0'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  io_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_manager_init00
static int tolua_PAC_dev_io_manager_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_manager* self = (io_manager*)  tolua_tousertype(tolua_S,1,0);
  int nodes_count = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init'", NULL);
#endif
  {
   self->init(nodes_count);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: add_node of class  io_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_manager_add_node00
static int tolua_PAC_dev_io_manager_add_node00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isstring(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,12,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,13,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_manager* self = (io_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int ntype = ((int)  tolua_tonumber(tolua_S,3,0));
  int address = ((int)  tolua_tonumber(tolua_S,4,0));
  char* IP_address = ((char*)  tolua_tostring(tolua_S,5,0));
  char* name = ((char*)  tolua_tostring(tolua_S,6,0));
  int DO_cnt = ((int)  tolua_tonumber(tolua_S,7,0));
  int DI_cnt = ((int)  tolua_tonumber(tolua_S,8,0));
  int AO_cnt = ((int)  tolua_tonumber(tolua_S,9,0));
  int AO_size = ((int)  tolua_tonumber(tolua_S,10,0));
  int AI_cnt = ((int)  tolua_tonumber(tolua_S,11,0));
  int AI_size = ((int)  tolua_tonumber(tolua_S,12,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'add_node'", NULL);
#endif
  {
   io_manager::io_node* tolua_ret = (io_manager::io_node*)  self->add_node(index,ntype,address,IP_address,name,DO_cnt,DI_cnt,AO_cnt,AO_size,AI_cnt,AI_size);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"io_manager::io_node");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'add_node'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_node_AO of class  io_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_manager_init_node_AO00
static int tolua_PAC_dev_io_manager_init_node_AO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_manager* self = (io_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int node_index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int AO_index = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_node_AO'", NULL);
#endif
  {
   self->init_node_AO(node_index,AO_index,type,offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_node_AO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_node_AI of class  io_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_io_manager_init_node_AI00
static int tolua_PAC_dev_io_manager_init_node_AI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"io_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  io_manager* self = (io_manager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int node_index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int AI_index = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_node_AI'", NULL);
#endif
  {
   self->init_node_AI(node_index,AI_index,type,offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_node_AI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_IO_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_IO_MANAGER00
static int tolua_PAC_dev_G_IO_MANAGER00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   io_manager* tolua_ret = (io_manager*)  G_IO_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"io_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_IO_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_new00
static int tolua_PAC_dev_PID_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int n = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   PID* tolua_ret = (PID*)  Mtolua_new((PID)(n));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PID");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_new00_local
static int tolua_PAC_dev_PID_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  int n = ((int)  tolua_tonumber(tolua_S,2,0));
  {
   PID* tolua_ret = (PID*)  Mtolua_new((PID)(n));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PID");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_on00
static int tolua_PAC_dev_PID_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  char is_down_to_inaccel_mode = ((char)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on(is_down_to_inaccel_mode);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_off00
static int tolua_PAC_dev_PID_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_reset00
static int tolua_PAC_dev_PID_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: eval of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_eval00
static int tolua_PAC_dev_PID_eval00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  float current_value = ((float)  tolua_tonumber(tolua_S,2,0));
  int delta_sign = ((int)  tolua_tonumber(tolua_S,3,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'eval'", NULL);
#endif
  {
   float tolua_ret = (float)  self->eval(current_value,delta_sign);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'eval'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_set00
static int tolua_PAC_dev_PID_set00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  float new_z = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set'", NULL);
#endif
  {
   self->set(new_z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_assignment of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_get_assignment00
static int tolua_PAC_dev_PID_get_assignment00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_assignment'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_assignment();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_assignment'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_param of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_init_param00
static int tolua_PAC_dev_PID_init_param00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  PID::PARAM par_n = ((PID::PARAM) (int)  tolua_tonumber(tolua_S,2,0));
  float val = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_param'", NULL);
#endif
  {
   self->init_param(par_n,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_param'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init_work_param of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_init_work_param00
static int tolua_PAC_dev_PID_init_work_param00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  PID::WORK_PARAM par_n = ((PID::WORK_PARAM) (int)  tolua_tonumber(tolua_S,2,0));
  float val = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init_work_param'", NULL);
#endif
  {
   self->init_work_param(par_n,val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init_work_param'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: save_param of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_save_param00
static int tolua_PAC_dev_PID_save_param00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'save_param'", NULL);
#endif
  {
   self->save_param();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'save_param'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_used_par of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_set_used_par00
static int tolua_PAC_dev_PID_set_used_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  int par_n = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_used_par'", NULL);
#endif
  {
   self->set_used_par(par_n);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_used_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: print of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_print00
static int tolua_PAC_dev_PID_print00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'print'", NULL);
#endif
  {
   self->print();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'print'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_get_state00
static int tolua_PAC_dev_PID_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  PID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PID_set_cmd00
static int tolua_PAC_dev_PID_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PID",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PID* self = (PID*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_off00
static int tolua_PAC_dev_camera_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_on00
static int tolua_PAC_dev_camera_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_state of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_set_state00
static int tolua_PAC_dev_camera_set_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
  int new_state = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_state'", NULL);
#endif
  {
   self->set_state(new_state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_value of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_get_value00
static int tolua_PAC_dev_camera_get_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_value'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_value();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_value of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_set_value00
static int tolua_PAC_dev_camera_set_value00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
  float new_value = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_value'", NULL);
#endif
  {
   self->set_value(new_value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_value'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_get_state00
static int tolua_PAC_dev_camera_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_result of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_get_result00
static int tolua_PAC_dev_camera_get_result00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"camera",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  camera* self = (camera*)  tolua_tousertype(tolua_S,1,0);
  int n = ((int)  tolua_tonumber(tolua_S,2,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_result'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_result(n);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_result'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_ready of class  camera */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_camera_is_ready00
static int tolua_PAC_dev_camera_is_ready00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const camera",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const camera* self = (const camera*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_ready'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_ready();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_ready'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par of class  PAC_info */
#ifndef TOLUA_DISABLE_tolua_get_PAC_info_par
static int tolua_get_PAC_info_par(lua_State* tolua_S)
{
  PAC_info* self = (PAC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par,"saved_params_u_int_4");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par of class  PAC_info */
#ifndef TOLUA_DISABLE_tolua_set_PAC_info_par
static int tolua_set_PAC_info_par(lua_State* tolua_S)
{
  PAC_info* self = (PAC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_u_int_4",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par = *((saved_params_u_int_4*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  PAC_info */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PAC_info_set_cmd00
static int tolua_PAC_dev_PAC_info_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PAC_info",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PAC_info* self = (PAC_info*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: is_emulator of class  PAC_info */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_PAC_info_is_emulator00
static int tolua_PAC_dev_PAC_info_is_emulator00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"PAC_info",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  PAC_info* self = (PAC_info*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'is_emulator'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->is_emulator();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'is_emulator'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: init of class  siren_lights_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_siren_lights_manager_init00
static int tolua_PAC_dev_siren_lights_manager_init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"siren_lights_manager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"device",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"device",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,4,"device",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,5,"device",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  siren_lights_manager* self = (siren_lights_manager*)  tolua_tousertype(tolua_S,1,0);
  device* red = ((device*)  tolua_tousertype(tolua_S,2,0));
  device* yellow = ((device*)  tolua_tousertype(tolua_S,3,0));
  device* green = ((device*)  tolua_tousertype(tolua_S,4,0));
  device* srn = ((device*)  tolua_tousertype(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'init'", NULL);
#endif
  {
   int tolua_ret = (int)  self->init(red,yellow,green,srn);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_red_blink of class  siren_lights_manager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_siren_lights_manager_set_red_blink00
static int tolua_PAC_dev_siren_lights_manager_set_red_blink00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"siren_lights_manager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  siren_lights_manager* self = (siren_lights_manager*)  tolua_tousertype(tolua_S,1,0);
  int type = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_red_blink'", NULL);
#endif
  {
   self->set_red_blink(type);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_red_blink'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_SIREN_LIGHTS_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_SIREN_LIGHTS_MANAGER00
static int tolua_PAC_dev_G_SIREN_LIGHTS_MANAGER00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   siren_lights_manager* tolua_ret = (siren_lights_manager*)  G_SIREN_LIGHTS_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"siren_lights_manager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_SIREN_LIGHTS_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_PAC_INFO */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_PAC_INFO00
static int tolua_PAC_dev_G_PAC_INFO00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   PAC_info* tolua_ret = (PAC_info*)  G_PAC_INFO();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PAC_info");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_PAC_INFO'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_sec */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_sec00
static int tolua_PAC_dev_get_sec00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   unsigned int tolua_ret = (unsigned int)  get_sec();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_sec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_millisec */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_millisec00
static int tolua_PAC_dev_get_millisec00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   unsigned int tolua_ret = (unsigned int)  get_millisec();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_millisec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_delta_millisec */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_delta_millisec00
static int tolua_PAC_dev_get_delta_millisec00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int time1 = ((unsigned int)  tolua_tonumber(tolua_S,1,0));
  {
   unsigned int tolua_ret = (unsigned int)  get_delta_millisec(time1);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_delta_millisec'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: sleep_ms */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_sleep_ms00
static int tolua_PAC_dev_sleep_ms00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned long ms = ((unsigned long)  tolua_tonumber(tolua_S,1,0));
  {
   sleep_ms(ms);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sleep_ms'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_sec of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_sec
static int tolua_get_tm_tm_sec(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_sec'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_sec);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_sec of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_sec
static int tolua_set_tm_tm_sec(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_sec'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_sec = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_min of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_min
static int tolua_get_tm_tm_min(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_min'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_min);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_min of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_min
static int tolua_set_tm_tm_min(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_min'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_min = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_hour of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_hour
static int tolua_get_tm_tm_hour(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_hour'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_hour);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_hour of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_hour
static int tolua_set_tm_tm_hour(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_hour'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_hour = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_mday of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_mday
static int tolua_get_tm_tm_mday(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_mday'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_mday);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_mday of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_mday
static int tolua_set_tm_tm_mday(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_mday'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_mday = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_mon of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_mon
static int tolua_get_tm_tm_mon(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_mon'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_mon);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_mon of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_mon
static int tolua_set_tm_tm_mon(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_mon'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_mon = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_year of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_year
static int tolua_get_tm_tm_year(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_year'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_year);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_year of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_year
static int tolua_set_tm_tm_year(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_year'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_year = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_wday of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_wday
static int tolua_get_tm_tm_wday(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_wday'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_wday);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_wday of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_wday
static int tolua_set_tm_tm_wday(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_wday'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_wday = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_yday of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_yday
static int tolua_get_tm_tm_yday(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_yday'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_yday);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_yday of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_yday
static int tolua_set_tm_tm_yday(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_yday'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_yday = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: tm_isdst of class  tm */
#ifndef TOLUA_DISABLE_tolua_get_tm_tm_isdst
static int tolua_get_tm_tm_isdst(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_isdst'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->tm_isdst);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: tm_isdst of class  tm */
#ifndef TOLUA_DISABLE_tolua_set_tm_tm_isdst
static int tolua_set_tm_tm_isdst(lua_State* tolua_S)
{
  tm* self = (tm*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'tm_isdst'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->tm_isdst = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* function: get_time */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_get_time00
static int tolua_PAC_dev_get_time00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   tm tolua_ret = (tm)  get_time();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((tm)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"tm");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(tm));
     tolua_pushusertype(tolua_S,tolua_obj,"tm");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_time'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: eval of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_eval00
static int tolua_PAC_dev_MSAPID_eval00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'eval'", NULL);
#endif
  {
   self->eval();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'eval'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: eval of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_eval01
static int tolua_PAC_dev_MSAPID_eval01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
  float inputvalue = ((float)  tolua_tonumber(tolua_S,2,0));
  float task = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'eval'", NULL);
#endif
  {
   self->eval(inputvalue,task);
  }
 }
 return 0;
tolua_lerror:
 return tolua_PAC_dev_MSAPID_eval00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: reset of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_reset00
static int tolua_PAC_dev_MSAPID_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reset'", NULL);
#endif
  {
   self->reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: on of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_on00
static int tolua_PAC_dev_MSAPID_on00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
  int accel = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'on'", NULL);
#endif
  {
   self->on(accel);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'on'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: off of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_off00
static int tolua_PAC_dev_MSAPID_off00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'off'", NULL);
#endif
  {
   self->off();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'off'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_set00
static int tolua_PAC_dev_MSAPID_set00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
  float new_z = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set'", NULL);
#endif
  {
   self->set(new_z);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_state of class  MSAPID */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_MSAPID_get_state00
static int tolua_PAC_dev_MSAPID_get_state00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"MSAPID",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  MSAPID* self = (MSAPID*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_state'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_state();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_state'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_new00
static int tolua_PAC_dev_cipline_tech_object_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   cipline_tech_object* tolua_ret = (cipline_tech_object*)  Mtolua_new((cipline_tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"cipline_tech_object");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_new00_local
static int tolua_PAC_dev_cipline_tech_object_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isstring(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,11,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,12,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int number = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int type = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  const char* name_Lua = ((const char*)  tolua_tostring(tolua_S,5,0));
  unsigned int states_count = ((unsigned int)  tolua_tonumber(tolua_S,6,0));
  unsigned int timers_count = ((unsigned int)  tolua_tonumber(tolua_S,7,0));
  unsigned int par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,8,0));
  unsigned int runtime_par_float_count = ((unsigned int)  tolua_tonumber(tolua_S,9,0));
  unsigned int par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,10,0));
  unsigned int runtime_par_uint_count = ((unsigned int)  tolua_tonumber(tolua_S,11,0));
  {
   cipline_tech_object* tolua_ret = (cipline_tech_object*)  Mtolua_new((cipline_tech_object)(name,number,type,name_Lua,states_count,timers_count,par_float_count,runtime_par_float_count,par_uint_count,runtime_par_uint_count));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"cipline_tech_object");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: blocked of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_blocked
static int tolua_get_cipline_tech_object_blocked(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'blocked'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->blocked);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: blocked of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_blocked
static int tolua_set_cipline_tech_object_blocked(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'blocked'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->blocked = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: opcip of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_opcip
static int tolua_get_cipline_tech_object_opcip(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'opcip'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->opcip);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: opcip of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_opcip
static int tolua_set_cipline_tech_object_opcip(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'opcip'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->opcip = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: curstep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_curstep
static int tolua_get_cipline_tech_object_curstep(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curstep'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->curstep);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: curstep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_curstep
static int tolua_set_cipline_tech_object_curstep(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curstep'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->curstep = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: state of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_state
static int tolua_get_cipline_tech_object_state(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'state'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->state);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: state of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_state
static int tolua_set_cipline_tech_object_state(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'state'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->state = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: curprg of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_curprg
static int tolua_get_cipline_tech_object_curprg(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curprg'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->curprg);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: curprg of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_curprg
static int tolua_set_cipline_tech_object_curprg(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'curprg'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->curprg = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: loadedRecipe of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_loadedRecipe
static int tolua_get_cipline_tech_object_loadedRecipe(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'loadedRecipe'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->loadedRecipe);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: loadedRecipe of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_loadedRecipe
static int tolua_set_cipline_tech_object_loadedRecipe(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'loadedRecipe'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->loadedRecipe = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: loadedProgram of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_loadedProgram
static int tolua_get_cipline_tech_object_loadedProgram(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'loadedProgram'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->loadedProgram);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: loadedProgram of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_loadedProgram
static int tolua_set_cipline_tech_object_loadedProgram(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'loadedProgram'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->loadedProgram = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nmr of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_nmr
static int tolua_get_cipline_tech_object_nmr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nmr'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->nmr);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nmr of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_nmr
static int tolua_set_cipline_tech_object_nmr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nmr'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nmr = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: nplaststate of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_nplaststate
static int tolua_get_cipline_tech_object_nplaststate(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nplaststate'",NULL);
#endif
  tolua_pushboolean(tolua_S,(bool)self->nplaststate);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: nplaststate of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_nplaststate
static int tolua_set_cipline_tech_object_nplaststate(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'nplaststate'",NULL);
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->nplaststate = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: pidf_override of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_pidf_override
static int tolua_get_cipline_tech_object_pidf_override(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'pidf_override'",NULL);
#endif
  tolua_pushboolean(tolua_S,(bool)self->pidf_override);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: pidf_override of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_pidf_override
static int tolua_set_cipline_tech_object_pidf_override(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'pidf_override'",NULL);
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->pidf_override = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: cip_in_error of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_cip_in_error
static int tolua_get_cipline_tech_object_cip_in_error(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cip_in_error'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->cip_in_error);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: cip_in_error of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_cip_in_error
static int tolua_set_cipline_tech_object_cip_in_error(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cip_in_error'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->cip_in_error = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: no_neutro of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_no_neutro
static int tolua_get_cipline_tech_object_no_neutro(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no_neutro'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->no_neutro);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: no_neutro of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_no_neutro
static int tolua_set_cipline_tech_object_no_neutro(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no_neutro'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->no_neutro = ((char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: dont_use_water_tank of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_dont_use_water_tank
static int tolua_get_cipline_tech_object_dont_use_water_tank(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dont_use_water_tank'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->dont_use_water_tank);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: dont_use_water_tank of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_dont_use_water_tank
static int tolua_set_cipline_tech_object_dont_use_water_tank(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'dont_use_water_tank'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->dont_use_water_tank = ((char)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: disable_tank_heating of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_disable_tank_heating
static int tolua_get_cipline_tech_object_disable_tank_heating(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'disable_tank_heating'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->disable_tank_heating);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: disable_tank_heating of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_disable_tank_heating
static int tolua_set_cipline_tech_object_disable_tank_heating(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'disable_tank_heating'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->disable_tank_heating = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ret_overrride of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ret_overrride
static int tolua_get_cipline_tech_object_ret_overrride(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ret_overrride'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->ret_overrride);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ret_overrride of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ret_overrride
static int tolua_set_cipline_tech_object_ret_overrride(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ret_overrride'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ret_overrride = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: return_ok of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_return_ok
static int tolua_get_cipline_tech_object_return_ok(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'return_ok'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->return_ok);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: return_ok of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_return_ok
static int tolua_set_cipline_tech_object_return_ok(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'return_ok'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->return_ok = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: concentration_ok of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_concentration_ok
static int tolua_get_cipline_tech_object_concentration_ok(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'concentration_ok'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->concentration_ok);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: concentration_ok of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_concentration_ok
static int tolua_set_cipline_tech_object_concentration_ok(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'concentration_ok'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->concentration_ok = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: enable_ret_pump of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_enable_ret_pump
static int tolua_get_cipline_tech_object_enable_ret_pump(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'enable_ret_pump'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->enable_ret_pump);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: enable_ret_pump of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_enable_ret_pump
static int tolua_set_cipline_tech_object_enable_ret_pump(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'enable_ret_pump'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->enable_ret_pump = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: clean_water_rinsing_return of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_clean_water_rinsing_return
static int tolua_get_cipline_tech_object_clean_water_rinsing_return(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'clean_water_rinsing_return'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->clean_water_rinsing_return);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: clean_water_rinsing_return of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_clean_water_rinsing_return
static int tolua_set_cipline_tech_object_clean_water_rinsing_return(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'clean_water_rinsing_return'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->clean_water_rinsing_return = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: scoldvalves of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_scoldvalves
static int tolua_get_cipline_tech_object_scoldvalves(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'scoldvalves'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->scoldvalves);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: scoldvalves of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_scoldvalves
static int tolua_set_cipline_tech_object_scoldvalves(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'scoldvalves'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->scoldvalves = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: no_acid_wash_max of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_no_acid_wash_max
static int tolua_get_cipline_tech_object_no_acid_wash_max(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no_acid_wash_max'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->no_acid_wash_max);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: no_acid_wash_max of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_no_acid_wash_max
static int tolua_set_cipline_tech_object_no_acid_wash_max(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'no_acid_wash_max'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->no_acid_wash_max = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: use_internal_medium_recipes of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_use_internal_medium_recipes
static int tolua_get_cipline_tech_object_use_internal_medium_recipes(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'use_internal_medium_recipes'",NULL);
#endif
  tolua_pushboolean(tolua_S,(bool)self->use_internal_medium_recipes);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: use_internal_medium_recipes of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_use_internal_medium_recipes
static int tolua_set_cipline_tech_object_use_internal_medium_recipes(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'use_internal_medium_recipes'",NULL);
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->use_internal_medium_recipes = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: disable_final_rinsing of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_disable_final_rinsing
static int tolua_get_cipline_tech_object_disable_final_rinsing(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'disable_final_rinsing'",NULL);
#endif
  tolua_pushboolean(tolua_S,(bool)self->disable_final_rinsing);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: disable_final_rinsing of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_disable_final_rinsing
static int tolua_set_cipline_tech_object_disable_final_rinsing(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'disable_final_rinsing'",NULL);
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->disable_final_rinsing = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: use_circulation_on_v2_supply of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_use_circulation_on_v2_supply
static int tolua_get_cipline_tech_object_use_circulation_on_v2_supply(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'use_circulation_on_v2_supply'",NULL);
#endif
  tolua_pushboolean(tolua_S,(bool)self->use_circulation_on_v2_supply);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: use_circulation_on_v2_supply of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_use_circulation_on_v2_supply
static int tolua_set_cipline_tech_object_use_circulation_on_v2_supply(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'use_circulation_on_v2_supply'",NULL);
  if (!tolua_isboolean(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->use_circulation_on_v2_supply = ((bool)  tolua_toboolean(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V00 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V00_ptr
static int tolua_get_cipline_tech_object_V00_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V00'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V00,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V00 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V00_ptr
static int tolua_set_cipline_tech_object_V00_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V00'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V00 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V01 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V01_ptr
static int tolua_get_cipline_tech_object_V01_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V01'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V01,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V01 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V01_ptr
static int tolua_set_cipline_tech_object_V01_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V01'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V01 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V02 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V02_ptr
static int tolua_get_cipline_tech_object_V02_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V02'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V02,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V02 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V02_ptr
static int tolua_set_cipline_tech_object_V02_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V02'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V02 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V03 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V03_ptr
static int tolua_get_cipline_tech_object_V03_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V03'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V03,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V03 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V03_ptr
static int tolua_set_cipline_tech_object_V03_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V03'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V03 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V04 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V04_ptr
static int tolua_get_cipline_tech_object_V04_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V04'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V04,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V04 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V04_ptr
static int tolua_set_cipline_tech_object_V04_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V04'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V04 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V05 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V05_ptr
static int tolua_get_cipline_tech_object_V05_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V05'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V05,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V05 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V05_ptr
static int tolua_set_cipline_tech_object_V05_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V05'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V05 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V06 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V06_ptr
static int tolua_get_cipline_tech_object_V06_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V06'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V06,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V06 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V06_ptr
static int tolua_set_cipline_tech_object_V06_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V06'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V06 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V07 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V07_ptr
static int tolua_get_cipline_tech_object_V07_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V07'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V07,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V07 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V07_ptr
static int tolua_set_cipline_tech_object_V07_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V07'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V07 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V08 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V08_ptr
static int tolua_get_cipline_tech_object_V08_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V08'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V08,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V08 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V08_ptr
static int tolua_set_cipline_tech_object_V08_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V08'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V08 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V09 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V09_ptr
static int tolua_get_cipline_tech_object_V09_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V09'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V09,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V09 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V09_ptr
static int tolua_set_cipline_tech_object_V09_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V09'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V09 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V10 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V10_ptr
static int tolua_get_cipline_tech_object_V10_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V10'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V10,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V10 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V10_ptr
static int tolua_set_cipline_tech_object_V10_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V10'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V10 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V11 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V11_ptr
static int tolua_get_cipline_tech_object_V11_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V11'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V11,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V11 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V11_ptr
static int tolua_set_cipline_tech_object_V11_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V11'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V11 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V12 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V12_ptr
static int tolua_get_cipline_tech_object_V12_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V12'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V12,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V12 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V12_ptr
static int tolua_set_cipline_tech_object_V12_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V12'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V12 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: V13 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_V13_ptr
static int tolua_get_cipline_tech_object_V13_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V13'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->V13,"i_DO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: V13 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_V13_ptr
static int tolua_set_cipline_tech_object_V13_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'V13'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->V13 = ((i_DO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: NP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_NP_ptr
static int tolua_get_cipline_tech_object_NP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NP'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->NP,"i_DO_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: NP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_NP_ptr
static int tolua_set_cipline_tech_object_NP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NP'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->NP = ((i_DO_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: NK of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_NK_ptr
static int tolua_get_cipline_tech_object_NK_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NK'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->NK,"i_DO_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: NK of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_NK_ptr
static int tolua_set_cipline_tech_object_NK_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NK'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->NK = ((i_DO_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: NS of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_NS_ptr
static int tolua_get_cipline_tech_object_NS_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NS'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->NS,"i_DO_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: NS of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_NS_ptr
static int tolua_set_cipline_tech_object_NS_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'NS'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DO_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->NS = ((i_DO_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LL_ptr
static int tolua_get_cipline_tech_object_LL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LL_ptr
static int tolua_set_cipline_tech_object_LL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LM of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LM_ptr
static int tolua_get_cipline_tech_object_LM_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LM'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LM,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LM of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LM_ptr
static int tolua_set_cipline_tech_object_LM_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LM'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LM = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LH_ptr
static int tolua_get_cipline_tech_object_LH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LH_ptr
static int tolua_set_cipline_tech_object_LH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LWL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LWL_ptr
static int tolua_get_cipline_tech_object_LWL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LWL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LWL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LWL_ptr
static int tolua_set_cipline_tech_object_LWL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LWL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LWH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LWH_ptr
static int tolua_get_cipline_tech_object_LWH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LWH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LWH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LWH_ptr
static int tolua_set_cipline_tech_object_LWH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LWH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LWH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LSL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LSL_ptr
static int tolua_get_cipline_tech_object_LSL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LSL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LSL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LSL_ptr
static int tolua_set_cipline_tech_object_LSL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LSL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LSH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LSH_ptr
static int tolua_get_cipline_tech_object_LSH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LSH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LSH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LSH_ptr
static int tolua_set_cipline_tech_object_LSH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LSH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LSH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LKL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LKL_ptr
static int tolua_get_cipline_tech_object_LKL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LKL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LKL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LKL_ptr
static int tolua_set_cipline_tech_object_LKL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LKL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: LKH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_LKH_ptr
static int tolua_get_cipline_tech_object_LKH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKH'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->LKH,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: LKH of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_LKH_ptr
static int tolua_set_cipline_tech_object_LKH_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'LKH'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->LKH = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: TP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_TP_ptr
static int tolua_get_cipline_tech_object_TP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TP'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->TP,"i_AI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: TP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_TP_ptr
static int tolua_set_cipline_tech_object_TP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TP'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->TP = ((i_AI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: TR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_TR_ptr
static int tolua_get_cipline_tech_object_TR_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TR'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->TR,"i_AI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: TR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_TR_ptr
static int tolua_set_cipline_tech_object_TR_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'TR'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->TR = ((i_AI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: Q of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_Q_ptr
static int tolua_get_cipline_tech_object_Q_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Q'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->Q,"i_AI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: Q of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_Q_ptr
static int tolua_set_cipline_tech_object_Q_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'Q'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->Q = ((i_AI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ao of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ao_ptr
static int tolua_get_cipline_tech_object_ao_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ao'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->ao,"i_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ao of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ao_ptr
static int tolua_set_cipline_tech_object_ao_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ao'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ao = ((i_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: PUMPFREQ of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_PUMPFREQ_ptr
static int tolua_get_cipline_tech_object_PUMPFREQ_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PUMPFREQ'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->PUMPFREQ,"i_AO_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: PUMPFREQ of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_PUMPFREQ_ptr
static int tolua_set_cipline_tech_object_PUMPFREQ_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PUMPFREQ'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_AO_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->PUMPFREQ = ((i_AO_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: FL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_FL_ptr
static int tolua_get_cipline_tech_object_FL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'FL'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->FL,"i_DI_device");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: FL of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_FL_ptr
static int tolua_set_cipline_tech_object_FL_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'FL'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_DI_device",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->FL = ((i_DI_device*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: cnt of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_cnt_ptr
static int tolua_get_cipline_tech_object_cnt_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cnt'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->cnt,"i_counter");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: cnt of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_cnt_ptr
static int tolua_set_cipline_tech_object_cnt_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'cnt'",NULL);
  if (!tolua_isusertype(tolua_S,2,"i_counter",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->cnt = ((i_counter*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: T of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_PAC_dev_cipline_tech_object_T
static int tolua_get_PAC_dev_cipline_tech_object_T(lua_State* tolua_S)
{
 int tolua_index;
  cipline_tech_object* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (cipline_tech_object*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=10)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  tolua_pushusertype(tolua_S,(void*)self->T[tolua_index],"timer");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: T of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_PAC_dev_cipline_tech_object_T
static int tolua_set_PAC_dev_cipline_tech_object_T(lua_State* tolua_S)
{
 int tolua_index;
  cipline_tech_object* self;
 lua_pushstring(tolua_S,".self");
 lua_rawget(tolua_S,1);
 self = (cipline_tech_object*)  lua_touserdata(tolua_S,-1);
#ifndef TOLUA_RELEASE
 {
  tolua_Error tolua_err;
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in array indexing.",&tolua_err);
 }
#endif
 tolua_index = (int)tolua_tonumber(tolua_S,2,0);
#ifndef TOLUA_RELEASE
 if (tolua_index<0 || tolua_index>=10)
  tolua_error(tolua_S,"array indexing out of range.",NULL);
#endif
  self->T[tolua_index] = ((timer*)  tolua_tousertype(tolua_S,3,0));
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: PIDP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_PIDP_ptr
static int tolua_get_cipline_tech_object_PIDP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PIDP'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->PIDP,"MSAPID");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: PIDP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_PIDP_ptr
static int tolua_set_cipline_tech_object_PIDP_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PIDP'",NULL);
  if (!tolua_isusertype(tolua_S,2,"MSAPID",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->PIDP = ((MSAPID*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: PIDF of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_PIDF_ptr
static int tolua_get_cipline_tech_object_PIDF_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PIDF'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)self->PIDF,"MSAPID");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: PIDF of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_PIDF_ptr
static int tolua_set_cipline_tech_object_PIDF_ptr(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'PIDF'",NULL);
  if (!tolua_isusertype(tolua_S,2,"MSAPID",0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->PIDF = ((MSAPID*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: msa_number of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_msa_number
static int tolua_get_cipline_tech_object_msa_number(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'msa_number'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->msa_number);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: msa_number of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_msa_number
static int tolua_set_cipline_tech_object_msa_number(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'msa_number'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->msa_number = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ncar1 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ncar1
static int tolua_get_cipline_tech_object_ncar1(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar1'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)self->ncar1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ncar1 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ncar1
static int tolua_set_cipline_tech_object_ncar1(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar1'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ncar1 = ((char*)  tolua_tostring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ncar2 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ncar2
static int tolua_get_cipline_tech_object_ncar2(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar2'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)self->ncar2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ncar2 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ncar2
static int tolua_set_cipline_tech_object_ncar2(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar2'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ncar2 = ((char*)  tolua_tostring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ncar3 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ncar3
static int tolua_get_cipline_tech_object_ncar3(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar3'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)self->ncar3);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ncar3 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ncar3
static int tolua_set_cipline_tech_object_ncar3(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar3'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ncar3 = ((char*)  tolua_tostring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: ncar4 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_ncar4
static int tolua_get_cipline_tech_object_ncar4(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar4'",NULL);
#endif
  tolua_pushstring(tolua_S,(const char*)self->ncar4);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: ncar4 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_ncar4
static int tolua_set_cipline_tech_object_ncar4(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'ncar4'",NULL);
  if (!tolua_isstring(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->ncar4 = ((char*)  tolua_tostring(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: switch1 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_switch1
static int tolua_get_cipline_tech_object_switch1(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch1'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->switch1);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: switch1 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_switch1
static int tolua_set_cipline_tech_object_switch1(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch1'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->switch1 = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: switch2 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_switch2
static int tolua_get_cipline_tech_object_switch2(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch2'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->switch2);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: switch2 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_switch2
static int tolua_set_cipline_tech_object_switch2(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch2'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->switch2 = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: switch3 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_switch3
static int tolua_get_cipline_tech_object_switch3(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch3'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->switch3);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: switch3 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_switch3
static int tolua_set_cipline_tech_object_switch3(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch3'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->switch3 = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: switch4 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_switch4
static int tolua_get_cipline_tech_object_switch4(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch4'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->switch4);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: switch4 of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_switch4
static int tolua_set_cipline_tech_object_switch4(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'switch4'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->switch4 = ((int)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_par_float
static int tolua_get_cipline_tech_object_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->par_float,"saved_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_par_float
static int tolua_set_cipline_tech_object_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"saved_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->par_float = *((saved_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: rt_par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_get_cipline_tech_object_rt_par_float
static int tolua_get_cipline_tech_object_rt_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
#endif
   tolua_pushusertype(tolua_S,(void*)&self->rt_par_float,"run_time_params_float");
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: rt_par_float of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_set_cipline_tech_object_rt_par_float
static int tolua_set_cipline_tech_object_rt_par_float(lua_State* tolua_S)
{
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'rt_par_float'",NULL);
  if ((tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"run_time_params_float",0,&tolua_err)))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->rt_par_float = *((run_time_params_float*)  tolua_tousertype(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_station_par of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_get_station_par00
static int tolua_PAC_dev_cipline_tech_object_get_station_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int parno = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_station_par'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_station_par(parno);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_station_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_station_par of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_station_par00
static int tolua_PAC_dev_cipline_tech_object_set_station_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int parno = ((int)  tolua_tonumber(tolua_S,2,0));
  float newval = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_station_par'", NULL);
#endif
  {
   self->set_station_par(parno,newval);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_station_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_selfclean_par of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_get_selfclean_par00
static int tolua_PAC_dev_cipline_tech_object_get_selfclean_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int parno = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_selfclean_par'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_selfclean_par(parno);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_selfclean_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_selfclean_par of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_selfclean_par00
static int tolua_PAC_dev_cipline_tech_object_set_selfclean_par00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int parno = ((int)  tolua_tonumber(tolua_S,2,0));
  float newval = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_selfclean_par'", NULL);
#endif
  {
   self->set_selfclean_par(parno,newval);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_selfclean_par'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_cmd00
static int tolua_PAC_dev_cipline_tech_object_set_cmd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  const char* val = ((const char*)  tolua_tostring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_cmd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_cmd of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_set_cmd01
static int tolua_PAC_dev_cipline_tech_object_set_cmd01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  const char* prop = ((const char*)  tolua_tostring(tolua_S,2,0));
  unsigned int idx = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  double val = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_cmd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->set_cmd(prop,idx,val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_PAC_dev_cipline_tech_object_set_cmd00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: initline of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_initline00
static int tolua_PAC_dev_cipline_tech_object_initline00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'initline'", NULL);
#endif
  {
   self->initline();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'initline'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluate of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_evaluate00
static int tolua_PAC_dev_cipline_tech_object_evaluate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'evaluate'", NULL);
#endif
  {
   int tolua_ret = (int)  self->evaluate();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'evaluate'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetConc of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_GetConc00
static int tolua_PAC_dev_cipline_tech_object_GetConc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetConc'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetConc(what);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetConc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SortRR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_SortRR00
static int tolua_PAC_dev_cipline_tech_object_SortRR00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int where = ((int)  tolua_tonumber(tolua_S,2,0));
  int forcetotank = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SortRR'", NULL);
#endif
  {
   self->SortRR(where,forcetotank);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SortRR'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRet of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_SetRet00
static int tolua_PAC_dev_cipline_tech_object_SetRet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int val = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRet'", NULL);
#endif
  {
   int tolua_ret = (int)  self->SetRet(val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceRet of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_ForceRet00
static int tolua_PAC_dev_cipline_tech_object_ForceRet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int val = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceRet'", NULL);
#endif
  {
   int tolua_ret = (int)  self->ForceRet(val);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceRet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRetState of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_GetRetState00
static int tolua_PAC_dev_cipline_tech_object_GetRetState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRetState'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetRetState();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRetState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasRet of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_HasRet00
static int tolua_PAC_dev_cipline_tech_object_HasRet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasRet'", NULL);
#endif
  {
   int tolua_ret = (int)  self->HasRet();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasRet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: timeIsOut of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object_timeIsOut00
static int tolua_PAC_dev_cipline_tech_object_timeIsOut00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'timeIsOut'", NULL);
#endif
  {
   int tolua_ret = (int)  self->timeIsOut();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'timeIsOut'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _DoStep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__DoStep00
static int tolua_PAC_dev_cipline_tech_object__DoStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int step_to_do = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_DoStep'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_DoStep(step_to_do);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_DoStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _GoToStep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__GoToStep00
static int tolua_PAC_dev_cipline_tech_object__GoToStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int cur = ((int)  tolua_tonumber(tolua_S,2,0));
  int param = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_GoToStep'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_GoToStep(cur,param);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_GoToStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitStep of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitStep00
static int tolua_PAC_dev_cipline_tech_object__InitStep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int step_to_init = ((int)  tolua_tonumber(tolua_S,2,0));
  int not_first_call = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitStep'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitStep(step_to_init,not_first_call);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitStep'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _LoadProgram of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__LoadProgram00
static int tolua_PAC_dev_cipline_tech_object__LoadProgram00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_LoadProgram'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_LoadProgram();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_LoadProgram'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _StopDev of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__StopDev00
static int tolua_PAC_dev_cipline_tech_object__StopDev00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_StopDev'", NULL);
#endif
  {
   self->_StopDev();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_StopDev'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _ResetLinesDevicesBeforeReset of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__ResetLinesDevicesBeforeReset00
static int tolua_PAC_dev_cipline_tech_object__ResetLinesDevicesBeforeReset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_ResetLinesDevicesBeforeReset'", NULL);
#endif
  {
   self->_ResetLinesDevicesBeforeReset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_ResetLinesDevicesBeforeReset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _OporCIP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__OporCIP00
static int tolua_PAC_dev_cipline_tech_object__OporCIP00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int where = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_OporCIP'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_OporCIP(where);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_OporCIP'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitOporCIP of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitOporCIP00
static int tolua_PAC_dev_cipline_tech_object__InitOporCIP00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int where = ((int)  tolua_tonumber(tolua_S,2,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,3,0));
  int not_first_call = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitOporCIP'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitOporCIP(where,step_to_init,not_first_call);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitOporCIP'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _CheckErr of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__CheckErr00
static int tolua_PAC_dev_cipline_tech_object__CheckErr00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_CheckErr'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_CheckErr();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_CheckErr'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _Circ of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__Circ00
static int tolua_PAC_dev_cipline_tech_object__Circ00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_Circ'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_Circ(what);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_Circ'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitCirc of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitCirc00
static int tolua_PAC_dev_cipline_tech_object__InitCirc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,3,0));
  int not_first_call = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitCirc'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitCirc(what,step_to_init,not_first_call);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitCirc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitToObject of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitToObject00
static int tolua_PAC_dev_cipline_tech_object__InitToObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int from = ((int)  tolua_tonumber(tolua_S,2,0));
  int where = ((int)  tolua_tonumber(tolua_S,3,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,4,0));
  int f = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitToObject'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitToObject(from,where,step_to_init,f);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitToObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitFromObject of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitFromObject00
static int tolua_PAC_dev_cipline_tech_object__InitFromObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
  int where = ((int)  tolua_tonumber(tolua_S,3,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,4,0));
  int f = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitFromObject'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitFromObject(what,where,step_to_init,f);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitFromObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitFilCirc of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitFilCirc00
static int tolua_PAC_dev_cipline_tech_object__InitFilCirc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int with_what = ((int)  tolua_tonumber(tolua_S,2,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,3,0));
  int f = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitFilCirc'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitFilCirc(with_what,step_to_init,f);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitFilCirc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitOporCirc of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitOporCirc00
static int tolua_PAC_dev_cipline_tech_object__InitOporCirc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int where = ((int)  tolua_tonumber(tolua_S,2,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,3,0));
  int not_first_call = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitOporCirc'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitOporCirc(where,step_to_init,not_first_call);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitOporCirc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _ToObject of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__ToObject00
static int tolua_PAC_dev_cipline_tech_object__ToObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int from = ((int)  tolua_tonumber(tolua_S,2,0));
  int where = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_ToObject'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_ToObject(from,where);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_ToObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _FromObject of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__FromObject00
static int tolua_PAC_dev_cipline_tech_object__FromObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
  int where = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_FromObject'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_FromObject(what,where);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_FromObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _FillCirc of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__FillCirc00
static int tolua_PAC_dev_cipline_tech_object__FillCirc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int with_what = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_FillCirc'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_FillCirc(with_what);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_FillCirc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _OporCirc of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__OporCirc00
static int tolua_PAC_dev_cipline_tech_object__OporCirc00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int where = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_OporCirc'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_OporCirc(where);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_OporCirc'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _RT of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__RT00
static int tolua_PAC_dev_cipline_tech_object__RT00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_RT'", NULL);
#endif
  {
   self->_RT();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_RT'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _Stop of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__Stop00
static int tolua_PAC_dev_cipline_tech_object__Stop00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int step_to_stop = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_Stop'", NULL);
#endif
  {
   self->_Stop(step_to_stop);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_Stop'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _InitDoseRR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__InitDoseRR00
static int tolua_PAC_dev_cipline_tech_object__InitDoseRR00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
  int step_to_init = ((int)  tolua_tonumber(tolua_S,3,0));
  int not_first_call = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_InitDoseRR'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_InitDoseRR(what,step_to_init,not_first_call);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_InitDoseRR'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: _DoseRR of class  cipline_tech_object */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_cipline_tech_object__DoseRR00
static int tolua_PAC_dev_cipline_tech_object__DoseRR00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"cipline_tech_object",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  cipline_tech_object* self = (cipline_tech_object*)  tolua_tousertype(tolua_S,1,0);
  int what = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function '_DoseRR'", NULL);
#endif
  {
   int tolua_ret = (int)  self->_DoseRR(what);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '_DoseRR'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_new00
static int tolua_PAC_dev_modbus_client_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isstring(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int id = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned int port = ((unsigned int)  tolua_tonumber(tolua_S,4,502));
  unsigned long exchangetimeout = ((unsigned long)  tolua_tonumber(tolua_S,5,50));
  const char* name = ((const char*)  tolua_tostring(tolua_S,6,nullptr));
  {
   modbus_client* tolua_ret = (modbus_client*)  Mtolua_new((modbus_client)(id,ip,port,exchangetimeout,name));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"modbus_client");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_new00_local
static int tolua_PAC_dev_modbus_client_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isstring(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int id = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  const char* ip = ((const char*)  tolua_tostring(tolua_S,3,0));
  unsigned int port = ((unsigned int)  tolua_tonumber(tolua_S,4,502));
  unsigned long exchangetimeout = ((unsigned long)  tolua_tonumber(tolua_S,5,50));
  const char* name = ((const char*)  tolua_tostring(tolua_S,6,nullptr));
  {
   modbus_client* tolua_ret = (modbus_client*)  Mtolua_new((modbus_client)(id,ip,port,exchangetimeout,name));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"modbus_client");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_discrete_inputs of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_discrete_inputs00
static int tolua_PAC_dev_modbus_client_read_discrete_inputs00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int start_address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_discrete_inputs'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_discrete_inputs(start_address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_discrete_inputs'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_coils of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_coils00
static int tolua_PAC_dev_modbus_client_read_coils00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int start_address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_coils'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_coils(start_address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_coils'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_holding_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_holding_registers00
static int tolua_PAC_dev_modbus_client_read_holding_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_holding_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_holding_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_holding_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: read_input_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_read_input_registers00
static int tolua_PAC_dev_modbus_client_read_input_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'read_input_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->read_input_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'read_input_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: write_coil of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_write_coil00
static int tolua_PAC_dev_modbus_client_write_coil00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned char value = ((unsigned char)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'write_coil'", NULL);
#endif
  {
   int tolua_ret = (int)  self->write_coil(address,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_coil'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: force_multiply_coils of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_force_multiply_coils00
static int tolua_PAC_dev_modbus_client_force_multiply_coils00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'force_multiply_coils'", NULL);
#endif
  {
   int tolua_ret = (int)  self->force_multiply_coils(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'force_multiply_coils'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: write_multiply_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_write_multiply_registers00
static int tolua_PAC_dev_modbus_client_write_multiply_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'write_multiply_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->write_multiply_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_multiply_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_read_discrete_inputs of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_read_discrete_inputs00
static int tolua_PAC_dev_modbus_client_async_read_discrete_inputs00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int start_address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_read_discrete_inputs'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_read_discrete_inputs(start_address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_read_discrete_inputs'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_read_coils of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_read_coils00
static int tolua_PAC_dev_modbus_client_async_read_coils00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int start_address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_read_coils'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_read_coils(start_address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_read_coils'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_read_holding_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_read_holding_registers00
static int tolua_PAC_dev_modbus_client_async_read_holding_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_read_holding_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_read_holding_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_read_holding_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_read_input_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_read_input_registers00
static int tolua_PAC_dev_modbus_client_async_read_input_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_read_input_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_read_input_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_read_input_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_write_coil of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_write_coil00
static int tolua_PAC_dev_modbus_client_async_write_coil00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned char value = ((unsigned char)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_write_coil'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_write_coil(address,value);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_write_coil'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_force_multiply_coils of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_force_multiply_coils00
static int tolua_PAC_dev_modbus_client_async_force_multiply_coils00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_force_multiply_coils'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_force_multiply_coils(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_force_multiply_coils'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_write_multiply_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_write_multiply_registers00
static int tolua_PAC_dev_modbus_client_async_write_multiply_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_write_multiply_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_write_multiply_registers(address,quantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_write_multiply_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_read_write_multiply_registers of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_read_write_multiply_registers00
static int tolua_PAC_dev_modbus_client_async_read_write_multiply_registers00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int readaddress = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int readquantity = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int wrireaddress = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
  unsigned int writequantity = ((unsigned int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_read_write_multiply_registers'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_read_write_multiply_registers(readaddress,readquantity,wrireaddress,writequantity);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_read_write_multiply_registers'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_mask_write_register of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_mask_write_register00
static int tolua_PAC_dev_modbus_client_async_mask_write_register00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int writeaddress = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int andmask = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int ormask = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_mask_write_register'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_mask_write_register(writeaddress,andmask,ormask);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'async_mask_write_register'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: async_mask_write_register of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_async_mask_write_register01
static int tolua_PAC_dev_modbus_client_async_mask_write_register01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int writeaddress = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'async_mask_write_register'", NULL);
#endif
  {
   int tolua_ret = (int)  self->async_mask_write_register(writeaddress);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_PAC_dev_modbus_client_async_mask_write_register00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_async_result of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_async_result00
static int tolua_PAC_dev_modbus_client_get_async_result00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_async_result'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_async_result();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_async_result'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_station of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_station00
static int tolua_PAC_dev_modbus_client_set_station00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned char new_station_id = ((unsigned char)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_station'", NULL);
#endif
  {
   self->set_station(new_station_id);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_station'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: zero_output_buff of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_zero_output_buff00
static int tolua_PAC_dev_modbus_client_zero_output_buff00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  int startpos = ((int)  tolua_tonumber(tolua_S,2,13));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'zero_output_buff'", NULL);
#endif
  {
   self->zero_output_buff(startpos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'zero_output_buff'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_byte of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_byte00
static int tolua_PAC_dev_modbus_client_set_byte00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  int address = ((int)  tolua_tonumber(tolua_S,2,0));
  unsigned char value = ((unsigned char)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_byte'", NULL);
#endif
  {
   self->set_byte(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_byte'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_byte of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_byte00
static int tolua_PAC_dev_modbus_client_get_byte00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  int address = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_byte'", NULL);
#endif
  {
   unsigned char tolua_ret = (unsigned char)  self->get_byte(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_byte'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int2 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int200
static int tolua_PAC_dev_modbus_client_set_int200(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  short value = ((short)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int2'", NULL);
#endif
  {
   self->set_int2(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int2'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int2 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int200
static int tolua_PAC_dev_modbus_client_get_int200(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int2'", NULL);
#endif
  {
   short tolua_ret = (short)  self->get_int2(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int2'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int4 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int400
static int tolua_PAC_dev_modbus_client_set_int400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int4'", NULL);
#endif
  {
   self->set_int4(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int4'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int4 of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int400
static int tolua_PAC_dev_modbus_client_get_int400(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int4'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_int4(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int4'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int4_ab_cd of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int4_ab_cd00
static int tolua_PAC_dev_modbus_client_set_int4_ab_cd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int4_ab_cd'", NULL);
#endif
  {
   self->set_int4_ab_cd(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int4_ab_cd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int4_ab_cd of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int4_ab_cd00
static int tolua_PAC_dev_modbus_client_get_int4_ab_cd00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int4_ab_cd'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_int4_ab_cd(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int4_ab_cd'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int4_cd_ab of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int4_cd_ab00
static int tolua_PAC_dev_modbus_client_set_int4_cd_ab00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int4_cd_ab'", NULL);
#endif
  {
   self->set_int4_cd_ab(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int4_cd_ab'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int4_cd_ab of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int4_cd_ab00
static int tolua_PAC_dev_modbus_client_get_int4_cd_ab00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int4_cd_ab'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_int4_cd_ab(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int4_cd_ab'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_int4_dc_ba of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_int4_dc_ba00
static int tolua_PAC_dev_modbus_client_set_int4_dc_ba00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_int4_dc_ba'", NULL);
#endif
  {
   self->set_int4_dc_ba(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_int4_dc_ba'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_int4_dc_ba of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_int4_dc_ba00
static int tolua_PAC_dev_modbus_client_get_int4_dc_ba00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_int4_dc_ba'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_int4_dc_ba(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_int4_dc_ba'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_float of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_float00
static int tolua_PAC_dev_modbus_client_set_float00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_float'", NULL);
#endif
  {
   self->set_float(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_float'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_float of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_float00
static int tolua_PAC_dev_modbus_client_get_float00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_float'", NULL);
#endif
  {
   float tolua_ret = (float)  self->get_float(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_float'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: set_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_set_bit00
static int tolua_PAC_dev_modbus_client_set_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'set_bit'", NULL);
#endif
  {
   self->set_bit(address,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'set_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_bit00
static int tolua_PAC_dev_modbus_client_get_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int address = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_bit'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_bit(address);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reg_get_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_reg_get_bit00
static int tolua_PAC_dev_modbus_client_reg_get_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int reg = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reg_get_bit'", NULL);
#endif
  {
   int tolua_ret = (int)  self->reg_get_bit(reg,offset);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reg_get_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reg_set_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_reg_set_bit00
static int tolua_PAC_dev_modbus_client_reg_set_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned int reg = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int offset = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  int value = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reg_set_bit'", NULL);
#endif
  {
   self->reg_set_bit(reg,offset,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reg_set_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: mask_reset of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_mask_reset00
static int tolua_PAC_dev_modbus_client_mask_reset00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'mask_reset'", NULL);
#endif
  {
   self->mask_reset();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'mask_reset'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: mask_set_bit of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_mask_set_bit00
static int tolua_PAC_dev_modbus_client_mask_set_bit00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  int pos = ((int)  tolua_tonumber(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'mask_set_bit'", NULL);
#endif
  {
   self->mask_set_bit(pos,value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'mask_set_bit'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: reverse of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_reverse00
static int tolua_PAC_dev_modbus_client_reverse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  unsigned char b = ((unsigned char)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'reverse'", NULL);
#endif
  {
   unsigned char tolua_ret = (unsigned char)  self->reverse(b);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'reverse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: swapBits of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_swapBits00
static int tolua_PAC_dev_modbus_client_swapBits00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int p1 = ((int)  tolua_tonumber(tolua_S,3,0));
  int p2 = ((int)  tolua_tonumber(tolua_S,4,0));
  int n = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'swapBits'", NULL);
#endif
  {
   int tolua_ret = (int)  self->swapBits(x,p1,p2,n);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'swapBits'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: get_id of class  modbus_client */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_modbus_client_get_id00
static int tolua_PAC_dev_modbus_client_get_id00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"modbus_client",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  modbus_client* self = (modbus_client*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'get_id'", NULL);
#endif
  {
   int tolua_ret = (int)  self->get_id();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'get_id'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UnpackInt16 of class  ModbusServ */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ModbusServ_UnpackInt1600
static int tolua_PAC_dev_ModbusServ_UnpackInt1600(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ModbusServ",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned char* buf = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
  int offset = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   short int tolua_ret = (short int)  ModbusServ::UnpackInt16(buf,offset);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnpackInt16'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UnpackInt32 of class  ModbusServ */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ModbusServ_UnpackInt3200
static int tolua_PAC_dev_ModbusServ_UnpackInt3200(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ModbusServ",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned char* buf = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
  int offset = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   long int tolua_ret = (long int)  ModbusServ::UnpackInt32(buf,offset);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnpackInt32'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UnpackFloat of class  ModbusServ */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ModbusServ_UnpackFloat00
static int tolua_PAC_dev_ModbusServ_UnpackFloat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ModbusServ",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned char* Buf = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
  int offset = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   float tolua_ret = (float)  ModbusServ::UnpackFloat(Buf,offset);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnpackFloat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UnpackWord of class  ModbusServ */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ModbusServ_UnpackWord00
static int tolua_PAC_dev_ModbusServ_UnpackWord00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ModbusServ",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned char* Buf = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
  int offset = ((int)  tolua_tonumber(tolua_S,3,0));
  {
   unsigned int tolua_ret = (unsigned int)  ModbusServ::UnpackWord(Buf,offset);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UnpackWord'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: write_log of class  i_log */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_i_log_write_log00
static int tolua_PAC_dev_i_log_write_log00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"i_log",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  i_log* self = (i_log*)  tolua_tousertype(tolua_S,1,0);
  i_log::PRIORITIES priority = ((i_log::PRIORITIES) (int)  tolua_tonumber(tolua_S,2,0));
  const char* debug_message = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'write_log'", NULL);
#endif
  {
   self->write_log(priority,debug_message);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'write_log'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_SYS_LOG */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_SYS_LOG00
static int tolua_PAC_dev_G_SYS_LOG00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   i_log* tolua_ret = (i_log*)  G_SYS_LOG();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"i_log");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_SYS_LOG'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getId of class  ParamsRecipeStorage */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeStorage_getId00
static int tolua_PAC_dev_ParamsRecipeStorage_getId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ParamsRecipeStorage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ParamsRecipeStorage* self = (const ParamsRecipeStorage*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getCount of class  ParamsRecipeStorage */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeStorage_getCount00
static int tolua_PAC_dev_ParamsRecipeStorage_getCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ParamsRecipeStorage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ParamsRecipeStorage* self = (const ParamsRecipeStorage*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getParamsCount of class  ParamsRecipeStorage */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeStorage_getParamsCount00
static int tolua_PAC_dev_ParamsRecipeStorage_getParamsCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ParamsRecipeStorage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ParamsRecipeStorage* self = (const ParamsRecipeStorage*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getParamsCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getParamsCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getParamsCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRecPar of class  ParamsRecipeStorage */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeStorage_setRecPar00
static int tolua_PAC_dev_ParamsRecipeStorage_setRecPar00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeStorage",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeStorage* self = (ParamsRecipeStorage*)  tolua_tousertype(tolua_S,1,0);
  int recNo = ((int)  tolua_tonumber(tolua_S,2,0));
  int parNo = ((int)  tolua_tonumber(tolua_S,3,0));
  float newValue = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setRecPar'", NULL);
#endif
  {
   self->setRecPar(recNo,parNo,newValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setRecPar'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getRecPar of class  ParamsRecipeStorage */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeStorage_getRecPar00
static int tolua_PAC_dev_ParamsRecipeStorage_getRecPar00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeStorage",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeStorage* self = (ParamsRecipeStorage*)  tolua_tousertype(tolua_S,1,0);
  int recNo = ((int)  tolua_tonumber(tolua_S,2,0));
  int parNo = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getRecPar'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getRecPar(recNo,parNo);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getRecPar'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addMap of class  ParamsRecipeAdapter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeAdapter_addMap00
static int tolua_PAC_dev_ParamsRecipeAdapter_addMap00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeAdapter",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeAdapter* self = (ParamsRecipeAdapter*)  tolua_tousertype(tolua_S,1,0);
  unsigned int startRecPar = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  unsigned int startObjPar = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
  unsigned int quantity = ((unsigned int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addMap'", NULL);
#endif
  {
   self->addMap(startRecPar,startObjPar,quantity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addMap'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: loadParams of class  ParamsRecipeAdapter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeAdapter_loadParams00
static int tolua_PAC_dev_ParamsRecipeAdapter_loadParams00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeAdapter",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeAdapter* self = (ParamsRecipeAdapter*)  tolua_tousertype(tolua_S,1,0);
  int techObject = ((int)  tolua_tonumber(tolua_S,2,0));
  unsigned int recNo = ((unsigned int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'loadParams'", NULL);
#endif
  {
   self->loadParams(techObject,recNo);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'loadParams'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setUseSeparateRecipeList of class  ParamsRecipeAdapter */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeAdapter_setUseSeparateRecipeList00
static int tolua_PAC_dev_ParamsRecipeAdapter_setUseSeparateRecipeList00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeAdapter",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeAdapter* self = (ParamsRecipeAdapter*)  tolua_tousertype(tolua_S,1,0);
  bool state = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setUseSeparateRecipeList'", NULL);
#endif
  {
   self->setUseSeparateRecipeList(state);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setUseSeparateRecipeList'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: evaluate of class  ParamsRecipeManager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeManager_evaluate00
static int tolua_PAC_dev_ParamsRecipeManager_evaluate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeManager* self = (ParamsRecipeManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'evaluate'", NULL);
#endif
  {
   self->evaluate();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'evaluate'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createRecipes of class  ParamsRecipeManager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeManager_createRecipes00
static int tolua_PAC_dev_ParamsRecipeManager_createRecipes00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeManager* self = (ParamsRecipeManager*)  tolua_tousertype(tolua_S,1,0);
  int size = ((int)  tolua_tonumber(tolua_S,2,0));
  int quantity = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createRecipes'", NULL);
#endif
  {
   ParamsRecipeStorage* tolua_ret = (ParamsRecipeStorage*)  self->createRecipes(size,quantity);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"ParamsRecipeStorage");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createRecipes'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createAdapter of class  ParamsRecipeManager */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_ParamsRecipeManager_createAdapter00
static int tolua_PAC_dev_ParamsRecipeManager_createAdapter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ParamsRecipeManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"ParamsRecipeStorage",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ParamsRecipeManager* self = (ParamsRecipeManager*)  tolua_tousertype(tolua_S,1,0);
  ParamsRecipeStorage* recStorage = ((ParamsRecipeStorage*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createAdapter'", NULL);
#endif
  {
   ParamsRecipeAdapter* tolua_ret = (ParamsRecipeAdapter*)  self->createAdapter(recStorage);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"ParamsRecipeAdapter");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createAdapter'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: G_PARAMS_RECIPE_MANAGER */
#ifndef TOLUA_DISABLE_tolua_PAC_dev_G_PARAMS_RECIPE_MANAGER00
static int tolua_PAC_dev_G_PARAMS_RECIPE_MANAGER00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnoobj(tolua_S,1,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ParamsRecipeManager* tolua_ret = (ParamsRecipeManager*)  G_PARAMS_RECIPE_MANAGER();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"ParamsRecipeManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'G_PARAMS_RECIPE_MANAGER'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_PAC_dev_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"i_DI_device","i_DI_device","",NULL);
  tolua_beginmodule(tolua_S,"i_DI_device");
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_DI_device_get_state00);
   tolua_function(tolua_S,"is_active",tolua_PAC_dev_i_DI_device_is_active00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_DO_device","i_DO_device","i_DI_device",NULL);
  tolua_beginmodule(tolua_S,"i_DO_device");
   tolua_function(tolua_S,"on",tolua_PAC_dev_i_DO_device_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_DO_device_off00);
   tolua_function(tolua_S,"direct_off",tolua_PAC_dev_i_DO_device_direct_off00);
   tolua_function(tolua_S,"instant_off",tolua_PAC_dev_i_DO_device_instant_off00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_i_DO_device_set_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_AI_device","i_AI_device","",NULL);
  tolua_beginmodule(tolua_S,"i_AI_device");
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_i_AI_device_get_value00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_AI_device_get_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_AO_device","i_AO_device","i_AI_device",NULL);
  tolua_beginmodule(tolua_S,"i_AO_device");
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_AO_device_off00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_i_AO_device_set_value00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_DO_AO_device","i_DO_AO_device","i_DO_device",NULL);
  tolua_beginmodule(tolua_S,"i_DO_AO_device");
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_DO_AO_device_get_state00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_i_DO_AO_device_set_state00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_i_DO_AO_device_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_i_DO_AO_device_off00);
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_i_DO_AO_device_get_value00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_i_DO_AO_device_set_value00);
   tolua_variable(tolua_S,"__i_AO_device__",tolua_get_i_DO_AO_device___i_AO_device__,NULL);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_counter","i_counter","",NULL);
  tolua_beginmodule(tolua_S,"i_counter");
   tolua_function(tolua_S,"pause",tolua_PAC_dev_i_counter_pause00);
   tolua_function(tolua_S,"start",tolua_PAC_dev_i_counter_start00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_i_counter_reset00);
   tolua_function(tolua_S,"restart",tolua_PAC_dev_i_counter_restart00);
   tolua_function(tolua_S,"get_quantity",tolua_PAC_dev_i_counter_get_quantity00);
   tolua_function(tolua_S,"get_flow",tolua_PAC_dev_i_counter_get_flow00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_counter_get_state00);
   tolua_function(tolua_S,"get_abs_quantity",tolua_PAC_dev_i_counter_get_abs_quantity00);
   tolua_function(tolua_S,"abs_reset",tolua_PAC_dev_i_counter_abs_reset00);
   tolua_constant(tolua_S,"DAY_T1",i_counter::DAY_T1);
   tolua_constant(tolua_S,"DAY_T2",i_counter::DAY_T2);
   tolua_function(tolua_S,"pause_daily",tolua_PAC_dev_i_counter_pause_daily00);
   tolua_function(tolua_S,"start_daily",tolua_PAC_dev_i_counter_start_daily00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"device","device","i_DO_AO_device",NULL);
  tolua_beginmodule(tolua_S,"device");
   tolua_function(tolua_S,"off",tolua_PAC_dev_device_off00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_device_on00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_device_set_state00);
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_device_get_value00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_device_set_value00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_device_set_cmd00);
   tolua_function(tolua_S,"set_par",tolua_PAC_dev_device_set_par00);
   tolua_function(tolua_S,"set_rt_par",tolua_PAC_dev_device_set_rt_par00);
   tolua_function(tolua_S,"set_property",tolua_PAC_dev_device_set_property00);
   tolua_function(tolua_S,"set_string_property",tolua_PAC_dev_device_set_string_property00);
   tolua_function(tolua_S,"set_descr",tolua_PAC_dev_device_set_descr00);
   tolua_function(tolua_S,"get_type_str",tolua_PAC_dev_device_get_type_str00);
   tolua_function(tolua_S,"param_emulator",tolua_PAC_dev_device_param_emulator00);
   tolua_constant(tolua_S,"DT_NONE",device::DT_NONE);
   tolua_constant(tolua_S,"DT_V",device::DT_V);
   tolua_constant(tolua_S,"DT_VC",device::DT_VC);
   tolua_constant(tolua_S,"DT_M",device::DT_M);
   tolua_constant(tolua_S,"DT_LS",device::DT_LS);
   tolua_constant(tolua_S,"DT_TE",device::DT_TE);
   tolua_constant(tolua_S,"DT_FS",device::DT_FS);
   tolua_constant(tolua_S,"DT_GS",device::DT_GS);
   tolua_constant(tolua_S,"DT_FQT",device::DT_FQT);
   tolua_constant(tolua_S,"DT_LT",device::DT_LT);
   tolua_constant(tolua_S,"DT_QT",device::DT_QT);
   tolua_constant(tolua_S,"DT_HA",device::DT_HA);
   tolua_constant(tolua_S,"DT_HL",device::DT_HL);
   tolua_constant(tolua_S,"DT_SB",device::DT_SB);
   tolua_constant(tolua_S,"DT_DI",device::DT_DI);
   tolua_constant(tolua_S,"DT_DO",device::DT_DO);
   tolua_constant(tolua_S,"DT_AI",device::DT_AI);
   tolua_constant(tolua_S,"DT_AO",device::DT_AO);
   tolua_constant(tolua_S,"DT_WT",device::DT_WT);
   tolua_constant(tolua_S,"DT_PT",device::DT_PT);
   tolua_constant(tolua_S,"DT_F",device::DT_F);
   tolua_constant(tolua_S,"DT_REGULATOR",device::DT_REGULATOR);
   tolua_constant(tolua_S,"DT_HLA",device::DT_HLA);
   tolua_constant(tolua_S,"DT_CAM",device::DT_CAM);
   tolua_constant(tolua_S,"DT_PDS",device::DT_PDS);
   tolua_constant(tolua_S,"DT_TS",device::DT_TS);
   tolua_constant(tolua_S,"DT_G",device::DT_G);
   tolua_constant(tolua_S,"DT_WATCHDOG",device::DT_WATCHDOG);
   tolua_constant(tolua_S,"DT_EY",device::DT_EY);
   tolua_constant(tolua_S,"DT_NODE",device::DT_NODE);
   tolua_constant(tolua_S,"DST_NONE",device::DST_NONE);
   tolua_constant(tolua_S,"DST_V_DO1",device::DST_V_DO1);
   tolua_constant(tolua_S,"DST_V_DO2",device::DST_V_DO2);
   tolua_constant(tolua_S,"DST_V_DO1_DI1_FB_OFF",device::DST_V_DO1_DI1_FB_OFF);
   tolua_constant(tolua_S,"DST_V_DO1_DI1_FB_ON",device::DST_V_DO1_DI1_FB_ON);
   tolua_constant(tolua_S,"DST_V_DO1_DI2",device::DST_V_DO1_DI2);
   tolua_constant(tolua_S,"DST_V_DO2_DI2",device::DST_V_DO2_DI2);
   tolua_constant(tolua_S,"DST_V_MIXPROOF",device::DST_V_MIXPROOF);
   tolua_constant(tolua_S,"DST_V_AS_MIXPROOF",device::DST_V_AS_MIXPROOF);
   tolua_constant(tolua_S,"DST_V_BOTTOM_MIXPROOF",device::DST_V_BOTTOM_MIXPROOF);
   tolua_constant(tolua_S,"DST_V_AS_DO1_DI2",device::DST_V_AS_DO1_DI2);
   tolua_constant(tolua_S,"V_DO2_DI2_BISTABLE",device::V_DO2_DI2_BISTABLE);
   tolua_constant(tolua_S,"V_IOLINK_VTUG_DO1",device::V_IOLINK_VTUG_DO1);
   tolua_constant(tolua_S,"V_IOLINK_VTUG_DO1_FB_OFF",device::V_IOLINK_VTUG_DO1_FB_OFF);
   tolua_constant(tolua_S,"V_IOLINK_VTUG_DO1_FB_ON",device::V_IOLINK_VTUG_DO1_FB_ON);
   tolua_constant(tolua_S,"V_IOLINK_MIXPROOF",device::V_IOLINK_MIXPROOF);
   tolua_constant(tolua_S,"V_IOLINK_DO1_DI2",device::V_IOLINK_DO1_DI2);
   tolua_constant(tolua_S,"V_IOLINK_VTUG_DO1_DI2",device::V_IOLINK_VTUG_DO1_DI2);
   tolua_constant(tolua_S,"DST_V_VIRT",device::DST_V_VIRT);
   tolua_constant(tolua_S,"DST_V_MINI_FLUSHING",device::DST_V_MINI_FLUSHING);
   tolua_constant(tolua_S,"V_IOL_TERMINAL_MIXPROOF_DO3",device::V_IOL_TERMINAL_MIXPROOF_DO3);
   tolua_constant(tolua_S,"V_IOL_TERMINAL_MIXPROOF_DO3_DI2",device::V_IOL_TERMINAL_MIXPROOF_DO3_DI2);
   tolua_constant(tolua_S,"DST_VC",device::DST_VC);
   tolua_constant(tolua_S,"DST_VC_IOLINK",device::DST_VC_IOLINK);
   tolua_constant(tolua_S,"DST_VC_VIRT",device::DST_VC_VIRT);
   tolua_constant(tolua_S,"DST_LS_MIN",device::DST_LS_MIN);
   tolua_constant(tolua_S,"DST_LS_MAX",device::DST_LS_MAX);
   tolua_constant(tolua_S,"LS_IOLINK_MIN",device::LS_IOLINK_MIN);
   tolua_constant(tolua_S,"LS_IOLINK_MAX",device::LS_IOLINK_MAX);
   tolua_constant(tolua_S,"DST_LS_VIRT",device::DST_LS_VIRT);
   tolua_constant(tolua_S,"DST_TE",device::DST_TE);
   tolua_constant(tolua_S,"DST_TE_IOLINK",device::DST_TE_IOLINK);
   tolua_constant(tolua_S,"DST_TE_VIRT",device::DST_TE_VIRT);
   tolua_constant(tolua_S,"DST_TE_ANALOG",device::DST_TE_ANALOG);
   tolua_constant(tolua_S,"DST_M",device::DST_M);
   tolua_constant(tolua_S,"DST_M_FREQ",device::DST_M_FREQ);
   tolua_constant(tolua_S,"DST_M_REV",device::DST_M_REV);
   tolua_constant(tolua_S,"DST_M_REV_FREQ",device::DST_M_REV_FREQ);
   tolua_constant(tolua_S,"DST_M_REV_2",device::DST_M_REV_2);
   tolua_constant(tolua_S,"DST_M_REV_FREQ_2",device::DST_M_REV_FREQ_2);
   tolua_constant(tolua_S,"M_REV_2_ERROR",device::M_REV_2_ERROR);
   tolua_constant(tolua_S,"DST_M_REV_FREQ_2_ERROR",device::DST_M_REV_FREQ_2_ERROR);
   tolua_constant(tolua_S,"M_ATV",device::M_ATV);
   tolua_constant(tolua_S,"DST_M_VIRT",device::DST_M_VIRT);
   tolua_constant(tolua_S,"M_ATV_LINEAR",device::M_ATV_LINEAR);
   tolua_constant(tolua_S,"DST_FQT",device::DST_FQT);
   tolua_constant(tolua_S,"DST_FQT_F",device::DST_FQT_F);
   tolua_constant(tolua_S,"DST_FQT_VIRT",device::DST_FQT_VIRT);
   tolua_constant(tolua_S,"DST_FQT_IOLINK",device::DST_FQT_IOLINK);
   tolua_constant(tolua_S,"DST_QT",device::DST_QT);
   tolua_constant(tolua_S,"DST_QT_OK",device::DST_QT_OK);
   tolua_constant(tolua_S,"DST_QT_IOLINK",device::DST_QT_IOLINK);
   tolua_constant(tolua_S,"DST_QT_VIRT",device::DST_QT_VIRT);
   tolua_constant(tolua_S,"DST_LT",device::DST_LT);
   tolua_constant(tolua_S,"DST_LT_CYL",device::DST_LT_CYL);
   tolua_constant(tolua_S,"DST_LT_CONE",device::DST_LT_CONE);
   tolua_constant(tolua_S,"DST_LT_TRUNC",device::DST_LT_TRUNC);
   tolua_constant(tolua_S,"DST_LT_IOLINK",device::DST_LT_IOLINK);
   tolua_constant(tolua_S,"DST_LT_VIRT",device::DST_LT_VIRT);
   tolua_constant(tolua_S,"DST_LT_CYL_HOR",device::DST_LT_CYL_HOR);
   tolua_constant(tolua_S,"DST_DO",device::DST_DO);
   tolua_constant(tolua_S,"DST_DO_VIRT",device::DST_DO_VIRT);
   tolua_constant(tolua_S,"DST_DI",device::DST_DI);
   tolua_constant(tolua_S,"DST_DI_VIRT",device::DST_DI_VIRT);
   tolua_constant(tolua_S,"DST_AO",device::DST_AO);
   tolua_constant(tolua_S,"DST_AO_VIRT",device::DST_AO_VIRT);
   tolua_constant(tolua_S,"DST_AI",device::DST_AI);
   tolua_constant(tolua_S,"DST_AI_VIRT",device::DST_AI_VIRT);
   tolua_constant(tolua_S,"DST_PT",device::DST_PT);
   tolua_constant(tolua_S,"DST_PT_IOLINK",device::DST_PT_IOLINK);
   tolua_constant(tolua_S,"DST_PT_VIRT",device::DST_PT_VIRT);
   tolua_constant(tolua_S,"DST_F",device::DST_F);
   tolua_constant(tolua_S,"DST_F_VIRT",device::DST_F_VIRT);
   tolua_constant(tolua_S,"DST_HLA",device::DST_HLA);
   tolua_constant(tolua_S,"DST_HLA_VIRT",device::DST_HLA_VIRT);
   tolua_constant(tolua_S,"DST_HLA_IOLINK",device::DST_HLA_IOLINK);
   tolua_constant(tolua_S,"DST_GS",device::DST_GS);
   tolua_constant(tolua_S,"DST_GS_VIRT",device::DST_GS_VIRT);
   tolua_constant(tolua_S,"DST_GS_INVERSE",device::DST_GS_INVERSE);
   tolua_constant(tolua_S,"DST_HA",device::DST_HA);
   tolua_constant(tolua_S,"DST_HA_VIRT",device::DST_HA_VIRT);
   tolua_constant(tolua_S,"DST_HL",device::DST_HL);
   tolua_constant(tolua_S,"DST_HL_VIRT",device::DST_HL_VIRT);
   tolua_constant(tolua_S,"DST_FS",device::DST_FS);
   tolua_constant(tolua_S,"DST_FS_VIRT",device::DST_FS_VIRT);
   tolua_constant(tolua_S,"DST_SB",device::DST_SB);
   tolua_constant(tolua_S,"DST_SB_VIRT",device::DST_SB_VIRT);
   tolua_constant(tolua_S,"DST_WT",device::DST_WT);
   tolua_constant(tolua_S,"DST_WT_VIRT",device::DST_WT_VIRT);
   tolua_constant(tolua_S,"DST_WT_RS232",device::DST_WT_RS232);
   tolua_constant(tolua_S,"DST_WT_ETH",device::DST_WT_ETH);
   tolua_constant(tolua_S,"DST_WT_PXC_AXL",device::DST_WT_PXC_AXL);
   tolua_constant(tolua_S,"DST_CAM_DO1_DI2",device::DST_CAM_DO1_DI2);
   tolua_constant(tolua_S,"DST_CAM_DO1_DI1",device::DST_CAM_DO1_DI1);
   tolua_constant(tolua_S,"DST_CAM_DO1_DI3",device::DST_CAM_DO1_DI3);
   tolua_constant(tolua_S,"DST_PDS",device::DST_PDS);
   tolua_constant(tolua_S,"DST_PDS_VIRT",device::DST_PDS_VIRT);
   tolua_constant(tolua_S,"DST_TS",device::DST_TS);
   tolua_constant(tolua_S,"DST_TS_VIRT",device::DST_TS_VIRT);
   tolua_constant(tolua_S,"DST_REGULATOR_PID",device::DST_REGULATOR_PID);
   tolua_constant(tolua_S,"DST_REGULATOR_THLD",device::DST_REGULATOR_THLD);
   tolua_constant(tolua_S,"DST_G_IOL_4",device::DST_G_IOL_4);
   tolua_constant(tolua_S,"DST_G_IOL_8",device::DST_G_IOL_8);
   tolua_constant(tolua_S,"DST_WATCHDOG",device::DST_WATCHDOG);
   tolua_constant(tolua_S,"DST_CONV_AO2",device::DST_CONV_AO2);
   tolua_constant(tolua_S,"DST_NODE",device::DST_NODE);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_wages","i_wages","",NULL);
  tolua_beginmodule(tolua_S,"i_wages");
   tolua_function(tolua_S,"tare",tolua_PAC_dev_i_wages_tare00);
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_i_wages_get_value00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_i_wages_get_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_motor","i_motor","device",NULL);
  tolua_beginmodule(tolua_S,"i_motor");
   tolua_function(tolua_S,"reverse",tolua_PAC_dev_i_motor_reverse00);
   tolua_function(tolua_S,"get_linear_speed",tolua_PAC_dev_i_motor_get_linear_speed00);
   tolua_function(tolua_S,"get_amperage",tolua_PAC_dev_i_motor_get_amperage00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"signal_column","signal_column","device",NULL);
  tolua_beginmodule(tolua_S,"signal_column");
   tolua_function(tolua_S,"off",tolua_PAC_dev_signal_column_off00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_signal_column_on00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_signal_column_set_state00);
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_signal_column_get_value00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_signal_column_set_value00);
   tolua_function(tolua_S,"turn_off_red",tolua_PAC_dev_signal_column_turn_off_red00);
   tolua_function(tolua_S,"turn_off_yellow",tolua_PAC_dev_signal_column_turn_off_yellow00);
   tolua_function(tolua_S,"turn_off_green",tolua_PAC_dev_signal_column_turn_off_green00);
   tolua_function(tolua_S,"turn_off_blue",tolua_PAC_dev_signal_column_turn_off_blue00);
   tolua_function(tolua_S,"turn_on_red",tolua_PAC_dev_signal_column_turn_on_red00);
   tolua_function(tolua_S,"turn_on_yellow",tolua_PAC_dev_signal_column_turn_on_yellow00);
   tolua_function(tolua_S,"turn_on_green",tolua_PAC_dev_signal_column_turn_on_green00);
   tolua_function(tolua_S,"turn_on_blue",tolua_PAC_dev_signal_column_turn_on_blue00);
   tolua_function(tolua_S,"normal_blink_red",tolua_PAC_dev_signal_column_normal_blink_red00);
   tolua_function(tolua_S,"normal_blink_yellow",tolua_PAC_dev_signal_column_normal_blink_yellow00);
   tolua_function(tolua_S,"normal_blink_green",tolua_PAC_dev_signal_column_normal_blink_green00);
   tolua_function(tolua_S,"normal_blink_blue",tolua_PAC_dev_signal_column_normal_blink_blue00);
   tolua_function(tolua_S,"slow_blink_red",tolua_PAC_dev_signal_column_slow_blink_red00);
   tolua_function(tolua_S,"slow_blink_yellow",tolua_PAC_dev_signal_column_slow_blink_yellow00);
   tolua_function(tolua_S,"slow_blink_green",tolua_PAC_dev_signal_column_slow_blink_green00);
   tolua_function(tolua_S,"slow_blink_blue",tolua_PAC_dev_signal_column_slow_blink_blue00);
   tolua_function(tolua_S,"turn_on_siren",tolua_PAC_dev_signal_column_turn_on_siren00);
   tolua_function(tolua_S,"turn_off_siren",tolua_PAC_dev_signal_column_turn_off_siren00);
   tolua_constant(tolua_S,"TURN_OFF",signal_column::TURN_OFF);
   tolua_constant(tolua_S,"TURN_ON",signal_column::TURN_ON);
   tolua_constant(tolua_S,"LIGHTS_OFF",signal_column::LIGHTS_OFF);
   tolua_constant(tolua_S,"GREEN_ON",signal_column::GREEN_ON);
   tolua_constant(tolua_S,"YELLOW_ON",signal_column::YELLOW_ON);
   tolua_constant(tolua_S,"RED_ON",signal_column::RED_ON);
   tolua_constant(tolua_S,"GREEN_OFF",signal_column::GREEN_OFF);
   tolua_constant(tolua_S,"YELLOW_OFF",signal_column::YELLOW_OFF);
   tolua_constant(tolua_S,"RED_OFF",signal_column::RED_OFF);
   tolua_constant(tolua_S,"GREEN_NORMAL_BLINK",signal_column::GREEN_NORMAL_BLINK);
   tolua_constant(tolua_S,"YELLOW_NORMAL_BLINK",signal_column::YELLOW_NORMAL_BLINK);
   tolua_constant(tolua_S,"RED_NORMAL_BLINK",signal_column::RED_NORMAL_BLINK);
   tolua_constant(tolua_S,"GREEN_SLOW_BLINK",signal_column::GREEN_SLOW_BLINK);
   tolua_constant(tolua_S,"YELLOW_SLOW_BLINK",signal_column::YELLOW_SLOW_BLINK);
   tolua_constant(tolua_S,"RED_SLOW_BLINK",signal_column::RED_SLOW_BLINK);
   tolua_constant(tolua_S,"SIREN_ON",signal_column::SIREN_ON);
   tolua_constant(tolua_S,"SIREN_OFF",signal_column::SIREN_OFF);
   tolua_function(tolua_S,"show_error_exists",tolua_PAC_dev_signal_column_show_error_exists00);
   tolua_function(tolua_S,"show_message_exists",tolua_PAC_dev_signal_column_show_message_exists00);
   tolua_function(tolua_S,"show_batch_is_not_running",tolua_PAC_dev_signal_column_show_batch_is_not_running00);
   tolua_function(tolua_S,"show_batch_is_running",tolua_PAC_dev_signal_column_show_batch_is_running00);
   tolua_function(tolua_S,"show_operation_is_not_running",tolua_PAC_dev_signal_column_show_operation_is_not_running00);
   tolua_function(tolua_S,"show_operation_is_running",tolua_PAC_dev_signal_column_show_operation_is_running00);
   tolua_function(tolua_S,"show_idle",tolua_PAC_dev_signal_column_show_idle00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"node_dev","node_dev","device",NULL);
  tolua_beginmodule(tolua_S,"node_dev");
   tolua_function(tolua_S,"set_io_node",tolua_PAC_dev_node_dev_set_io_node00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"V",tolua_PAC_dev_V00);
  tolua_function(tolua_S,"VC",tolua_PAC_dev_VC00);
  tolua_function(tolua_S,"M",tolua_PAC_dev_M00);
  tolua_function(tolua_S,"LS",tolua_PAC_dev_LS00);
  tolua_function(tolua_S,"FS",tolua_PAC_dev_FS00);
  tolua_function(tolua_S,"AI",tolua_PAC_dev_AI00);
  tolua_function(tolua_S,"AO",tolua_PAC_dev_AO00);
  tolua_function(tolua_S,"FQT",tolua_PAC_dev_FQT00);
  tolua_function(tolua_S,"virtual_FQT",tolua_PAC_dev_virtual_FQT00);
  tolua_function(tolua_S,"FQT_IOLINK",tolua_PAC_dev_FQT_IOLINK00);
  tolua_function(tolua_S,"TE",tolua_PAC_dev_TE00);
  tolua_function(tolua_S,"LT",tolua_PAC_dev_LT00);
  tolua_function(tolua_S,"GS",tolua_PAC_dev_GS00);
  tolua_function(tolua_S,"HA",tolua_PAC_dev_HA00);
  tolua_function(tolua_S,"HL",tolua_PAC_dev_HL00);
  tolua_function(tolua_S,"HLA",tolua_PAC_dev_HLA00);
  tolua_function(tolua_S,"SB",tolua_PAC_dev_SB00);
  tolua_function(tolua_S,"DI",tolua_PAC_dev_DI00);
  tolua_function(tolua_S,"DO",tolua_PAC_dev_DO00);
  tolua_function(tolua_S,"QT",tolua_PAC_dev_QT00);
  tolua_function(tolua_S,"WT",tolua_PAC_dev_WT00);
  tolua_function(tolua_S,"PT",tolua_PAC_dev_PT00);
  tolua_function(tolua_S,"F",tolua_PAC_dev_F00);
  tolua_function(tolua_S,"C",tolua_PAC_dev_C00);
  tolua_function(tolua_S,"CAM",tolua_PAC_dev_CAM00);
  tolua_function(tolua_S,"PDS",tolua_PAC_dev_PDS00);
  tolua_function(tolua_S,"TS",tolua_PAC_dev_TS00);
  tolua_function(tolua_S,"G",tolua_PAC_dev_G00);
  tolua_function(tolua_S,"WATCHDOG",tolua_PAC_dev_WATCHDOG00);
  tolua_function(tolua_S,"EY",tolua_PAC_dev_EY00);
  tolua_function(tolua_S,"STUB",tolua_PAC_dev_STUB00);
  tolua_function(tolua_S,"DEVICE",tolua_PAC_dev_DEVICE00);
  tolua_cclass(tolua_S,"dev_stub","dev_stub","i_counter",NULL);
  tolua_beginmodule(tolua_S,"dev_stub");
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_dev_stub_get_value00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_dev_stub_set_value00);
   tolua_function(tolua_S,"is_active",tolua_PAC_dev_dev_stub_is_active00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_dev_stub_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_dev_stub_off00);
   tolua_function(tolua_S,"instant_off",tolua_PAC_dev_dev_stub_instant_off00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_dev_stub_set_state00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_dev_stub_get_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"errors_manager","errors_manager","",NULL);
  tolua_beginmodule(tolua_S,"errors_manager");
   tolua_function(tolua_S,"get_instance",tolua_PAC_dev_errors_manager_get_instance00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_errors_manager_set_cmd00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"dev_errors_manager","dev_errors_manager","",NULL);
  tolua_beginmodule(tolua_S,"dev_errors_manager");
   tolua_function(tolua_S,"get_instance",tolua_PAC_dev_dev_errors_manager_get_instance00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_dev_errors_manager_set_cmd00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"valve","valve","",NULL);
  tolua_beginmodule(tolua_S,"valve");
   tolua_function(tolua_S,"is_opened",tolua_PAC_dev_valve_is_opened00);
   tolua_function(tolua_S,"is_closed",tolua_PAC_dev_valve_is_closed00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_valve_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_valve_off00);
   tolua_function(tolua_S,"direct_off",tolua_PAC_dev_valve_direct_off00);
   tolua_function(tolua_S,"instant_off",tolua_PAC_dev_valve_instant_off00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_valve_set_state00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_valve_get_state00);
   tolua_function(tolua_S,"get_on_fb_value",tolua_PAC_dev_valve_get_on_fb_value00);
   tolua_function(tolua_S,"get_off_fb_value",tolua_PAC_dev_valve_get_off_fb_value00);
   tolua_constant(tolua_S,"V_LOWER_SEAT",valve::V_LOWER_SEAT);
   tolua_constant(tolua_S,"V_UPPER_SEAT",valve::V_UPPER_SEAT);
   tolua_constant(tolua_S,"V_ON",valve::V_ON);
   tolua_constant(tolua_S,"V_OFF",valve::V_OFF);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"level","level","i_AI_device",NULL);
  tolua_beginmodule(tolua_S,"level");
   tolua_function(tolua_S,"get_volume",tolua_PAC_dev_level_get_volume00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"virtual_counter","virtual_counter","device",NULL);
  tolua_beginmodule(tolua_S,"virtual_counter");
   tolua_function(tolua_S,"reset",tolua_PAC_dev_virtual_counter_reset00);
   tolua_function(tolua_S,"get_quantity",tolua_PAC_dev_virtual_counter_get_quantity00);
   tolua_function(tolua_S,"get_flow",tolua_PAC_dev_virtual_counter_get_flow00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_virtual_counter_get_state00);
   tolua_function(tolua_S,"get_abs_quantity",tolua_PAC_dev_virtual_counter_get_abs_quantity00);
   tolua_function(tolua_S,"abs_reset",tolua_PAC_dev_virtual_counter_abs_reset00);
   tolua_function(tolua_S,"set",tolua_PAC_dev_virtual_counter_set00);
   tolua_function(tolua_S,"eval",tolua_PAC_dev_virtual_counter_eval00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"counter_iolink","counter_iolink","device",NULL);
  tolua_beginmodule(tolua_S,"counter_iolink");
   tolua_function(tolua_S,"get_temperature",tolua_PAC_dev_counter_iolink_get_temperature00);
   tolua_function(tolua_S,"get_conductivity",tolua_PAC_dev_counter_iolink_get_conductivity00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"tech_object","tech_object","",tolua_collect_tech_object);
  #else
  tolua_cclass(tolua_S,"tech_object","tech_object","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"tech_object");
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_tech_object_set_cmd00);
   tolua_function(tolua_S,"new",tolua_PAC_dev_tech_object_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_tech_object_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_tech_object_new00_local);
   tolua_function(tolua_S,"delete",tolua_PAC_dev_tech_object_delete00);
   tolua_function(tolua_S,"set_mode",tolua_PAC_dev_tech_object_set_mode00);
   tolua_function(tolua_S,"get_mode",tolua_PAC_dev_tech_object_get_mode00);
   tolua_function(tolua_S,"get_operation_state",tolua_PAC_dev_tech_object_get_operation_state00);
   tolua_function(tolua_S,"exec_cmd",tolua_PAC_dev_tech_object_exec_cmd00);
   tolua_function(tolua_S,"get_modes_count",tolua_PAC_dev_tech_object_get_modes_count00);
   tolua_function(tolua_S,"check_operation_on",tolua_PAC_dev_tech_object_check_operation_on00);
   tolua_variable(tolua_S,"par_float",tolua_get_tech_object_par_float,tolua_set_tech_object_par_float);
   tolua_variable(tolua_S,"rt_par_float",tolua_get_tech_object_rt_par_float,tolua_set_tech_object_rt_par_float);
   tolua_variable(tolua_S,"par_uint",tolua_get_tech_object_par_uint,tolua_set_tech_object_par_uint);
   tolua_variable(tolua_S,"rt_par_uint",tolua_get_tech_object_rt_par_uint,tolua_set_tech_object_rt_par_uint);
   tolua_variable(tolua_S,"timers",tolua_get_tech_object_timers,tolua_set_tech_object_timers);
   tolua_function(tolua_S,"get_modes_manager",tolua_PAC_dev_tech_object_get_modes_manager00);
   tolua_function(tolua_S,"is_idle",tolua_PAC_dev_tech_object_is_idle00);
   tolua_function(tolua_S,"get_number",tolua_PAC_dev_tech_object_get_number00);
   tolua_constant(tolua_S,"ERR_CANT_ON",tech_object::ERR_CANT_ON);
   tolua_constant(tolua_S,"ERR_ON_WITH_ERRORS",tech_object::ERR_ON_WITH_ERRORS);
   tolua_constant(tolua_S,"ERR_OFF",tech_object::ERR_OFF);
   tolua_constant(tolua_S,"ERR_OFF_AND_ON",tech_object::ERR_OFF_AND_ON);
   tolua_constant(tolua_S,"ERR_DURING_WORK",tech_object::ERR_DURING_WORK);
   tolua_constant(tolua_S,"ERR_ALARM",tech_object::ERR_ALARM);
   tolua_constant(tolua_S,"ERR_TO_FAIL_STATE",tech_object::ERR_TO_FAIL_STATE);
   tolua_constant(tolua_S,"ERR_CANT_ON_2_OPER",tech_object::ERR_CANT_ON_2_OPER);
   tolua_constant(tolua_S,"ERR_CANT_ON_2_OBJ",tech_object::ERR_CANT_ON_2_OBJ);
   tolua_function(tolua_S,"set_err_msg",tolua_PAC_dev_tech_object_set_err_msg00);
   tolua_function(tolua_S,"is_any_message",tolua_PAC_dev_tech_object_is_any_message00);
   tolua_function(tolua_S,"is_any_error",tolua_PAC_dev_tech_object_is_any_error00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_TECH_OBJECT_MNGR",tolua_PAC_dev_G_TECH_OBJECT_MNGR00);
  tolua_cclass(tolua_S,"tech_object_manager","tech_object_manager","",NULL);
  tolua_beginmodule(tolua_S,"tech_object_manager");
   tolua_function(tolua_S,"get_object_with_active_mode",tolua_PAC_dev_tech_object_manager_get_object_with_active_mode00);
   tolua_function(tolua_S,"get_tech_objects",tolua_PAC_dev_tech_object_manager_get_tech_objects00);
   tolua_function(tolua_S,"print",tolua_PAC_dev_tech_object_manager_print00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"operation","operation","",NULL);
  tolua_beginmodule(tolua_S,"operation");
   tolua_constant(tolua_S,"IDLE",operation::IDLE);
   tolua_constant(tolua_S,"RUN",operation::RUN);
   tolua_constant(tolua_S,"PAUSE",operation::PAUSE);
   tolua_constant(tolua_S,"STOP",operation::STOP);
   tolua_function(tolua_S,"evaluation_time",tolua_PAC_dev_operation_evaluation_time00);
   tolua_function(tolua_S,"active_step",tolua_PAC_dev_operation_active_step00);
   tolua_function(tolua_S,"active_step_evaluation_time",tolua_PAC_dev_operation_active_step_evaluation_time00);
   tolua_function(tolua_S,"get_active_step_set_time",tolua_PAC_dev_operation_get_active_step_set_time00);
   tolua_function(tolua_S,"to_step",tolua_PAC_dev_operation_to_step00);
   tolua_function(tolua_S,"to_next_step",tolua_PAC_dev_operation_to_next_step00);
   tolua_function(tolua_S,"turn_off_active_step",tolua_PAC_dev_operation_turn_off_active_step00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_operation__geti00);
   tolua_function(tolua_S,"on_extra_step",tolua_PAC_dev_operation_on_extra_step00);
   tolua_function(tolua_S,"off_extra_step",tolua_PAC_dev_operation_off_extra_step00);
   tolua_function(tolua_S,"switch_active_extra_step",tolua_PAC_dev_operation_switch_active_extra_step00);
   tolua_function(tolua_S,"is_active_extra_step",tolua_PAC_dev_operation_is_active_extra_step00);
   tolua_function(tolua_S,"add_step",tolua_PAC_dev_operation_add_step00);
   tolua_function(tolua_S,"get_name",tolua_PAC_dev_operation_get_name00);
   tolua_function(tolua_S,"set_step_cooperate_time_par_n",tolua_PAC_dev_operation_set_step_cooperate_time_par_n00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_operation_get_state00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"operation_manager","operation_manager","",NULL);
  tolua_beginmodule(tolua_S,"operation_manager");
   tolua_function(tolua_S,"add_mode",tolua_PAC_dev_operation_manager_add_mode00);
   tolua_function(tolua_S,"add_operation",tolua_PAC_dev_operation_manager_add_operation00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_operation_manager__geti00);
   tolua_function(tolua_S,"get_idle_time_sec",tolua_PAC_dev_operation_manager_get_idle_time_sec00);
   tolua_function(tolua_S,"get_idle_time",tolua_PAC_dev_operation_manager_get_idle_time00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"operation_state","operation_state","",NULL);
  tolua_beginmodule(tolua_S,"operation_state");
   tolua_function(tolua_S,".geti",tolua_PAC_dev_operation_state__geti00);
   tolua_function(tolua_S,"steps_count",tolua_PAC_dev_operation_state_steps_count00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"step","step","",NULL);
  tolua_beginmodule(tolua_S,"step");
   tolua_function(tolua_S,".geti",tolua_PAC_dev_step__geti00);
   tolua_function(tolua_S,"get_name",tolua_PAC_dev_step_get_name00);
   tolua_constant(tolua_S,"A_CHECKED_DEVICES",step::A_CHECKED_DEVICES);
   tolua_constant(tolua_S,"A_ON",step::A_ON);
   tolua_constant(tolua_S,"A_ON_REVERSE",step::A_ON_REVERSE);
   tolua_constant(tolua_S,"A_OFF",step::A_OFF);
   tolua_constant(tolua_S,"A_UPPER_SEATS_ON",step::A_UPPER_SEATS_ON);
   tolua_constant(tolua_S,"A_LOWER_SEATS_ON",step::A_LOWER_SEATS_ON);
   tolua_constant(tolua_S,"A_REQUIRED_FB",step::A_REQUIRED_FB);
   tolua_constant(tolua_S,"A_DI_DO",step::A_DI_DO);
   tolua_constant(tolua_S,"A_INVERTED_DI_DO",step::A_INVERTED_DI_DO);
   tolua_constant(tolua_S,"A_AI_AO",step::A_AI_AO);
   tolua_constant(tolua_S,"A_WASH",step::A_WASH);
   tolua_constant(tolua_S,"A_ENABLE_STEP_BY_SIGNAL",step::A_ENABLE_STEP_BY_SIGNAL);
   tolua_constant(tolua_S,"A_DELAY_ON",step::A_DELAY_ON);
   tolua_constant(tolua_S,"A_DELAY_OFF",step::A_DELAY_OFF);
   tolua_constant(tolua_S,"A_JUMP_IF",step::A_JUMP_IF);
   tolua_function(tolua_S,"is_active",tolua_PAC_dev_step_is_active00);
   tolua_function(tolua_S,"get_tag",tolua_PAC_dev_step_get_tag00);
   tolua_function(tolua_S,"set_tag",tolua_PAC_dev_step_set_tag00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"action","action","",NULL);
  tolua_beginmodule(tolua_S,"action");
   tolua_function(tolua_S,"add_dev",tolua_PAC_dev_action_add_dev00);
   tolua_function(tolua_S,"set_param_idx",tolua_PAC_dev_action_set_param_idx00);
   tolua_function(tolua_S,"set_bool_property",tolua_PAC_dev_action_set_bool_property00);
   tolua_function(tolua_S,"set_int_property",tolua_PAC_dev_action_set_int_property00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_DEVICE_MANAGER",tolua_PAC_dev_G_DEVICE_MANAGER00);
  tolua_function(tolua_S,"G_TECH_OBJECTS",tolua_PAC_dev_G_TECH_OBJECTS00);
  tolua_cclass(tolua_S,"device_manager","device_manager","",NULL);
  tolua_beginmodule(tolua_S,"device_manager");
   tolua_function(tolua_S,"print",tolua_PAC_dev_device_manager_print00);
   tolua_function(tolua_S,"add_io_device",tolua_PAC_dev_device_manager_add_io_device00);
   tolua_function(tolua_S,"get_device",tolua_PAC_dev_device_manager_get_device00);
   tolua_function(tolua_S,"get_device",tolua_PAC_dev_device_manager_get_device01);
   tolua_function(tolua_S,"get_node",tolua_PAC_dev_device_manager_get_node00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"io_device","io_device","",NULL);
  tolua_beginmodule(tolua_S,"io_device");
   tolua_constant(tolua_S,"WAGO",io_device::WAGO);
   tolua_constant(tolua_S,"PHOENIX",io_device::PHOENIX);
   tolua_function(tolua_S,"init",tolua_PAC_dev_io_device_init00);
   tolua_function(tolua_S,"init_channel",tolua_PAC_dev_io_device_init_channel00);
   tolua_function(tolua_S,"set_io_vendor",tolua_PAC_dev_io_device_set_io_vendor00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"timer_manager","timer_manager","",NULL);
  tolua_beginmodule(tolua_S,"timer_manager");
   tolua_function(tolua_S,".geti",tolua_PAC_dev_timer_manager__geti00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"timer","timer","",tolua_collect_timer);
  #else
  tolua_cclass(tolua_S,"timer","timer","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"timer");
   tolua_constant(tolua_S,"S_STOP",timer::S_STOP);
   tolua_constant(tolua_S,"S_WORK",timer::S_WORK);
   tolua_constant(tolua_S,"S_PAUSE",timer::S_PAUSE);
   tolua_function(tolua_S,"start",tolua_PAC_dev_timer_start00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_timer_reset00);
   tolua_function(tolua_S,"pause",tolua_PAC_dev_timer_pause00);
   tolua_function(tolua_S,"is_time_up",tolua_PAC_dev_timer_is_time_up00);
   tolua_function(tolua_S,"get_work_time",tolua_PAC_dev_timer_get_work_time00);
   tolua_function(tolua_S,"set_countdown_time",tolua_PAC_dev_timer_set_countdown_time00);
   tolua_function(tolua_S,"get_countdown_time",tolua_PAC_dev_timer_get_countdown_time00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_timer_get_state00);
   tolua_function(tolua_S,"new",tolua_PAC_dev_timer_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_timer_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_timer_new00_local);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"saved_params_float","saved_params_float","",NULL);
  tolua_beginmodule(tolua_S,"saved_params_float");
   tolua_function(tolua_S,"save",tolua_PAC_dev_saved_params_float_save00);
   tolua_function(tolua_S,"save_all",tolua_PAC_dev_saved_params_float_save_all00);
   tolua_function(tolua_S,".seti",tolua_PAC_dev_saved_params_float__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_saved_params_float__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_saved_params_float_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"saved_params_u_int_4","saved_params_u_int_4","",NULL);
  tolua_beginmodule(tolua_S,"saved_params_u_int_4");
   tolua_function(tolua_S,"save",tolua_PAC_dev_saved_params_u_int_4_save00);
   tolua_function(tolua_S,"save_all",tolua_PAC_dev_saved_params_u_int_4_save_all00);
   tolua_function(tolua_S,".seti",tolua_PAC_dev_saved_params_u_int_4__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_saved_params_u_int_4__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_saved_params_u_int_4_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"run_time_params_u_int_4","run_time_params_u_int_4","",NULL);
  tolua_beginmodule(tolua_S,"run_time_params_u_int_4");
   tolua_function(tolua_S,".seti",tolua_PAC_dev_run_time_params_u_int_4__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_run_time_params_u_int_4__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_run_time_params_u_int_4_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"run_time_params_float","run_time_params_float","",NULL);
  tolua_beginmodule(tolua_S,"run_time_params_float");
   tolua_function(tolua_S,".seti",tolua_PAC_dev_run_time_params_float__seti00);
   tolua_function(tolua_S,".geti",tolua_PAC_dev_run_time_params_float__geti00);
   tolua_function(tolua_S,"reset_to_0",tolua_PAC_dev_run_time_params_float_reset_to_000);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"io_manager","io_manager","",NULL);
  tolua_beginmodule(tolua_S,"io_manager");
   tolua_function(tolua_S,"init",tolua_PAC_dev_io_manager_init00);
   tolua_function(tolua_S,"add_node",tolua_PAC_dev_io_manager_add_node00);
   tolua_function(tolua_S,"init_node_AO",tolua_PAC_dev_io_manager_init_node_AO00);
   tolua_function(tolua_S,"init_node_AI",tolua_PAC_dev_io_manager_init_node_AI00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_IO_MANAGER",tolua_PAC_dev_G_IO_MANAGER00);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"PID","PID","",tolua_collect_PID);
  #else
  tolua_cclass(tolua_S,"PID","PID","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"PID");
   tolua_constant(tolua_S,"P_k",PID::P_k);
   tolua_constant(tolua_S,"P_Ti",PID::P_Ti);
   tolua_constant(tolua_S,"P_Td",PID::P_Td);
   tolua_constant(tolua_S,"P_dt",PID::P_dt);
   tolua_constant(tolua_S,"P_max",PID::P_max);
   tolua_constant(tolua_S,"P_min",PID::P_min);
   tolua_constant(tolua_S,"P_acceleration_time",PID::P_acceleration_time);
   tolua_constant(tolua_S,"P_is_manual_mode",PID::P_is_manual_mode);
   tolua_constant(tolua_S,"P_U_manual",PID::P_U_manual);
   tolua_constant(tolua_S,"P_k2",PID::P_k2);
   tolua_constant(tolua_S,"P_Ti2",PID::P_Ti2);
   tolua_constant(tolua_S,"P_Td2",PID::P_Td2);
   tolua_constant(tolua_S,"P_out_max",PID::P_out_max);
   tolua_constant(tolua_S,"P_out_min",PID::P_out_min);
   tolua_constant(tolua_S,"WP_Z",PID::WP_Z);
   tolua_constant(tolua_S,"WP_U",PID::WP_U);
   tolua_function(tolua_S,"new",tolua_PAC_dev_PID_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_PID_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_PID_new00_local);
   tolua_function(tolua_S,"on",tolua_PAC_dev_PID_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_PID_off00);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_PID_reset00);
   tolua_function(tolua_S,"eval",tolua_PAC_dev_PID_eval00);
   tolua_function(tolua_S,"set",tolua_PAC_dev_PID_set00);
   tolua_function(tolua_S,"get_assignment",tolua_PAC_dev_PID_get_assignment00);
   tolua_function(tolua_S,"init_param",tolua_PAC_dev_PID_init_param00);
   tolua_function(tolua_S,"init_work_param",tolua_PAC_dev_PID_init_work_param00);
   tolua_function(tolua_S,"save_param",tolua_PAC_dev_PID_save_param00);
   tolua_function(tolua_S,"set_used_par",tolua_PAC_dev_PID_set_used_par00);
   tolua_function(tolua_S,"print",tolua_PAC_dev_PID_print00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_PID_get_state00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_PID_set_cmd00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"camera","camera","",NULL);
  tolua_beginmodule(tolua_S,"camera");
   tolua_function(tolua_S,"off",tolua_PAC_dev_camera_off00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_camera_on00);
   tolua_function(tolua_S,"set_state",tolua_PAC_dev_camera_set_state00);
   tolua_function(tolua_S,"get_value",tolua_PAC_dev_camera_get_value00);
   tolua_function(tolua_S,"set_value",tolua_PAC_dev_camera_set_value00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_camera_get_state00);
   tolua_function(tolua_S,"get_result",tolua_PAC_dev_camera_get_result00);
   tolua_function(tolua_S,"is_ready",tolua_PAC_dev_camera_is_ready00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"PAC_info","PAC_info","i_Lua_save_device",NULL);
  tolua_beginmodule(tolua_S,"PAC_info");
   tolua_constant(tolua_S,"P_MIX_FLIP_PERIOD",PAC_info::P_MIX_FLIP_PERIOD);
   tolua_constant(tolua_S,"P_MIX_FLIP_UPPER_TIME",PAC_info::P_MIX_FLIP_UPPER_TIME);
   tolua_constant(tolua_S,"P_MIX_FLIP_LOWER_TIME",PAC_info::P_MIX_FLIP_LOWER_TIME);
   tolua_constant(tolua_S,"P_V_OFF_DELAY_TIME",PAC_info::P_V_OFF_DELAY_TIME);
   tolua_constant(tolua_S,"P_V_BOTTOM_OFF_DELAY_TIME",PAC_info::P_V_BOTTOM_OFF_DELAY_TIME);
   tolua_constant(tolua_S,"P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME",PAC_info::P_WAGO_TCP_NODE_WARN_ANSWER_AVG_TIME);
   tolua_constant(tolua_S,"P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME",PAC_info::P_MAIN_CYCLE_WARN_ANSWER_AVG_TIME);
   tolua_constant(tolua_S,"P_RESTRICTIONS_MODE",PAC_info::P_RESTRICTIONS_MODE);
   tolua_constant(tolua_S,"P_RESTRICTIONS_MANUAL_TIME",PAC_info::P_RESTRICTIONS_MANUAL_TIME);
   tolua_constant(tolua_S,"P_AUTO_PAUSE_OPER_ON_DEV_ERR",PAC_info::P_AUTO_PAUSE_OPER_ON_DEV_ERR);
   tolua_constant(tolua_S,"P_AUTO_OPERATION_WAIT_TIME",PAC_info::P_AUTO_OPERATION_WAIT_TIME);
   tolua_constant(tolua_S,"P_AUTO_OPERATION_WARN_TIME",PAC_info::P_AUTO_OPERATION_WARN_TIME);
   tolua_constant(tolua_S,"P_IS_OPC_UA_SERVER_ACTIVE",PAC_info::P_IS_OPC_UA_SERVER_ACTIVE);
   tolua_constant(tolua_S,"P_IS_OPC_UA_SERVER_CONTROL",PAC_info::P_IS_OPC_UA_SERVER_CONTROL);
   tolua_constant(tolua_S,"P_BK_ANSWER_MAX_WAIT_TIME",PAC_info::P_BK_ANSWER_MAX_WAIT_TIME);
   tolua_variable(tolua_S,"par",tolua_get_PAC_info_par,tolua_set_PAC_info_par);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_PAC_info_set_cmd00);
   tolua_function(tolua_S,"is_emulator",tolua_PAC_dev_PAC_info_is_emulator00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"siren_lights_manager","siren_lights_manager","",NULL);
  tolua_beginmodule(tolua_S,"siren_lights_manager");
   tolua_function(tolua_S,"init",tolua_PAC_dev_siren_lights_manager_init00);
   tolua_function(tolua_S,"set_red_blink",tolua_PAC_dev_siren_lights_manager_set_red_blink00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_SIREN_LIGHTS_MANAGER",tolua_PAC_dev_G_SIREN_LIGHTS_MANAGER00);
  tolua_function(tolua_S,"G_PAC_INFO",tolua_PAC_dev_G_PAC_INFO00);
  tolua_function(tolua_S,"get_sec",tolua_PAC_dev_get_sec00);
  tolua_function(tolua_S,"get_millisec",tolua_PAC_dev_get_millisec00);
  tolua_function(tolua_S,"get_delta_millisec",tolua_PAC_dev_get_delta_millisec00);
  tolua_function(tolua_S,"sleep_ms",tolua_PAC_dev_sleep_ms00);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"tm","tm","",tolua_collect_tm);
  #else
  tolua_cclass(tolua_S,"tm","tm","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"tm");
   tolua_variable(tolua_S,"tm_sec",tolua_get_tm_tm_sec,tolua_set_tm_tm_sec);
   tolua_variable(tolua_S,"tm_min",tolua_get_tm_tm_min,tolua_set_tm_tm_min);
   tolua_variable(tolua_S,"tm_hour",tolua_get_tm_tm_hour,tolua_set_tm_tm_hour);
   tolua_variable(tolua_S,"tm_mday",tolua_get_tm_tm_mday,tolua_set_tm_tm_mday);
   tolua_variable(tolua_S,"tm_mon",tolua_get_tm_tm_mon,tolua_set_tm_tm_mon);
   tolua_variable(tolua_S,"tm_year",tolua_get_tm_tm_year,tolua_set_tm_tm_year);
   tolua_variable(tolua_S,"tm_wday",tolua_get_tm_tm_wday,tolua_set_tm_tm_wday);
   tolua_variable(tolua_S,"tm_yday",tolua_get_tm_tm_yday,tolua_set_tm_tm_yday);
   tolua_variable(tolua_S,"tm_isdst",tolua_get_tm_tm_isdst,tolua_set_tm_tm_isdst);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"get_time",tolua_PAC_dev_get_time00);
  tolua_cclass(tolua_S,"MSAPID","MSAPID","",NULL);
  tolua_beginmodule(tolua_S,"MSAPID");
   tolua_function(tolua_S,"eval",tolua_PAC_dev_MSAPID_eval00);
   tolua_function(tolua_S,"eval",tolua_PAC_dev_MSAPID_eval01);
   tolua_function(tolua_S,"reset",tolua_PAC_dev_MSAPID_reset00);
   tolua_function(tolua_S,"on",tolua_PAC_dev_MSAPID_on00);
   tolua_function(tolua_S,"off",tolua_PAC_dev_MSAPID_off00);
   tolua_function(tolua_S,"set",tolua_PAC_dev_MSAPID_set00);
   tolua_function(tolua_S,"get_state",tolua_PAC_dev_MSAPID_get_state00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"cipline_tech_object","cipline_tech_object","tech_object",tolua_collect_cipline_tech_object);
  #else
  tolua_cclass(tolua_S,"cipline_tech_object","cipline_tech_object","tech_object",NULL);
  #endif
  tolua_beginmodule(tolua_S,"cipline_tech_object");
   tolua_function(tolua_S,"new",tolua_PAC_dev_cipline_tech_object_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_cipline_tech_object_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_cipline_tech_object_new00_local);
   tolua_variable(tolua_S,"blocked",tolua_get_cipline_tech_object_blocked,tolua_set_cipline_tech_object_blocked);
   tolua_variable(tolua_S,"opcip",tolua_get_cipline_tech_object_opcip,tolua_set_cipline_tech_object_opcip);
   tolua_variable(tolua_S,"curstep",tolua_get_cipline_tech_object_curstep,tolua_set_cipline_tech_object_curstep);
   tolua_variable(tolua_S,"state",tolua_get_cipline_tech_object_state,tolua_set_cipline_tech_object_state);
   tolua_variable(tolua_S,"curprg",tolua_get_cipline_tech_object_curprg,tolua_set_cipline_tech_object_curprg);
   tolua_variable(tolua_S,"loadedRecipe",tolua_get_cipline_tech_object_loadedRecipe,tolua_set_cipline_tech_object_loadedRecipe);
   tolua_variable(tolua_S,"loadedProgram",tolua_get_cipline_tech_object_loadedProgram,tolua_set_cipline_tech_object_loadedProgram);
   tolua_variable(tolua_S,"nmr",tolua_get_cipline_tech_object_nmr,tolua_set_cipline_tech_object_nmr);
   tolua_variable(tolua_S,"nplaststate",tolua_get_cipline_tech_object_nplaststate,tolua_set_cipline_tech_object_nplaststate);
   tolua_variable(tolua_S,"pidf_override",tolua_get_cipline_tech_object_pidf_override,tolua_set_cipline_tech_object_pidf_override);
   tolua_variable(tolua_S,"cip_in_error",tolua_get_cipline_tech_object_cip_in_error,tolua_set_cipline_tech_object_cip_in_error);
   tolua_variable(tolua_S,"no_neutro",tolua_get_cipline_tech_object_no_neutro,tolua_set_cipline_tech_object_no_neutro);
   tolua_variable(tolua_S,"dont_use_water_tank",tolua_get_cipline_tech_object_dont_use_water_tank,tolua_set_cipline_tech_object_dont_use_water_tank);
   tolua_variable(tolua_S,"disable_tank_heating",tolua_get_cipline_tech_object_disable_tank_heating,tolua_set_cipline_tech_object_disable_tank_heating);
   tolua_variable(tolua_S,"ret_overrride",tolua_get_cipline_tech_object_ret_overrride,tolua_set_cipline_tech_object_ret_overrride);
   tolua_variable(tolua_S,"return_ok",tolua_get_cipline_tech_object_return_ok,tolua_set_cipline_tech_object_return_ok);
   tolua_variable(tolua_S,"concentration_ok",tolua_get_cipline_tech_object_concentration_ok,tolua_set_cipline_tech_object_concentration_ok);
   tolua_variable(tolua_S,"enable_ret_pump",tolua_get_cipline_tech_object_enable_ret_pump,tolua_set_cipline_tech_object_enable_ret_pump);
   tolua_variable(tolua_S,"clean_water_rinsing_return",tolua_get_cipline_tech_object_clean_water_rinsing_return,tolua_set_cipline_tech_object_clean_water_rinsing_return);
   tolua_variable(tolua_S,"scoldvalves",tolua_get_cipline_tech_object_scoldvalves,tolua_set_cipline_tech_object_scoldvalves);
   tolua_variable(tolua_S,"no_acid_wash_max",tolua_get_cipline_tech_object_no_acid_wash_max,tolua_set_cipline_tech_object_no_acid_wash_max);
   tolua_variable(tolua_S,"use_internal_medium_recipes",tolua_get_cipline_tech_object_use_internal_medium_recipes,tolua_set_cipline_tech_object_use_internal_medium_recipes);
   tolua_variable(tolua_S,"disable_final_rinsing",tolua_get_cipline_tech_object_disable_final_rinsing,tolua_set_cipline_tech_object_disable_final_rinsing);
   tolua_variable(tolua_S,"use_circulation_on_v2_supply",tolua_get_cipline_tech_object_use_circulation_on_v2_supply,tolua_set_cipline_tech_object_use_circulation_on_v2_supply);
   tolua_variable(tolua_S,"V00",tolua_get_cipline_tech_object_V00_ptr,tolua_set_cipline_tech_object_V00_ptr);
   tolua_variable(tolua_S,"V01",tolua_get_cipline_tech_object_V01_ptr,tolua_set_cipline_tech_object_V01_ptr);
   tolua_variable(tolua_S,"V02",tolua_get_cipline_tech_object_V02_ptr,tolua_set_cipline_tech_object_V02_ptr);
   tolua_variable(tolua_S,"V03",tolua_get_cipline_tech_object_V03_ptr,tolua_set_cipline_tech_object_V03_ptr);
   tolua_variable(tolua_S,"V04",tolua_get_cipline_tech_object_V04_ptr,tolua_set_cipline_tech_object_V04_ptr);
   tolua_variable(tolua_S,"V05",tolua_get_cipline_tech_object_V05_ptr,tolua_set_cipline_tech_object_V05_ptr);
   tolua_variable(tolua_S,"V06",tolua_get_cipline_tech_object_V06_ptr,tolua_set_cipline_tech_object_V06_ptr);
   tolua_variable(tolua_S,"V07",tolua_get_cipline_tech_object_V07_ptr,tolua_set_cipline_tech_object_V07_ptr);
   tolua_variable(tolua_S,"V08",tolua_get_cipline_tech_object_V08_ptr,tolua_set_cipline_tech_object_V08_ptr);
   tolua_variable(tolua_S,"V09",tolua_get_cipline_tech_object_V09_ptr,tolua_set_cipline_tech_object_V09_ptr);
   tolua_variable(tolua_S,"V10",tolua_get_cipline_tech_object_V10_ptr,tolua_set_cipline_tech_object_V10_ptr);
   tolua_variable(tolua_S,"V11",tolua_get_cipline_tech_object_V11_ptr,tolua_set_cipline_tech_object_V11_ptr);
   tolua_variable(tolua_S,"V12",tolua_get_cipline_tech_object_V12_ptr,tolua_set_cipline_tech_object_V12_ptr);
   tolua_variable(tolua_S,"V13",tolua_get_cipline_tech_object_V13_ptr,tolua_set_cipline_tech_object_V13_ptr);
   tolua_variable(tolua_S,"NP",tolua_get_cipline_tech_object_NP_ptr,tolua_set_cipline_tech_object_NP_ptr);
   tolua_variable(tolua_S,"NK",tolua_get_cipline_tech_object_NK_ptr,tolua_set_cipline_tech_object_NK_ptr);
   tolua_variable(tolua_S,"NS",tolua_get_cipline_tech_object_NS_ptr,tolua_set_cipline_tech_object_NS_ptr);
   tolua_variable(tolua_S,"LL",tolua_get_cipline_tech_object_LL_ptr,tolua_set_cipline_tech_object_LL_ptr);
   tolua_variable(tolua_S,"LM",tolua_get_cipline_tech_object_LM_ptr,tolua_set_cipline_tech_object_LM_ptr);
   tolua_variable(tolua_S,"LH",tolua_get_cipline_tech_object_LH_ptr,tolua_set_cipline_tech_object_LH_ptr);
   tolua_variable(tolua_S,"LWL",tolua_get_cipline_tech_object_LWL_ptr,tolua_set_cipline_tech_object_LWL_ptr);
   tolua_variable(tolua_S,"LWH",tolua_get_cipline_tech_object_LWH_ptr,tolua_set_cipline_tech_object_LWH_ptr);
   tolua_variable(tolua_S,"LSL",tolua_get_cipline_tech_object_LSL_ptr,tolua_set_cipline_tech_object_LSL_ptr);
   tolua_variable(tolua_S,"LSH",tolua_get_cipline_tech_object_LSH_ptr,tolua_set_cipline_tech_object_LSH_ptr);
   tolua_variable(tolua_S,"LKL",tolua_get_cipline_tech_object_LKL_ptr,tolua_set_cipline_tech_object_LKL_ptr);
   tolua_variable(tolua_S,"LKH",tolua_get_cipline_tech_object_LKH_ptr,tolua_set_cipline_tech_object_LKH_ptr);
   tolua_variable(tolua_S,"TP",tolua_get_cipline_tech_object_TP_ptr,tolua_set_cipline_tech_object_TP_ptr);
   tolua_variable(tolua_S,"TR",tolua_get_cipline_tech_object_TR_ptr,tolua_set_cipline_tech_object_TR_ptr);
   tolua_variable(tolua_S,"Q",tolua_get_cipline_tech_object_Q_ptr,tolua_set_cipline_tech_object_Q_ptr);
   tolua_variable(tolua_S,"ao",tolua_get_cipline_tech_object_ao_ptr,tolua_set_cipline_tech_object_ao_ptr);
   tolua_variable(tolua_S,"PUMPFREQ",tolua_get_cipline_tech_object_PUMPFREQ_ptr,tolua_set_cipline_tech_object_PUMPFREQ_ptr);
   tolua_variable(tolua_S,"FL",tolua_get_cipline_tech_object_FL_ptr,tolua_set_cipline_tech_object_FL_ptr);
   tolua_variable(tolua_S,"cnt",tolua_get_cipline_tech_object_cnt_ptr,tolua_set_cipline_tech_object_cnt_ptr);
   tolua_array(tolua_S,"T",tolua_get_PAC_dev_cipline_tech_object_T,tolua_set_PAC_dev_cipline_tech_object_T);
   tolua_variable(tolua_S,"PIDP",tolua_get_cipline_tech_object_PIDP_ptr,tolua_set_cipline_tech_object_PIDP_ptr);
   tolua_variable(tolua_S,"PIDF",tolua_get_cipline_tech_object_PIDF_ptr,tolua_set_cipline_tech_object_PIDF_ptr);
   tolua_variable(tolua_S,"msa_number",tolua_get_cipline_tech_object_msa_number,tolua_set_cipline_tech_object_msa_number);
   tolua_variable(tolua_S,"ncar1",tolua_get_cipline_tech_object_ncar1,tolua_set_cipline_tech_object_ncar1);
   tolua_variable(tolua_S,"ncar2",tolua_get_cipline_tech_object_ncar2,tolua_set_cipline_tech_object_ncar2);
   tolua_variable(tolua_S,"ncar3",tolua_get_cipline_tech_object_ncar3,tolua_set_cipline_tech_object_ncar3);
   tolua_variable(tolua_S,"ncar4",tolua_get_cipline_tech_object_ncar4,tolua_set_cipline_tech_object_ncar4);
   tolua_variable(tolua_S,"switch1",tolua_get_cipline_tech_object_switch1,tolua_set_cipline_tech_object_switch1);
   tolua_variable(tolua_S,"switch2",tolua_get_cipline_tech_object_switch2,tolua_set_cipline_tech_object_switch2);
   tolua_variable(tolua_S,"switch3",tolua_get_cipline_tech_object_switch3,tolua_set_cipline_tech_object_switch3);
   tolua_variable(tolua_S,"switch4",tolua_get_cipline_tech_object_switch4,tolua_set_cipline_tech_object_switch4);
   tolua_variable(tolua_S,"par_float",tolua_get_cipline_tech_object_par_float,tolua_set_cipline_tech_object_par_float);
   tolua_variable(tolua_S,"rt_par_float",tolua_get_cipline_tech_object_rt_par_float,tolua_set_cipline_tech_object_rt_par_float);
   tolua_function(tolua_S,"get_station_par",tolua_PAC_dev_cipline_tech_object_get_station_par00);
   tolua_function(tolua_S,"set_station_par",tolua_PAC_dev_cipline_tech_object_set_station_par00);
   tolua_function(tolua_S,"get_selfclean_par",tolua_PAC_dev_cipline_tech_object_get_selfclean_par00);
   tolua_function(tolua_S,"set_selfclean_par",tolua_PAC_dev_cipline_tech_object_set_selfclean_par00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_cipline_tech_object_set_cmd00);
   tolua_function(tolua_S,"set_cmd",tolua_PAC_dev_cipline_tech_object_set_cmd01);
   tolua_function(tolua_S,"initline",tolua_PAC_dev_cipline_tech_object_initline00);
   tolua_function(tolua_S,"evaluate",tolua_PAC_dev_cipline_tech_object_evaluate00);
   tolua_function(tolua_S,"GetConc",tolua_PAC_dev_cipline_tech_object_GetConc00);
   tolua_function(tolua_S,"SortRR",tolua_PAC_dev_cipline_tech_object_SortRR00);
   tolua_function(tolua_S,"SetRet",tolua_PAC_dev_cipline_tech_object_SetRet00);
   tolua_function(tolua_S,"ForceRet",tolua_PAC_dev_cipline_tech_object_ForceRet00);
   tolua_function(tolua_S,"GetRetState",tolua_PAC_dev_cipline_tech_object_GetRetState00);
   tolua_function(tolua_S,"HasRet",tolua_PAC_dev_cipline_tech_object_HasRet00);
   tolua_function(tolua_S,"timeIsOut",tolua_PAC_dev_cipline_tech_object_timeIsOut00);
   tolua_function(tolua_S,"_DoStep",tolua_PAC_dev_cipline_tech_object__DoStep00);
   tolua_function(tolua_S,"_GoToStep",tolua_PAC_dev_cipline_tech_object__GoToStep00);
   tolua_function(tolua_S,"_InitStep",tolua_PAC_dev_cipline_tech_object__InitStep00);
   tolua_function(tolua_S,"_LoadProgram",tolua_PAC_dev_cipline_tech_object__LoadProgram00);
   tolua_function(tolua_S,"_StopDev",tolua_PAC_dev_cipline_tech_object__StopDev00);
   tolua_function(tolua_S,"_ResetLinesDevicesBeforeReset",tolua_PAC_dev_cipline_tech_object__ResetLinesDevicesBeforeReset00);
   tolua_function(tolua_S,"_OporCIP",tolua_PAC_dev_cipline_tech_object__OporCIP00);
   tolua_function(tolua_S,"_InitOporCIP",tolua_PAC_dev_cipline_tech_object__InitOporCIP00);
   tolua_function(tolua_S,"_CheckErr",tolua_PAC_dev_cipline_tech_object__CheckErr00);
   tolua_function(tolua_S,"_Circ",tolua_PAC_dev_cipline_tech_object__Circ00);
   tolua_function(tolua_S,"_InitCirc",tolua_PAC_dev_cipline_tech_object__InitCirc00);
   tolua_function(tolua_S,"_InitToObject",tolua_PAC_dev_cipline_tech_object__InitToObject00);
   tolua_function(tolua_S,"_InitFromObject",tolua_PAC_dev_cipline_tech_object__InitFromObject00);
   tolua_function(tolua_S,"_InitFilCirc",tolua_PAC_dev_cipline_tech_object__InitFilCirc00);
   tolua_function(tolua_S,"_InitOporCirc",tolua_PAC_dev_cipline_tech_object__InitOporCirc00);
   tolua_function(tolua_S,"_ToObject",tolua_PAC_dev_cipline_tech_object__ToObject00);
   tolua_function(tolua_S,"_FromObject",tolua_PAC_dev_cipline_tech_object__FromObject00);
   tolua_function(tolua_S,"_FillCirc",tolua_PAC_dev_cipline_tech_object__FillCirc00);
   tolua_function(tolua_S,"_OporCirc",tolua_PAC_dev_cipline_tech_object__OporCirc00);
   tolua_function(tolua_S,"_RT",tolua_PAC_dev_cipline_tech_object__RT00);
   tolua_function(tolua_S,"_Stop",tolua_PAC_dev_cipline_tech_object__Stop00);
   tolua_function(tolua_S,"_InitDoseRR",tolua_PAC_dev_cipline_tech_object__InitDoseRR00);
   tolua_function(tolua_S,"_DoseRR",tolua_PAC_dev_cipline_tech_object__DoseRR00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"modbus_client","modbus_client","",tolua_collect_modbus_client);
  #else
  tolua_cclass(tolua_S,"modbus_client","modbus_client","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"modbus_client");
   tolua_function(tolua_S,"new",tolua_PAC_dev_modbus_client_new00);
   tolua_function(tolua_S,"new_local",tolua_PAC_dev_modbus_client_new00_local);
   tolua_function(tolua_S,".call",tolua_PAC_dev_modbus_client_new00_local);
   tolua_function(tolua_S,"read_discrete_inputs",tolua_PAC_dev_modbus_client_read_discrete_inputs00);
   tolua_function(tolua_S,"read_coils",tolua_PAC_dev_modbus_client_read_coils00);
   tolua_function(tolua_S,"read_holding_registers",tolua_PAC_dev_modbus_client_read_holding_registers00);
   tolua_function(tolua_S,"read_input_registers",tolua_PAC_dev_modbus_client_read_input_registers00);
   tolua_function(tolua_S,"write_coil",tolua_PAC_dev_modbus_client_write_coil00);
   tolua_function(tolua_S,"force_multiply_coils",tolua_PAC_dev_modbus_client_force_multiply_coils00);
   tolua_function(tolua_S,"write_multiply_registers",tolua_PAC_dev_modbus_client_write_multiply_registers00);
   tolua_function(tolua_S,"async_read_discrete_inputs",tolua_PAC_dev_modbus_client_async_read_discrete_inputs00);
   tolua_function(tolua_S,"async_read_coils",tolua_PAC_dev_modbus_client_async_read_coils00);
   tolua_function(tolua_S,"async_read_holding_registers",tolua_PAC_dev_modbus_client_async_read_holding_registers00);
   tolua_function(tolua_S,"async_read_input_registers",tolua_PAC_dev_modbus_client_async_read_input_registers00);
   tolua_function(tolua_S,"async_write_coil",tolua_PAC_dev_modbus_client_async_write_coil00);
   tolua_function(tolua_S,"async_force_multiply_coils",tolua_PAC_dev_modbus_client_async_force_multiply_coils00);
   tolua_function(tolua_S,"async_write_multiply_registers",tolua_PAC_dev_modbus_client_async_write_multiply_registers00);
   tolua_function(tolua_S,"async_read_write_multiply_registers",tolua_PAC_dev_modbus_client_async_read_write_multiply_registers00);
   tolua_function(tolua_S,"async_mask_write_register",tolua_PAC_dev_modbus_client_async_mask_write_register00);
   tolua_function(tolua_S,"async_mask_write_register",tolua_PAC_dev_modbus_client_async_mask_write_register01);
   tolua_function(tolua_S,"get_async_result",tolua_PAC_dev_modbus_client_get_async_result00);
   tolua_function(tolua_S,"set_station",tolua_PAC_dev_modbus_client_set_station00);
   tolua_function(tolua_S,"zero_output_buff",tolua_PAC_dev_modbus_client_zero_output_buff00);
   tolua_function(tolua_S,"set_byte",tolua_PAC_dev_modbus_client_set_byte00);
   tolua_function(tolua_S,"get_byte",tolua_PAC_dev_modbus_client_get_byte00);
   tolua_function(tolua_S,"set_int2",tolua_PAC_dev_modbus_client_set_int200);
   tolua_function(tolua_S,"get_int2",tolua_PAC_dev_modbus_client_get_int200);
   tolua_function(tolua_S,"set_int4",tolua_PAC_dev_modbus_client_set_int400);
   tolua_function(tolua_S,"get_int4",tolua_PAC_dev_modbus_client_get_int400);
   tolua_function(tolua_S,"set_int4_ab_cd",tolua_PAC_dev_modbus_client_set_int4_ab_cd00);
   tolua_function(tolua_S,"get_int4_ab_cd",tolua_PAC_dev_modbus_client_get_int4_ab_cd00);
   tolua_function(tolua_S,"set_int4_cd_ab",tolua_PAC_dev_modbus_client_set_int4_cd_ab00);
   tolua_function(tolua_S,"get_int4_cd_ab",tolua_PAC_dev_modbus_client_get_int4_cd_ab00);
   tolua_function(tolua_S,"set_int4_dc_ba",tolua_PAC_dev_modbus_client_set_int4_dc_ba00);
   tolua_function(tolua_S,"get_int4_dc_ba",tolua_PAC_dev_modbus_client_get_int4_dc_ba00);
   tolua_function(tolua_S,"set_float",tolua_PAC_dev_modbus_client_set_float00);
   tolua_function(tolua_S,"get_float",tolua_PAC_dev_modbus_client_get_float00);
   tolua_function(tolua_S,"set_bit",tolua_PAC_dev_modbus_client_set_bit00);
   tolua_function(tolua_S,"get_bit",tolua_PAC_dev_modbus_client_get_bit00);
   tolua_function(tolua_S,"reg_get_bit",tolua_PAC_dev_modbus_client_reg_get_bit00);
   tolua_function(tolua_S,"reg_set_bit",tolua_PAC_dev_modbus_client_reg_set_bit00);
   tolua_function(tolua_S,"mask_reset",tolua_PAC_dev_modbus_client_mask_reset00);
   tolua_function(tolua_S,"mask_set_bit",tolua_PAC_dev_modbus_client_mask_set_bit00);
   tolua_function(tolua_S,"reverse",tolua_PAC_dev_modbus_client_reverse00);
   tolua_function(tolua_S,"swapBits",tolua_PAC_dev_modbus_client_swapBits00);
   tolua_function(tolua_S,"get_id",tolua_PAC_dev_modbus_client_get_id00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"ModbusServ","ModbusServ","",NULL);
  tolua_beginmodule(tolua_S,"ModbusServ");
   tolua_function(tolua_S,"UnpackInt16",tolua_PAC_dev_ModbusServ_UnpackInt1600);
   tolua_function(tolua_S,"UnpackInt32",tolua_PAC_dev_ModbusServ_UnpackInt3200);
   tolua_function(tolua_S,"UnpackFloat",tolua_PAC_dev_ModbusServ_UnpackFloat00);
   tolua_function(tolua_S,"UnpackWord",tolua_PAC_dev_ModbusServ_UnpackWord00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"i_log","i_log","",NULL);
  tolua_beginmodule(tolua_S,"i_log");
   tolua_constant(tolua_S,"P_EMERG",i_log::P_EMERG);
   tolua_constant(tolua_S,"P_ALERT",i_log::P_ALERT);
   tolua_constant(tolua_S,"P_CRIT",i_log::P_CRIT);
   tolua_constant(tolua_S,"P_ERR",i_log::P_ERR);
   tolua_constant(tolua_S,"P_WARNING",i_log::P_WARNING);
   tolua_constant(tolua_S,"P_NOTICE",i_log::P_NOTICE);
   tolua_constant(tolua_S,"P_INFO",i_log::P_INFO);
   tolua_constant(tolua_S,"P_DEBUG",i_log::P_DEBUG);
   tolua_function(tolua_S,"write_log",tolua_PAC_dev_i_log_write_log00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_SYS_LOG",tolua_PAC_dev_G_SYS_LOG00);
  tolua_cclass(tolua_S,"ParamsRecipeStorage","ParamsRecipeStorage","",NULL);
  tolua_beginmodule(tolua_S,"ParamsRecipeStorage");
   tolua_function(tolua_S,"getId",tolua_PAC_dev_ParamsRecipeStorage_getId00);
   tolua_function(tolua_S,"getCount",tolua_PAC_dev_ParamsRecipeStorage_getCount00);
   tolua_function(tolua_S,"getParamsCount",tolua_PAC_dev_ParamsRecipeStorage_getParamsCount00);
   tolua_function(tolua_S,"setRecPar",tolua_PAC_dev_ParamsRecipeStorage_setRecPar00);
   tolua_function(tolua_S,"getRecPar",tolua_PAC_dev_ParamsRecipeStorage_getRecPar00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"ParamsRecipeAdapter","ParamsRecipeAdapter","",NULL);
  tolua_beginmodule(tolua_S,"ParamsRecipeAdapter");
   tolua_function(tolua_S,"addMap",tolua_PAC_dev_ParamsRecipeAdapter_addMap00);
   tolua_function(tolua_S,"loadParams",tolua_PAC_dev_ParamsRecipeAdapter_loadParams00);
   tolua_function(tolua_S,"setUseSeparateRecipeList",tolua_PAC_dev_ParamsRecipeAdapter_setUseSeparateRecipeList00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"ParamsRecipeManager","ParamsRecipeManager","",NULL);
  tolua_beginmodule(tolua_S,"ParamsRecipeManager");
   tolua_function(tolua_S,"evaluate",tolua_PAC_dev_ParamsRecipeManager_evaluate00);
   tolua_function(tolua_S,"createRecipes",tolua_PAC_dev_ParamsRecipeManager_createRecipes00);
   tolua_function(tolua_S,"createAdapter",tolua_PAC_dev_ParamsRecipeManager_createAdapter00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"G_PARAMS_RECIPE_MANAGER",tolua_PAC_dev_G_PARAMS_RECIPE_MANAGER00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_PAC_dev (lua_State* tolua_S) {
 return tolua_PAC_dev_open(tolua_S);
};
#endif

