#include "modbus_serv.h"

#include "lua_manager.h"

#include "utf2cp1251.h"

int ModbusServ::confirmRecUpdateCtr[11] = {0};
char ModbusServ::updateRecFlag[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int ModbusServ::confirmPrgUpdateCtr[11] = {0};
char ModbusServ::updatePrgFlag[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

extern int isMsa;

unsigned char ModbusServ::UTable[128][2] =
	{
		{0x4,0x2},
		{0x4,0x3},
		{0x20,0x1A},
		{0x4,0x53},
		{0x20,0x1E},
		{0x20,0x26},
		{0x20,0x20},
		{0x20,0x21},
		{0x20,0xAC},
		{0x20,0x30},
		{0x4,0x9},
		{0x20,0x39},
		{0x4,0xA},
		{0x4,0xC},
		{0x4,0xB},
		{0x4,0xF},
		{0x4,0x52},
		{0x20,0x18},
		{0x20,0x19},
		{0x20,0x1C},
		{0x20,0x1D},
		{0x20,0x22},
		{0x20,0x13},
		{0x20,0x14},
		{0x0,0x0},
		{0x21,0x22},
		{0x4,0x59},
		{0x20,0x3A},
		{0x4,0x5A},
		{0x4,0x5C},
		{0x4,0x5B},
		{0x4,0x5F},
		{0x0,0xA0},
		{0x4,0xE},
		{0x4,0x5E},
		{0x4,0x8},
		{0x0,0xA4},
		{0x4,0x90},
		{0x0,0xA6},
		{0x0,0xA7},
		{0x4,0x1},
		{0x0,0xA9},
		{0x4,0x4},
		{0x0,0xAB},
		{0x0,0xAC},
		{0x0,0xAD},
		{0x0,0xAE},
		{0x4,0x7},
		{0x0,0xB0},
		{0x0,0xB1},
		{0x4,0x6},
		{0x4,0x56},
		{0x4,0x91},
		{0x0,0xB5},
		{0x0,0xB6},
		{0x0,0xB7},
		{0x4,0x51},
		{0x21,0x16},
		{0x4,0x54},
		{0x0,0xBB},
		{0x4,0x58},
		{0x4,0x5},
		{0x4,0x55},
		{0x4,0x57},
		{0x4,0x10},
		{0x4,0x11},
		{0x4,0x12},
		{0x4,0x13},
		{0x4,0x14},
		{0x4,0x15},
		{0x4,0x16},
		{0x4,0x17},
		{0x4,0x18},
		{0x4,0x19},
		{0x4,0x1A},
		{0x4,0x1B},
		{0x4,0x1C},
		{0x4,0x1D},
		{0x4,0x1E},
		{0x4,0x1F},
		{0x4,0x20},
		{0x4,0x21},
		{0x4,0x22},
		{0x4,0x23},
		{0x4,0x24},
		{0x4,0x25},
		{0x4,0x26},
		{0x4,0x27},
		{0x4,0x28},
		{0x4,0x29},
		{0x4,0x2A},
		{0x4,0x2B},
		{0x4,0x2C},
		{0x4,0x2D},
		{0x4,0x2E},
		{0x4,0x2F},
		{0x4,0x30},
		{0x4,0x31},
		{0x4,0x32},
		{0x4,0x33},
		{0x4,0x34},
		{0x4,0x35},
		{0x4,0x36},
		{0x4,0x37},
		{0x4,0x38},
		{0x4,0x39},
		{0x4,0x3A},
		{0x4,0x3B},
		{0x4,0x3C},
		{0x4,0x3D},
		{0x4,0x3E},
		{0x4,0x3F},
		{0x4,0x40},
		{0x4,0x41},
		{0x4,0x42},
		{0x4,0x43},
		{0x4,0x44},
		{0x4,0x45},
		{0x4,0x46},
		{0x4,0x47},
		{0x4,0x48},
		{0x4,0x49},
		{0x4,0x4A},
		{0x4,0x4B},
		{0x4,0x4C},
		{0x4,0x4D},
		{0x4,0x4E},
		{0x4,0x4F}
	};

long ModbusServ::ModbusService( long len, unsigned char *data,unsigned char *outdata )
	{
	lua_State* L = lua_manager::get_instance()->get_Lua();

	switch ( data[1] ) //Modbus command
		{

		case 0x01: //Read Coils
			{
			unsigned int startingAddress  = data[2] * 256 + data[3];
			unsigned int numberofElements = data[4] * 256 + data[5];
			unsigned char numberofBytes   = numberofElements / 8;
			if ( numberofElements % 8 ) numberofBytes++;
			outdata[ 2 ] = numberofBytes;

			unsigned int coilgroup = data[0];

			if (isMsa)
				{
				unsigned int objnumber;
				unsigned int i;
				unsigned int line;
				for (i = 0; i < numberofElements; i++)
					{
					objnumber = i+startingAddress;
					coilgroup = data[0];
					switch (coilgroup)
						{
						case C_V:
						case C_M:
						case C_N:
						case C_FS:
						case C_PUMPS:
						case C_LINE1VALVES:
						case C_LINE2VALVES:
						case C_LINE3VALVES:
						case C_LINE4VALVES:
						case C_LINE5VALVES:
						case C_LINE6VALVES:
						case C_LINE7VALVES:
						case C_LINE8VALVES:
						case C_LINE9VALVES:
							ForceBit(i, &outdata[3], get_device(coilgroup, objnumber)->get_state());
							break;
						case C_LS:
							ForceBit(i, &outdata[3], get_device(coilgroup, objnumber)->is_active());
							break;
						case C_OTHER:
							line = (i + startingAddress) / 1000;
							if (line > u_int(cipline_tech_object::MdlsCNT))
								{
								continue;
								}
							objnumber = (i+startingAddress) - line * 1000;
							switch (objnumber)
								{
								case OTHER_SWITCH1:
								case OTHER_SWITCH2:
								case OTHER_SWITCH3:
								case OTHER_SWITCH4:
									ForceBit(i, &outdata[3], cipline_tech_object::Mdls[line-1]->getSwitch(objnumber + 1 - OTHER_SWITCH1));
									break;
								}
							break;
						case C_MSA_STATIONPARAMS:
							ForceBit(i,&outdata[3],cipline_tech_object::Mdls[0]->get_station_par(objnumber) != 0 ? 1 : 0);
							break;
						case C_MSA_CONTROL:
							line = objnumber / 100;
							objnumber -= 100 * line;
							if (line <= (unsigned int)cipline_tech_object::MdlsCNT)
								{
								switch (objnumber)
									{
									case RETPUMP_STATE:
										ForceBit(i,&outdata[3],cipline_tech_object::Mdls[line-1]->GetRetState());
										break;
									case RETPUMP_ISUSED:
										ForceBit(i,&outdata[3],cipline_tech_object::Mdls[line-1]->HasRet());
										break;
									}
								}
							break;
						case C_MSA_LINE1PARAMS:
						case C_MSA_LINE2PARAMS:
						case C_MSA_LINE3PARAMS:
						case C_MSA_LINE4PARAMS:
						case C_MSA_LINE5PARAMS:
						case C_MSA_LINE6PARAMS:
						case C_MSA_LINE7PARAMS:
						case C_MSA_LINE8PARAMS:
						case C_MSA_LINE9PARAMS:
							if (coilgroup - C_MSA_LINE1PARAMS < (unsigned int)cipline_tech_object::MdlsCNT)
								{
								ForceBit(i,&outdata[3],cipline_tech_object::Mdls[coilgroup - C_MSA_LINE1PARAMS]->rt_par_float[objnumber - 1] != 0 ? 1 : 0);
								}
							break;
                        default:
                            goto luareadcoils;
						}
					}
				}
			else
				{
                luareadcoils:
				static char has_Lua_read_coils = 0;
				if ( has_Lua_read_coils == 0 )
					{
					lua_getglobal( L, "read_coils" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_read_coils = 2;
						}
					else
						{
						has_Lua_read_coils = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "read_coils".
					}

				int i = 0;
				if ( has_Lua_read_coils == 2 )
					{
					lua_getglobal( L, "read_coils" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, numberofElements );

                    if ( auto i_line = lua_pcall( L, 3, 1, 0 ); i_line != 0 )
						{
						printf( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );
						return 0;
						}

					if ( lua_istable( L, -1 ) )
						{
						lua_pushnil( L );
						while( lua_next( L, -2 ) )
							{
							int bit_res = ( int ) lua_tonumber( L, -1 );
							lua_pop( L, 1 );
							ForceBit( i, &outdata[ 3 ], bit_res );
							i++;
							}
						}
					lua_pop( L, 1 );
					}
				}

			return 3+numberofBytes;
			}

		case 0x03: //Read Holding Registers
			{
			unsigned int startingAddress = data[2] * 256 + data[3];
			unsigned int numberofElements = data[4] * 256 + data[5];
			outdata[ 2 ] = (numberofElements * 2) & 0xFF;
			unsigned int coilgroup = data[0];

			if (isMsa)
				{
				unsigned int i,j;
                int k;
				unsigned int objnumber, line;
				int modstate;
				for (i = 0; i < numberofElements; i++)
					{
					objnumber = i + startingAddress;
					switch (coilgroup)
						{
						case C_AO:
							PackFloat(get_device(coilgroup, objnumber)->get_value(),&outdata[3+i*2]);
							i++;
							break;
						case C_FE:
							if (objnumber > 0)
								{
								PackFloat(((i_counter*)get_device(coilgroup, objnumber))->get_flow(),&outdata[3+i*2]);
								}
							i++;
							break;
						case C_CTR:
							if (objnumber > 0)
								{
								PackFloat((float)((i_counter*)get_device(coilgroup, objnumber))->get_quantity(),&outdata[3+i*2]);
								}
							i++;
							break;
						case C_TE:
						case C_QE:
						case C_AI:
						case C_LE:
							PackFloat(get_device(coilgroup, objnumber)->get_value(),&outdata[3+i*2]);
							i++;
							break;
						case C_OTHER:
							line = (i + startingAddress) / 1000;
							if (line > u_int(cipline_tech_object::MdlsCNT))
								{
								continue;
								}
							objnumber = (i+startingAddress) - line * 1000;
							switch (objnumber)
								{
								case OTHER_CARNO1:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->ncar1, &outdata[3+i*2]);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CARNO2:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->ncar2, &outdata[3+i*2]);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CARNO3:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->ncar3, &outdata[3+i*2]);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CARNO4:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->ncar4, &outdata[3+i*2]);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CAUSTIC_COUNT:
									PackInt16(cipline_tech_object::Mdls[line - 1]->objectstats->objcausticwashes, &outdata[3 + i * 2]);
									break;
								case OTHER_LAST_ACID_WASH:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->objectstats->objlastacidwash, &outdata[3 + i * 2]);
									i += MAX_FIELD_LENGTH-1;
									break;
								case OTHER_LAST_WASH:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->objectstats->objlastwash, &outdata[3 + i * 2]);
									i += MAX_FIELD_LENGTH-1;
									break;
								case OTHER_LAST_WASH_PROGRAM:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->objectstats->objlastwashprogram, &outdata[3 + i * 2]);
									i += MAX_FIELD_LENGTH-1;
									break;
                                case OTHER_PLC_TIME:
                                    char tempdatetime[17];
                                    cipline_tech_object::Mdls[0]->DateToChar(tempdatetime);
                                    tempdatetime[16] = 0;
                                    PackString(tempdatetime, &outdata[3 + i * 2]);
                                    i += 7;
                                    break;
								default:
									break;
								}
							break;
						case C_MSA_RECIPES:
							line = (i + startingAddress) / 3000 + 1;
							if (line > u_int(cipline_tech_object::MdlsCNT))
								{
								continue;
								}
							objnumber = (i+startingAddress) - (line - 1) * 3000;
							switch (objnumber)
								{
								case RC_RECIPE_CONTROL:
									PackInt16(0,&outdata[3+i*2]);
									break;
								case RC_RECIPE_CONTROL_NO:
									PackInt16(int_2(cipline_tech_object::Mdls[line - 1]->rt_par_float[P_CUR_REC]),&outdata[3+i*2]);
									break;
								case RC_SELECTED_REC:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->loadedRecName, &outdata[3+i*2]);
									i+=TRecipeManager::recipeNameLength - 1;
									break;
								case RC_SELECTED_PRG:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->currentProgramName, &outdata[3+i*2]);
									i+= PROGRAM_MAX_LEN - 1;
									break;
								case RC_EDITED_REC:
									Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->lineRecipes->currentRecipeName, &outdata[3+i*2]);
									i+=TRecipeManager::recipeNameLength - 1;
									break;
								case RC_SELECT_REC:
									PackInt16(-1,&outdata[3+i*2]);
									break;
								case RC_LIST_UPDATE:
									PackInt16(updateRecFlag[line],&outdata[3+i*2]);
									break;
								case RC_LIST_LENGTH:
									k = 0;
									for (j = 0; j < u_int(cipline_tech_object::Mdls[line - 1]->lineRecipes->recipePerLine); j++)
										{
										if (1 == cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeValue(j, TRecipeManager::RV_IS_USED))
											{
											k++;
											}
										}
									PackInt16(k,&outdata[3+i*2]);
									break;
								case RC_PRG_SELECT:
									PackInt16(-1,&outdata[3+i*2]);
									break;
								case RC_PRG_UPDATE:
									PackInt16(updatePrgFlag[line],&outdata[3+i*2]);
									break;
								case RC_PRG_LENGTH:
									PackInt16(cipline_tech_object::Mdls[line-1]->prgListLen,&outdata[3+i*2]);
									break;
								default:
									if (objnumber >= RC_LIST_START && objnumber < RC_PRG_SELECT)
										{
										int recipeno = (objnumber - RC_LIST_START) / TRecipeManager::recipeNameLength;
										//								printf("\n\rRecipeNo = %d", recipeno);
										k = -1;
										for (j = 0; j < u_int(cipline_tech_object::Mdls[line - 1]->lineRecipes->recipePerLine); j++)
											{
											if (1 == cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeValue(j, TRecipeManager::RV_IS_USED))
												{
												k++;
												}
											if (k == recipeno)
												{
												char myrec[MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER];
												cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeName(j, myrec);
												Utf8toUnicode(myrec, &outdata[3+i*2]);
												break;
												}
											}
										i+= TRecipeManager::recipeNameLength - 1;
										}
									else
										{
										if (objnumber >= RC_PRG_START && objnumber < RC_RECIPE_PAR_START)
											{
                                            if ( auto prgno = ( objnumber - RC_PRG_START ) / PROGRAM_MAX_LEN;
												prgno < cipline_tech_object::Mdls[line-1]->prgArray.size())
											    Utf8toUnicode(cipline_tech_object::Mdls[line - 1]->prgArray[prgno].data(), &outdata[3+i*2]);
											i+= PROGRAM_MAX_LEN - 1;
											}
										else
											{
											if (objnumber >= RC_RECIPE_PAR_START)
												{
												k = (objnumber - RC_RECIPE_PAR_START) / 2;
												if (k < cipline_tech_object::Mdls[line - 1]->lineRecipes->GetParamsCount())
													{
													PackFloat(cipline_tech_object::Mdls[line - 1]->lineRecipes->getValue(k - 1), &outdata[3+i*2]);
													i++;
													}
												}
											else
                                                {
                                                if ( G_DEBUG )
                                                    {
                                                    printf("\n\rRead Unsigned register");
                                                    }
                                                }
											}
										}
									break;
								}
							break;

						case C_MSA_STATIONPARAMS:
							PackFloat(cipline_tech_object::Mdls[0]->get_station_par(objnumber/2), &outdata[3+i*2]);
							i++;
							break;

						case C_MSA_CONTROL:
							line = objnumber / 100;
							objnumber -= 100 * line;
							if (line < (unsigned int)cipline_tech_object::MdlsCNT)
								{
								switch (objnumber)
									{
									case MSACTRL_OPERATION:
										PackInt16(cipline_tech_object::Mdls[line]->curstep, &outdata[3+i*2]);
										break;
									case MSACTRL_STATE:
										modstate = cipline_tech_object::Mdls[line]->state;
										if (modstate < 0)
											{
											if (modstate == -2)
												{
												modstate = 80;
												}
											else
												{
												if (modstate == -1000)
													{
													modstate = 90;
													}
												else
													{
													if (modstate == -1)
														{
														modstate = 50;
														}
													else
														{
														modstate = -modstate;
														}
													}
												}
											}
										PackInt16(modstate, &outdata[3+i*2]);
										break;
									case MSACTRL_RETTEMP:
										PackFloat(cipline_tech_object::Mdls[line]->rt_par_float[P_ZAD_PODOGR] - cipline_tech_object::Mdls[line]->rt_par_float[P_DELTA_TR], &outdata[3+i*2]);
										i++;
										break;
									case MSACTRL_MSANUMBER:
										PackInt16(cipline_tech_object::msa_number, &outdata[3+i*2]);
										break;
									default:
										break;
									}
								}
							break;

						case C_MSA_LINE1PARAMS:
						case C_MSA_LINE2PARAMS:
						case C_MSA_LINE3PARAMS:
						case C_MSA_LINE4PARAMS:
						case C_MSA_LINE5PARAMS:
						case C_MSA_LINE6PARAMS:
						case C_MSA_LINE7PARAMS:
						case C_MSA_LINE8PARAMS:
						case C_MSA_LINE9PARAMS:
							if (coilgroup - C_MSA_LINE1PARAMS < (unsigned int)(cipline_tech_object::MdlsCNT))
								{
								PackFloat(cipline_tech_object::Mdls[coilgroup - C_MSA_LINE1PARAMS]->rt_par_float[objnumber/2], &outdata[3+i*2]);
								}
							i++;
							break;
						default:
                            goto luareadregisters;
						}
					}
				}
			else
				{
                luareadregisters:
				int idx = 0;
				static char has_Lua_read_holding_registers = 0;
				if ( has_Lua_read_holding_registers == 0 )
					{
					lua_getglobal( L, "read_holding_registers" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_read_holding_registers = 2;
						}
					else
						{
						has_Lua_read_holding_registers = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "read_holding_registers".
					}

				if ( has_Lua_read_holding_registers == 2 )
					{
					lua_getglobal( L, "read_holding_registers" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, numberofElements );

                    if ( auto i_line = lua_pcall( L, 3, 1, 0 ); i_line != 0 )
						{
						printf( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );
						return 0;
						}

					if ( lua_istable( L, -1 ) )
						{
						lua_pushnil( L );
						while( lua_next( L, -2 ) )
							{
							int t = ( int ) lua_tonumber( L, -1 );
							lua_pop( L, 1 );

							if ( lua_next( L, -2 ) )
								{
								switch ( t )
									{
									case 1: // int16
										{
										int val = ( int ) lua_tonumber( L, -1 );
										PackInt16( val, &outdata[ 3 + idx * 2 ] );
										break;
										}

									case 2: // float
										{
										float val = ( float ) lua_tonumber( L, -1 );
										PackFloat( val, &outdata[ 3 + idx * 2 ] );
										break;
										}

									case 3: // time
										{
										u_long val = ( u_long ) lua_tonumber( L, -1 );
										PackTime( val, &outdata[ 3 + idx * 2 ] );
										break;
										}

									case 4: // String
										{
										const char *val = lua_tostring( L, -1 );
										Utf8toUnicode( val, &outdata[ 3 + idx * 2 ] );
										break;
										}

									case 5: // UInt32
										{
										u_long val = ( u_long ) lua_tonumber( L, -1 );
										PackLong( val, &outdata[ 3 + idx * 2 ] );
										break;
										}
									}

								lua_pop( L, 1 );
								}
							idx++;
							}

						lua_pop( L, 1 );
						}
					else
						{
						printf( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );
						return 0;
						}
					}
				}

			return 3+numberofElements*2;
			}

		case 0x05: //Write Single Coil
			{
			unsigned int startingAddress = data[ 2 ] * 256 + data[ 3 ];
			unsigned int coilgroup       = data[ 0 ];
			int value                    = data[ 4 ] > 0 ? 1 : 0;
			int line = 0;
			unsigned int objnumber = startingAddress;

			if (isMsa)
				{
				switch (coilgroup)
					{
					case C_V:
					case C_M:
					case C_N:
					case C_PUMPS:
					case C_LINE1VALVES:
					case C_LINE2VALVES:
					case C_LINE3VALVES:
					case C_LINE4VALVES:
					case C_LINE5VALVES:
					case C_LINE6VALVES:
					case C_LINE7VALVES:
					case C_LINE8VALVES:
					case C_LINE9VALVES:
						get_device(coilgroup, objnumber)->set_state(data[4] ? ON : OFF);
						break;
					case C_OTHER:
						line = (startingAddress) / 1000;
						if (line > cipline_tech_object::MdlsCNT)
							{
							break;
							}
						objnumber = (startingAddress) - (line) * 1000;
						switch (objnumber)
							{
							case OTHER_SWITCH1:
							case OTHER_SWITCH2:
							case OTHER_SWITCH3:
							case OTHER_SWITCH4:
								cipline_tech_object::Mdls[line - 1]->setSwitch(objnumber + 1 - OTHER_SWITCH1, data[4] ? ON : OFF);
								break;
							}
						break;
					case C_MSA_STATIONPARAMS:
						if (objnumber < cipline_tech_object::Mdls[0]->parpar->get_count())
							{
							cipline_tech_object::Mdls[0]->set_station_par(objnumber, (float)(data[4] ? 1 : 0));
							}
						break;
					case C_MSA_LINE1PARAMS:
					case C_MSA_LINE2PARAMS:
					case C_MSA_LINE3PARAMS:
					case C_MSA_LINE4PARAMS:
					case C_MSA_LINE5PARAMS:
					case C_MSA_LINE6PARAMS:
					case C_MSA_LINE7PARAMS:
					case C_MSA_LINE8PARAMS:
					case C_MSA_LINE9PARAMS:
						if (coilgroup - C_MSA_LINE1PARAMS < (unsigned int)cipline_tech_object::MdlsCNT)
							{
							if (cipline_tech_object::Mdls[coilgroup - C_MSA_LINE1PARAMS]->rt_par_float.get_count() >= objnumber)
								{
								cipline_tech_object::Mdls[coilgroup - C_MSA_LINE1PARAMS]->rt_par_float[objnumber - 1] = (float)(data[4] ? 1 : 0);
								}
							}
						break;
                    default:
                        goto luawritecoils;
					}
				}
			else
				{
                luawritecoils:
				static char has_Lua_write_coils = 0;
				if ( has_Lua_write_coils == 0 )
					{
					lua_getglobal( L, "write_coils" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_write_coils = 2;
						}
					else
						{
						has_Lua_write_coils = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "write_coils".
					}

				if ( has_Lua_write_coils == 2 )
					{
					lua_getglobal( L, "write_coils" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, value );

					int i_line = lua_pcall( L, 3, 0, 0 );

					if ( i_line != 0 )
						{
						printf( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );
						return 0;
						}
					}
				}

			return 6;
			}

		case 0x10: //Force Multiply Registers
			{
			unsigned int startingAddress  = data[2] * 256 + data[3];
			unsigned int numberofElements = data[4] * 256 + data[5];
			unsigned int coilgroup        = data[ 0 ];
			unsigned int i,j;
                        int k;
			int recipe_to_load;
			int line;

			if (isMsa)
				{
				for (i = 0; i < numberofElements; i++)
					{
					unsigned int objnumber = i+startingAddress;
					switch (coilgroup)
						{
						case C_AO:
							get_device(coilgroup, objnumber)->set_value(UnpackFloat(&data[7+i*2]));
							i++;
							break;
						case C_OTHER:
							line = (i + startingAddress) / 1000;
							if (line > cipline_tech_object::MdlsCNT)
								{
								continue;
								}
							objnumber = (i+startingAddress) - line * 1000;
							switch (objnumber)
								{
								case OTHER_CARNO1:
                                    if (cipline_tech_object::Mdls[line - 1]->state == 0)
                                        {
                                        UnicodetoUtf8(cipline_tech_object::Mdls[line - 1]->ncar1, &data[7 + i * 2], 15);
                                        cipline_tech_object::Mdls[line - 1]->objectstats = cipline_tech_object::statsbase->stats_if_exists(cipline_tech_object::Mdls[line - 1]->ncar1, cipline_tech_object::Mdls[line - 1]->emptystats);
                                        }
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CARNO2:
									UnicodetoUtf8(cipline_tech_object::Mdls[line - 1]->ncar2,  &data[7+i*2], 15);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CARNO3:
									UnicodetoUtf8(cipline_tech_object::Mdls[line - 1]->ncar3,  &data[7+i*2], 15);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								case OTHER_CARNO4:
									UnicodetoUtf8(cipline_tech_object::Mdls[line - 1]->ncar4,  &data[7+i*2], 15);
									i+= CAR_NAME_MAX_LENGTH;
									break;
								default:
									break;
								}
							break;
						case C_MSA_RECIPES:
							line = (i + startingAddress) / 3000 + 1;
							if (line > cipline_tech_object::MdlsCNT)
								{
								continue;
								}
							objnumber = (i+startingAddress) - (line - 1) * 3000;
							switch (objnumber)
								{
								case RC_RECIPE_CONTROL:
									k = UnpackInt16(&data[7+i*2]);
									if (1 == k)
										{
										cipline_tech_object::Mdls[line - 1]->lineRecipes->NextRecipe();
										cipline_tech_object::Mdls[line - 1]->rt_par_float[P_CUR_REC] = (float)(cipline_tech_object::Mdls[line - 1]->lineRecipes->getCurrentRecipe() + 1);
										}
									if (2 == k)
										{
										cipline_tech_object::Mdls[line - 1]->lineRecipes->PrevRecipe();
										cipline_tech_object::Mdls[line - 1]->rt_par_float[P_CUR_REC] = (float)(cipline_tech_object::Mdls[line - 1]->lineRecipes->getCurrentRecipe() + 1);
										}
									break;
								case RC_RECIPE_CONTROL_NO:
									k = UnpackInt16(&data[7+i*2]);
									cipline_tech_object::Mdls[line - 1]->rt_par_float[P_CUR_REC] =  (float)k;
									break;
								case RC_SELECT_REC:
									recipe_to_load = UnpackInt16(&data[7+i*2]);
									k = -1;
									for (j = 0; j < (unsigned int)(cipline_tech_object::Mdls[line - 1]->lineRecipes->recipePerLine); j++)
										{
										if (1 == cipline_tech_object::Mdls[line - 1]->lineRecipes->getRecipeValue(j, TRecipeManager::RV_IS_USED))
											{
											k++;
											}
										if (k == recipe_to_load && cipline_tech_object::Mdls[line - 1]->state == 0)
											{
											cipline_tech_object::Mdls[line - 1]->rt_par_float[P_SELECT_REC] = (float)(j + 1);
											break;
											}
										}
									break;
								case RC_EDITED_REC:
									//printf("\n\rEdit recipe. Words - %d", numberofElements);
									UnicodetoUtf8(cipline_tech_object::Mdls[line-1]->lineRecipes->currentRecipeName, &data[7+i*2], 24);
									i+= cipline_tech_object::Mdls[line-1]->lineRecipes->recipeNameLength - 1;
									break;
								case RC_LIST_UPDATE:
									k = UnpackInt16(&data[7+i*2]);
									if (k)
										{
										updateRecFlag[line] = 1;
										confirmRecUpdateCtr[line] = 0;
										}
									else
										{
										(confirmRecUpdateCtr[line])++;
										if (confirmRecUpdateCtr[line] >= MAX_UPDATE_CONFIRMS)
											{
											updateRecFlag[line] = 0;
											confirmRecUpdateCtr[line] = 0;
											}
										}
									break;
								case RC_PRG_SELECT:
									k = UnpackInt16(&data[7+i*2]);
									if (0 == cipline_tech_object::Mdls[line - 1]->state)
										{
										cipline_tech_object::Mdls[line - 1]->rt_par_float[P_SELECT_PRG] = (float)(cipline_tech_object::Mdls[line - 1]->prgNumber[k]);
										}
									break;
								case RC_PRG_UPDATE:
									k = UnpackInt16(&data[7+i*2]);
									if (k)
										{
										updatePrgFlag[line] = 1;
										confirmPrgUpdateCtr[line] = 0;
										}
									else
										{
										(confirmPrgUpdateCtr[line])++;
										if (confirmPrgUpdateCtr[line] >= MAX_UPDATE_CONFIRMS)
											{
											updatePrgFlag[line] = 0;
											confirmPrgUpdateCtr[line] = 0;
											}
										}
									break;
								default:
									if (objnumber >= RC_RECIPE_PAR_START)
										{
										k = (objnumber - RC_RECIPE_PAR_START) / 2;
										if (k <= cipline_tech_object::Mdls[line - 1]->lineRecipes->GetParamsCount())
											{
											cipline_tech_object::Mdls[line - 1]->lineRecipes->setValue(k - 1, UnpackFloat(&data[7+i*2]));
											i++;
											}
										}
									break;
								}

							break;
						case C_MSA_STATIONPARAMS:
							if ((objnumber / 2) <= cipline_tech_object::Mdls[0]->parpar->get_count())
								{
                                if ( G_DEBUG )
                                    {
                                    printf("\n\rWrite Station param %d = %f", objnumber / 2, UnpackFloat(&data[7+i*2]));
                                    }
								cipline_tech_object::Mdls[0]->set_station_par(objnumber / 2, UnpackFloat(&data[7+i*2]));
								}
							i++;
							break;
						case C_MSA_CONTROL:
							line = objnumber / 100;
							objnumber -= 100 * line;
							k = UnpackInt16(&data[7+i*2]);
							switch (objnumber)
								{
								case MSACTRL_COMMAND:
									cipline_tech_object::Mdls[line]->SetCommand(k);
									break;
								default:
									break;
								}
							break;
						case C_MSA_LINE1PARAMS:
						case C_MSA_LINE2PARAMS:
						case C_MSA_LINE3PARAMS:
						case C_MSA_LINE4PARAMS:
						case C_MSA_LINE5PARAMS:
						case C_MSA_LINE6PARAMS:
						case C_MSA_LINE7PARAMS:
						case C_MSA_LINE8PARAMS:
						case C_MSA_LINE9PARAMS:
							if (coilgroup - C_MSA_LINE1PARAMS < (unsigned int)(cipline_tech_object::MdlsCNT))
								{
								if (cipline_tech_object::Mdls[coilgroup - C_MSA_LINE1PARAMS]->rt_par_float.get_count() >= objnumber / 2)
									{
									cipline_tech_object::Mdls[coilgroup - C_MSA_LINE1PARAMS]->rt_par_float[objnumber / 2] = UnpackFloat(&data[7+i*2]);
									}
								}
							i++;
							break;
						default:
                            goto luawriteregisters;
						}
					}
				}
			else
				{
                luawriteregisters:
				static char has_Lua_write_registers = 0;
				if ( has_Lua_write_registers == 0 )
					{
					lua_getglobal( L, "write_holding_registers" );

					if ( lua_isfunction( L, -1 ) )
						{
						has_Lua_write_registers = 2;
						}
					else
						{
						has_Lua_write_registers = 1;
						}

					lua_remove( L, -1 );  // Stack: remove function "write_holding_registers".
					}

				if ( has_Lua_write_registers == 2 )
					{
					lua_getglobal( L, "write_holding_registers" );
					lua_pushnumber( L, coilgroup );
					lua_pushnumber( L, startingAddress );
					lua_pushnumber( L, numberofElements );
					lua_pushlstring( L, ( const char* ) outdata + 7, 2 * numberofElements );

					int i_line = lua_pcall( L, 4, 0, 0 );

					if ( i_line != 0 )
						{
						printf( "Evaluate Modbus service error: %s\n",
							lua_tostring( L, -1 ) );

						lua_pop( L, 1 );
						return 0;
						}
					}
				}

			return 6;
			}
		}

	return 0;
	}

int ModbusServ::ForceBit(int bit, unsigned char* Buf, int toset)
	{
	int bitpos,bytepos;
	bitpos = bit % 8;
	bytepos = bit / 8;
	if (toset)
		{
		Buf[bytepos] = Buf[bytepos] | (1 << bitpos);
		}
	else
		{
		Buf[bytepos] = Buf[bytepos] & (0xFF ^ (1 << bitpos));
		}
	return 0;
	}

int ModbusServ::PackString( char* String, unsigned char* Buf )
	{
	int i = 0;
	while (0 != String[i])
		{
		if (i % 2)
			{
			Buf[i-1] = String[i];
			}
		else
			{
			Buf[i+1] = String[i];
			}
		i++;
		}
	if (i % 2)
		{
		Buf[i-1] = ' ';
		i++;
		}
	return i;
	}

int ModbusServ::PackTime( unsigned long timevar, unsigned char* Buf, int units /*= 0*/ )
	{
	unsigned long temptime;
	unsigned int seconds, minites, hours;
	if (units)
		{
		temptime = timevar;
		}
	else
		{
		temptime = timevar / 1000;
		}
	seconds = temptime % 60;
	minites = (temptime / 60) % 60;
	hours = (temptime / 3600) % 100;
	sprintf((char*)Buf,"%.1hd%.1hd%.1hd::%.1hd%.1hd%.1hd",
		static_cast<short>(hours % 10), static_cast<short>(hours / 10),
		static_cast<short>(minites / 10), static_cast<short>(minites % 10),
		static_cast<short>(seconds % 10), static_cast<short>(seconds / 10));
	return 3;
	}

int ModbusServ::PackFloat( float fvalue, unsigned char* Buf )
	{
	Buf[0] = ((char*)&fvalue)[1];
	Buf[1] = ((char*)&fvalue)[0];
	Buf[2] = ((char*)&fvalue)[3];
	Buf[3] = ((char*)&fvalue)[2];
	return 1;
	}

int ModbusServ::PackWord( unsigned int wvalue,char* Buf )
	{
	Buf[0] = ((char*)&wvalue)[1];
	Buf[1] = ((char*)&wvalue)[0];
	return 0;
	}

int ModbusServ::PackLong( unsigned long lvalue,unsigned char* Buf )
	{
	Buf[0] = ((char*)&lvalue)[1];
	Buf[1] = ((char*)&lvalue)[0];
	Buf[2] = ((char*)&lvalue)[3];
	Buf[3] = ((char*)&lvalue)[2];
	return 1;
	}

float ModbusServ::UnpackFloat( unsigned char* Buf )
	{
	float result = 0;
	((char*)&result)[0] = Buf[1];
	((char*)&result)[1] = Buf[0];
	((char*)&result)[2] = Buf[3];
	((char*)&result)[3] = Buf[2];
	return result;
	}

unsigned int ModbusServ::UnpackWord( unsigned char* Buf )
	{
	unsigned int result = 0;
	((char*)&result)[0] = Buf[1];
	((char*)&result)[1] = Buf[0];
	return result;
	}

unsigned long ModbusServ::UnpackLong( unsigned char* Buf )
	{
	unsigned long result = 0;
	((char*)&result)[0] = Buf[1];
	((char*)&result)[1] = Buf[0];
	((char*)&result)[2] = Buf[3];
	((char*)&result)[3] = Buf[2];
	return result;
	}

int_2 ModbusServ::PackInt16( int_2 wvalue,unsigned char* Buf )
	{
	Buf[0] = ((char*)&wvalue)[1];
	Buf[1] = ((char*)&wvalue)[0];
	return 0;
	}

int_2 ModbusServ::UnpackInt16( unsigned char* Buf )
	{
	int_2 result = 0;
	((char*)&result)[0] = Buf[1];
	((char*)&result)[1] = Buf[0];
	return result;
	}

int ModbusServ::CP1251toUnicode( const char* Input, unsigned char* Buf)
	{
	size_t inputlen = strlen( Input );

	char input_end = 0;
	for ( size_t i = 0; i <= inputlen; i++)
		{
		unsigned char mychar = (unsigned char)Input[i];
		if (i == inputlen)
			{
			input_end = 1;
			}
		if (input_end)
			{
			Buf[i*2] = 0;
			Buf[i*2 + 1] = 0;
			}
		else
			{
			if (mychar < 128)
				{
				((unsigned char*)Buf)[i*2] = 0;
				((unsigned char*)Buf)[i*2 + 1] = mychar;
				}
			else
				{
				((unsigned char*)Buf)[i*2] = UTable[mychar - 128][0];
				((unsigned char*)Buf)[i*2 + 1] = UTable[mychar - 128][1];
				}

			}
        if ( G_DEBUG )
            {
            //printf("\n\r%d. %d %d", i, Buf[i*2], Buf[i*2 + 1]);
            }
		}
	return 0;
	}

void ModbusServ::UpdateRecipes()
	{
	for (int i = 0; i < 11; i++)
		{
		confirmRecUpdateCtr[i] = 0;
		updateRecFlag[i] = 1;
		}
	}

void ModbusServ::UpdatePrograms()
	{
	for (int i = 0; i < 11; i++)
		{
		confirmPrgUpdateCtr[i] = 0;
		updatePrgFlag[i] = 1;
		}
	}

void ModbusServ::UpdateLineRecipes( int line )
	{
	if (line > 0 && line < 11)
		{
		confirmRecUpdateCtr[line] = 0;
		updateRecFlag[line] = 1;
		}
	}

void ModbusServ::UpdateLinePrograms( int line )
	{
	if (line > 0 && line < 11)
		{
		confirmPrgUpdateCtr[line] = 0;
		updatePrgFlag[line] = 1;
		}
	}


int ModbusServ::UnicodetoCP1251( char* Output, unsigned char* Buf, int inputlen )
	{
	int i;
	unsigned char upperbyte;
	unsigned char lowerbyte;
	for (i = 0; i < inputlen; i++)
		{
		upperbyte = Buf[2 * i];
		lowerbyte = Buf[2 * i + 1];
        if ( G_DEBUG )
            {
            printf("\n\r%d symbol codes: %d %d", i + 1, upperbyte, lowerbyte);
            }

		if (i == inputlen - 1 || (0 == upperbyte && 0 == lowerbyte))
			{
			Output[i] = 0;
			break;
			}
		else
			{
			if (0 == upperbyte && lowerbyte < 128)
				{
				Output[i] = lowerbyte;
				}
			else
				{
				for (int j = 0; j<128; j++)
					{
					if (UTable[j][0] == upperbyte && UTable[j][1] == lowerbyte)
						{
						Output[i] = 128 + j;
						break;
						}
					}
				}

			}
		}
	return i/2 - 1;
	}

int ModbusServ::Utf8toUnicode(const char* Input, unsigned char* Buf)
    {
	char outbuf[MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER] = { 0 };
	convert_utf8_to_windows1251(Input, outbuf,
        MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER,
        MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER );
	return CP1251toUnicode(outbuf, Buf);
    }

int ModbusServ::UnicodetoUtf8(char* Output, unsigned char* Buf, int inputlen)
    {
	char inbuf[MAX_REC_NAME_LENGTH * UNICODE_MULTIPLIER] = { 0 };
	UnicodetoCP1251(inbuf, Buf, inputlen);
	convert_windows1251_to_utf8(Output, inbuf);
	return 0;
    }

device* ModbusServ::get_device( unsigned int group, unsigned int number )
	{
	unsigned int line;
	device* ret = G_DEVICE_MANAGER()->get_stub_device();
	line = number / 100;
	char devname[20] = {0};
	switch (group)
		{
		case C_V:
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT && number % 100 < 14)
			{
			if (cipline_tech_object::Mdls[line - 1]->is_old_definition)
				{
				sprintf(devname, "LINE%dV%d", line, number);
				}
			else
				{
				sprintf(devname, "LINE%dV%d", line, number % 100);
				}
			ret = (device*)V(devname);
			}
			break;
		case C_M:
		case C_N:
			switch (number % 100)
				{
				case 1:
					ret = (device*)cipline_tech_object::Mdls[0]->NS;
					break;
				case 2:
					ret = (device*)cipline_tech_object::Mdls[0]->NK;
					break;
				case 3:
					if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
						{
						ret = (device*)cipline_tech_object::Mdls[line - 1]->NP;
						}
					break;
				}
			break;
		case C_LS:
			switch (number)
				{
				case 4:
					ret = (device*)cipline_tech_object::Mdls[0]->LSH;
					break;
				case 5:
					ret = (device*)cipline_tech_object::Mdls[0]->LSL;
					break;
				case 6:
					ret = (device*)cipline_tech_object::Mdls[0]->LKH;
					break;
				case 7:
					ret = (device*)cipline_tech_object::Mdls[0]->LKL;
					break;
				case 8:
					ret = (device*)cipline_tech_object::Mdls[0]->LWH;
					break;
				case 9:
					ret = (device*)cipline_tech_object::Mdls[0]->LWL;
					break;
				default:
					if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
						{
						switch (number % 100)
							{
							case 1:
								ret = (device*)cipline_tech_object::Mdls[line-1]->LH;
								break;
							case 2:
								ret = (device*)cipline_tech_object::Mdls[line-1]->LM;
								break;
							case 3:
								ret = (device*)cipline_tech_object::Mdls[line-1]->LL;
								break;
							}
						}
					break;
				}

			break;
		case C_FS:
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
				{
				ret = (device*)cipline_tech_object::Mdls[line-1]->FL;
				}
			break;
		case C_AO:
			line = line / 2;
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
				{
				switch (number % 100)
					{
					case 2:
						ret = (device*)cipline_tech_object::Mdls[line-1]->NP;
						break;
					case 28:
						ret = (device*)cipline_tech_object::Mdls[line-1]->ao;
						break;
					}
				}
			break;
		case C_FE:
		case C_CTR:
			line = line / 2;
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
				{
				ret = (device*)cipline_tech_object::Mdls[line-1]->cnt;
				}
			break;
		case C_TE:
			line = line / 2;
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
				{
				switch (number % 100)
					{
					case 2:
						ret = (device*)cipline_tech_object::Mdls[line-1]->TP;
						break;
					case 4:
						ret = (device*)cipline_tech_object::Mdls[line-1]->TR;
						break;
					}
				}
			break;
		case C_QE:
			line = line / 2;
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
				{
				ret = (device*)cipline_tech_object::Mdls[line-1]->Q;
				}
			break;
		case C_LE:
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT)
				{
				switch (number % 100)
					{
					case 0:
						ret = (device*)cipline_tech_object::Mdls[line-1]->LTS;
						break;
					case 2:
						ret = (device*)cipline_tech_object::Mdls[line-1]->LTK;
						break;
					case 4:
						ret = (device*)cipline_tech_object::Mdls[line-1]->LTW;
						break;
					}
				}
			break;
		case C_PUMPS:
			line = number / 1000;
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT && (number % 1000 == 1))
				{
				sprintf(devname, "LINE%dM%d", line, number % 1000 + 1000);
				ret = (device*)M(devname);
				}
			break;
		case C_LINE1VALVES:
		case C_LINE2VALVES:
		case C_LINE3VALVES:
		case C_LINE4VALVES:
		case C_LINE5VALVES:
		case C_LINE6VALVES:
		case C_LINE7VALVES:
		case C_LINE8VALVES:
		case C_LINE9VALVES:
			line = group - C_LINE1VALVES + 1;
			if (line > 0 && line <= (unsigned int)cipline_tech_object::MdlsCNT && (number == 1001 || number == 1002 || 1004 == number || 1006 == number ||
				(number / 1000 == 1 && (number - 1000) % 10 <= 2 && (number - 1000) / 10 <= 4 )))
				{
				sprintf(devname, "LINE%dV%d", line, number);
				ret = (device*)V(devname);
				}
			break;
		}
		return ret;
	}
