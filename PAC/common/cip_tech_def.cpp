#ifdef WIN_OS
#pragma warning(push)
#pragma warning(disable: 4244)
#endif // WIN_OS

#include "fmt/format.h"

#include "cip_tech_def.h"
#include "lua_manager.h"
#include "utf2cp1251.h"

const std::map<int, const char*> ERR_MSG =
    {
    { ERR_UNKNOWN_STEP, "Неизвестный шаг" },
    { ERR_POSSIBLE_NO_MEDIUM, "Возможно отсутствует концентрированный раствор" },
    { ERR_NO_ACID, "Нет моющего раствора кислоты в танке" },
    { ERR_NO_ALKALINE, "Нет моющего раствора щелочи в танке" },
    { ERR_NO_RETURN, "Нет расхода в возвратной трубе" },
    { ERR_NO_CONC, "Нет концентрации в возвратной трубе" },
    { ERR_IS_CONC, "Высокая концентрация в возвратной трубе" },
    { ERR_WRONG_RET, "Ошибка возвратного насоса" },
    { ERR_PUMP, "Ошибка подающего насоса" },
    { ERR_NO_FLOW, "Нет расхода на подаче" },
    { ERR_AIR, "Нет воздуха" },
    { ERR_OS, "Ошибка объекта CIP" },
    { ERR_CIP_OBJECT, "Прерывание со стороны клиента" },
    { ERR_WRONG_OS_OR_RECIPE_ERROR, "Несуществующая обратная связь ошибка в рецепте" },
    { ERR_VALVES_ARE_IN_CONFLICT, "Конфликт клапанов в рецептах" },
    { ERR_ACID_WASH_REQUIRED, "Требуется мойка кислотой" },
    { ERR_LEVEL_BACHOK, "Ошибка уровня в бачке" },
    { ERR_LEVEL_TANK_S, "Ошибка уровня танка щелочи" },
    { ERR_LEVEL_TANK_K, "Ошибка уровня танка кислоты" },
    { ERR_LEVEL_TANK_W, "Ошибка уровня танка вторичной воды" },
    { ERR_SUPPLY_TEMP_SENSOR, "Ошибка температуры на подаче" },
    { ERR_RETURN_TEMP_SENSOR, "Ошибка температуры на возврате" },
    { ERR_CONCENTRATION_SENSOR, "Ошибка концентрации в возвратной трубе" },
    { ERR_WATCHDOG, "Ошибка сторожевого таймера" },

    { ERR_NO_DESINFECTION_MEDIUM, "Нет дезинфицирующего средства" },
    { ERR_DESINFECTION_MEDIUM_MAX_TIME,
        "Превышено максимальное время дозирования дезинфицирующего средства" },
    { ERR_DESINFECTION_MEDIUM_INSUFFICIENT_TIME,
        "Недостаточное время дозирования дезинфицирующего средства" },

    { ERR_RET, "Ошибка возвратного насоса" }
    };

const char* const UNKNOWN_ERR_MSG = "неизвестная ошибка";

TMediumRecipeManager* cipline_tech_object::causticRecipes = nullptr;

char* cipline_tech_object::causticName = nullptr;

int cipline_tech_object::causticLoadedRecipe = -1;

TMediumRecipeManager* cipline_tech_object::acidRecipes = nullptr;

char* cipline_tech_object::acidName = nullptr;

int cipline_tech_object::acidLoadedRecipe = -1;

cip_stats* cipline_tech_object::statsbase = nullptr;

std::unordered_set<int> cipline_tech_object::steps_v2_supply = { 7, 35, 55, 64, 85 };

std::unordered_set<int> cipline_tech_object::steps_additional_rinse = { 8, 37, 57, 86 };

std::unordered_set<int> cipline_tech_object::steps_circulation = { 28, 48, 66, 76, 77 };

std::unordered_set<int> cipline_tech_object::steps_caustic = { 22, 23, 24, 26, 28, 29, 31, 33, 34, 35 };

std::unordered_set<int> cipline_tech_object::steps_acid = { 42, 43, 44 , 46, 48, 49, 53, 54, 55 };

int isMsa = 0;

int getNexpPrg(int cur, unsigned long prg)
    {
    int i;
    int tmp;

    if (cur<0)
        {
        tmp=15;
        }
    else
        {
        tmp=cur-1;
        }
    if (tmp>=0 && tmp<16)
        {
        for (i=tmp; i>=0; i--)
            {
            if (((prg>>i) & 1) == 1)
                {
                return i;
                }
            }
        return -1;
        }
    return -1;
    }




cipline_tech_object::cipline_tech_object(const char* name, u_int number, u_int type,
    const char* name_Lua, u_int states_count, u_int timers_count,
    u_int par_float_count, u_int runtime_par_float_count, u_int par_uint_count,
    u_int runtime_par_uint_count) : tech_object(name, number, type,
    name_Lua, states_count, timers_count, par_float_count, runtime_par_float_count,
    par_uint_count, runtime_par_uint_count)
    {
    if (G_DEBUG)
        {
        printf("\n\r Create cip tech_object\n\r");
        }
    int i;
    nmr = number;
    tech_type = type;
    for ( i = 0; i < panelMaxPrograms; i++ ) prgArray.emplace_back( "" );
    if (nullptr == parpar)
        {
        parpar = new saved_params<float, true>(STATION_PAR_COUNT, "PAR_MAIN");
        }
    if (nullptr == causticRecipes)
        {
        causticRecipes = new TMediumRecipeManager(TMediumRecipeManager::MT_CAUSTIC);
        }
    if (nullptr == acidRecipes)
        {
        acidRecipes = new TMediumRecipeManager(TMediumRecipeManager::MT_ACID);
        }
    if (nullptr == causticName)
        {
        causticName = new char[TMediumRecipeManager::recipeNameLength];
        strcpy(causticName, "");
        }
    if (nullptr == acidName)
        {
        acidName = new char[TMediumRecipeManager::recipeNameLength];
        strcpy(acidName, "");
        }
    if (nullptr == statsbase)
        {
        statsbase = new cip_stats();
        statsbase->loadFromFile(statsbase->filename);
        }
    emptystats = new cip_object_stats("emptyname");
    objectstats = emptystats;
    for (i = 0; i < TMR_CNT; i++)
        {
        T[i]=new timer;
        }
    for (i = 0; i < SAV_CNT; i++)
        {
        SAV[i]=new TSav;
        }
    program_has_acid = false;
    program_has_caustic = false;
    program_has_cold_desinfection = false;
    program_has_desinfection = false;
    program_was_terminated = false;
    is_reset = false;
    no_acid_wash_max = 0;
    pidf_override = false;
    nplaststate = false;
    flagnplaststate = false;
    no_neutro = 0;
    dont_use_water_tank = 0;
    ret_circ_flag = 0;
    ret_circ_delay = get_millisec();
    ret_overrride = 0;
    concentration_ok = 0;
    return_ok = 0;
    enable_ret_pump = 0;
    enddelayTimer = 0;
    blocked = 0;
    disable_tank_heating = 0;
    default_programlist = 0x13FB;
    use_internal_medium_recipes = false;
    use_circulation_on_v2_supply = false;
    bachok_lvl_err_delay = get_millisec();
    steam_valve_delay = get_millisec();
    loadedRecName = new char[TRecipeManager::recipeNameLength * UNICODE_MULTIPLIER];
    currentProgramName = new char[PROGRAM_MAX_LEN * UNICODE_MULTIPLIER];
    strcpy(currentProgramName, "");
    ncar1 = new char[CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER];
    strcpy(ncar1, "");
    ncar2 = new char[CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER];
    strcpy(ncar2, "");
    ncar3 = new char[CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER];
    strcpy(ncar3, "");
    ncar4 = new char[CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER];
    strcpy(ncar4, "");
    switch1 = 0;
    switch2 = 0;
    switch3 = 0;
    switch4 = 0;

    lineRecipes = new TRecipeManager(number - 1);
    opcip=0;
    curstep=0;
    ncmd=0;
    state=0;
    loadedRecipe = -1;
    lastloadedRecipe = -1;
    loadedProgram = -1;
    curprg=-1;
    cip_in_error = 0;

    resetProgramName();
    resetRecipeName();
    resetProgramList();
    ResetStat();
    rt_par_float[P_CUR_REC] =  (float)(lineRecipes->getCurrentRecipe() + 1);
    rt_par_float[P_SELECT_REC] = 0;
    rt_par_float[P_SELECT_PRG] = 0;

    dev_upr_ret = nullptr;
    dev_m_ret = nullptr;
    dev_os_object = nullptr;
    dev_os_object_ready = nullptr;
    dev_os_cip_ready = nullptr;
    dev_os_object_empty = nullptr;
    dev_upr_medium_change = nullptr;
    dev_upr_caustic = nullptr;
    dev_upr_acid = nullptr;
    dev_upr_water = nullptr;
    dev_upr_desinfection = nullptr;
    dev_upr_cip_ready = nullptr;
    dev_upr_cip_finished = nullptr;
    dev_upr_cip_finished2 = nullptr;
    dev_upr_cip_in_progress = nullptr;
    dev_ai_pump_frequency = nullptr;
    dev_ai_pump_feedback = nullptr;
    dev_upr_sanitizer_pump = nullptr;
    dev_os_object_pause = nullptr;
    dev_upr_circulation = nullptr;
    dev_watchdog = nullptr;
    dev_os_pump_can_run = nullptr;
    dev_os_can_continue = nullptr;
    dev_ls_ret_pump = nullptr;

    dev_upr_prerinse = nullptr;
    dev_upr_intermediate_rinse = nullptr;
    dev_upr_postrinse = nullptr;
    dev_upr_pump_stopped = nullptr;
    dev_ao_flow_task = nullptr;
    dev_ao_temp_task = nullptr;
    dev_upr_wash_aborted = nullptr;

    pumpflag = 0;
    pumptimer = get_millisec();

    tankempty = 0;
    tankemptytimer = get_millisec();
    tankfull = 0;
    tankfulltimer = get_millisec();
    sort_last_destination = -1;
    sort_delay = get_millisec();

    forcesortrr = 0;

    //Переменные для циркуляции
    circ_tank_s = 0;
    circ_tank_k = 0;
    circ_podp_water = 0;
    circ_podp_s = 0;
    circ_podp_k = 0;
    circ_podp_count = 0;
    circ_podp_max_count = 0;
    circ_water_no_pump_stop = 0;
    circ_medium_no_pump_stop = 0;
    circ_max_timer = get_millisec();
    circ_return_timer = get_millisec();

    is_in_evaluate_func = 0;
    is_in_error_func = 0;
    is_InitCustomStep_func = 0;
    is_DoCustomStep_func = 0;
    is_GoToStep_func = 0;
    is_DoStep_func = 0;
    is_InitStep_func = 0;
    is_LoadProgram_func = 0;
    is_StopDev_func = 0;
    is_ResetLinesDevicesBeforeReset_func = 0;
    is_OporCIP_func = 0;
    is_InitOporCIP_func = 0;
    is_CheckErr_func = 0;
    is_Circ_func = 0;
    is_InitCirc_func = 0;
    is_ToObject_func = 0;
    is_InitToObject_func = 0;
    is_FromObject_func = 0;
    is_InitFromObject_func = 0;
    is_FillCirc_func = 0;
    is_InitFillCirc_func = 0;
    is_OporCirc_func = 0;
    is_InitOporCirc_func = 0;
    is_RT_func = 0;
    is_Stop_func = 0;
    is_DoseRR_func = 0;
    is_InitDoseRR_func = 0;
    is_On_Resume_func = 0;
    is_ConfigureLine_func = 0;

    //для ошибки "возможно отсутствует концентрированный раствор"
    no_liquid_is_warning = 0;
    no_liquid_phase = 0;
    no_liquid_last_time = 0;

    clean_water_rinsing_return = TANK_W; //по-умолчанию возвращаем в танк со вторичной водой.
    disable_final_rinsing = false;

    if (tech_type == TECH_TYPE_SELF_CLEAN || tech_type == TECH_TYPE_CAR_WASH_SELF_CLEAN)
        {
        scenabled = 1;
        scline = 0;
        resetProgramList();
        VSDREN = V(1);
        VKDREN = V(2);
        VWDREN = V(3);
        VSMG = V(11);
        VKMG = V(12);
        VWMG = V(13);
        if (nullptr == scparams)
            {
            scparams = new saved_params<float, true>(SELFCLEAN_PAR_COUNT, "PAR_SELFCLEAN");
            }
        }
    else
        {
        scparams = parpar;
        }
    }

cipline_tech_object::~cipline_tech_object()
    {
    int i;
    if (parpar != nullptr)
        {
        if ( scparams && scparams == parpar )
            {
            scparams = nullptr;
            // Сама память высвобождается при удалении parpar - строки ниже.
            }

        delete(parpar);
        parpar = nullptr;
        }

    if (PIDF != nullptr)
        {
        delete(PIDF);
        PIDF = nullptr;
        }
    if (PIDP != nullptr)
        {
        delete(PIDP);
        PIDP = nullptr;
        }
    for (i = 0; i < SAV_CNT; i++) {
        delete SAV[i];
        SAV[i] = nullptr;
        }
    for (i = 0; i < TMR_CNT; i++) {
        delete T[i];
        T[i] = nullptr;
        }
    if (lineRecipes)
        {
        delete lineRecipes;
        lineRecipes = nullptr;
        }
    if (causticRecipes)
        {
        delete causticRecipes;
        causticRecipes = nullptr;
        }
    if (acidRecipes)
        {
        delete acidRecipes;
        acidRecipes = nullptr;
        }
    if (acidName)
        {
        delete[] acidName;
        acidName = nullptr;
        }
    if (causticName)
        {
        delete[] causticName;
        causticName = nullptr;
        }
    if (statsbase)
    {
        statsbase->clear();
        delete statsbase;
        statsbase = nullptr;
    }
    objectstats = nullptr;
    delete emptystats;
    emptystats = nullptr;
    delete[] loadedRecName;
    loadedRecName= nullptr;
    delete[] currentProgramName;
    currentProgramName = nullptr;
    }

int cipline_tech_object::save_device( char *buff ) const
    {
    int i;
    int answer_size = sprintf( buff, "t.%s = t.%s or {}\nt.%s=\n\t{\n",
        name_Lua, name_Lua, name_Lua );

    //Команда
    answer_size += sprintf( buff + answer_size, "\tCMD=%d,\n", ncmd );

    //Состояние
    answer_size += sprintf( buff + answer_size, "\tSTATE=%d,\n", state );

    //Текущая операция
    answer_size += sprintf( buff + answer_size, "\tOPER=%d,\n", curstep );

    //Загруженный рецепт
    answer_size += sprintf(buff + answer_size, "\tLOADED_REC='%s',\n", loadedRecName);

    //Имя рецепта для редактирования
    answer_size += sprintf(buff + answer_size, "\tCUR_REC='%s',\n", lineRecipes->currentRecipeName);

    //Номер последнего загруженного рецепта
    answer_size += sprintf( buff + answer_size, "\tLASTRECNMR=%d,\n", lastLoadedRecipeNmr );

    //Имя последнего загруженного рецепта
    answer_size += sprintf( buff + answer_size, "\tLASTRECNAME='%s',\n", lastLoadedRecipeName.c_str() );

    //Выбранная программа мойки
    answer_size += sprintf(buff + answer_size, "\tCUR_PRG='%s',\n", currentProgramName);

    //Список доступных программ мойки
    answer_size += sprintf(buff + answer_size, "\tPRG_LIST='%s',\n", programList.data());

    //Список доступных объектов мойки
    answer_size += sprintf(buff + answer_size, "\tREC_LIST='%s',\n", lineRecipes->recipeList);

    //Номер машины
    answer_size += sprintf(buff + answer_size, "\tNCAR='%s',\n", ncar1);
    answer_size += sprintf(buff + answer_size, "\tNCAR1='%s',\n", ncar1);
    answer_size += sprintf(buff + answer_size, "\tNCAR2='%s',\n", ncar2);
    answer_size += sprintf(buff + answer_size, "\tNCAR3='%s',\n", ncar3);
    answer_size += sprintf(buff + answer_size, "\tNCAR4='%s',\n", ncar4);
    answer_size += sprintf(buff + answer_size, "\tSWITCH1=%d,\n", switch1);
    answer_size += sprintf(buff + answer_size, "\tSWITCH2=%d,\n", switch2);
    answer_size += sprintf(buff + answer_size, "\tSWITCH3=%d,\n", switch3);
    answer_size += sprintf(buff + answer_size, "\tSWITCH4=%d,\n", switch4);

    //Время простоя.
    char up_time_str [ 50 ];

    auto up_secs = operations_manager->get_idle_time_sec();
    auto up_hours = up_secs / ( 60 * 60 );
    auto up_mins = up_secs / 60 % 60 ;
    up_secs %= 60;

    sprintf( up_time_str, "\tIDLE_TIME = \'%02lu:%02lu:%02lu\',\n",
        up_hours, up_mins, up_secs );
    answer_size += sprintf( buff + answer_size, "%s", up_time_str );

    //Параметры.
    answer_size += par_float.save_device( buff + answer_size, "\t" );
    answer_size += par_uint.save_device( buff + answer_size, "\t" );
    answer_size += rt_par_float.save_device( buff + answer_size, "\t" );
    answer_size += rt_par_uint.save_device( buff + answer_size, "\t" );
    //Параметры станции
    if (1 == number)
        {
        answer_size += parpar->save_device(buff + answer_size, "\t");
        if (scenabled)
            {
            answer_size += scparams->save_device(buff + answer_size, "\t");
            }
        }
    //Параметры текущего рецепта
    answer_size += sprintf(buff + answer_size, "\tREC_PAR = \n\t{\n\t\t");
    for (i = 1; i <= lineRecipes->GetParamsCount(); i++)
        {
        answer_size += sprintf(buff + answer_size, "%.2f, ", lineRecipes->getValue(i-1));
        }
    answer_size += sprintf(buff + answer_size, "\n\t},\n");

    //Статистика по мойке выбранного объекта
    answer_size += sprintf(buff + answer_size, "\tOBJSTATS_LASTWASH='%s',\n", objectstats->objlastwash);
    answer_size += sprintf(buff + answer_size, "\tOBJSTATS_LASTPROGRAM='%s',\n", objectstats->objlastwashprogram);
    answer_size += sprintf(buff + answer_size, "\tOBJSTATS_CAUSTICCOUNT=%d,\n", objectstats->objcausticwashes);
    answer_size += sprintf(buff + answer_size, "\tOBJSTATS_LASTACIDWASH='%s',\n", objectstats->objlastacidwash);


    //Выбор моющих средств
    if ( nmr == FIRST_CIPLINE_OBJECT_NUMBER )
    {
        //Список доступных щелочных растворов
        answer_size += sprintf(buff + answer_size, "\tCAUSTIC_REC_LIST='%s',\n", causticRecipes->recipeList);
        answer_size += sprintf(buff + answer_size, "\tCAUSTIC_REC_NMR='%d',\n", causticRecipes->getCurrentRecipe());
        answer_size += sprintf(buff + answer_size, "\tCAUSTICNAME='%s',\n", causticName);
        answer_size += sprintf(buff + answer_size, "\tCAUSTIC_PAR_NAME='%s',\n", causticRecipes->currentRecipeName);
        answer_size += sprintf(buff + answer_size, "\tCAUSTIC_PAR = \n\t{\n\t\t");
        for (i = 1; i <= causticRecipes->GetParamsCount(); i++)
            {
            answer_size += sprintf(buff + answer_size, "%.2f, ", causticRecipes->getValue(i - 1));
            }
        answer_size += sprintf(buff + answer_size, "\n\t},\n");

        //Список доступных кислотных растворов
        answer_size += sprintf(buff + answer_size, "\tACID_REC_LIST='%s',\n", acidRecipes->recipeList);
        answer_size += sprintf(buff + answer_size, "\tACID_REC_NMR='%d',\n", acidRecipes->getCurrentRecipe());
        answer_size += sprintf(buff + answer_size, "\tACIDNAME='%s',\n", acidName);
        answer_size += sprintf(buff + answer_size, "\tACID_PAR_NAME='%s',\n", acidRecipes->currentRecipeName);
        answer_size += sprintf(buff + answer_size, "\tACID_PAR = \n\t{\n\t\t");
        for (i = 1; i <= acidRecipes->GetParamsCount(); i++)
            {
            answer_size += sprintf(buff + answer_size, "%.2f, ", acidRecipes->getValue(i - 1));
            }
        answer_size += sprintf(buff + answer_size, "\n\t}\n");
    }

    answer_size += sprintf( buff + answer_size, "\t}\n" );
    return answer_size;
    }

int cipline_tech_object::set_cmd( const char *prop, u_int idx, double val )
    {
    if ( strcmp( prop, "S_PAR_F" ) == 0 )
        {
        par_float.save( idx, ( float ) val );
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_F" ) == 0 )
        {
        rt_par_float[ idx ] = ( float ) val;
        if (idx >= PIDP_Z && idx <= PIDF_Uk)
            {
            if (state && loadedRecipe >= 0)
                {
                lineRecipes->setRecipeValue(loadedRecipe, idx - PIDP_Z + TRecipeManager::RV_PIDP_Z, float(val));
                }
            }
        return 0;
        }

    if ( strcmp( prop, "PAR_MAIN" ) == 0 )
        {
        if (idx == P_CAUSTIC_SELECTED)
        {
            causticLoadedRecipe = -1;
        }
        if (idx == P_ACID_SELECTED)
        {
            acidLoadedRecipe = -1;
        }
        parpar->save( idx, ( float ) val );
        return 0;
        }

    if ( strcmp( prop, "PAR_SELFCLEAN" ) == 0 )
        {
        scparams->save( idx, ( float ) val );
        return 0;
        }

    if ( strcmp( prop, "S_PAR_UI" ) == 0 )
        {
        par_uint.save( idx, ( u_int_4 ) val );
        return 0;
        }

    if ( strcmp( prop, "RT_PAR_UI" ) == 0 )
        {
        rt_par_uint[ idx ] = ( u_int_4 ) val;
        return 0;
        }

    if ( strcmp( prop, "REC_PAR" ) == 0 )
        {
        lineRecipes->setValue(idx - 1, (float)val);
        return 0;
        }

    if (strcmp(prop, "CAUSTIC_PAR") == 0)
        {
        causticRecipes->setValue(idx - 1, (float)val);
        return 0;
        }

    if (strcmp(prop, "ACID_PAR") == 0)
        {
        acidRecipes->setValue(idx - 1, (float)val);
        return 0;
        }

    if ( strcmp( prop, "CMD" ) == 0 )
        {
        ncmd = (u_int_4) val;
        return 0;
        }

    if (0 == strcmp(prop, "NCAR"))
        {
        switch (idx)
            {
            case 0:
            case 1:
                sprintf(ncar1, "%d", (int)val);
                objectstats = statsbase->stats_if_exists(ncar1, emptystats);
                break;
            case 2:
                sprintf(ncar2, "%d", (int)val);
                break;
            case 3:
                sprintf(ncar3, "%d", (int)val);
                break;
            case 4:
                sprintf(ncar4, "%d", (int)val);
                break;
            }

        return 0;
        }


    if ( strcmp( prop, "SWITCH" ) == 0 )
        {
        setSwitch(idx, (int) val);
        return 0;
        }




#ifdef DEBUG
    printf( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %f\n",
        prop, idx, val );
#endif // DEBUG

    return 1;
    }

int cipline_tech_object::set_cmd( const char *prop, u_int idx, const char* val )
    {
    if (0 == strcmp(prop, "CUR_REC"))
        {
        // LCOV_EXCL_START
        auto res = fmt::format_to_n(
            lineRecipes->currentRecipeName,
            TRecipeManager::recipeNameLength * UNICODE_MULTIPLIER - 1,
            "{}", val);
        // LCOV_EXCL_STOP
        *res.out = '\0';
        return 0;
        }

    if (0 == strcmp(prop, "CAUSTIC_PAR_NAME"))
        {
        // LCOV_EXCL_START
        auto res = fmt::format_to_n(
            causticRecipes->currentRecipeName,
            TMediumRecipeManager::recipeNameLength * UNICODE_MULTIPLIER - 1,
            "{}", val);
        // LCOV_EXCL_STOP
        *res.out = '\0';
        return 0;
        }

    if (0 == strcmp(prop, "ACID_PAR_NAME"))
        {
        // LCOV_EXCL_START
        auto res = fmt::format_to_n(
            acidRecipes->currentRecipeName,
            TMediumRecipeManager::recipeNameLength * UNICODE_MULTIPLIER - 1,
            "{}", val);
        // LCOV_EXCL_STOP
        *res.out = '\0';
        return 0;
        }

    if (0 == strcmp(prop, "NCAR"))
        {
        switch (idx)
            {
            case 0:
            case 1:
                {
                auto res = fmt::format_to_n(ncar1,
                    CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER - 1, "{}", val);
                *res.out = '\0';
                objectstats = statsbase->stats_if_exists(ncar1, emptystats);
                break;
                }
            case 2:
                {
                auto res = fmt::format_to_n(ncar2,
                    CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER - 1, "{}", val);
                *res.out = '\0';
                break;
                }
            case 3:
                {
                auto res = fmt::format_to_n(ncar3,
                    CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER - 1, "{}", val);
                *res.out = '\0';
                break;
                }
            case 4:
                {
                auto res = fmt::format_to_n(ncar4,
                    CAR_NAME_MAX_LENGTH * UNICODE_MULTIPLIER - 1, "{}", val);
                *res.out = '\0';
                break;
                }
            }

        return 0;
        }

#ifdef DEBUG
    printf( "Eror tech_object::set_cmd(...), prop = \"%s\", idx = %u, val = %s\n",
        prop, idx, val );
#endif // DEBUG
    return 1;
    }

int cipline_tech_object::evaluate()
    {
    int res;
    if (EvalBlock())
        {
        return -1;
        }

    if (dev_upr_cip_in_progress)
        {
        if (state > 0)
            {
            dev_upr_cip_in_progress->on();
            }
        else
            {
            dev_upr_cip_in_progress->off();
            }
        }

    if ( nmr == FIRST_CIPLINE_OBJECT_NUMBER )
    {
        statsbase->evaluate();
    }
    EvalRecipes();
    EvalCipReadySignal();

    if (ncmd!=0)
        {
        EvalCommands();
        ncmd=0;
        }
    else
        {

        if (2 == is_in_evaluate_func)
            {
            lua_State* L = lua_manager::get_instance()->get_Lua();
            lua_getglobal( L, name_Lua );
            lua_getfield( L, -1, "cip_in_evaluate" );
            lua_remove( L, -2 );  // Stack: remove OBJECT.
            lua_getglobal( L, name_Lua );
            if (0 != lua_pcall(L, 1, 0, 0))
                {
                printf("Error in calling cip_in_evaluate: %s\n", lua_tostring(L, -1));
                lua_pop(L, 1);
                }
            }


        if (state>0)
            {
            EvalPIDS();
            res = EvalCipInProgress();
            if (res < 0)
                {
                return res;
                }
            cip_in_error = 0;
            }
        else
            {
            if (state < 0)
                {
                EvalCipInError();
                if (!cip_in_error)
                    {
                    rt_par_float[STP_ERRCOUNT] = rt_par_float[STP_ERRCOUNT] + 1;
                    cip_in_error = 1;
                    }
                }
            }
        }
    return 0;
    }

int cipline_tech_object::init_params()
    {
    tech_object::init_params();

    if ( number == FIRST_CIPLINE_OBJECT_NUMBER )
        {
        parpar->reset_to_0();
        if ( scparams != parpar ) scparams->reset_to_0();
        }
    return 0;
    }

int cipline_tech_object::init_runtime_params()
    {
    tech_object::init_runtime_params();

    rt_par_float[ P_R_NO_FLOW ] = 2;
    rt_par_float[ P_TM_R_NO_FLOW ] = 20;
    rt_par_float[ P_TM_NO_FLOW_R ] = 20;
    rt_par_float[ P_TM_NO_CONC ] = 20;
    rt_par_float[ PIDP_Z ] = 95;
    rt_par_float[ PIDP_k ] = 2;
    rt_par_float[ PIDP_Ti ] = 30;
    rt_par_float[ PIDP_Td ] = (float)0.2;
    rt_par_float[ PIDP_dt ] = 500;
    rt_par_float[ PIDP_dmax ] = 130;
    rt_par_float[ PIDP_dmin ] = 0;
    rt_par_float[ PIDP_AccelTime ] = 30;
    rt_par_float[ PIDP_IsManualMode ] = 0;
    rt_par_float[ PIDP_UManual ] = 30;
    rt_par_float[ PIDP_Uk ] = 0;
    rt_par_float[ PIDF_Z ] = 15;
    rt_par_float[ PIDF_k ] = (float)0.5;
    rt_par_float[ PIDF_Ti ] = 10;
    rt_par_float[ PIDF_Td ] = (float)0.1;
    rt_par_float[ PIDF_dt ] = 1000;
    rt_par_float[ PIDF_dmax ] = 40;
    rt_par_float[ PIDF_dmin ] = 0;
    rt_par_float[ PIDF_AccelTime ] = 2;
    rt_par_float[ PIDF_IsManualMode ] = 0;
    rt_par_float[ PIDF_UManual ] = 15;
    rt_par_float[ PIDF_Uk ] = 0;
    return 0;
    }

void cipline_tech_object::initline()
    {
    is_old_definition = 1;
    char devname[20] = {0};
    char devname2[20] = {0};
    G_DEVICE_MANAGER()->disable_error_logging = true;
    sprintf(devname, "LINE%dV%d", number, number * 100);
    sprintf(devname2, "LINE%dV%d", number, number * 100 + 11);
    if (((device*)V(devname))->get_type() == device::DT_NONE || ((device*)V(devname2))->get_type() == device::DT_NONE)
        {
        is_old_definition = 0;
        }
        G_DEVICE_MANAGER()->disable_error_logging = false;
    if (is_old_definition)
        {
        sprintf(devname, "LINE%dV%d", number, number * 100);
        V00 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 1);
        V01 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 2);
        V02 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 3);
        V03 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 4);
        V04 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 5);
        V05 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 6);
        V06 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 7);
        V07 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 8);
        V08 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 9);
        V09 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 10);
        V10 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 11);
        V11 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 12);
        V12 = V(devname);
        sprintf(devname, "LINE%dV%d", number, number * 100 + 13);
        V13 = V(devname);

        sprintf(devname, "LINE%dM%d", number, number * 100 + 3);
        NP = M(devname);
        NK = M(2);
        NS = M(1);
        sprintf(devname, "LINE%dLS%d", number, number * 100 + 3);
        LL = LS(devname);
        sprintf(devname, "LINE%dLS%d", number, number * 100 + 2);
        LM = LS(devname);
        sprintf(devname, "LINE%dLS%d", number, number * 100 + 1);
        LH = LS(devname);
        LWH = LS(8);
        LWL = LS(9);
        LSH= LS(4);
        LSL = LS(5);
        LKH = LS(6);
        LKL = LS(7);


        LTS = LT("LT1");
        LTK = LT("LT2");
        LTW = LT("LT3");


        sprintf(devname, "LINE%dTE%d", number, number * 100 + 1);
        TP = TE(devname);
        sprintf(devname, "LINE%dTE%d", number, number * 100 + 2);
        TR = TE(devname);

        sprintf(devname, "LINE%dFQT%d", number, number * 100 + 1);
        cnt = FQT(devname);
        sprintf(devname, "LINE%dQT%d", number, number * 100 + 1);
        Q = QT(devname);
        sprintf(devname, "LINE%dFS%d", number, number * 100 + 1);
        FL = FS(devname);
        PUMPFREQ = NP;
        sprintf(devname, "LINE%dVC%d", number, number * 100 + 14);
        ao = VC(devname);
        }
    else
        {
        sprintf(devname, "LINE%dV%d", number, 0);
        V00 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 1);
        V01 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 2);
        V02 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 3);
        V03 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 4);
        V04 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 5);
        V05 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 6);
        V06 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 7);
        V07 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 8);
        V08 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 9);
        V09 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 10);
        V10 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 11);
        V11 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 12);
        V12 = V(devname);
        sprintf(devname, "LINE%dV%d", number, 13);
        V13 = V(devname);

        sprintf(devname, "LINE%dM%d", number, 1);
        NP = M(devname);
        G_DEVICE_MANAGER()->disable_error_logging = true;
        if (((device*)M("M1"))->get_type() == device::DT_NONE)
            {
            NS = (i_DO_AO_device*)V(101);
            }
        else
            {
            NS = M("M1");
            if (dynamic_cast<device*>(NS)->get_type() == device::DT_NONE)
                {
                G_LOG->error("NS not found");
                }
            }
        if (((device*)M("M2"))->get_type() == device::DT_NONE)
            {
            NK = (i_DO_AO_device*)V(102);
            }
        else
            {
            NK = M("M2");
            if (dynamic_cast<device*>(NK)->get_type() == device::DT_NONE)
                {
                G_LOG->error("NK not found");
                }
            }
        G_DEVICE_MANAGER()->disable_error_logging = false;
        sprintf(devname, "LINE%dLS%d", number, 3);
        LL = LS(devname);
        sprintf(devname, "LINE%dLS%d", number, 2);
        LM = LS(devname);
        sprintf(devname, "LINE%dLS%d", number, 1);
        LH = LS(devname);
        LWH = LS(8);
        LWL = LS(9);
        LSH= LS(4);
        LSL = LS(5);
        LKH = LS(6);
        LKL = LS(7);

        G_DEVICE_MANAGER()->disable_error_logging = true;
        LTS = LT("LT1");
        LTK = LT("LT2");
        LTW = LT("LT3");
        G_DEVICE_MANAGER()->disable_error_logging = false;

        sprintf(devname, "LINE%dTE%d", number, 1);
        TP = TE(devname);
        sprintf(devname, "LINE%dTE%d", number, 2);
        TR = TE(devname);

        sprintf(devname, "LINE%dFQT%d", number, 1);
        cnt = FQT(devname);
        sprintf(devname, "LINE%dQT%d", number, 1);
        Q = QT(devname);
        sprintf(devname, "LINE%dFS%d", number, 1);
        FL = FS(devname);
        PUMPFREQ = NP;
        sprintf(devname, "LINE%dVC%d", number, 14);
        ao = VC(devname);

        sprintf(devname, "LINE%dPT%d", number, 1);
        G_DEVICE_MANAGER()->disable_error_logging = true;
        PRESSURE = PT(devname);
        if (dynamic_cast<device*>(PRESSURE)->get_type() == device::DT_NONE)
            {
            PRESSURE = nullptr;
            G_LOG->info("PRESSURE not used");
            }
        G_DEVICE_MANAGER()->disable_error_logging = false;
        }

    PIDF = new MSAPID(&rt_par_float, PIDF_Z, P_ZAD_FLOW, PUMPFREQ, nullptr, cnt, P_PIDF_MAX_OUT);
    PIDP = new MSAPID(&rt_par_float, PIDP_Z, P_ZAD_PODOGR, ao, TP, nullptr, P_PIDP_MAX_OUT);

    if ( G_PAC_INFO()->is_emulator() )
        {
        LSL->set_cmd("ST", 0, 1);
        LSH->set_cmd("ST", 0, 1);
        LKL->set_cmd("ST", 0, 1);
        LKH->set_cmd("ST", 0, 1);
        LWL->set_cmd("ST", 0, 1);
        LWH->set_cmd("ST", 0, 1);
        LL->set_cmd("ST", 0, 1);
        LM->set_cmd("ST", 0, 1);
        LH->set_cmd("ST", 0, 1);
        G_LOG->info("Init Line %d", number);
        }

    Mdls[nmr - 1] = this;
    MdlsCNT++;
    rt_par_float[P_R_NO_FLOW] = 2;
    rt_par_float[P_TM_R_NO_FLOW] = 20;
    rt_par_float[P_TM_NO_FLOW_R] = 20;
    rt_par_float[P_TM_NO_CONC] = 20;
    rt_par_float[PIDP_Z] = 95;
    rt_par_float[PIDP_k] = 2;
    rt_par_float[PIDP_Ti] = 30;
    rt_par_float[PIDP_Td] = (float)0.2;
    rt_par_float[PIDP_dt] = 500;
    rt_par_float[PIDP_dmax] = 130;
    rt_par_float[PIDP_dmin] = 0;
    rt_par_float[PIDP_AccelTime] = 30;
    rt_par_float[PIDP_IsManualMode] = 0;
    rt_par_float[PIDP_UManual] = 30;
    rt_par_float[PIDP_Uk] = 0;
    rt_par_float[PIDF_Z] = 15;
    rt_par_float[PIDF_k] = (float)0.5;
    rt_par_float[PIDF_Ti] = 10;
    rt_par_float[PIDF_Td] = (float)0.1;
    rt_par_float[PIDF_dt] = 1000;
    rt_par_float[PIDF_dmax] = 40;
    rt_par_float[PIDF_dmin] = 0;
    rt_par_float[PIDF_AccelTime] = 2;
    rt_par_float[PIDF_IsManualMode] = 0;
    rt_par_float[PIDF_UManual] = 15;
    rt_par_float[PIDF_Uk] = 0;

    isMsa = 1;

    auto check_Lua_function = [=]( const char* function_name, int& is_function )
        {
        if ( !is_function )
            {
            lua_State* L = lua_manager::get_instance()->get_Lua();
            lua_getglobal( L, this->name_Lua );
            if ( lua_istable( L, -1 ) )
                {
                lua_getfield( L, -1, function_name );
                if ( lua_isfunction( L, -1 ) )
                    {
                    is_function = 2;
                    }
                else
                    {
                    is_function = 1;
                    }
                lua_remove( L, -1 ); // Stack: remove function "function_name".
                }
            lua_remove( L, -1 );  // Stack: remove OBJECT.
            }
        };

    check_Lua_function( "init_custom_step", is_InitCustomStep_func );
    check_Lua_function( "do_custom_step", is_DoCustomStep_func );
    check_Lua_function( "cip_GoToStep", is_GoToStep_func );
    check_Lua_function( "cip_DoStep", is_DoStep_func );
    check_Lua_function( "cip_InitStep", is_InitStep_func );
    check_Lua_function( "cip_LoadProgram", is_LoadProgram_func );
    check_Lua_function( "cip_StopDev", is_StopDev_func );
    check_Lua_function( "cip_ResetLinesDevicesBeforeReset", is_ResetLinesDevicesBeforeReset_func );
    check_Lua_function( "cip_OporCIP", is_OporCIP_func );
    check_Lua_function( "cip_InitOporCIP", is_InitOporCIP_func );
    check_Lua_function( "cip_in_evaluate", is_in_evaluate_func );
    check_Lua_function( "cip_in_error", is_in_error_func );
    check_Lua_function( "cip_InitCirc", is_InitCirc_func );
    check_Lua_function( "cip_Circ", is_Circ_func );
    check_Lua_function( "cip_CheckErr", is_CheckErr_func );
    check_Lua_function( "cip_ToObject", is_ToObject_func );
    check_Lua_function( "cip_InitToObject", is_InitToObject_func );
    check_Lua_function( "cip_FromObject", is_FromObject_func );
    check_Lua_function( "cip_InitFromObject", is_InitFromObject_func );
    check_Lua_function( "cip_RT", is_RT_func );
    check_Lua_function( "cip_Stop", is_Stop_func );
    check_Lua_function( "cip_DoseRR", is_DoseRR_func );
    check_Lua_function( "cip_InitDoseRR", is_InitDoseRR_func );
    check_Lua_function( "cip_On_Resume", is_On_Resume_func );
    check_Lua_function( "cip_ConfigureLine", is_ConfigureLine_func );

    if ( nmr == FIRST_CIPLINE_OBJECT_NUMBER )
        {
        causticLoadedRecipe = (int)(parpar[0][P_CAUSTIC_SELECTED]);
        if (causticLoadedRecipe >= 0 && causticLoadedRecipe < TMediumRecipeManager::recipePerLine)
            {
            causticRecipes->getRecipeName(causticLoadedRecipe, causticName);
            }
        else
            {
            parpar->save(P_CAUSTIC_SELECTED, -1);
            causticLoadedRecipe = -1;
            }

        acidLoadedRecipe = (int)(parpar[0][P_ACID_SELECTED]);
        if (acidLoadedRecipe >= 0 && acidLoadedRecipe < TMediumRecipeManager::recipePerLine)
            {
            acidRecipes->getRecipeName(acidLoadedRecipe, acidName);
            }
        else
            {
            parpar->save(P_ACID_SELECTED, -1);
            acidLoadedRecipe = -1;
            }
        }
    }

void cipline_tech_object::resetProgramName()
    {
    snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Не выбран");
    }

void cipline_tech_object::resetRecipeName()
    {
    sprintf(loadedRecName, "Не выбран");
    //Обнуляем ссылку на статистику мойки объекта.
    emptystats->resetstats();
    objectstats = emptystats;
    }

void cipline_tech_object::resetProgramList( unsigned long programmask /*= 0xB00*/ )
    {
    prgListLen = 0;
    ModbusServ::UpdateLinePrograms(nmr);
    programList.clear();
    if ((SPROG_ACID_PREPARATION & programmask) == SPROG_ACID_PREPARATION)
        {
        addProgramToList("Наведение кислоты", SPROG_ACID_PREPARATION);
        }
    if ((SPROG_CAUSTIC_PREPARATION & programmask) == SPROG_CAUSTIC_PREPARATION)
        {
        addProgramToList("Наведение щелочи", SPROG_CAUSTIC_PREPARATION);
        }
    if ( scenabled && scline == 0 && (SPROG_SELF_CLEAN & programmask) == SPROG_SELF_CLEAN )
        {
        addProgramToList("Очистка танков", SPROG_SELF_CLEAN);
        }
    }

void cipline_tech_object::addProgramToList( const char *programName, int programNmr )
    {
    std::stringstream programStr;
    programStr << programNmr << "##" << programName << "||";
    programList.append(programStr.str());
    if ( prgListLen < panelMaxPrograms )
        {
        prgArray[ prgListLen ] = programName;
        prgNumber[ prgListLen ] = programNmr;
        prgListLen++;
        }
    }

void cipline_tech_object::formProgramList( unsigned long programmask )
    {
    prgListLen = 0;
    ModbusServ::UpdateLinePrograms( nmr );
    if ( programmask == 0 )
        {
        programmask = default_programlist;
        }
    programList.clear();
    if ((programmask >> 0) & 1 )
        {
        addProgramToList("Дезинфекция", SPROG_HOTWATER);
        }
    if ((programmask >> 1) & 1 )
        {
        addProgramToList("Ополаск", SPROG_RINSING);
        }
    if ((programmask >> 2) & 1 )
        {
        addProgramToList("Опол+Дез", SPROG_RINSING_HOTWATER);
        }
    if ((programmask >> 3) & 1 )
        {
        addProgramToList("Кислота", SPROG_ACID);
        }
    if ((programmask >> 4) & 1 )
        {
        addProgramToList("Кисл+Дез", SPROG_ACID_HOTWATER);
        }
    if ((programmask >> 5) & 1 )
        {
        addProgramToList("Щелочь", SPROG_CAUSTIC);
        }
    if ((programmask >> 6) & 1 )
        {
        addProgramToList("Щел+Дез", SPROG_CAUSTIC_HOTWATER);
        }
    if ((programmask >> 7) & 1 )
        {
        addProgramToList("Щел+Кисл+Дез", SPROG_CAUSTIC_ACID_HOTWATER);
        }
    if ((programmask >> 8) & 1 )
        {
        addProgramToList("Щел+Кисл", SPROG_CAUSTIC_ACID);
        }
    if ((programmask >> 9) & 1 )
        {
        addProgramToList("Опол.ч.водой в канал.", SPROG_AP_RC_KANAL);
        }
    if ((programmask >> 12) & 1 )
        {
        addProgramToList("Опол.ч.водой в танк", SPROG_AP_RC_SW);
        }
    if ((programmask >> 13) & 1 )
        {
        addProgramToList("Щел+ДезСР", SPROG_CAUSTIC_SANITIZER);
        }
    if ((programmask >> 14) & 1 )
        {
        addProgramToList("Кисл+ДезСР", SPROG_ACID_SANITIZER);
        }
    if ((programmask >> 15) & 1 )
        {
        addProgramToList("Щел+Кисл+ДезСР", SPROG_CAUSTIC_ACID_SANITIZER);
        }
    if ((programmask >> 10) & 1 )
        {
        addProgramToList("ДезСР", SPROG_SANITIZER);
        }
    if ((programmask >> 11) & 1 )
        {
        addProgramToList("Управляемая", SPROG_REMOTE);
        }
    if (((programmask >> 16) & 1) && (static_cast<int>(rt_par_float[ P_PROGRAM ]) & (1 << PRG_D)))
        {
        addProgramToList("Без ополаск.(опция)", SPROG_OPTION_DISABLE_RINSE);
        }
    }

void cipline_tech_object::loadProgramFromList( int selectedPrg )
    {
    loadedProgram = selectedPrg;
    switch (selectedPrg)
        {
        case SPROG_RINSING_CLEAN:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Опол.чист.водой");
            rt_par_float[P_PROGRAM] = SPROG_RINSING_CLEAN;
            disable_final_rinsing = false;
            break;
        case SPROG_SANITIZER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "ДезРаствор");
            rt_par_float[P_PROGRAM] =  SPROG_SANITIZER;
            disable_final_rinsing = false;
            break;
        case SPROG_HOTWATER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Дезинфекция");
            rt_par_float[P_PROGRAM] =  SPROG_HOTWATER;
            disable_final_rinsing = false;
            break;
        case SPROG_RINSING:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Ополаскивание");
            rt_par_float[P_PROGRAM] =  SPROG_RINSING;
            disable_final_rinsing = false;
            break;
        case SPROG_RINSING_HOTWATER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Опол+Дезинф");
            rt_par_float[P_PROGRAM] =  SPROG_RINSING_HOTWATER;
            disable_final_rinsing = false;
            break;
        case SPROG_ACID:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Кислота");
            rt_par_float[P_PROGRAM] =  SPROG_ACID;
            disable_final_rinsing = false;
            break;
        case SPROG_ACID_HOTWATER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Кислота+Дез");
            rt_par_float[P_PROGRAM] =  SPROG_ACID_HOTWATER;
            disable_final_rinsing = false;
            break;
        case SPROG_CAUSTIC:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щелочь");
            rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC;
            disable_final_rinsing = false;
            break;
        case SPROG_CAUSTIC_HOTWATER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+Дезинф");
            rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_HOTWATER;
            disable_final_rinsing = false;
            break;
        case SPROG_CAUSTIC_ACID:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+Кислота");
            rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_ACID;
            disable_final_rinsing = false;
            break;
        case SPROG_CAUSTIC_ACID_HOTWATER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+Кисл+Дез");
            rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_ACID_HOTWATER;
            disable_final_rinsing = false;
            break;
        case SPROG_CAUSTIC_ACID_SANITIZER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+Кисл+ДезСР");
            rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_ACID_SANITIZER;
            disable_final_rinsing = false;
            break;
        case SPROG_CAUSTIC_SANITIZER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+ДезСР");
            rt_par_float[P_PROGRAM] = SPROG_CAUSTIC_SANITIZER;
            disable_final_rinsing = false;
            break;
        case SPROG_ACID_SANITIZER:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Кисл+ДезСР");
            rt_par_float[P_PROGRAM] = SPROG_ACID_SANITIZER;
            disable_final_rinsing = false;
            break;
        case SPROG_ACID_PREPARATION:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Нав. кислоты");
            rt_par_float[P_PROGRAM] =  SPROG_ACID_PREPARATION;
            break;
        case SPROG_CAUSTIC_PREPARATION:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Нав. щелочи");
            rt_par_float[P_PROGRAM] =  SPROG_CAUSTIC_PREPARATION;
            break;
        case SPROG_SELF_CLEAN:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Очистка танков");
            rt_par_float[P_PROGRAM] =  SPROG_SELF_CLEAN;
            break;
        case SPROG_REMOTE:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Управляемая мойка");
            rt_par_float[P_PROGRAM] = SPROG_REMOTE;
            disable_final_rinsing = false;
            break;
        case SPROG_AP_RC_KANAL:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Опол.чист.водой в канал.");
            clean_water_rinsing_return = KANAL;
            rt_par_float[P_PROGRAM] = SPROG_RINSING_CLEAN;
            disable_final_rinsing = false;
            break;
        case SPROG_AP_RC_SW:
            snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Опол.чист.водой в танк");
            clean_water_rinsing_return = TANK_W;
            rt_par_float[P_PROGRAM] = SPROG_RINSING_CLEAN;
            disable_final_rinsing = false;
            break;
        case SPROG_OPTION_DISABLE_RINSE:
            switch ( static_cast<int>(rt_par_float[ P_PROGRAM ]))
                {
                case SPROG_HOTWATER:
                    snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "ДезБО" );
                    disable_final_rinsing = true;
                    loadedProgram = SPROG_HOTWATER;
                    break;
                case SPROG_CAUSTIC_HOTWATER:
                    snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+ДезБО" );
                    disable_final_rinsing = true;
                    loadedProgram = SPROG_CAUSTIC_HOTWATER;
                    break;
                case SPROG_ACID_HOTWATER:
                    snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Кисл+ДезБО" );
                    disable_final_rinsing = true;
                    loadedProgram = SPROG_ACID_HOTWATER;
                    break;
                case SPROG_CAUSTIC_ACID_HOTWATER:
                    snprintf(currentProgramName, PROGRAM_MAX_LEN * UNICODE_MULTIPLIER, "Щел+Кисл+ДезБО" );
                    disable_final_rinsing = true;
                    loadedProgram = SPROG_CAUSTIC_ACID_HOTWATER;
                    break;
                default:
                    resetProgramName( );
                    loadedProgram = -1;
                    disable_final_rinsing = false;
                    rt_par_float[ P_PROGRAM ] = SPROG_NO_PROGRAM;
                    break;
                }
            break;
        default:
            resetProgramName( );
            loadedProgram = -1;
            disable_final_rinsing = false;
            rt_par_float[ P_PROGRAM ] = SPROG_NO_PROGRAM;
        }
    formProgramList(rt_par_float[P_PROGRAM_MASK]);
    }

void cipline_tech_object::closeLineValves() const
    {
    unsigned int i;
    unsigned int vstart = 1001;
    unsigned int vend = 1099;
    char devname[25];
    G_LOG->debug("Closing line valves from %d to %d...", vstart, vend);
    G_DEVICE_MANAGER()->disable_error_logging = true;
    for (i = vstart; i <= vend; i++)
        {
        sprintf(devname, "LINE%dV%d", nmr, i);
        V(devname)->off();
        }
    G_DEVICE_MANAGER()->disable_error_logging = false;
    }

int cipline_tech_object::isTank()
    {
    if ((((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 1 || (((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 3)
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

int cipline_tech_object::isLine()
    {
    if ((((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 2 || (((int)rt_par_float[P_OBJ_TYPE]) & 0xFF) == 4)
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

int cipline_tech_object::getValvesConflict() const
    {
    int i,j,k;
    for (i = TRecipeManager::RV_FIRSTVALVEON; i<=TRecipeManager::RV_LASTVALVEOFF; i++)
        {
        if (lineRecipes->getRecipeValue(loadedRecipe,i) != 0)
            {
            for (j = 0; j < MdlsCNT; j++)
                {
                if (Mdls[j]->nmr != nmr && Mdls[j]->state != 0 && Mdls[j]->loadedRecipe >=0)
                    {
                    if (i <= TRecipeManager::RV_LASTVALVEON)
                        {
                        for (k = TRecipeManager::RV_FIRSTVALVEON; k <= TRecipeManager::RV_LASTVALVEOFF; k++)
                            {
                            if (lineRecipes->getRecipeValue(loadedRecipe, i) == Mdls[j]->lineRecipes->getRecipeValue(Mdls[j]->loadedRecipe, k) && lineRecipes->getRecipeValue(loadedRecipe, i) < 1000)
                                {
                                if ( G_DEBUG )
                                    {
                                    printf("Opened valve %d on line %d conflicts with valve on line %d", (int)lineRecipes->getRecipeValue(loadedRecipe, i), nmr, Mdls[j]->nmr);
                                    }
                                return 1;
                                }
                            }
                        }
                    else
                        {
                        for (k = TRecipeManager::RV_FIRSTVALVEON; k <= TRecipeManager::RV_LASTVALVEON; k++)
                            {
                            if (lineRecipes->getRecipeValue(loadedRecipe, i) == Mdls[j]->lineRecipes->getRecipeValue(Mdls[j]->loadedRecipe, k) && lineRecipes->getRecipeValue(loadedRecipe, i) < 1000)
                                {
                                if ( G_DEBUG )
                                    {
                                    printf("Closed valve %d on line %d conflicts with opened valve on line %d",(int)lineRecipes->getRecipeValue(loadedRecipe, i), nmr, Mdls[j]->nmr);
                                    }
                                return 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    return 0;
    }

void cipline_tech_object::set_station_par( int parno, float newval )
    {
    parpar->save(parno, newval);
    }

float cipline_tech_object::get_station_par( int parno )
    {
    return parpar[0][parno];
    }

void cipline_tech_object::ResetStat( )
    {
    for ( int i = STP_QAVS; i <= STP_PODP_WATER; i++ )
        {
        rt_par_float[i] = 0;
        }
    }

int cipline_tech_object::EvalBlock()
    {
    if (ncmd == MCMD_LOCK_MODULE)
        {
        blocked = 1;
        ncmd = 0;
        }
    if (ncmd == MCMD_UNLOCK_MODULE)
        {
        blocked = 0;
        if (curstep>0)
            {
            state = -1;
            }
        else
            {
            state = 0;
            }
        ncmd = 0;
        }
    if (0 == blocked)
        {
        return 0;
        }
    if (state>0)
        {
        Stop(curstep);
        }
    ncmd = 0;
    state = -1000;
    StopDev();
    return 1;
    }

void cipline_tech_object::_StopDev( )
    {
    V01->off();
    V03->off();
    V02->off();
    V04->off();
    V13->off();
    V05->off();
    V06->off();
    if (state != ERR_CIP_OBJECT && state != ERR_OS && state != ERR_WATCHDOG )
        {
        V07->off();
        V08->off();
        V09->off();
        V10->off();
        V11->off();
        V12->off();
        }
    V00->instant_off();
    NP->off();
    nplaststate = false;
    if (PIDP->HI==0) PIDP->off();
    if (PIDF->HI==0) PIDF->off();
    ret_overrride = 0;
    SetRet(OFF);
    //Смена среды
    if (dev_upr_medium_change)
        {
        dev_upr_medium_change->off();
        }
    if (dev_upr_acid)
        {
        dev_upr_acid->off();
        }
    if (dev_upr_water)
        {
        dev_upr_water->off();
        }
    if (dev_upr_caustic)
        {
        dev_upr_caustic->off();
        }
    if (dev_upr_sanitizer_pump)
        {
        dev_upr_sanitizer_pump->off();
        }
    if (dev_upr_desinfection)
        {
        dev_upr_desinfection->off();
        }
    if (dev_upr_circulation)
        {
        dev_upr_circulation->off();
        }
    if (dev_upr_prerinse)
        {
        dev_upr_prerinse->off();
        }
    if (dev_upr_intermediate_rinse)
        {
        dev_upr_intermediate_rinse->off();
        }
    if (dev_upr_postrinse)
        {
        dev_upr_postrinse->off();
        }
    if (dev_ao_flow_task)
        {
        dev_ao_flow_task->set_value(0);
        }
    if (dev_ao_temp_task)
        {
        dev_ao_temp_task->set_value(0);
        }
    if (scenabled && scline == nmr)
        {
        VSMG->off();
        VSDREN->off();
        VKMG->off();
        VKDREN->off();
        VWMG->off();
        VWDREN->off();
        }
    RHI();
    }

void cipline_tech_object::_Stop( int step_to_stop )
    {
    switch (step_to_stop)
        {
        case 105:
        case 109:
            NS->off();
            break;
        case 115:
        case 119:
            NK->off();
            break;
        }
    RHI();
    PauseTimers();
    cnt->pause();
    StopDev();
    state=0;
    }

void cipline_tech_object::RHI( )
    {
    PIDP->HI=0;
    PIDF->HI=0;
    }

void cipline_tech_object::PauseTimers( )
    {
    int i;
    for (i=0; i<TMR_CNT; i++) T[i]->pause();
    }

void cipline_tech_object::_RT( )
    {
    int i;
    for (i=0; i<TMR_CNT; i++) T[i]->reset();
    }

int cipline_tech_object::EvalRecipes()
    {
    if (1 == nmr && use_internal_medium_recipes)
        {
        acidRecipes->EvalRecipe();
        causticRecipes->EvalRecipe();

        //Выбор раствора
        if ((int)(parpar[0][P_CAUSTIC_SELECTED]) != causticLoadedRecipe)
            {
            int newcausticrecipe = (int)(parpar[0][P_CAUSTIC_SELECTED]);
            if (newcausticrecipe >= 0 && newcausticrecipe < causticRecipes->recipePerLine)
                {
                causticRecipes->getRecipeName(newcausticrecipe, causticName);
                causticRecipes->LoadRecipeToParams(newcausticrecipe, parpar[0]);
                causticLoadedRecipe = newcausticrecipe;
                }
            else
                {
                parpar->save(P_CAUSTIC_SELECTED, causticLoadedRecipe);
                }
            }

        if ((int)(parpar[0][P_ACID_SELECTED]) != acidLoadedRecipe)
            {
            int newacidrecipe = (int)(parpar[0][P_ACID_SELECTED]);
            if (newacidrecipe >= 0 && newacidrecipe < acidRecipes->recipePerLine)
                {
                acidRecipes->getRecipeName(newacidrecipe, acidName);
                acidRecipes->LoadRecipeToParams(newacidrecipe, parpar[0]);
                acidLoadedRecipe = newacidrecipe;
                }
            else
                {
                parpar->save(P_ACID_SELECTED, acidLoadedRecipe);
                }
            }
        }


    lineRecipes->EvalRecipe();
    rt_par_float[P_LOADED_RECIPE] = loadedRecipe + 1;
    //переход к рецепту
    if (rt_par_float[P_CUR_REC] != lineRecipes->getCurrentRecipe() + 1)
        {
        lineRecipes->ToRecipe( ( int ) rt_par_float[P_CUR_REC] - 1);
        rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
        }
    //выбор рецепта
    if (rt_par_float[P_SELECT_REC] > 0)
        {
        lineRecipes->LoadRecipeToParams( ( int ) rt_par_float[P_SELECT_REC] - 1,
            TRecipeManager::RecipeValues::RV_V1,
            workParameters::PV1,
            TRecipeManager::RecipeValues::RV_RESERV_START, &rt_par_float);
        lineRecipes->getRecipeName( ( int ) rt_par_float[P_SELECT_REC] - 1,
            loadedRecName);
        loadedRecipe = ( int ) rt_par_float[P_SELECT_REC] - 1;
        rt_par_float[P_SELECT_REC] = 0;
        formProgramList((unsigned int)(rt_par_float[P_PROGRAM_MASK]));
        //Загрузка статистики
        if ((tech_type != TECH_TYPE_CAR_WASH) && (tech_type != TECH_TYPE_CAR_WASH_SELF_CLEAN))
            {
            objectstats = statsbase->get_obj_stats(loadedRecName);
            objectstats->changed = 0;
            }
        }
    //выбор программы
    if (rt_par_float[P_SELECT_PRG] > 0)
        {
        loadProgramFromList( ( int ) rt_par_float[P_SELECT_PRG]);
        rt_par_float[P_SELECT_PRG] = 0;
        }
    return 0;
    }

int cipline_tech_object::SetRet( int val )
    {
    if (ret_overrride)
        {
        return 0;
        }

    int toset = val;

    if (dev_ls_ret_pump)
        {
        if (!dev_ls_ret_pump->is_active())
            {
            float rettimer = rt_par_float[P_RET_PUMP_SENSOR_DELAY];
            if (get_delta_millisec(ret_pums_ls_timer) > rettimer)
                {
                toset = 0;
                }
            }
        }

    if (dev_m_ret)
        {
        if (toset)
            {
            dev_m_ret->on();
            rt_par_float[P_RET_STATE] = ON;
            }
        else
            {
            dev_m_ret->off();
            rt_par_float[P_RET_STATE] = OFF;
            }
        }

    if (dev_upr_ret)
        {
        if (toset)
            {
            dev_upr_ret->on();
            rt_par_float[P_RET_STATE] = ON;
            }
        else
            {
            dev_upr_ret->off();
            rt_par_float[P_RET_STATE] = OFF;
            }
        }
    return 0;
    }

int cipline_tech_object::EvalCommands()
    {
    int i;
    switch (ncmd)
        {
        case MCMD_NEXT:
            if (state !=0 && curstep != 555)
                {
                rt_par_float[STP_STEPS_OVER] = rt_par_float[STP_STEPS_OVER] + 1; //количество пропущенных операций
                rt_par_float[STP_LAST_STEP] = curstep;
                curstep=GoToStep(curstep, 0);
                state=1;
                InitStep(curstep, 0);
                }
            break;
        case MCMD_PAUSE:
            if (state>0 && curstep != 555)
                {
                Stop(curstep);
                ncmd=0;
                state=-1;
                return -1;
                }
            break;
        case MCMD_RESET:
            if (state != 0)
                {
                rt_par_float[STP_RESETSTEP] = curstep; //шаг, на котором было сброшено
                Stop(curstep);
                curstep = 555;
                program_was_terminated = true;
                is_reset = true;
                InitStep(curstep, 0);
                state = 1;
                }
            else
                {
                resetProgramName();
                resetRecipeName();
                resetProgramList();
                resetCarNumber();
                loadedRecipe = -1;
                loadedProgram = -1;
                rt_par_float[P_PROGRAM] = 0;
                }
            break;
        case MCMD_EVALUATE:
            if (state<0)
                {
                if (!valvesAreInConflict)
                    {
                    if (init_object_devices())
                        {
                        state = ERR_WRONG_OS_OR_RECIPE_ERROR;
                        }
                    else
                        {
                        if (2 == is_On_Resume_func)
                            {
                            lua_State* L = lua_manager::get_instance()->get_Lua();
                            lua_getglobal( L, name_Lua );
                            lua_getfield( L, -1, "cip_On_Resume" );
                            lua_remove( L, -2 );  // Stack: remove OBJECT.
                            lua_getglobal( L, name_Lua );
                            if (0 != lua_pcall(L, 1, 0, 0))
                                {
                                printf("Error in calling cip_On_Resume: %s\n", lua_tostring(L, -1));
                                lua_pop(L, 1);
                                }
                            }
                        else
                            {
                            if (state != ERR_ACID_WASH_REQUIRED)
                                {
                                state = 1;
                                InitStep(curstep, 1);
                                }
                            }
                        }
                    }
                }
            if (state == 0 && (loadedRecipe >= 0 || rt_par_float[P_PROGRAM] >= SPROG_ACID_PREPARATION) && rt_par_float[P_PROGRAM] > 0  )
                {
                program_was_terminated = false;
                if (scenabled)
                    {
                    if (scline > 0 && scline != nmr) //если идет самоочистка танков, то нельзя включить мойку на других линиях.
                        {
                        return 0;
                        }
                    if (0 == scline && SPROG_SELF_CLEAN == rt_par_float[P_PROGRAM]) //нельзя включить самоочистку, если на других линиях идет мойка
                        {
                        for (int tmpline = 0; tmpline < MdlsCNT; tmpline++)
                            {
                            if (Mdls[tmpline]->state && Mdls[tmpline]->nmr != nmr)
                                {
                                return 0;
                                }
                            }
                        }

                    if (SPROG_SELF_CLEAN == rt_par_float[P_PROGRAM])
                        {
                        scline = nmr;
                        for (int tmpline = 0; tmpline < MdlsCNT; tmpline++)
                            {
                            if (Mdls[tmpline]->nmr != nmr)
                                Mdls[tmpline]->resetProgramList();
                            }
                        }
                    }

                if (TECH_TYPE_CAR_WASH == tech_type || TECH_TYPE_CAR_WASH_SELF_CLEAN == tech_type)
                    {
                    if (!(switch1 || switch2 || switch3 || switch4) && rt_par_float[P_PROGRAM] != SPROG_ACID_PREPARATION && rt_par_float[P_PROGRAM] != SPROG_CAUSTIC_PREPARATION && rt_par_float[P_PROGRAM] != SPROG_SELF_CLEAN)
                        {
                        return 0;
                        }
                    if (rt_par_float[P_PROGRAM] == SPROG_CAUSTIC_PREPARATION || rt_par_float[P_PROGRAM] == SPROG_ACID_PREPARATION || rt_par_float[P_PROGRAM] == SPROG_SELF_CLEAN)
                        {
                        switch1 = 0;
                        switch2 = 0;
                        switch3 = 0;
                        switch4 = 0;
                        }
                    else
                        {
                        objectstats = statsbase->get_obj_stats(ncar1);
                        }
                    }

                state=1;
                valvesAreInConflict = getValvesConflict();
                if (valvesAreInConflict)
                    {
                    state = ERR_VALVES_ARE_IN_CONFLICT;
                    }
                else
                    {
                    if (init_object_devices())
                        {
                        state = ERR_WRONG_OS_OR_RECIPE_ERROR;
                        }
                    else
                        {
                        bool is_acid_program = false;
                        bool is_caustic_program = false;
                        int cur_selected_program = rt_par_float[P_PROGRAM];
                        if (cur_selected_program & 0x10) is_acid_program = true;
                        if (cur_selected_program & 0x20) is_caustic_program = true;
                        if (no_acid_wash_max > 0 && objectstats->objcausticwashes > no_acid_wash_max && !is_acid_program && is_caustic_program)
                            {
                            set_err_msg("Необходима мойка кислотой", 0, 0, ERR_MSG_TYPES::ERR_ALARM);
                            state = ERR_ACID_WASH_REQUIRED;
                            }
                        else
                            {
                            closeLineValves();

                            if (2 == is_ConfigureLine_func)
                                {
                                lua_State* L = lua_manager::get_instance()->get_Lua();
                                lua_getglobal(L, name_Lua);
                                lua_getfield(L, -1, "cip_ConfigureLine");
                                lua_remove(L, -2);  // Stack: remove OBJECT.
                                lua_getglobal(L, name_Lua);
                                lua_pushinteger(L, switch1);
                                lua_pushinteger(L, switch2);
                                lua_pushinteger(L, switch3);
                                lua_pushinteger(L, switch4);
                                if (0 == lua_pcall(L, 5, 1, 0))
                                    {
                                    lua_pop(L, 1);
                                    }
                                else
                                    {
                                    printf("Error in calling cip_ConfigureLIne: %s\n", lua_tostring(L, -1));
                                    lua_pop(L, 1);
                                    }
                                }

                            lineRecipes->OnRecipeDevices(loadedRecipe, nmr);
                            if (TECH_TYPE_CAR_WASH == tech_type || TECH_TYPE_CAR_WASH_SELF_CLEAN == tech_type)
                                {
                                curstep = LoadProgram();
                                }
                            InitStep(curstep, 0);
                            }
                        }
                    }
                }
            break;
        case MCMD_RESET_ALARM:
            blockAlarm = 1;
            break;
        case MCMD_NEXT_RECIPE:
            lineRecipes->NextRecipe();
            rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
            break;
        case MCMD_PREV_RECIPE:
            lineRecipes->PrevRecipe();
            rt_par_float[P_CUR_REC] = lineRecipes->getCurrentRecipe() + 1;
            break;
        case MCMD_CAUSTIC_NEXT_RECIPE:
            causticRecipes->NextRecipe();
            break;
        case MCMD_CAUSTIC_PREV_RECIPE:
            causticRecipes->PrevRecipe();
            break;
        case MCMD_ACID_NEXT_RECIPE:
            acidRecipes->NextRecipe();
            break;
        case MCMD_ACID_PREV_RECIPE:
            acidRecipes->PrevRecipe();
            break;
        case MCMD_FORCE_RET_ON:
            if (state)
                {
                ForceRet(ON);
                }
            break;
        case MCMD_FORCE_RET_OFF:
            if (state)
                {
                ForceRet(OFF);
                }
            break;
        case MCMD_RELOAD_RECIPES:
            lineRecipes->LoadFromFile(lineRecipes->defaultfilename);
            lineRecipes->LoadRecipeName();
            break;
        case MCMD_RELOAD_CAUSTIC_RECIPES:
            causticRecipes->LoadFromFile(causticRecipes->defaultfilename);
            causticRecipes->LoadRecipeName();
            break;
        case MCMD_RELOAD_ACID_RECIPES:
            acidRecipes->LoadFromFile(acidRecipes->defaultfilename);
            acidRecipes->LoadRecipeName();
            break;
        case MCMD_RELOAD_WASH_STATS:
            for (i = 0; i < MdlsCNT; i++)
                {
                Mdls[i]->objectstats = Mdls[i]->emptystats;
                }
            statsbase->clear();
            statsbase->loadFromFile(statsbase->filename);
            for (i = 0; i < MdlsCNT; i++)
                {
                if (Mdls[i]->state != 0 && Mdls[i]->rt_par_float[P_PROGRAM] != SPROG_CAUSTIC_PREPARATION && Mdls[i]->rt_par_float[P_PROGRAM] != SPROG_ACID_PREPARATION &&
                    Mdls[i]->rt_par_float[P_PROGRAM] != SPROG_SELF_CLEAN)
                    {
                    if ((tech_type != TECH_TYPE_CAR_WASH) && (tech_type  != TECH_TYPE_CAR_WASH_SELF_CLEAN))
                        {
                        Mdls[i]->objectstats = statsbase->stats_if_exists(Mdls[i]->loadedRecName, Mdls[i]->emptystats);
                        Mdls[i]->objectstats->changed = 0;
                        }
                    else
                        {
                        Mdls[i]->objectstats = statsbase->stats_if_exists(Mdls[i]->ncar1, Mdls[i]->emptystats);
                        Mdls[i]->objectstats->changed = 0;
                        }
                    }
                }
            break;
        }
    return 0;
    }

int cipline_tech_object::_GoToStep( int cur, int param )
    {
    switch(cur)
        {
        case 0:	return LoadProgram();
        case 5: return 7;
        case 6:
        case 7: return cur+1;
        case 8: return 16;
        case 9:	return LoadProgram();
        case 10:
        case 11:
        case 12:
        case 13:
        case 14: return cur+1;
        case 15: return LoadProgram();
        case 16:
            if (rt_par_float[P_PROGRAM] != SPROG_RINSING)
                {
                return LoadProgram();
                }
            else
                {
                return 9;
                }
        case 22:
        case 23: return cur+1;
        case 24:
            if (circ_tank_s)
                {
                return 28;
                }
            else
                {
                return 26;
                }
        case 26: return 28;
        case 28: return 29;
        case 29: return 31;
        case 31: return 33;
        case 33: return 34;
        case 34: return 35;
        case 35: return 37;
        case 37: return 39;
        case 39: return 40;
        case 40: return LoadProgram();
        case 42:
        case 43: return cur+1;
        case 44:
            if (circ_tank_k)
                {
                return 48;
                }
            else
                {
                return 46;
                }
        case 46: return 48;
        case 48: return 49;
        case 49: return 53;
        case 53: return 54;
        case 54: return 55;
        case 55: return 57;
        case 57: return 59;
        case 59: return 60;
        case 60: return LoadProgram();
        case 61:
            if ((int)(rt_par_float[P_PROGRAM]) & KS_MASK)
                {
                rt_par_float[STP_USED_HOTWATER] = rt_par_float[STP_USED_HOTWATER] + rt_par_float[PV1] + rt_par_float[PV2]; //Если в программе присутствует мойка щелочью или кислотой, то контур на дезинфекции уже заполнен чистой водой.
                return 66;
                }
            return 62;
        case 62:
        case 63: return cur+1;
        case 64:
        case 65:
        case 66: return cur+1;
        case 67:
            if ( rt_par_float[ P_DOP_V_OK_OP ] < 0 )
                {
                disable_final_rinsing = true;
                }
            return LoadProgram();
        case 71:
        case 72:
        case 73:
        case 74:
        case 75:
        case 76:
        case 77:
        case 78: return cur+1;
        case 79: return LoadProgram();

        case 81: return 83;
        case 83:
        case 84:
        case 85: return cur+1;
        case 86: return 91;
        case 91: return LoadProgram();
        case 105: return 106;
        case 106: return 108;
        case 108:
            if (param==1)
                {
                return 111;
                }
            else
                {
                return 109;
                }
        case 109: return 106;
        case 111: return LoadProgram();
        case 115: return 116;
        case 116: return 118;
        case 118:
            if (param==1)
                {
                return 121;
                }
            else
                {
                return 119;
                }
        case 119: return 116;
        case 121: return LoadProgram();

        case 151:
        case 152:
        case 153:
        case 154:
        case 155:
        case 156:
        case 158:
        case 159:
        case 160:
        case 161:
        case 162:
        case 163:
        case 165:
        case 166:
        case 167:
        case 168:
        case 169:
        case 171:
        case 172:
        case 173:
        case 174:
        case 175:
        case 177:
        case 178:
        case 179:
        case 180:
        case 181:
        case 182:
        case 183:
        case 184:
        case 185:
        case 186:
        case 187:
            return cur + 1;
        case 157:
            if (scparams[0][SCP_V_PROM_TS] > 0)
                {
                return 160;
                }
            else
                {
                return 161;
                }
        case 164:
            if (scparams[0][SCP_V_PROM_TK] > 0)
                {
                return 167;
                }
            else
                {
                return 168;
                }
        case 170:
            if (scparams[0][SCP_V_PROM_TW] > 0)
                {
                return 173;
                }
            else
                {
                return 174;
                }
        case 176:
            if (scparams[0][SCP_V_PROM_TS] > 0)
                {
                return 179;
                }
            else
                {
                return 180;
                }
        case 188:
            return LoadProgram();
        case 300:
            return LoadProgram();

        }
    return ERR_UNKNOWN_STEP;
    }

int cipline_tech_object::_InitStep( int step_to_init, int not_first_call )
{
    int i, pr_media;
    sort_delay = get_millisec();
    ret_pums_ls_timer = get_millisec();
    if (sort_delay > SORT_SWITCH_DELAY + 1)
        {
        sort_delay -= SORT_SWITCH_DELAY + 1;
        }

    ret_overrride = 0;
    //Смена среды
    if (dev_upr_medium_change)
    {
    dev_upr_medium_change->off();
    }
    if (dev_upr_sanitizer_pump)
    {
    dev_upr_sanitizer_pump->off();
    }
    tankempty = 0;
    tankemptytimer = get_millisec();
    tankfull = 0;
    tankfulltimer = get_millisec();
    sort_last_destination = -1;
    sort_delay = get_millisec();
    forcesortrr = 1;

    PIDP->reset();
    PIDF->reset();
    ResetErr();
    if (not_first_call==0)
        {
        RT();
        rt_par_float[STP_LAST_STEP_COUNTER] = cnt->get_quantity();
        cnt->reset();
        cnt->pause();
        rt_par_float[ STP_LV] = 0 ; //stat._lv=0;
        for (i=0; i<SAV_CNT; i++)
            {
            SAV[i]->R();
            }
        is_ready_to_end = false;
        }

    pr_media=WATER;
    if (step_to_init>30 && step_to_init<42)
        {
        if (((int)rt_par_float[P_PROGRAM]>>PRG_K) & 1)
            {
            pr_media=TANK_W;
            }
        }

    int tank_w_dest = TANK_W;
    int tank_w_src = TANK_W;

    if (1 == dont_use_water_tank || (rt_par_float[P_DONT_USE_WATER_TANK] == 1))
    {
        tank_w_dest = KANAL;
        tank_w_src = WATER;
        pr_media = WATER;
    }

    int cleanrinsingto = clean_water_rinsing_return;

    switch (step_to_init)
        {
        case 0: //INIT fill circ tank
            RHI();
            StopDev();
            if (!LM->is_active())
                {
                V00->on();
                }
            return 0;
        case 5:  return InitToObject(tank_w_src, KANAL, step_to_init, not_first_call);
        case 6:  return InitOporCIP(KANAL, step_to_init, not_first_call);
        case 7:  return InitFromObject(tank_w_src, KANAL, step_to_init, not_first_call);
        case 8:  return InitToObject(tank_w_src, KANAL, step_to_init, not_first_call);
        case 9:  return InitOporCIP(KANAL, step_to_init, not_first_call);
        case 10: return InitFilCirc(WITH_RETURN, step_to_init, not_first_call);
        case 11: return InitCirc(WATER, step_to_init, not_first_call);
        case 12: return InitOporCirc(KANAL, step_to_init, not_first_call);
        case 13: return InitFilCirc(WITH_RETURN, step_to_init, not_first_call);
        case 14: return InitToObject(tank_w_src, KANAL, step_to_init, not_first_call);
        case 15: return InitOporCirc(KANAL, step_to_init, not_first_call);
        case 16: return InitOporCirc(KANAL, step_to_init, not_first_call);
        case 22: return InitToObject(TANK_S, KANAL, step_to_init, not_first_call);
        case 23: return InitOporCIP(KANAL, step_to_init, not_first_call);
        case 24: return InitFromObject(TANK_S, KANAL, step_to_init, not_first_call);
        case 26: return InitFilCirc(WITH_RETURN, step_to_init, not_first_call);
        case 28: return InitCirc(SHCH, step_to_init, not_first_call);
        case 29: return InitOporCirc(TANK_S, step_to_init, not_first_call);

        case 31: return InitFilCirc(WITH_WATER, step_to_init, not_first_call);
        case 33: return InitToObject(pr_media, TANK_S, step_to_init, not_first_call);
        case 34: return InitOporCIP(TANK_S, step_to_init, not_first_call);
        case 35: return InitFromObject(pr_media, TANK_S, step_to_init, not_first_call);
        case 37: return InitToObject(pr_media, tank_w_dest, step_to_init, not_first_call);
        case 39: return InitOporCirc(tank_w_dest, step_to_init, not_first_call);
        case 40: return InitOporCIP(tank_w_dest, step_to_init, not_first_call);

        case 42: return InitToObject(TANK_K, KANAL, step_to_init, not_first_call);
        case 43: return InitOporCIP(KANAL, step_to_init, not_first_call);
        case 44: return InitFromObject(TANK_K, KANAL, step_to_init, not_first_call);
        case 46: return InitFilCirc(WITH_RETURN, step_to_init, not_first_call);
        case 48: return InitCirc(KISL, step_to_init, not_first_call);
        case 49: return InitOporCirc(TANK_K, step_to_init, not_first_call);
        case 53: return InitToObject(WATER, TANK_K, step_to_init, not_first_call);
        case 54: return InitOporCIP(TANK_K, step_to_init, not_first_call);
        case 55: return InitFromObject(WATER, TANK_K, step_to_init, not_first_call);
        case 57: return InitToObject(WATER, tank_w_dest, step_to_init, not_first_call);
        case 59: return InitOporCirc(tank_w_dest, step_to_init, not_first_call);
        case 60: return InitOporCIP(tank_w_dest, step_to_init, not_first_call);
        case 61: return InitFilCirc(WITH_WATER, step_to_init, not_first_call);
        case 62: return InitToObject(WATER, tank_w_dest, step_to_init, not_first_call);
        case 63: return InitOporCIP(tank_w_dest, step_to_init, not_first_call);
        case 64: return InitFromObject(WATER, tank_w_dest, step_to_init, not_first_call);
        case 65: return InitFilCirc(WITH_WATER, step_to_init, not_first_call);
        case 66: return InitCirc(HOT_WATER, step_to_init, not_first_call);
        case 67: return InitOporCirc(tank_w_dest, step_to_init, not_first_call);

        case 71: return InitFilCirc(SANITIZER, step_to_init, not_first_call);
        case 72: return InitToObject(SANITIZER, tank_w_dest, step_to_init, not_first_call);
        case 73: return InitOporCIP(tank_w_dest, step_to_init, not_first_call);
        case 74: return InitFromObject(SANITIZER, tank_w_dest, step_to_init, not_first_call);
        case 75: return InitFilCirc(SANITIZER, step_to_init, not_first_call);
        case 76: return InitDoseRR(SANITIZER, step_to_init, not_first_call);
        case 77: return InitCirc(SANITIZER, step_to_init, not_first_call);
        case 78: return InitOporCirc(KANAL, step_to_init, not_first_call);
        case 79: return InitOporCIP(KANAL, step_to_init, not_first_call);

        case 81: return InitFilCirc(WITH_WATER, step_to_init, not_first_call);
        case 83: return InitToObject(WATER, cleanrinsingto, step_to_init, not_first_call);
        case 84: return InitOporCIP(cleanrinsingto, step_to_init, not_first_call);
        case 85: return InitFromObject(WATER, cleanrinsingto, step_to_init, not_first_call);
        case 86: return InitToObject(WATER, cleanrinsingto, step_to_init, not_first_call);
        case 91: return InitOporCIP(cleanrinsingto, step_to_init, not_first_call);
        case 105: return InitFilRR(TANK_S);
        case 106: return InitCircRR(TANK_S);
        case 108: return InitCheckConc(TANK_S);
        case 109: return InitAddRR(TANK_S, step_to_init, not_first_call);
        case 111: return InitOpolRR(TANK_S);
        case 115: return InitFilRR(TANK_K);
        case 116: return InitCircRR(TANK_K);
        case 118: return InitCheckConc(TANK_K);
        case 119: return InitAddRR(TANK_K, step_to_init, not_first_call);
        case 121: return InitOpolRR(TANK_K);

        case 151: return SCInitPumping(WATER, TANK_W, KANAL, -1, step_to_init, not_first_call);
        case 152: return SCInitPumping(WATER, TANK_W, KANAL, -1, step_to_init, not_first_call);
        case 153: return SCInitPumping(WATER, WATER, TANK_W_MG, -1, step_to_init, not_first_call);
        case 154: return SCInitPumping(WATER, -1, -1, TANK_W_DREN, step_to_init, not_first_call);
        case 155: return SCInitPumping(SHCH, TANK_S, TANK_S, -1, step_to_init, not_first_call);
        case 156: return SCInitPumping(SHCH, TANK_S, TANK_W_MG, -1, step_to_init, not_first_call);
        case 157: return SCInitPumping(SHCH, TANK_S, TANK_W_MG, -1, step_to_init, not_first_call);
            //	case 158: return SCInitPumping(SHCH, TANK_S, TANK_W_MG, -1, step, f);
            //	case 159: return SCInitPumping(SHCH, TANK_S, NEUTRO, -1, step, f);
        case 160: return SCInitPumping(SHCH, WATER, TANK_S, -1, step_to_init, not_first_call);
        case 161: return SCInitPumping(SHCH, TANK_W, TANK_W_MG, TANK_S_DREN, step_to_init, not_first_call);
        case 162: return SCInitPumping(KISL, TANK_K, TANK_K, -1, step_to_init, not_first_call);
        case 163: return SCInitPumping(KISL, TANK_K, TANK_S_MG, -1, step_to_init, not_first_call);
        case 164: return SCInitPumping(KISL, TANK_K, TANK_S_MG, -1, step_to_init, not_first_call);
            //	case 165: return SCInitPumping(KISL, TANK_K, TANK_S_MG, -1, step, f);
            //	case 166: return SCInitPumping(KISL, TANK_K, NEUTRO, -1, step, f);
        case 167: return SCInitPumping(KISL, WATER,TANK_K_MG, -1, step_to_init, not_first_call);
        case 168: return SCInitPumping(KISL, TANK_S, TANK_S_MG, TANK_K_DREN, step_to_init, not_first_call);
        case 169: return SCInitPumping(SHCH, TANK_W, TANK_K_MG, -1, step_to_init, not_first_call);
        case 170: return SCInitPumping(SHCH, TANK_W, TANK_K_MG, -1, step_to_init, not_first_call);
            //	case 171: return SCInitPumping(SHCH, TANK_W, TANK_K_MG, -1, step, f);
            //	case 172: return SCInitPumping(SHCH, TANK_W, NEUTRO, -1, step, f);
        case 173: return SCInitPumping(SHCH, WATER, TANK_W, -1, step_to_init, not_first_call);
        case 174: return SCInitPumping(SHCH, TANK_K,TANK_K_MG, TANK_W_DREN, step_to_init, not_first_call);
        case 175: return SCInitPumping(KISL, TANK_S, TANK_W_MG, -1, step_to_init, not_first_call);
        case 176: return SCInitPumping(KISL, TANK_S, NEUTRO, -1, step_to_init, not_first_call);
        case 177: return SCInitPumping(KISL, TANK_S, TANK_W_MG, -1, step_to_init, not_first_call);
        case 178: return SCInitPumping(KISL, TANK_S, NEUTRO, -1, step_to_init, not_first_call);
        case 179: return SCInitPumping(KISL, WATER, TANK_S_MG, -1, step_to_init, not_first_call);
        case 180: return SCInitPumping(KISL, TANK_W, TANK_W_MG, TANK_S_DREN, step_to_init, not_first_call);
        case 181: return SCInitPumping(SHCH, TANK_K, NEUTRO, -1, step_to_init, not_first_call);
        case 182: return SCInitPumping(SHCH, TANK_K, NEUTRO, -1, step_to_init, not_first_call);
        case 183: return SCInitPumping(KISL, TANK_W, NEUTRO, TANK_K_DREN, step_to_init, not_first_call);
        case 184: return SCInitPumping(KISL, TANK_W, NEUTRO, TANK_K_DREN, step_to_init, not_first_call);
        case 185: return SCInitPumping(WATER, WATER, TANK_S_MG, TANK_KW_DREN, step_to_init, not_first_call);
        case 186: return SCInitPumping(WATER, WATER, TANK_K_MG, TANK_SW_DREN, step_to_init, not_first_call);
        case 187: return SCInitPumping(WATER, WATER, TANK_W_MG, TANK_SK_DREN, step_to_init, not_first_call);
        case 188: return SCInitPumping(-1, -1, -1, TANK_SKW_DREN, step_to_init, not_first_call);

        case 300: return InitCustomStep(WATER, WATER, KANAL, 0, step_to_init, not_first_call);

        case 555:
            RHI();
            PauseTimers();
            cnt->pause();
            NP->off();
            nplaststate = false;
            if (PIDP->HI==0) PIDP->off();
            if (PIDF->HI==0) PIDF->off();
            ret_overrride = 0;
            SetRet(OFF);
            StopDev();
            V11->on();
            //Смена среды
            if (dev_upr_medium_change)
                {
                dev_upr_medium_change->off();
                }
            if (dev_upr_cip_finished)
                {
                dev_upr_cip_finished->on();
                }
            if (dev_upr_cip_finished2)
                {
                if (dev_upr_wash_aborted)
                    {
                    if (program_was_terminated)
                        {
                        dev_upr_wash_aborted->on();
                        }
                    else
                        {
                        dev_upr_cip_finished2->on();
                        }
                    }
                else
                    {
                    dev_upr_cip_finished2->on();
                    }
                }
            enddelayTimer = get_millisec();
            return 0;
        }
    return ERR_UNKNOWN_STEP;
    }

int cipline_tech_object::EvalPIDS()
    {
    PIDP->eval();
    if (!pidf_override)
        {
        if (dev_ai_pump_frequency)
            {
            if (dev_ai_pump_feedback)
                {
                PIDF->eval(dev_ai_pump_feedback->get_value(), dev_ai_pump_frequency->get_value());
                }
            else
                {
                NP->set_value(dev_ai_pump_frequency->get_value());
                }
            }
        else if (dev_ai_pump_feedback)
            {
            if (rt_par_float[P_PRESSURE_CONTROL] > 0 && PRESSURE)
                {
                PIDF->eval(PRESSURE->get_value(), dev_ai_pump_feedback->get_value());
                rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
                }
            else
                {
                PIDF->eval(cnt->get_flow(), dev_ai_pump_feedback->get_value());
                }
            }
        else
            {
            if (rt_par_float[P_PRESSURE_CONTROL] > 0 && PRESSURE)
                {
                PIDF->eval(PRESSURE->get_value(), rt_par_float[P_PRESSURE_CONTROL]);
                rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
                }
            else
                {
                PIDF->eval();
                }
            }
        }

    if (dev_os_object_pause || dev_os_pump_can_run)
        {
        bool pump_can_run = true;
        if (dev_os_pump_can_run)
            {
            if (!dev_os_pump_can_run->get_state()) pump_can_run = false;
            }

        if (dev_os_object_pause)
            {
            if (dev_os_object_pause->get_state()) pump_can_run = false;
            }

        if (!pump_can_run)
            {
            flagnplaststate = true;
            NP->off();
            NP->set_value(0);
            ao->set_value(0);
            if (dev_upr_pump_stopped)
                {
                if (cnt->get_flow() < rt_par_float[P_R_NO_FLOW])
                    {
                    dev_upr_pump_stopped->on();
                    }
                else
                    {
                    dev_upr_pump_stopped->off();
                    }
                }
            }
        else
            {
            if (dev_upr_pump_stopped)
                {
                dev_upr_pump_stopped->off();
                }
            if (flagnplaststate)
                {

                if (nplaststate)
                    {
                    PIDF->pid_reset();
                    PIDP->pid_reset();
                    NP->on();
                    flagnplaststate = false;
                    }
                }
            }

        if (dev_ao_flow_task)
        {
            dev_ao_flow_task->set_value(rt_par_float[P_ZAD_FLOW]);
        }

        if (dev_ao_temp_task)
        {
            dev_ao_temp_task->set_value(rt_par_float[P_ZAD_PODOGR]);
        }

        }


    //Клапан пара
    if (ao->get_value()>1 && PIDP->get_state() == ON && cnt->get_flow() > rt_par_float[P_R_NO_FLOW] && NP->get_state() == ON)
        {
        if (get_delta_millisec(steam_valve_delay) > STEAM_VALVE_MIN_DELAY)
            {
            V13->on();
            }
        }
    else
        {
        V13->off();
        steam_valve_delay = get_millisec();
        }
    return 0;
    }

int cipline_tech_object::EvalCipInProgress()
    {
    int res;

    res=DoStep(curstep);

    if (res>0 && curstep != 555)
        { //All Ok step is over
        int nowstep = curstep;
        curstep=GoToStep(curstep, res);
        if (curstep!=nowstep)
            {
            rt_par_float[STP_LAST_STEP] = nowstep;
            InitStep(curstep, 0);
            }
        }
    else
        {
        if (res<0)
            {//was error, time it stopped
            state = res;
            Stop(curstep);
            state = res;

            auto err_msg_it = ERR_MSG.find( res );
            const char* err_msg =
                ( err_msg_it != ERR_MSG.end() ) ? err_msg_it->second : UNKNOWN_ERR_MSG;
            std::string formatted_err_msg = fmt::format( "ошибка '{}' ({})", err_msg, res );
            set_err_msg( formatted_err_msg.c_str(), 0, 0, ERR_MSG_TYPES::ERR_ALARM );
            return res;
            }
        else
            {
            if (isLine() && curstep != 0 && curstep != 555) SetRet(ON);
            }
        }
    return 0;
    }

int cipline_tech_object::_DoStep( int step_to_do )
{
    int res, pr_media;
    bool is_caustic = false;
    bool is_acid = false;
    bool is_water = false;
    bool is_prerinse = false;
    bool is_intermediate_rinse = false;
    bool is_postrinse = false;

    if (step_to_do >= 24 && step_to_do <= 33) is_caustic = true;
    if (step_to_do >= 44 && step_to_do <= 53) is_acid = true;


    if (step_to_do == 7 || step_to_do == 8 || step_to_do == 5 || step_to_do == 16) is_prerinse = true;

    if (step_to_do == 35 || step_to_do == 37 || step_to_do == 55 || step_to_do == 57) is_intermediate_rinse = true;

    if (step_to_do == 85 || step_to_do == 86)
        {
        is_postrinse = true;
        }

    if (is_prerinse || is_postrinse || is_intermediate_rinse) is_water = true;

    if (dev_upr_prerinse && dev_upr_prerinse == dev_upr_intermediate_rinse && dev_upr_intermediate_rinse == dev_upr_postrinse)
        {
        if (is_prerinse || is_intermediate_rinse || is_postrinse)
            {
            dev_upr_prerinse->on();
            }
        else
            {
            dev_upr_prerinse->off();
            }
        }
    else
        {
        if (dev_upr_prerinse)
            {
            if (is_prerinse) dev_upr_prerinse->on(); else dev_upr_prerinse->off();
            }
        if (dev_upr_intermediate_rinse)
            {
            if (is_intermediate_rinse) dev_upr_intermediate_rinse->on(); else dev_upr_intermediate_rinse->off();
            }
        if (dev_upr_postrinse)
            {
            if (is_postrinse) dev_upr_postrinse->on(); else dev_upr_postrinse->off();
            }
        }


    if (dev_upr_caustic && dev_upr_caustic == dev_upr_acid)
        {
        if (is_caustic || is_acid)
            {
            dev_upr_caustic->on();
            }
        else
            {
            dev_upr_caustic->off();
            }
        }
    else
        {
        if (dev_upr_caustic)
            {
            if (is_caustic) dev_upr_caustic->on(); else dev_upr_caustic->off();
            }
        if (dev_upr_acid)
            {
            if (is_acid) dev_upr_acid->on(); else dev_upr_acid->off();
            }
        }
    if (dev_upr_water)
        {
        if (is_water) dev_upr_water->on(); else dev_upr_water->off();
        }
    if (dev_upr_desinfection)
        {
        if ( ( step_to_do >= 64 && step_to_do <= 66 ) || step_to_do == 77 ) dev_upr_desinfection->on(); else dev_upr_desinfection->off();
        }
    if (dev_upr_circulation)
        {
        if  (   (steps_circulation.count(step_to_do) && circ_temp_reached) ||
                (
                    (
                        steps_additional_rinse.count(step_to_do) ||
                        (use_circulation_on_v2_supply && steps_v2_supply.count(step_to_do))
                    )
                )
            )
            {
            dev_upr_circulation->on();
            }
        else
            {
            dev_upr_circulation->off();
            }
        }

    if (dev_ls_ret_pump)
        {
        if (dev_ls_ret_pump->is_active())
            {
            ret_pums_ls_timer = get_millisec();
            }
        }

    res=CheckErr();
    if (res!=0)
        {
        blockAlarm = 0;
        return res;
        }

    pr_media=WATER;
    if (step_to_do>30 && step_to_do<42)
        {
        if ((((int)rt_par_float[P_PROGRAM]) >> PRG_K) & 1)
            {
            pr_media=TANK_W;
            }
        }

    int cleanrinsingto = clean_water_rinsing_return;

    int tank_w_dest = TANK_W;
    int tank_w_src = TANK_W;

    if (1 == dont_use_water_tank || (rt_par_float[P_DONT_USE_WATER_TANK] == 1))
    {
        tank_w_dest = KANAL;
        tank_w_src = WATER;
        pr_media = WATER;
    }

    switch (step_to_do)
        {
        case 0:
            if (LM->is_active()) return 1;
            return 0;
        case 5:  return ToObject(tank_w_src, KANAL);
        case 6:  return OporCIP(KANAL);
        case 7:  return FromObject(tank_w_src, KANAL);
        case 8:  return ToObject(tank_w_src, KANAL);
        case 9:  return OporCIP(KANAL);
        case 10: return FillCirc(WITH_RETURN);
        case 11: return Circ(WATER);
        case 12: return OporCirc(KANAL);
        case 13: return FillCirc(WITH_RETURN);
        case 14: return ToObject(tank_w_src, KANAL);
        case 15: return OporCirc(KANAL);
        case 16: return OporCirc(KANAL);
        case 22: return ToObject(TANK_S, KANAL);
        case 23: return OporCIP(KANAL);
        case 24: return FromObject(TANK_S, KANAL);
        case 26: return FillCirc(WITH_RETURN);
        case 28: return Circ(SHCH);
        case 29: return OporCirc(TANK_S);

        case 31: return FillCirc(WITH_WATER);
        case 33: return ToObject(pr_media, TANK_S);
        case 34: return OporCIP(TANK_S);
        case 35: return FromObject(pr_media, TANK_S);
        case 37: return ToObject(pr_media, tank_w_dest);
        case 39: return OporCirc(tank_w_dest);
        case 40: return OporCIP(tank_w_dest);

        case 42: return ToObject(TANK_K, KANAL);
        case 43: return OporCIP(KANAL);
        case 44: return FromObject(TANK_K, KANAL);
        case 46: return FillCirc(WITH_RETURN);
        case 48: return Circ(KISL);
        case 49: return OporCirc(TANK_K);
        case 53: return ToObject(WATER, TANK_K);
        case 54: return OporCIP(TANK_K);
        case 55: return FromObject(WATER, TANK_K);
        case 57: return ToObject(WATER, tank_w_dest);
        case 59: return OporCirc(tank_w_dest);
        case 60: return OporCIP(tank_w_dest);
        case 61: return FillCirc(WITH_WATER);
        case 62: return ToObject(WATER, tank_w_dest);
        case 63: return OporCIP(tank_w_dest);
        case 64: return FromObject(WATER, tank_w_dest);
        case 65: return FillCirc(WITH_WATER);
        case 66: return Circ(HOT_WATER);
        case 67: return OporCirc(tank_w_dest);

        case 71: return FillCirc(SANITIZER);
        case 72: return ToObject(SANITIZER, tank_w_dest);
        case 73: return OporCIP(tank_w_dest);
        case 74: return FromObject(SANITIZER,tank_w_dest);
        case 75: return FillCirc(SANITIZER);
        case 76: return DoseRR(SANITIZER);
        case 77: return Circ(SANITIZER);
        case 78: return OporCirc(KANAL);
        case 79: return OporCIP(KANAL);

        case 81: return FillCirc(WITH_WATER);
        case 83: return ToObject(WATER, cleanrinsingto);
        case 84: return OporCIP(cleanrinsingto);
        case 85: return FromObject(WATER, cleanrinsingto);
        case 86: return ToObject(WATER, cleanrinsingto);
        case 91: return OporCIP(cleanrinsingto);
        case 105: return FilRR(TANK_S);
        case 106: return CircRR(TANK_S);
        case 108: return CheckConc(TANK_S);
        case 109: return AddRR(TANK_S);
        case 111: return OpolRR(TANK_S);
        case 115: return FilRR(TANK_K);
        case 116: return CircRR(TANK_K);
        case 118: return CheckConc(TANK_K);
        case 119: return AddRR(TANK_K);
        case 121: return OpolRR(TANK_K);

        case 151: return SCPumping(WATER, TANK_W, KANAL, -1);
        case 152: return SCPumping(WATER, TANK_W, KANAL, -1);
        case 153: return SCPumping(WATER, WATER, TANK_W_MG, -1);
        case 154: return SCPumping(WATER, -1, -1, TANK_W_DREN);
        case 155: return SCPumping(SHCH, TANK_S, TANK_S, -1);
        case 156: return SCPumping(SHCH, TANK_S, TANK_W_MG, -1);
        case 157: return SCPumping(SHCH, TANK_S, TANK_W_MG, -1);
            //	case 158: return SCPumping(SHCH, TANK_S, TANK_W_MG, -1);
            //	case 159: return SCPumping(SHCH, TANK_S, NEUTRO, -1);
        case 160: return SCPumping(SHCH, WATER, TANK_S, -1);
        case 161: return SCPumping(SHCH, TANK_W, TANK_W_MG, TANK_S_DREN);
        case 162: return SCPumping(KISL, TANK_K, TANK_K, -1);
        case 163: return SCPumping(KISL, TANK_K, TANK_S_MG, -1);
        case 164: return SCPumping(KISL, TANK_K, TANK_S_MG, -1);
            //	case 165: return SCPumping(KISL, TANK_K, TANK_S_MG, -1);
            //	case 166: return SCPumping(KISL, TANK_K, NEUTRO, -1);
        case 167: return SCPumping(KISL, WATER,TANK_K_MG, -1);
        case 168: return SCPumping(KISL, TANK_S, TANK_S_MG, TANK_K_DREN);
        case 169: return SCPumping(SHCH, TANK_W, TANK_K_MG, -1);
        case 170: return SCPumping(SHCH, TANK_W, TANK_K_MG, -1);
            //	case 171: return SCPumping(SHCH, TANK_W, TANK_K_MG, -1);
            //	case 172: return SCPumping(SHCH, TANK_W, NEUTRO, -1);
        case 173: return SCPumping(SHCH, WATER, TANK_W, -1);
        case 174: return SCPumping(SHCH, TANK_K,TANK_K_MG, TANK_W_DREN);
        case 175: return SCPumping(KISL, TANK_S, TANK_W_MG, -1);
        case 176: return SCPumping(KISL, TANK_S, TANK_W_MG, -1);
            //	case 177: return SCPumping(KISL, TANK_S, TANK_W_MG, -1);
            //	case 178: return SCPumping(KISL, TANK_S, NEUTRO, -1);
        case 179: return SCPumping(KISL, WATER, TANK_S_MG, -1);
        case 180: return SCPumping(KISL, TANK_W, TANK_W_MG, TANK_S_DREN);
        case 181: return SCPumping(SHCH, TANK_K, NEUTRO, -1);
        case 182: return SCPumping(SHCH, TANK_K, NEUTRO, -1);
        case 183: return SCPumping(KISL, TANK_W, NEUTRO, TANK_K_DREN);
        case 184: return SCPumping(KISL, TANK_W, NEUTRO, TANK_K_DREN);
        case 185: return SCPumping(WATER, WATER, TANK_S_MG, TANK_KW_DREN);
        case 186: return SCPumping(WATER, WATER, TANK_K_MG, TANK_SW_DREN);
        case 187: return SCPumping(WATER, WATER, TANK_W_MG, TANK_SK_DREN);
        case 188: return SCPumping(-1, -1, -1, TANK_SKW_DREN);

        case 300: return EvalCustomStep(WATER, WATER, KANAL, 0);

        case 555:
            bool can_end = true;
            if (dev_os_cip_ready && (dev_upr_cip_finished || dev_upr_cip_finished2))
                {
                if (dev_os_cip_ready->get_state() == ON)
                    {
                    can_end = false;
                    }
                }

            if (is_reset) can_end = true;

            unsigned long wash_end_delay = WASH_END_DELAY;
            if (parpar[0][P_END_WASH_DELAY] > 0) wash_end_delay = parpar[0][P_END_WASH_DELAY];

            if (get_delta_millisec(enddelayTimer) > wash_end_delay && can_end)
                {
                strcpy(objectstats->objlastwashprogram, currentProgramName);
                DateToChar(objectstats->objlastwash);
                objectstats->changed = 0;
                if (rt_par_float[STP_RESETSTEP] == 0)
                    {
                    statsbase->apply();
                    }
                Stop(curstep);
                curstep=0;
                ResetLinesDevicesBeforeReset();
                ResetWP();
                return 0;
                }
            else
                {
                return 0;
                }
        }
    return ERR_UNKNOWN_STEP;
    }

    int cipline_tech_object::EvalCipInError()
        {
        if (rt_par_float[P_RESUME_CIP_ON_SIGNAL] != 0)
            {
            int objready = 1;
            if (dev_os_object)
                {
                if (dev_os_object->get_state() == OFF)
                    {
                    objready = 0;
                    }
                }
            if (dev_os_object_ready)
                {
                if (dev_os_object_ready->get_state() == OFF)
                    {
                    objready = 0;
                    }
                }

            if (dev_os_cip_ready)
                {
                if (dev_os_cip_ready->get_state() == OFF)
                    {
                    objready = 0;
                    }
                }
            if ( dev_watchdog && dev_watchdog->get_state() != ON )
                {
                objready = 0;
                }

            if (objready && (state == ERR_CIP_OBJECT || state == ERR_OS || state == ERR_WATCHDOG ))
                {
                state = 1;
                InitStep(curstep, 1);
                }
            }

        if (2 == is_in_error_func)
            {
            lua_State* L = lua_manager::get_instance()->get_Lua();
            lua_getglobal( L, name_Lua );
            lua_getfield( L, -1, "cip_in_error" );
            lua_remove( L, -2 );  // Stack: remove OBJECT.
            lua_getglobal( L, name_Lua );
            if (0 != lua_pcall(L, 1, 0, 0))
                {
                printf( "Error in calling cip_in_error: %s\n", lua_tostring( L, -1 ) );
                lua_pop(L, 1);
                }
            }

        return 0;
        }

void cipline_tech_object::_ResetLinesDevicesBeforeReset( )
    {
    curprg = -1;
    if (valvesAreInConflict)
        {
        valvesAreInConflict = 0;
        }
    else
        {
        lineRecipes->OffRecipeDevices(loadedRecipe, nmr);
        closeLineValves();
        }
    loadedRecipe = -1;
    loadedProgram = -1;
    ResetStat();
    rt_par_float[P_PROGRAM] = 0;
    rt_par_float[P_RET_STATE] = 0;
    if (dev_upr_acid)
        {
        dev_upr_acid->off();
        }
    if (dev_upr_water)
        {
        dev_upr_water->off();
        }
    if (dev_upr_circulation)
        {
        dev_upr_circulation->off();
        }
    if (dev_upr_caustic)
        {
        dev_upr_caustic->off();
        }
    if (dev_upr_cip_finished)
        {
        dev_upr_cip_finished->off();
        }
    if (dev_upr_cip_finished2)
        {
        dev_upr_cip_finished2->off();
        }
    if (dev_upr_cip_in_progress)
        {
        dev_upr_cip_in_progress->off();
        }
    if (dev_upr_cip_ready)
        {
        dev_upr_cip_ready->off();
        }
    if (dev_upr_sanitizer_pump)
        {
        dev_upr_sanitizer_pump->off();
        }
    if (dev_upr_desinfection)
        {
        dev_upr_desinfection->off();
        }
    if (dev_upr_prerinse)
        {
        dev_upr_prerinse->off();
        }
    if (dev_upr_intermediate_rinse)
        {
        dev_upr_intermediate_rinse->off();
        }
    if (dev_upr_postrinse)
        {
        dev_upr_postrinse->off();
        }
    if (dev_upr_pump_stopped)
        {
        dev_upr_pump_stopped->off();
        }
    if (dev_upr_wash_aborted)
        {
        dev_upr_wash_aborted->off();
        }
    if (dev_ao_flow_task)
        {
        dev_ao_flow_task->set_value(0);
        }
    if (dev_ao_temp_task)
        {
        dev_ao_temp_task->set_value(0);
        }

    dev_upr_circulation = nullptr;
    dev_upr_cip_in_progress = nullptr;
    dev_upr_ret = nullptr;
    dev_m_ret = nullptr;
    nplaststate = false;
    flagnplaststate = false;
    dev_os_object = nullptr;
    dev_os_object_pause = nullptr;
    dev_os_object_ready = nullptr;
    dev_os_cip_ready = nullptr;
    dev_os_object_empty = nullptr;
    dev_upr_medium_change = nullptr;
    dev_upr_caustic = nullptr;
    dev_upr_acid = nullptr;
    dev_upr_water = nullptr;
    dev_upr_desinfection = nullptr;
    dev_upr_cip_ready = nullptr;
    dev_upr_cip_finished = nullptr;
    dev_upr_cip_finished2 = nullptr;
    dev_upr_sanitizer_pump = nullptr;
    dev_os_pump_can_run = nullptr;
    dev_ai_pump_feedback = nullptr;
    dev_ai_pump_frequency = nullptr;
    dev_ls_ret_pump = nullptr;
    dev_os_can_continue = nullptr;

    dev_upr_prerinse = nullptr;
    dev_upr_intermediate_rinse = nullptr;
    dev_upr_postrinse = nullptr;
    dev_upr_pump_stopped = nullptr;
    dev_ao_flow_task = nullptr;
    dev_ao_temp_task = nullptr;
    dev_upr_wash_aborted = nullptr;
    dev_watchdog = nullptr;

    no_liquid_is_warning = 0;
    no_liquid_phase = 0;
    pidf_override = false;

    program_has_acid = false;
    program_has_caustic = false;
    program_has_cold_desinfection = false;
    program_has_desinfection = false;
    program_was_terminated = false;
    //Переменные для циркуляции
    circ_tank_s = 0;
    circ_tank_k = 0;
    circ_podp_water = 0;
    circ_podp_s = 0;
    circ_podp_k = 0;
    circ_podp_count = 0;
    circ_podp_max_count = 0;
    circ_water_no_pump_stop = 0;
    circ_medium_no_pump_stop = 0;
    clean_water_rinsing_return = TANK_W;
    disable_final_rinsing = false;
    if (scenabled && scline == nmr)
        {
        scline = 0;
        for (int tmpline = 0; tmpline < MdlsCNT; tmpline++)
            {
            if (Mdls[tmpline]->nmr != nmr)
                Mdls[tmpline]->resetProgramList();
            }
        }
    is_reset = false;
    }

int cipline_tech_object::SetCommand( int command )
    {
    int l;
    if (ncmd==0)
        {
        ncmd=command;
        return 0;
        }
    l=ncmd;
    ncmd=command;
    return l;
    }

int cipline_tech_object::_LoadProgram( )
    {
    int selectedprogram = (int)rt_par_float[P_PROGRAM];
    if (selectedprogram & (1 << PRG_K))
        {
        program_has_acid = true;
        }
    else
        {
        program_has_acid = false;
        }

    if (selectedprogram & ((1 << PRG_S) + (1 << PRG_S1)))
        {
        program_has_caustic = true;
        }
    else
        {
        program_has_caustic = false;
        }

    if (selectedprogram & (1 << PRG_D))
        {
        program_has_desinfection = true;
        }
    else
        {
        program_has_desinfection = false;
        }

    if (selectedprogram & (1 << PRG_SANITIZER))
        {
        program_has_cold_desinfection = true;
        }
    else
        {
        program_has_cold_desinfection = false;
        }


    curprg=getNexpPrg(curprg, selectedprogram);
    switch (curprg)
        {
        case PRG_PRO:
            return 5;
        case PRG_CIRC:
            return 10;
        case PRG_S:
            return 22;
        case PRG_K:
            return 42;
        case PRG_S1:
            return 22;
        case PRG_D:
            return 61;
        case PRG_SANITIZER:
            return 71;
        case PRG_OKO:
            if (disable_final_rinsing) return 91;
            if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
                {
                return 83;
                }
            else
                {
                return 81;
                }
        case PRG_NAVS:
            return 105;
        case PRG_NAVK:
            return 115;
        case PRG_SELFCLEAN:
            return 151;
        case PRG_ASO:
            return 300;
        case -1:
            return 555;
        default:
            return 555;
        }
    }

void cipline_tech_object::ResetWP()
    {
    for ( auto i = 1; i < P_RESERV_START; i++ )
        {
        // Некоторые параметры используются при наведении раствора,
        // поэтому их не сбрасываем.
        // Необходимо сделать рецепты для наведения растворов.
        if ( i != P_CONC_RATE && ( i < P_R_NO_FLOW || i > PIDF_Uk ) )
            {
            rt_par_float[ i ] = 0;
            }
        }
    rt_par_float[ P_SELECT_PRG ] = -1;
    rt_par_float[ P_SELECT_REC ] = -1;
    rt_par_float[ P_CUR_REC ] = 1;
    rt_par_float[ P_LOADED_RECIPE ] = 0;
    resetProgramName();
    resetRecipeName();
    resetProgramList();
    resetCarNumber();
    switch1 = 0;
    switch2 = 0;
    switch3 = 0;
    switch4 = 0;
    }

int cipline_tech_object::GetRetState() const
    {
    if (0 == state)
        {
        return 0;
        }
    if (dev_m_ret)
        {
        return dev_m_ret->get_state();
        }
    if (dev_upr_ret)
        {
        return dev_upr_ret->get_state();
        }
    return 0;
    }

int cipline_tech_object::HasRet() const
    {
    if (0 == state)
        {
        return 0;
        }
    if (dev_upr_ret || dev_m_ret)
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

int cipline_tech_object::ForceRet( int val )
    {
    ret_overrride = 0;
    SetRet(val);
    ret_overrride = 1;
    return 0;
    }

void cipline_tech_object::ResetErr( )
    {
    T[TMR_NO_FLOW]->reset();
    }

int cipline_tech_object::_CheckErr( )
    {
    float delta=0;
    auto block_flags = (unsigned long)parpar[0][P_BLOCK_ERRORS];

    if (555 == curstep)
        {
        return 0;
        }

    if (NP->get_state() <= -1)
        {
        if (!pumpflag)
            {
            pumpflag = 1;
            pumptimer = get_millisec();
            }
        if (get_delta_millisec(pumptimer) > 1500)
            {
            return ERR_PUMP;
            }
        }
    else
        {
        pumpflag = 0;
        }

    //обратная связь возвратного насоса
    if (dev_m_ret)
        {
        if (-1 == dev_m_ret->get_state()) return ERR_RET;
        }
    //Проверка обратной связи объекта
    if (dev_os_object)
        {
        if (!dev_os_object->get_state())
            {
            return ERR_OS;
            }
        }
    if (dev_os_object_ready)
        {
        if (!dev_os_object_ready->get_state())
            {
            return ERR_CIP_OBJECT;
            }
        }
    if (dev_os_cip_ready)
        {
        if (!dev_os_cip_ready->get_state())
            {
            return ERR_CIP_OBJECT;
            }
        }
    if ( dev_watchdog && dev_watchdog->get_state() != ON )
        {
        return ERR_WATCHDOG;
        }

    //проверка уровней в бачке
    if ((!LL->is_active() && (LM->is_active() || LH->is_active())) || (!LM->is_active() && LH->is_active()))
        {
        if ( get_delta_millisec( bachok_lvl_err_delay ) > 5000L && !(block_flags & (1 << BE_ERR_LEVEL_BACHOK))) //если ошибка уровня больше 5 секунд
            {
            return ERR_LEVEL_BACHOK;
            }
        }
    else
        {
        bachok_lvl_err_delay = get_millisec();
        }

    //проверка уровней в танке щелочи
    if ((((curstep >= 22) && (curstep <= 40)) || ((curstep >= 105) && (curstep <= 111))) &&
        !LSL->is_active( ) && LSH->is_active( ) && !(block_flags & (1 << BE_ERR_LEVEL_TANK_S)))
        {
        return ERR_LEVEL_TANK_S;
        }

    //проверка уровней в танке кислоты
    if ((((curstep >= 42) && (curstep <= 60)) || ((curstep >= 115) && (curstep <= 121))) &&
        !LKL->is_active( ) && LKH->is_active( ) && !(block_flags & (1 << BE_ERR_LEVEL_TANK_K)))
        {
        return ERR_LEVEL_TANK_K;
        }

    //проверка уровней в танке вторичной воды
    if ( !LWL->is_active( ) && LWH->is_active( ) && !(block_flags & (1 << BE_ERR_LEVEL_TANK_W)))
        {
        return ERR_LEVEL_TANK_W;
        }

    //проверка датчика температуры на подаче
    if ((TP->get_state( ) <= -1 || TP->get_value( ) <= -1.0f) && !(block_flags & (1 << BE_ERR_SUPPLY_TEMP_SENSOR)))
        {
        return ERR_SUPPLY_TEMP_SENSOR;
        }

    //проверка датчика температуры на возврате
    if ((TR->get_state( ) <= -1 || TR->get_value( ) <= -1.0f) && !(block_flags & (1 << BE_ERR_RETURN_TEMP_SENSOR)))
        {
        return ERR_RETURN_TEMP_SENSOR;
        }

    //проверка датчика концентрации
    if ((steps_caustic.count( curstep ) || steps_acid.count( curstep )) && Q->get_state( ) <= -1 &&
        !(block_flags & (1 << BE_ERR_CONCENTRATION_SENSOR)))
        {
        return ERR_CONCENTRATION_SENSOR;
        }

    // Нет расхода на подаче
    if ( auto time = static_cast<uint32_t>( rt_par_float[ P_TM_R_NO_FLOW ] * 1000 );
        T[ TMR_NO_FLOW ]->get_countdown_time() != time )
        {
        T[ TMR_NO_FLOW ]->set_countdown_time( time );
        }
    if (NP->get_state() == ON)
        {
        delta = rt_par_float[P_R_NO_FLOW];
        if (cnt->get_flow() <= delta)
            {
            T[TMR_NO_FLOW]->start();
            if (T[TMR_NO_FLOW]->is_time_up())
                {
				return ERR_NO_FLOW;
                }
            }
        else
            {
            T[TMR_NO_FLOW]->reset();
            }
        }
    return 0;
    }

void cipline_tech_object::SortRR( int where, int forcetotank /*= 0*/ )
{
    if (get_delta_millisec(sort_delay) < SORT_SWITCH_DELAY && !forcesortrr)
        {
        return;
        }
    else
        {
        sort_delay = get_millisec();
        forcesortrr = 0;
        }
    float c;
    int d;
    d=0;
    V07->off();
    switch (where)
        {
        case TANK_S:
            V08->off();
            c=GetConc(SHCH);

            if (LSH->is_active() && !forcetotank)
                {
                tankfull = 1;
                tankfulltimer = get_millisec();
                }

            if (tankfull)
                {
                if (get_delta_millisec(tankfulltimer) > 20000L)
                    {
                    tankfull = 0;
                    }
                }

            if (c>=parpar[0][P_CMIN_S] && !tankfull)
                {
                d=2;
                }
            else
                {
                if (c>=parpar[0][P_CKANAL_S])
                    {
                    d=1;
                    }
                else
                    {
                    d=0;
                    }
                }
            switch (d)
                {
                case 0: //kanal
                    V09->off();
                    V10->off();
                    V11->on();
                    V12->off();
                    break;
                case 1: //neutro
                    V09->off();
                    V10->off();
                    V11->off();
                    V12->off();
                    if (no_neutro) {V11->on();} else {V12->on();}
                    break;
                case 2: //tank
                    V09->on();
                    V10->off();
                    V11->off();
                    V12->off();
                    break;
                }
            break;
        case TANK_K:
            V09->off();
            c=GetConc(KISL);

            if (LKH->is_active() && !forcetotank)
                {
                tankfull = 1;
                tankfulltimer = get_millisec();
                }

            if (tankfull)
                {
                if (get_delta_millisec(tankfulltimer) > 20000L)
                    {
                    tankfull = 0;
                    }
                }

            if (c>=parpar[0][P_CMIN_K] && !tankfull)
                {
                d=2;
                }
            else
                {
                if (c>=parpar[0][P_CKANAL_K])
                    {
                    d=1;
                    }
                else
                    {
                    d=0;
                    }
                }
            switch (d)
                {
                case 0: //kanal
                    V08->off();
                    V10->off();
                    V11->on();
                    V12->off();
                    break;
                case 1: //neutro
                    V08->off();
                    V10->off();
                    V11->off();
                    V12->off();
                    if (no_neutro) {V11->on();} else {V12->on();}
                    break;
                case 2: //tank
                    V08->on();
                    V10->off();
                    V11->off();
                    V12->off();
                    break;
                }
            break;
        }
    }

float cipline_tech_object::GetConc( int what ) const
    {
    float x25, a, ms25, c25, c, divider;
    c=0;
    switch (what)
        {
        case SHCH:
            a=parpar[0][P_ALFS]/100;
            ms25=parpar[0][P_MS25S];
            c25=parpar[0][P_CONC25S];
            divider = 1+a*(TR->get_value()-25);
            if (0 == divider)
                {
                divider = 1;
                }
            x25 = Q->get_value() /divider;
            divider = parpar[0][P_MS25W]-ms25;
            if (0 == divider)
                {
                divider = 1;
                }
            c = c25 * (ms25-x25) / divider + c25;
            break;
        case KISL:
            a=parpar[0][P_ALFK]/100;
            ms25=parpar[0][P_MS25K];
            c25=parpar[0][P_CONC25K];
            divider = 1+a*(TR->get_value()-25);
            if (0 == divider)
                {
                divider = 1;
                }
            x25=Q->get_value() / divider;
            divider = parpar[0][P_MS25W]-ms25;
            if (0 == divider)
                {
                divider = 1;
                }
            c = c25 * (ms25-x25) / divider + c25;
            break;
        }
    if (c<0)
        {
        return 0;
        }
    else
        {
        return c;
        }
    }

int cipline_tech_object::InitFilRR( int where )
    {
    float v = 0, pd = 0, kk = 0, kz = 0, ro = 0;
    V05->off();
    V06->on();
    V00->on();
    V01->on();
    V02->off();
    V03->off();
    V04->off();
    switch (where)
        {
        case TANK_K:
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            //       NK->on();
            pd=parpar[0][P_PDNK];
            kk=parpar[0][P_K_K]/100;
            kz=parpar[0][P_CZAD_K]/100;
            ro=parpar[0][P_RO_K];
            break;
        case TANK_S:
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            //       NS->on();
            pd=parpar[0][P_PDNS];
            kk=parpar[0][P_K_S]/100;
            kz=parpar[0][P_CZAD_S]/100;
            ro=parpar[0][P_RO_S];
            break;
        }

    NP->on();
    nplaststate = true;
    rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
    PIDP->on();
    V13->on();

    //calculate flow
    float divider = 1 - kz;
    if (0 == divider)
        {
        divider = 1;
        }
    if (kk == 0)
        {
        kk = 1;
        }
    if (ro == 0)
        {
        ro = 1;
        }
    v=kz*1000/((divider)*kk*ro);
    //   printf("\n\rfillRR v1M3: %f,pd: %f, kk: %f, kz: %f, ro: %f", v, pd, kk, kz, ro);
    //   par->setParamM(P_ZAD_FLOW, pd/v);
    rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
    PIDF->on();

    rt_par_float[P_VRAB] = 0;
    rt_par_float[P_ZAD_CONC] = kz*100;
    rt_par_float[P_CONC] = 0;
    rt_par_float[P_CONC_RATE] = 0;  //  Расход концентрата при наведении
    rt_par_float[P_OP_TIME_LEFT] = 0;
    divider = pd;
    if (0 == divider)
        {
        divider = 1;
        }
    rt_par_float[P_MAX_OPER_TM] = (v/divider)*MAX_OP_TIME*3600;
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    rt_par_float[P_SUM_OP] = 0;
    cnt->start();
    return 0;
    }

int cipline_tech_object::InitCircRR( int where )
    {
    float z=0;
    V05->off();
    V06->on();
    V00->instant_off();
    V01->off();
    V04->off();
    V07->off();

    V10->off();
    V11->off();
    V12->off();
    switch (where)
        {
        case TANK_K:
            V02->on();
            V03->off();
            V08->on();
            V09->off();
            NK->off();
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            V02->off();
            V03->on();
            V08->off();
            V09->on();
            NS->off();
            z=parpar[0][P_CZAD_S];
            break;
        }

    NP->on();
    nplaststate = true;
    rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
    PIDP->on();

    rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
    PIDF->on();

    rt_par_float[P_VRAB] = 0;
    rt_par_float[P_ZAD_CONC] = z;
    rt_par_float[P_CONC] = 0;

    rt_par_float[P_OP_TIME_LEFT] = 0;
    rt_par_float[P_MAX_OPER_TM] = parpar[0][P_TM_CIRC_RR];
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    rt_par_float[P_SUM_OP] = 0;
    cnt->start();
    return 0;
    }

int cipline_tech_object::InitCheckConc( int where )
    {
    InitCircRR(where);
    rt_par_float[P_MAX_OPER_TM] = parpar[0][P_TM_CHKC];
    T[TMR_OP_TIME]->reset();
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    return 0;
    }

int cipline_tech_object::InitAddRR( int where, int step, int first_init_flag )
{
    float v, pd = 0, kk = 0, kz = 0, ro = 0, vt = 0, kt;
    V05->off();
    V06->on();
    V00->instant_off();
    V01->off();
    V04->off();
    V07->off();
    V10->off();
    V11->off();
    V12->off();
    switch (where)
        {
        case TANK_K:
            V02->on();
            V03->off();
            V08->on();
            V09->off();
            NK->on();
            pd=parpar[0][P_PDNK];
            kk=parpar[0][P_K_K]/100;
            kz=parpar[0][P_CZAD_K]/100;
            ro=parpar[0][P_RO_K];
            vt=parpar[0][P_VTANKK];
            break;
        case TANK_S:
            V02->off();
            V03->on();
            V08->off();
            V09->on();
            NS->on();
            pd=parpar[0][P_PDNS];
            kk=parpar[0][P_K_S]/100;
            kz=parpar[0][P_CZAD_S]/100;
            ro=parpar[0][P_RO_S];
            vt=parpar[0][P_VTANKS];
            break;
        }
    kt=rt_par_float[P_CONC] / 100;

    NP->on();
    nplaststate = true;
    rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
    PIDP->on();

    rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
    PIDF->on();
    rt_par_float[P_ZAD_CONC] = kz*100;
    rt_par_float[P_CONC] = 0;

    rt_par_float[P_VRAB] = 0;

    rt_par_float[P_OP_TIME_LEFT] = 0;

    if (0 == first_init_flag)
        {
        float divider = (1-kz)*kk*ro;
        if (0 == divider)
            {
            divider = 1;
            }
        v=(kz-kt)*vt/(divider);

        if (v<1) v=1;
        divider = pd;
        if (0 == divider)
            {
            divider = 1;
            }
        rt_par_float[P_MAX_OPER_TM] = (v/divider)*3600;
        switch (no_liquid_phase)
            {
            case 0: //начало первого цикла добавления раствора - запоминаем время
                no_liquid_last_time = v/divider*1000;
                no_liquid_phase = 1;
                no_liquid_is_warning = 0;
                break;
            case 1: //не первый цикл. сравниваем время с предыдущим. Если совпадают с допуском до 10% выдаем 1 раз ошибку.
                if (fabs(no_liquid_last_time - v/divider*1000) < no_liquid_last_time * 0.1)
                    {
                    no_liquid_is_warning = 1;
                    }
                no_liquid_last_time = v/divider*1000;
                break;
            default:
                no_liquid_last_time = v/divider*1000;
                no_liquid_phase = 1;
                no_liquid_is_warning = 0;
                break;
            }
        T[ TMR_OP_TIME ]->set_countdown_time(
            static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );

        rt_par_float[P_SUM_OP] = 0;
        }
    T[TMR_OP_TIME]->start();
    cnt->start();

    return 0;
    }

int cipline_tech_object::InitOpolRR( int where )
    {
    float z=0;
    V05->off();
    V06->on();
    V00->on();
    V01->on();
    V02->off();
    V03->off();
    V04->off();
    switch (where)
        {
        case TANK_K:
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            NK->off();
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            NS->off();
            z=parpar[0][P_CZAD_S];
            break;
        }
    NP->on();
    nplaststate = true;
    rt_par_float[P_ZAD_PODOGR] = parpar[0][P_T_RR];
    PIDP->on();

    rt_par_float[P_ZAD_FLOW] = parpar[0][P_FLOW_RR];
    PIDF->on();

    rt_par_float[P_VRAB] = 0;

    rt_par_float[P_OP_TIME_LEFT] = 0;
    rt_par_float[P_MAX_OPER_TM] = 300;
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    T[TMR_CHK_CONC]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[P_TM_NO_CONC] * 1000 ) );
    rt_par_float[P_SUM_OP] = 0;

    rt_par_float[P_ZAD_CONC] = z;
    rt_par_float[P_CONC] = 0;

    cnt->start();
    return 0;
    }

int cipline_tech_object::FilRR( int where )
    {
    unsigned long tmp;
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    tmp=cnt->get_quantity();


    rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV];
    rt_par_float[STP_LV] = tmp;
    switch (where)
        {
        case TANK_K:
            if (parpar[0][P_MAX_BULK_FOR_ACID] > 1)
                {
                if (LTK->get_value() > parpar[0][P_MAX_BULK_FOR_ACID])
                    {
                    return 1;
                    }
                }
            if (LKH->is_active())
                {
                return 1;
                }
            break;
        case TANK_S:
            if (parpar[0][P_MAX_BULK_FOR_CAUSTIC] > 1)
                {
                if (LTS->get_value() > parpar[0][P_MAX_BULK_FOR_CAUSTIC])
                    {
                    return 1;
                    }
                }
            if (LSH->is_active())
                {
                return 1;
                }
            break;
        }
    if (!LM->is_active()) V00->on();
    if (LH->is_active()) V00->instant_off();
    return 0;
    }

int cipline_tech_object::CircRR( int where )
    {
    float c = 0;
    switch (where)
        {
        case TANK_S:
            c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNS] *
                ( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT ])/3600;
            rt_par_float[P_CONC_RATE] = c;

            c=GetConc(SHCH);
            break;
        case TANK_K:
            c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNK] *
                ( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT])/3600;
            rt_par_float[P_CONC_RATE] = c;

            c=GetConc(KISL);
            break;
        }

    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    rt_par_float[P_CONC] = c;
    if (T[TMR_OP_TIME]->is_time_up())
        {
        return 1;
        }
    return 0;
    }

int cipline_tech_object::CheckConc( int where )
    {
    float c = 0, z = 0;
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    switch (where)
        {
        case TANK_K:
            c=GetConc(KISL);
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            c=GetConc(SHCH);
            z=parpar[0][P_CZAD_S];
            break;
        }
    SAV[SAV_CONC]->Add(c, cnt->get_quantity());
    rt_par_float[P_CONC] = SAV[SAV_CONC]->Q();
    if (T[TMR_OP_TIME]->is_time_up())
        {
        switch (where)
            {
            case TANK_K:
                if ( 0 == rt_par_float[STP_QAVK] )
                    {
                    rt_par_float[STP_QAVK] = c;
                    }
                break;
            case TANK_S:
                if ( 0 == rt_par_float[STP_QAVS] )
                    {
                    rt_par_float[STP_QAVS] = c;
                    }
                break;
            }

        if ( c >= z * (1 - parpar[ 0 ][ P_NAV_TOLERANCE ] / 100))
            return 1;
        else
            return 2;
        }
    return 0;
    }

int cipline_tech_object::AddRR( int where )
    {
    float c = 0;
    switch (where)
        {
        case TANK_S:
            c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNS] *
                ( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT])/3600;
            rt_par_float[P_CONC_RATE] = c;

            c=GetConc(SHCH);
            if (c > parpar[0][P_CZAD_S] * (1 + parpar[0][P_NAV_OVERREGULATE]/100))
                {
                return 1;
                }
            break;
        case TANK_K:
            c = rt_par_float[P_CONC_RATE] + parpar[0][P_PDNK] *
                ( (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000) - rt_par_float[P_OP_TIME_LEFT])/3600;
            rt_par_float[P_CONC_RATE] = c;

            c=GetConc(KISL);
            if (c > parpar[0][P_CZAD_K] * (1 + parpar[0][P_NAV_OVERREGULATE]/100))
                {
                return 1;
                }
            break;
        }

    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    rt_par_float[P_CONC] = c;
    if (T[TMR_OP_TIME]->is_time_up())
        {
        return 1;
        }
    if (no_liquid_is_warning)
        {
        no_liquid_is_warning = 0;
        return ERR_POSSIBLE_NO_MEDIUM;
        }
    return 0;
    }

int cipline_tech_object::OpolRR( int where )
    {
    float c = 0;
    float noConc = NOCONC;
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    switch (where)
        {
        case TANK_K:
            c=GetConc(KISL);
            noConc = get_station_par(P_CKANAL_K);
            break;
        case TANK_S:
            c=GetConc(SHCH);
            noConc = get_station_par(P_CKANAL_S);
            break;
        }
    rt_par_float[P_CONC] = c;
    SortRR(where, 1);

    if (c >= noConc)
        {
        T[TMR_CHK_CONC]->reset();
        }
    else
        {
        T[TMR_CHK_CONC]->start();
        if (T[TMR_CHK_CONC]->is_time_up()) return 1;
        }
    if (!LM->is_active()) V00->on();
    if (LH->is_active()) V00->instant_off();
    return 0;
    }

int cipline_tech_object::_InitToObject( int from, int where, int step_to_init, int f )
    {
    float v=1, p=1, z=0;
    //	int ot;
    V05->on();
    V06->off();

    if (step_to_init == 5 || step_to_init == 22 || step_to_init == 42 || step_to_init == 62 || step_to_init == 72 || (step_to_init == 83 && rt_par_float[P_PROGRAM] == SPROG_HOTWATER))
        {
        enable_ret_pump = 1;
        }
    else
        {
        enable_ret_pump = 0;
        }

    switch (from)
        {
        case WATER:
        case SANITIZER:
            V00->on();
            V01->on();
            V02->off();
            V03->off();
            V04->off();
            break;
        case TANK_W:
            V00->instant_off();
            V01->off();
            V02->off();
            V03->off();
            V04->on();
            break;
        case TANK_K:
            V00->instant_off();
            V01->off();
            V02->on();
            V03->off();
            V04->off();
            break;
        case TANK_S:
            V00->instant_off();
            V01->off();
            V02->off();
            V03->on();
            V04->off();
            break;
        }
    switch (where)
        {
        case KANAL:
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->on();
            V12->off();
            break;
        case TANK_W:
            if (LWH->is_active())
                {
                V08->off();
                V09->off();
                V10->off();
                V11->on();
                V12->off();
                V07->off();
                }
            else
                {
                V08->off();
                V09->off();
                V10->off();
                V11->off();
                V12->off();
                V07->on();
                }
            break;
        case TANK_K:
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            z=parpar[0][P_CZAD_S];
            break;
        }

    NP->on();
    nplaststate = true;

    switch (step_to_init)
        {
        case 5:
        case 8:
            v=rt_par_float[P_T_WP];
            break;
        case 14:
            v=rt_par_float[P_T_WP];
            break;
        case 22:
            v=rt_par_float[P_T_S];
            break;
        case 33:
        case 37:
            v=rt_par_float[P_T_WSP];
            break;
        case 53:
        case 57:
            v=rt_par_float[P_T_WKP];
            break;
        case 42:
            v=rt_par_float[P_T_K];
            break;
        case 62:
            v=rt_par_float[P_T_D];
            break;
        case 72:
            v=rt_par_float[P_T_SANITIZER];
            break;
        case 83:
        case 86:
            if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
                {
                v=rt_par_float[P_T_CLEAN_RINSING];
                }
            else
                {
                if (program_has_cold_desinfection)
                    {
                    v = rt_par_float[P_T_SANITIZER_RINSING];
                    }
                else
                    {
                    v=rt_par_float[P_T_WOP];
                    }
                }
            break;
        }
    switch (step_to_init)
        {
        case 5:
        case 22:
        case 33:
        case 42:
        case 53:
        case 62:
        case 72:
        case 83:
            p = rt_par_float[PV1];
            break;
        case 8:
            p = rt_par_float[P_DOP_V_PR_OP];
            break;
        case 37:
            p = rt_par_float[P_DOP_V_AFTER_S];
            break;
        case 57:
            p = rt_par_float[P_DOP_V_AFTER_K];
            break;
        case 86:
            if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
                {
                p = rt_par_float[P_V_CLEAN_RINSING];
                }
            else
                {
                if (program_has_cold_desinfection)
                    {
                    p = rt_par_float[P_V_SANITIZER_RINSING];
                    }
                else
                    {
                    p = rt_par_float[P_DOP_V_OK_OP];
                    }
                }
            break;
        }
    rt_par_float[P_ZAD_PODOGR] = v;
    if (/*disable_tank_heating && */isTank() && (22 == step_to_init || 42 == step_to_init || 62 == step_to_init))
        {
        rt_par_float[P_ZAD_PODOGR] = 0;
        PIDP->off();
        }
    else
        {
        PIDP->on();
        }
    rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
    PIDF->on();
    rt_par_float[P_VRAB] = p;
    rt_par_float[P_OP_TIME_LEFT] = 0;
    float divider = rt_par_float[P_ZAD_FLOW];
    if (0 == divider)
        {
        divider = 1;
        }
    rt_par_float[P_MAX_OPER_TM] = 3.6f * MAX_OP_TIME * p / divider;
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();

    rt_par_float[P_ZAD_CONC] = z;
    rt_par_float[P_CONC] = 0;

    cnt->start();
    return 0;
    }

int cipline_tech_object::_InitFromObject( int what, int where, int step_to_init, int f )
    {
    float v = 0, p = 0, z=0;
    //	int ot;
    if (0 == f)
        {
        return_ok = 0;
        concentration_ok = 0;
        }
    V05->on();
    V06->off();
    switch (what)
        {
        case WATER:
        case SANITIZER:
            V00->on();
            V01->on();
            V02->off();
            V03->off();
            V04->off();
            break;
        case TANK_W:
            V00->instant_off();
            V01->off();
            V02->off();
            V03->off();
            V04->on();
            break;
        case TANK_K:
            V00->instant_off();
            V01->off();
            V02->on();
            V03->off();
            V04->off();
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            V00->instant_off();
            V01->off();
            V02->off();
            V03->on();
            V04->off();
            z=parpar[0][P_CZAD_S];
            break;
        }
    switch (where)
        {
        case KANAL:
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->on();
            V12->off();
            break;
        case TANK_W:
            V07->on();
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            break;
        case TANK_K:
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            z=parpar[0][P_CZAD_S];
            break;
        }

    NP->on();
    nplaststate = true;
    SetRet(OFF);

    switch (step_to_init)
        {
        case 7:
            v=rt_par_float[P_T_WP];
            break;
        case 24:
            v=rt_par_float[P_T_S];
            break;
        case 35:
            v=rt_par_float[P_T_WSP];
            break;
        case 55:
            v=rt_par_float[P_T_WKP];
            break;
        case 44:
            v=rt_par_float[P_T_K];
            break;
        case 64:
            v=rt_par_float[P_T_D];
            break;
        case 74:
            v=rt_par_float[P_T_SANITIZER];
            break;
        case 85:
            if (rt_par_float[P_PROGRAM] == SPROG_RINSING_CLEAN)
                {
                v=rt_par_float[P_T_CLEAN_RINSING];
                }
            else
                {
                if (program_has_cold_desinfection)
                    {
                    v = rt_par_float[P_T_SANITIZER_RINSING];
                    }
                else
                    {
                    v=rt_par_float[P_T_WOP];
                    }
                }
            break;
        }
    p=rt_par_float[PV2];
    rt_par_float[P_ZAD_PODOGR] = v;
    if (disable_tank_heating && isTank() && (24 == step_to_init || 44 == step_to_init || 64 == step_to_init))
        {
        rt_par_float[P_ZAD_PODOGR] = 0;
        PIDP->off();
        }
    else
        {
        PIDP->on();
        }
    rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
    PIDF->on();
    rt_par_float[P_VRAB] = p;
    rt_par_float[P_OP_TIME_LEFT] = 0;
    float divider = rt_par_float[P_ZAD_FLOW];
    if (0 == divider)
        {
        divider = 1;
        }
    rt_par_float[P_MAX_OPER_TM] = 3.6f * MAX_OP_TIME * p / divider;
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    T[TMR_RETURN]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[P_TM_NO_FLOW_R] * 1000 ) );
    T[TMR_RETURN]->reset();
    T[TMR_CHK_CONC]->reset();
    if (what == TANK_K || what == TANK_S || where == TANK_S || where == TANK_K)
        {
        T[TMR_CHK_CONC]->set_countdown_time(
            static_cast<uint32_t>( rt_par_float[ P_TM_NO_CONC ] *
                1000 * 2 ) ); // Для ошибки "высокая концентрация в возвратной трубе".
        }
    else
        {
        T[TMR_CHK_CONC]->set_countdown_time(
            static_cast<uint32_t>( rt_par_float[ P_TM_NO_CONC ] *
                1000 ) ); // Для ошибки "нет концентрации в возвратной трубе".
        }
    rt_par_float[P_SUM_OP] = 0;

    rt_par_float[P_ZAD_CONC] = z;
    rt_par_float[P_CONC] = 0;

    cnt->start();
    return 0;
    }

int cipline_tech_object::_InitOporCIP( int where, int step_to_init, int not_first_call )
{
    float z=0;
    opcip=0;
    if (isLine()) return 0;
    //Смена среды
    if (dev_upr_medium_change)
        {
        dev_upr_medium_change->on();
        }
    V05->on();
    V06->off();
    V00->instant_off();
    V01->off();
    V03->off();
    V02->off();
    V04->off();
    switch (where)
        {
        case KANAL:
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->on();
            V12->off();
            break;
        case TANK_W:
            V07->on();
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            break;
        case TANK_K:
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            z=parpar[0][P_CZAD_K];
            break;
        case TANK_S:
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            z=parpar[0][P_CZAD_S];
            break;
        }

    NP->off();
    nplaststate = false;
    rt_par_float[P_ZAD_PODOGR] = 0;
    PIDP->off();
    V13->off();
    rt_par_float[P_ZAD_FLOW] = 0;
    PIDF->off();
    rt_par_float[P_VRAB] = 0;
    rt_par_float[P_OP_TIME_LEFT] = 0;
    rt_par_float[P_MAX_OPER_TM] = rt_par_float[P_TM_MAX_TIME_OPORCIP];
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    T[TMR_RETURN]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_TM_RET_IS_EMPTY ] * 1000 ) );
    T[TMR_RETURN]->reset();
    rt_par_float[P_SUM_OP] = 0;

    rt_par_float[P_ZAD_CONC] = z;
    rt_par_float[P_CONC] = 0;
    cnt->start();
    return 0;
    }

int cipline_tech_object::_InitFilCirc( int with_what, int step_to_init, int f )
    {
    V01->on();
    V11->off();
    V12->off();
    switch (with_what)
        {
        case WITH_WATER:
        case SANITIZER:
            V00->on();
            V10->off();
            V11->on();
            NP->off();
            nplaststate = false;
            SetRet(OFF);
            PIDP->off();
            PIDF->off();
            break;
        case WITH_RETURN:
            V00->instant_off();
            V10->on();
            V07->off();
            V08->off();
            V09->off();
            V05->on();
            V06->off();
            NP->on();
            nplaststate = true;
            SetRet(ON);
            if (isTank() && (26 == step_to_init || 46 == step_to_init || 65 == step_to_init))
                {
                PIDP->off();
                }
            else
                {
                PIDP->on();
                }
            PIDF->on();
            break;
        }
    switch (step_to_init)
        {
        case 26:
            V03->on();
            break;
        case 46:
            V02->on();
            break;
        }

    //   cnt->start();
    rt_par_float[P_SUM_OP] = 0;
    rt_par_float[P_VRAB] = rt_par_float[P_V_RAB_ML];
    rt_par_float[P_OP_TIME_LEFT] = 0;
    rt_par_float[P_MAX_OPER_TM] = 300;
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    return 0;
    }

int cipline_tech_object::_InitOporCirc( int where, int step_to_init, int not_first_call )
{
    V11->off();
    V00->instant_off();
    V01->on();
    V03->off();
    V02->off();
    V04->off();
    NP->on();
    nplaststate = true;
    SetRet(ON);
    V05->on();
    V06->off();
    switch (where)
        {
        case KANAL:
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->on();
            V12->off();
            break;
        case TANK_W:
            V07->on();
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            break;
        case TANK_S:
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            break;
        case TANK_K:
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            break;
        }
    switch (step_to_init)
        {
        case 29:
            rt_par_float[P_ZAD_PODOGR] = rt_par_float[P_T_WSP];
            break;
        case 49:
            rt_par_float[P_ZAD_PODOGR] = rt_par_float[P_T_WKP];
            break;
        case 67:
            rt_par_float[P_ZAD_PODOGR] = rt_par_float[P_T_WOP];
            break;
        case 78:
            rt_par_float[P_ZAD_PODOGR] = rt_par_float[P_T_SANITIZER_RINSING];
            break;
        default:
            rt_par_float[P_ZAD_PODOGR] = 0;
            break;
        }
    PIDF->on();
    PIDP->on();
    //   cnt->start();
    rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
    rt_par_float[P_SUM_OP] = 0;
    rt_par_float[P_VRAB] = rt_par_float[P_V_LL_BOT];
    rt_par_float[P_OP_TIME_LEFT] = 0;
    rt_par_float[P_MAX_OPER_TM] = rt_par_float[P_TM_MAX_TIME_OPORBACHOK];
    T[ TMR_OP_TIME ]->set_countdown_time(
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    T[TMR_OP_TIME]->pause();
    return 0;
    }

int cipline_tech_object::_InitCirc( int what, int step_to_init, int not_first_call )
    {
    if (0 == not_first_call)
        {
        circ_podp_count = 0;
        circ_was_feed = 0;
        circ_temp_reached = 0;
        }
    if (circ_was_feed)
        {
        circ_podp_count++;
        circ_was_feed = 0;
        }

    float t=600, z=0;
    uint32_t tm = 0;
    ret_circ_flag = 0;
    if ((circ_tank_s && 28 == step_to_init) || (circ_tank_k && 48 == step_to_init))
        {
        V01->off();
        V10->off();
        }
    else
        {
        V01->on();
        V10->on();
        }
    V00->instant_off();
    V03->off();
    V04->off();
    V02->off();
    V05->on();
    V06->off();
    V07->off();
    V08->off();
    V09->off();
    V11->off();
    V12->off();
    switch (what)
        {
        case WATER:
            t=rt_par_float[P_T_WP];
            tm = static_cast<uint32_t>( rt_par_float[ PTM_OP ] * 1000 );
            break;
        case SANITIZER:
            t=rt_par_float[P_T_SANITIZER];
            tm = static_cast<uint32_t>( rt_par_float[ PTM_SANITIZER ] * 1000 );
            break;
        case HOT_WATER:
            t=rt_par_float[P_T_D];
            tm = static_cast<uint32_t>( rt_par_float[ PTM_D ] * 1000 );
            break;
        case SHCH:
            if (circ_tank_s)
                {
                V03->on();
                V09->on();
                }
            t=rt_par_float[P_T_S];
            if (((int)rt_par_float[P_PROGRAM] & ((1 << PRG_K) + (1 << PRG_S))) == ((1 << PRG_K) + (1 << PRG_S)))
                {
                tm = static_cast<uint32_t>( rt_par_float[ PTM_S_SK ] * 1000 );
                }
            else
                {
                tm = static_cast<uint32_t>( rt_par_float[ PTM_S ] * 1000 );
                }
            z=parpar[0][P_CZAD_S];
            break;
        case KISL:
            if (circ_tank_k)
                {
                V02->on();
                V08->on();
                }
            z=parpar[0][P_CZAD_K];
            t=rt_par_float[P_T_K];
            if (((int)rt_par_float[P_PROGRAM] & ((1 << PRG_K) + (1 << PRG_S))) == ((1 << PRG_K) + (1 << PRG_S)))
                {
                tm = static_cast<uint32_t>( rt_par_float[ PTM_K_SK ] * 1000 );
                }
            else
                {
                tm = static_cast<uint32_t>( rt_par_float[ PTM_K ] * 1000 );
                }
            break;
        }

    rt_par_float[P_ZAD_PODOGR] = t;
    PIDP->on();
    NP->on();
    nplaststate = true;
    SetRet(ON);
    rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
    PIDF->on();
    rt_par_float[P_VRAB] = 0;

    rt_par_float[P_ZAD_CONC] = z;
    rt_par_float[P_CONC] = 0;

    rt_par_float[P_OP_TIME_LEFT] = 0;
    rt_par_float[P_MAX_OPER_TM] = tm/1000;

    T[TMR_OP_TIME]->set_countdown_time(tm);
    rt_par_float[P_SUM_OP] = 0;

    cnt->start();
    return 0;
    }

int cipline_tech_object::_ToObject( int from, int where )
    {
    float c=0;
    unsigned long tmp;
    rt_par_float[ P_OP_TIME_LEFT ] = T[ TMR_OP_TIME ]->get_work_time() / 1000;
    rt_par_float[P_SUM_OP] = cnt->get_quantity();


    if (enable_ret_pump)
        {
        if ((cnt->get_quantity() >= rt_par_float[P_VRAB]-rt_par_float[P_RET_STOP]) || (cnt->get_quantity() <= rt_par_float[P_RET_STOP]))
            {
            SetRet(OFF);
            }
        else
            {
            SetRet(ON);
            }
        }
    else
        {
        SetRet(ON);
        }


    switch (from)
        {
        case WATER:
        case SANITIZER:
            if (!LM->is_active()) V00->on();
            if (LH->is_active()) V00->instant_off();
            tmp=cnt->get_quantity();
            rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV];
            if (curstep == 62)
            {
            rt_par_float[STP_USED_HOTWATER] = rt_par_float[STP_USED_HOTWATER] + tmp - rt_par_float[STP_LV];
            }
            rt_par_float[STP_LV] = tmp;
            break;
        case TANK_W:
            if ( waterTankIsEmpty( ))
                {
                tankempty = 1;
                tankemptytimer = get_millisec( );
                } else
                {
                if ( tankempty && get_delta_millisec( tankemptytimer ) > toTankSwitchTimeMs )
                    {
                    tankempty = 0;
                    }
                }

            if (tankempty)
                {
                tmp=cnt->get_quantity();
                rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV];
                rt_par_float[STP_WC_INST_WS] = rt_par_float[STP_WC_INST_WS] + tmp - rt_par_float[STP_LV];
                rt_par_float[STP_LV] = tmp;
                V04->off();
                V01->on();
                if (!LM->is_active()) V00->on();
                if (LH->is_active()) V00->instant_off();
                }
            else
                {
                tmp=cnt->get_quantity();
                rt_par_float[STP_WS] = rt_par_float[STP_WS] + tmp - rt_par_float[STP_LV];
                rt_par_float[STP_LV] = tmp;
                V04->on();
                V00->instant_off();
                V01->off();
                }
            break;
        case TANK_K:
            c=GetConc(KISL);
            tmp=cnt->get_quantity();
            rt_par_float[STP_USED_ACID] = rt_par_float[STP_USED_ACID] + tmp - rt_par_float[STP_LV];
            rt_par_float[STP_LV] = tmp;
            if (!LKL->is_active())
                {
                return ERR_NO_ACID;
                }
            break;
        case TANK_S:
            c=GetConc(SHCH);
            tmp=cnt->get_quantity();
            rt_par_float[STP_USED_CAUSTIC] = rt_par_float[STP_USED_CAUSTIC] + tmp - rt_par_float[STP_LV];
            rt_par_float[STP_LV] = tmp;
            if (!LSL->is_active())
                {
                return ERR_NO_ALKALINE;
                }
            break;
        }

    switch (where)
        {
        case TANK_W:
            if (LWH->is_active())
                {
                tankfull = 1;
                tankfulltimer = get_millisec();
                }

            if (tankfull)
                {
                if ( get_delta_millisec(tankfulltimer) > toTankSwitchTimeMs && (!LWH->is_active()))
                    {
                    tankfull = 0;
                    }
                }


            if (tankfull || (from == TANK_W))
                {
                V08->off();
                V09->off();
                V10->off();
                V11->on();
                V12->off();
                V07->off();
                }
            else
                {
                V08->off();
                V09->off();
                V10->off();
                V11->off();
                V12->off();
                V07->on();
                }
            break;
        case TANK_K:
            c=GetConc(KISL);
            SortRR(TANK_K);
            break;
        case TANK_S:
            c=GetConc(SHCH);
            SortRR(TANK_S);
            break;
        }

    rt_par_float[P_CONC] = c;

    if (steps_additional_rinse.count(curstep))
        {
        if (dev_os_can_continue)
            {
            if (dev_os_can_continue->get_state() == OFF)
                {
                return 0;
                }
            }
        }

    if (cnt->get_quantity() >= rt_par_float[P_VRAB])
        {
        return 1;
        }

    if (curstep==86 && TR->get_value()<5 && rt_par_float[P_PROGRAM] != SPROG_RINSING_CLEAN)
        {
        return 1;
        }



    return 0;
    }

int cipline_tech_object::_FromObject( int what, int where )
    {
    float c = 0;
    float noConc = NOCONC;
    int dst;
    unsigned long tmp;
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    if (cnt->get_quantity()>=rt_par_float[PV_RET_DEL])
        {
        SetRet(ON);
        }
    if (cnt->get_quantity()>=rt_par_float[P_VRAB])
        {
        if (!return_ok)
            {
            if (FL->get_state()==FLIS) {
                return_ok = 1;
                }
            else
                {
                T[TMR_RETURN]->start();
                if (T[TMR_RETURN]->is_time_up()==1)
                    {
                    return ERR_NO_RETURN;
                    }
                }
            }
        dst=0;
        switch (what)
            {
            case TANK_K:
                dst=1;
                c=GetConc(KISL);
                noConc = get_station_par(P_CKANAL_K);
                break;
            case TANK_S:
                dst=1;
                c=GetConc(SHCH);
                noConc = get_station_par(P_CKANAL_S);
                break;
            }
        switch (where)
            {
            case TANK_K:
                dst=2;
                c=GetConc(KISL);
                noConc = get_station_par(P_CKANAL_K);
                break;
            case TANK_S:
                dst=2;
                c=GetConc(SHCH);
                noConc = get_station_par(P_CKANAL_S);
                break;
            }
        switch (dst)
            {
            case 0:
                concentration_ok = 1;
                break;
            case 1:  //must BE
                if (c >= noConc)
                    {
                    concentration_ok = 1;
                    }
                else
                    {
                    T[TMR_CHK_CONC]->start();
                    if (T[TMR_CHK_CONC]->is_time_up())
                        {
                        T[TMR_CHK_CONC]->reset();
                        return ERR_NO_CONC;
                        }
                    concentration_ok = 0;
                    }
                break;
            case 2: //must not BE
                if ( c <= noConc)
                    {
                    concentration_ok = 1;
                    }
                else
                    {
                    T[TMR_CHK_CONC]->start();
                    if (T[TMR_CHK_CONC]->is_time_up())
                        {
                        T[TMR_CHK_CONC]->reset();
                        return ERR_IS_CONC;
                        }
                    concentration_ok = 0;
                    }
                break;
            }

        if (return_ok == 1 && concentration_ok == 1) return 1;
        }

    c=0;
    switch (what)
        {
        case WATER:
        case SANITIZER:
            if (!LM->is_active()) V00->on();
            if (LH->is_active()) V00->instant_off();
            tmp=cnt->get_quantity();
            rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV] ;
            if (curstep == 64)
                {
                rt_par_float[STP_USED_HOTWATER] = rt_par_float[STP_USED_HOTWATER] + tmp - rt_par_float[STP_LV];
                }
            rt_par_float[STP_LV] = tmp ;
            break;
        case TANK_W:
            if ( waterTankIsEmpty( ))
                {
                tankempty = 1;
                tankemptytimer = get_millisec( );
                } else
                {
                if ( tankempty && get_delta_millisec( tankemptytimer ) > toTankSwitchTimeMs )
                    {
                    tankempty = 0;
                    }
                }

            if (tankempty)
                {
                tmp=cnt->get_quantity();
                rt_par_float[STP_WC] = rt_par_float[STP_WC] + tmp - rt_par_float[STP_LV] ;
                rt_par_float[STP_WC_INST_WS] = rt_par_float[STP_WC_INST_WS] + tmp - rt_par_float[STP_LV] ;
                rt_par_float[STP_LV] = tmp ;
                V04->off();
                V01->on();
                if (!LM->is_active()) V00->on();
                if (LH->is_active()) V00->instant_off();
                }
            else
                {
                tmp=cnt->get_quantity();
                rt_par_float[STP_WS] = rt_par_float[STP_WS] + tmp - rt_par_float[STP_LV] ;
                rt_par_float[STP_LV] = tmp ;
                V04->on();
                V00->instant_off();
                V01->off();
                }
            break;
        case TANK_K:
            c=GetConc(KISL);
            tmp=cnt->get_quantity();
            rt_par_float[STP_USED_ACID] = rt_par_float[STP_USED_ACID] + tmp - rt_par_float[STP_LV];
            rt_par_float[STP_LV] = tmp;
            if (!LKL->is_active())
                {
                return ERR_NO_ACID;
                }
            SortRR(TANK_K);
            break;
        case TANK_S:
            c=GetConc(SHCH);
            tmp=cnt->get_quantity();
            rt_par_float[STP_USED_CAUSTIC] = rt_par_float[STP_USED_CAUSTIC] + tmp - rt_par_float[STP_LV];
            rt_par_float[STP_LV] = tmp;
            if (!LSL->is_active())
                {
                return ERR_NO_ALKALINE;
                }
            SortRR(TANK_S);
            break;
        }

    switch (where)
        {
        case TANK_W:
            if (LWH->is_active())
                {
                tankfull = 1;
                tankfulltimer = get_millisec();
                }

            if (tankfull)
                {
                if ( get_delta_millisec(tankfulltimer) > toTankSwitchTimeMs && (!LWH->is_active()))
                    {
                    tankfull = 0;
                    }
                }


            if (tankfull)
                {
                V08->off();
                V09->off();
                V10->off();
                V11->on();
                V12->off();
                V07->off();
                }
            else
                {
                V08->off();
                V09->off();
                V10->off();
                V11->off();
                V12->off();
                V07->on();
                }
            break;
        case TANK_K:
            c=GetConc(KISL);
            SortRR(TANK_K);
            break;
        case TANK_S:
            c=GetConc(SHCH);
            SortRR(TANK_S);
            break;
        }
    rt_par_float[P_CONC] = c;
    return 0;
    }

int cipline_tech_object::_OporCIP( int where )
    {
    //				int ot;
    float c;
    if (isLine()) return 1;
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    SetRet(ON);

    c=0;
    switch (where)
        {
        case TANK_W:
            if (LWH->is_active())
                {
                V08->off();
                V09->off();
                V10->off();
                V11->on();
                V12->off();
                V07->off();
                }
            else
                {
                V08->off();
                V09->off();
                V10->off();
                V11->off();
                V12->off();
                V07->on();
                }
            break;
        case TANK_K:
            c=GetConc(KISL);
            SortRR(TANK_K);
            break;
        case TANK_S:
            c=GetConc(SHCH);
            SortRR(TANK_S);
            break;
        }
    rt_par_float[P_CONC] = c;

    if (dev_os_object_empty)
        {
        if (ON == dev_os_object_empty->get_state())
            {
            return 1;
            }
        else
            {
            return 0;
            }
        }

    if ( dev_ls_ret_pump &&
        !dev_ls_ret_pump->is_active() &&
        get_delta_millisec( ret_pums_ls_timer ) >
        static_cast<uint32_t>( rt_par_float[ P_TM_RET_IS_EMPTY ] * 1000 ) )
        {
        return 1;
        }

    if (FL->get_state() == FLIS)
        {
        opcip=1;
        T[TMR_RETURN]->reset();
        }
    else
        {
        if (opcip==1)
            {
            T[TMR_RETURN]->start();
            if (T[TMR_RETURN]->is_time_up())
                {
                return 1;
                }
            }
        }

    if (rt_par_float[P_MAX_OPER_TM]*500 < T[TMR_OP_TIME]->get_work_time())
        {
        if ((curstep==6) || (opcip == 0)) return 1;
        }

    if (T[TMR_OP_TIME]->is_time_up())
        {
        return 1;
        }

    return 0;
    }

int cipline_tech_object::_FillCirc( int with_what )
    {
    if (with_what == WITH_RETURN)
        {
        SetRet(ON);
        }
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    if (LM->is_active()) cnt->start();
    if (cnt->get_quantity() >= rt_par_float[P_V_RAB_ML]) return 1;
    if (LH->is_active()) return 1;
    return 0;
    }

int cipline_tech_object::_Circ( int what )
    {
    float c;
    V06->off();
#ifdef RETALWAYSON
    SetRet(ON);
#else

    if ((!LH->is_active() && 0 == ret_circ_flag) || circ_tank_s || circ_tank_k)
        {
        SetRet(ON);
        }

    char dont_stop_pump = 0;

    if ((curstep == 66 || curstep == 77) && circ_water_no_pump_stop)
        {
        dont_stop_pump = 1;
        }

    if ((curstep == 28 || curstep == 48) && circ_medium_no_pump_stop)
        {
        dont_stop_pump = 1;
        }

    if (LH->is_active() && V10->get_state() && !dont_stop_pump)
        {
        SetRet(OFF);
        ret_circ_flag = 1;
        }

    if (0 == ret_circ_flag || LH->is_active())
        {
        ret_circ_delay = get_millisec();
        }

    if (!LM->is_active() || get_delta_millisec(ret_circ_delay) > 10000L)
        {
        ret_circ_flag = 0;
        }
#endif

    if (TR->get_value() >= rt_par_float[P_ZAD_PODOGR]-rt_par_float[P_DELTA_TR] && cnt->get_flow() > rt_par_float[P_R_NO_FLOW])
        {
        T[TMR_OP_TIME]->start();
        circ_temp_reached = 1;
        }
    else
        {
        T[TMR_OP_TIME]->pause();
        }

    c=GetConc(what);
    SAV[SAV_CONC]->Add(c, cnt->get_quantity());
    rt_par_float[P_CONC] = c;
    switch (what)
        {
        case KISL:
            rt_par_float[ STP_QAVK] = SAV[SAV_CONC]->Q();
            if (circ_podp_k)
                {
                if (!LM->is_active() && !LL->is_active() && !LH->is_active() && circ_podp_count < circ_podp_max_count)
                    {
                    V02->on();
                    if (0 == circ_was_feed)
                        {
                        circ_max_timer = get_millisec();
                        }
                    circ_was_feed = 1;
                    }
                if (LM->is_active() || LH->is_active() || (circ_was_feed && get_delta_millisec(circ_max_timer) > circMaxFeedTimeMs))
                    {
                    V02->off();
                    if (1 == circ_was_feed)
                        {
                        circ_podp_count++;
                        rt_par_float[STP_PODP_ACID] = rt_par_float[STP_PODP_ACID] + 1;
                        }
                    circ_was_feed = 0;
                    }
                }
            if (circ_medium_no_pump_stop)
                {
                if (LH->is_active())
                    {
                    if (get_delta_millisec(circ_return_timer) > 1000)
                        {
                        V10->off();
                        V11->off();
                        V12->off();
                        V07->off();
                        V08->on();
                        V09->off();
                        circ_return_timer = get_millisec();
                        }
                    }
                else
                    {
                    if (!LM->is_active())
                        {
                        V10->on();
                        V11->off();
                        V12->off();
                        V07->off();
                        V08->off();
                        V09->off();
                        }
                    circ_return_timer = get_millisec();
                    }
                }
            break;
        case SHCH:
            rt_par_float[ STP_QAVS] = SAV[SAV_CONC]->Q();
            if (circ_podp_s)
                {
                if (!LM->is_active() && !LL->is_active() && !LH->is_active() && circ_podp_count < circ_podp_max_count)
                    {
                    V03->on();
                    if (0 == circ_was_feed)
                        {
                        circ_max_timer = get_millisec();
                        }
                    circ_was_feed = 1;
                    }
                if (LM->is_active() || LH->is_active() || (circ_was_feed && get_delta_millisec(circ_max_timer) > circMaxFeedTimeMs))
                    {
                    V03->off();
                    if (1 == circ_was_feed)
                        {
                        circ_podp_count++;
                        rt_par_float[STP_PODP_CAUSTIC] = rt_par_float[STP_PODP_CAUSTIC] + 1;
                        }
                    circ_was_feed = 0;
                    }
                }
            if (circ_medium_no_pump_stop)
                {
                if (LH->is_active())
                    {
                    if (get_delta_millisec(circ_return_timer) > 1000)
                        {
                        V10->off();
                        V11->off();
                        V12->off();
                        V07->off();
                        V08->off();
                        V09->on();
                        circ_return_timer = get_millisec();
                        }
                    }
                else
                    {
                    if (!LM->is_active())
                        {
                        V10->on();
                        V11->off();
                        V12->off();
                        V07->off();
                        V08->off();
                        V09->off();
                        }
                    circ_return_timer = get_millisec();
                    }
                }
            break;
        case WATER:
        case HOT_WATER:
        case SANITIZER:
            if (circ_podp_water)
                {
                if (!LM->is_active() && !LL->is_active() && !LH->is_active() && circ_podp_count < circ_podp_max_count)
                    {
                    V00->on();
                    if (0 == circ_was_feed)
                        {
                        circ_max_timer = get_millisec();
                        }
                    circ_was_feed = 1;
                    }
                if (LM->is_active() || LH->is_active() || (circ_was_feed && get_delta_millisec(circ_max_timer) > circMaxFeedTimeMs))
                    {
                    V00->instant_off();
                    if (1 == circ_was_feed)
                        {
                        circ_podp_count++;
                        rt_par_float[STP_PODP_WATER] = rt_par_float[STP_PODP_WATER] + 1;
                        }
                    circ_was_feed = 0;
                    }
                }
            break;
        }
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();

    if (T[TMR_OP_TIME]->is_time_up()==1)
        {
        if (!is_ready_to_end)
            {
            if (what == SHCH)
                {
                objectstats->objcausticwashes++;
                objectstats->changed = 1;
                }
            if (what == KISL)
                {
                objectstats->objcausticwashes = 0;
                DateToChar(objectstats->objlastacidwash);
                objectstats->changed = 1;
                }
            is_ready_to_end = true;
            }
        if (dev_os_can_continue)
            {
            if (dev_os_can_continue->get_state() == ON)
                {
                return 1;
                }
            else
                {
                return 0;
                }
            }
        else
            {
            return 1;
            }
        }
    return 0;
    }

int cipline_tech_object::_OporCirc( int where )
    {
    SetRet(ON);
    switch (where)
        {
        case KANAL:
            break;
        case TANK_W:
            if (LWH->is_active())
                {
                V07->off();
                V11->on();
                }
            else
                {
                V07->on();
                V11->off();
                }
            break;
        case TANK_S:
            break;
        case TANK_K:
            break;
        }

    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    if (!LL->is_active())
        {
        cnt->start();
        T[TMR_OP_TIME]->start();
        }
    if (cnt->get_quantity() >= rt_par_float[P_V_LL_BOT] || T[TMR_OP_TIME]->is_time_up()) return 1;
    return 0;
    }

int cipline_tech_object::_InitDoseRR( int what, int step_to_init, int not_first_call )
{
    switch (what)
        {
        case SANITIZER:
            RHI();
            StopDev();
            V01->on();
            V10->on();
            V00->instant_off();
            V03->off();
            V04->off();
            V02->off();
            V05->on();
            V06->off();
            V07->off();
            V08->off();
            V09->off();
            V11->off();
            V12->off();
            break;
        }
    if (dev_upr_sanitizer_pump)
    {
    dev_upr_sanitizer_pump->on();
    }
    rt_par_float[P_MAX_OPER_TM] = rt_par_float[PTM_SANITIZER_INJECT];
    T[ TMR_OP_TIME ]->set_countdown_time( static_cast<uint32_t>(
        rt_par_float[ P_MAX_OPER_TM ] * 1000 ) );
    T[TMR_OP_TIME]->start();
    rt_par_float[P_SUM_OP] = 0;
    NP->on();
    nplaststate = true;
    SetRet(ON);
    rt_par_float[P_ZAD_PODOGR] = rt_par_float[P_T_SANITIZER];
    rt_par_float[P_ZAD_FLOW] = rt_par_float[P_FLOW];
    PIDF->on();
    rt_par_float[P_VRAB] = 0;
    cnt->start();
    return 0;
    }

int cipline_tech_object::_DoseRR( int what )
    {
    if (!LH->is_active())
        {
        SetRet(ON);
        }
    else
        {
        SetRet(OFF);
        }

    switch (what)
        {
        case SANITIZER:
            break;
        }
    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();
    if (T[TMR_OP_TIME]->is_time_up())
        {
        return 1;
        }
    return 0;
    }

int cipline_tech_object::init_object_devices()
    {
    auto circflag = (unsigned long)(rt_par_float[P_PODP_CIRC]);
    circ_tank_s = circflag & CIRC_TANK_S ? 1:0;
    circ_tank_k = circflag & CIRC_TANK_K ? 1:0;
    circ_podp_water = circflag & CIRC_PODP_HOTWATER ? 1:0;
    circ_podp_s = circflag & CIRC_PODP_SCHC ? 1:0;
    circ_podp_k = circflag & CIRC_PODP_KISL ? 1:0;
    circ_podp_max_count = (circflag >> 12) & 0xFF;
    if (0 == circ_podp_max_count)
        {
        circ_podp_max_count = CIRC_DEFAULT_FEED_COUNT;
        }
    circ_water_no_pump_stop = circflag & CIRC_STOP_PUMP_HOTWATER ? 1:0;
    circ_medium_no_pump_stop = circflag & CIRC_STOP_PUMP_MEDIUM ? 1:0;

    if ((circ_tank_s && circ_podp_s) || (circ_tank_k && circ_podp_k))
        {
        return ERR_WRONG_OS_OR_RECIPE_ERROR;
        }
    if ( G_DEBUG )
        {
        printf("Circ options:FW=%d,FS=%d,FK=%d,CS=%d,CK=%d,SP=%d,SPM=%d,count=%d\n\r",
            circ_podp_water, circ_podp_s, circ_podp_k, circ_tank_s,
            circ_tank_k, circ_water_no_pump_stop, circ_medium_no_pump_stop, circ_podp_max_count);
        }

    if ( G_DEBUG )
        {
        printf("init_object_devices\n\r");
        }
    // Сторожевой таймер связи.
    if ( check_device( dev_watchdog, P_WATCHDOG, device::DEVICE_TYPE::DT_WATCHDOG ) ) return -1;

    //Обратная связь
    if (check_DI(dev_os_object, P_OS)) return -1;
    //Обратная связь №2(готовность объекта к мойке)
    if (check_DI(dev_os_object_ready, P_SIGNAL_OBJECT_READY)) return -1;
    //Обратная связь №3(готовность объекта к мойке)
    if (check_DI(dev_os_cip_ready, P_SIGNAL_IN_CIP_READY)) return -1;
    //Сигнал что можно перейти к следующей операции(флипы окончены)
    if (check_DI(dev_os_can_continue, P_SIGNAL_CAN_CONTINUE)) return -1;
    //Обратная связь - пауза(остановка мойки)
    if (check_DI(dev_os_object_pause, P_SIGNAL_OBJECT_PAUSE)) return -1;
    //Возвратный насос
    if (check_M(dev_m_ret, P_N_RET)) return -1;
    //Сигнал управления возвратным насосом
    if (check_DO(dev_upr_ret, P_N_UPR)) return -1;
    //Сигнал управления насосом подачи дезинфицирующего средства
    if (check_DO(dev_upr_sanitizer_pump, P_SIGNAL_SANITIZER_PUMP)) return -1;
    //Смена среды
    if (check_DO(dev_upr_medium_change, P_SIGNAL_MEDIUM_CHANGE)) return -1;
    //Объект опорожнен
    if (check_DI(dev_os_object_empty, P_OBJ_EMPTY)) return -1;
    //Щелочь
    if (check_DO(dev_upr_caustic, P_SIGNAL_CAUSTIC)) return -1;
    //Кислота
    if (check_DO(dev_upr_acid, P_SIGNAL_ACID)) return -1;
    //Вода в трубе
    if (check_DO(dev_upr_water, P_SIGNAL_WATER)) return -1;
    //Дезинфекция
    if (check_DO(dev_upr_desinfection, P_SIGNAL_DESINSECTION)) return -1;
    //Циркуляция
    if (check_DO(dev_upr_circulation, P_SIGNAL_CIRCULATION)) return -1;
    //Мойка окончена
    if (check_DO(dev_upr_cip_finished, P_SIGNAL_CIPEND)) return -1;
    //Мойка окончена 2
    if (check_DO(dev_upr_cip_finished2, P_SIGNAL_CIPEND2)) return -1;
    //Мойка идет
    if (check_DO(dev_upr_cip_in_progress, P_SIGNAL_CIP_IN_PROGRESS)) return -1;
    //Сигнал управления производительностью подающего насоса
    if (check_AI(dev_ai_pump_frequency, P_SIGNAL_PUMP_CONTROL)) return -1;
    //Сигнал уровня для управления производительностью подающего насоса
    if (check_AI(dev_ai_pump_feedback, P_SIGNAL_PUMP_CONTROL_FEEDBACK)) return -1;
    //Сигнал, запрещающий включение подающего насоса.
    if (check_DI(dev_os_pump_can_run, P_SIGNAL_PUMP_CAN_RUN)) return -1;
    //Сигнал уровня перед возвратным насосом.
    if (check_LS(dev_ls_ret_pump, P_SIGNAL_RET_PUMP_SENSOR)) return -1;
    //Сигнал предварительной промывки
    if (check_DO(dev_upr_prerinse, P_SIGNAL_PRERINSE)) return -1;
    //Сигнал промежуточной промывки
    if (check_DO(dev_upr_intermediate_rinse, P_SIGNAL_INTERMEDIATE_RINSE)) return -1;
    //Сигнал окончательной промывки
    if (check_DO(dev_upr_postrinse, P_SIGNAL_POSTRINSE)) return -1;
    //Сигнал аварийного завершения мойки
    if (check_DO(dev_upr_wash_aborted, P_SIGNAL_WASH_ABORTED)) return -1;
    //Сигнал остановки подающего насоса
    if (check_DO(dev_upr_pump_stopped, P_SIGNAL_PUMP_STOPPED)) return -1;
    //Задание подающему насосу
    if (check_AO(dev_ao_flow_task, P_SIGNAL_FLOW_TASK)) return -1;
    //Задание подогрева
    if (check_AO(dev_ao_temp_task, P_SIGNAL_TEMP_TASK)) return -1;

    return 0;
    }

int cipline_tech_object::check_device( device*& outdev, int parno, device::DEVICE_TYPE type )
    {
    outdev = nullptr;

    if ( type < 0 || type >= device::DEVICE_TYPE::C_DEVICE_TYPE_CNT )
        {
        return -1;
        }
    if ( rt_par_float[ parno ] <= 0 )
        {
        return 0;
        }

    auto dev_no = (u_int)rt_par_float[ parno ];
    char devname[ MAX_DEV_NAME * UNICODE_MULTIPLIER ] = { 0 };
	// LCOV_EXCL_START
    auto res = fmt::format_to_n( devname,
        MAX_DEV_NAME * UNICODE_MULTIPLIER - 1,
        "LINE{}{}{}", nmr, device::DEV_NAMES[ type ], dev_no );
	// LCOV_EXCL_STOP
    *res.out = '\0';

    if ( auto dev = G_DEVICE_MANAGER()->get_device( devname );
        dev && dev->get_serial_n() > 0 && dev->get_type() == type )
        {
        outdev = dev;
        return 0;
        }

    if ( type == device::DEVICE_TYPE::DT_WATCHDOG )
        {
		// LCOV_EXCL_START
        auto res2 = fmt::format_to_n( devname,
            MAX_DEV_NAME * UNICODE_MULTIPLIER - 1,
            "{}{}", device::DEV_NAMES[ type ], dev_no );
		// LCOV_EXCL_STOP
        *res2.out = '\0';

        if ( auto dev = G_DEVICE_MANAGER()->get_device( devname );
            dev && dev->get_serial_n() > 0 && dev->get_type() == type )
            {
            outdev = dev;
            return 0;
            }
        }

    return -2;
    }

    int cipline_tech_object::check_DI(device*& outdev, int parno)
        {
        auto dev_no = (u_int)rt_par_float[parno];
        char devname[MAX_DEV_NAME * UNICODE_MULTIPLIER] = {0};
        device* dev;
        if (dev_no > 0)
            {
            sprintf(devname, "LINE%dDI%d", nmr, dev_no);
            dev = (device*)DI(devname);
            if (dev->get_serial_n() > 0)
                {
                outdev = dev;
                }
            else
                {
                dev = (device*)(DI(dev_no));
                if (dev->get_serial_n() > 0)
                    {
                    outdev = dev;
                    }
                else
                    {
                    dev = DEVICE(dev_no);
                    if (dev->get_serial_n() > 0 && dev->get_type() == device::DT_DI)
                        {
                        outdev = dev;
                        }
                    else
                        {
                        outdev = nullptr;
                        return -1;
                        }
                    }
                }
            }
        else
            {
            outdev = nullptr;
            }
        return 0;
        }

    int cipline_tech_object::check_DO(device*& outdev, int parno)
        {
        auto dev_no = (u_int)rt_par_float[parno];
        char devname[MAX_DEV_NAME * UNICODE_MULTIPLIER] = { 0 };
        device* dev;
        if (dev_no > 0)
            {
            sprintf(devname, "LINE%dDO%d", nmr, dev_no);
            dev = (device*)DO(devname);
            if (dev->get_serial_n() > 0)
                {
                outdev = dev;
                }
            else
                {
                dev = (device*)(DO(dev_no));
                if (dev->get_serial_n() > 0)
                    {
                    outdev = dev;
                    }
                else
                    {
                    dev = DEVICE(dev_no);
                    if (dev->get_serial_n() > 0 && dev->get_type() == device::DT_DO)
                        {
                        outdev = dev;
                        }
                    else
                        {
                        outdev = nullptr;
                        return -1;
                        }
                    }
                }
            }
        else
            {
            outdev = nullptr;
            }
        return 0;
        }

    int cipline_tech_object::check_AI(device*& outdev, int parno)
        {
        auto dev_no = (u_int)rt_par_float[parno];
        char devname[MAX_DEV_NAME * UNICODE_MULTIPLIER] = { 0 };
        device* dev;
        if (dev_no > 0)
            {
            sprintf(devname, "LINE%dAI%d", nmr, dev_no);
            dev = (device*)AI(devname);
            if (dev->get_serial_n() > 0)
                {
                outdev = dev;
                }
            else
                {
                dev = DEVICE(dev_no);
                if (dev->get_serial_n() > 0 && dev->get_type() == device::DT_AI)
                    {
                    outdev = dev;
                    }
                else
                    {
                    outdev = nullptr;
                    return -1;
                    }
                }
            }
        else
            {
            outdev = nullptr;
            }
        return 0;
        }

    int cipline_tech_object::check_AO(device*& outdev, int parno)
        {
        auto dev_no = (u_int)rt_par_float[parno];
        char devname[MAX_DEV_NAME * UNICODE_MULTIPLIER] = { 0 };
        device* dev;
        if (dev_no > 0)
            {
            sprintf(devname, "LINE%dAO%d", nmr, dev_no);
            dev = (device*)AO(devname);
            if (dev->get_serial_n() > 0)
                {
                outdev = dev;
                }
            else
                {
                dev = (device*)(AO(dev_no));
                if (dev->get_serial_n() > 0)
                    {
                    outdev = dev;
                    }
                else
                    {
                    dev = DEVICE(dev_no);
                    if (dev->get_serial_n() > 0 && dev->get_type() == device::DT_AO)
                        {
                        outdev = dev;
                        }
                    else
                        {
                        outdev = nullptr;
                        return -1;
                        }
                    }
                }
            }
        else
            {
            outdev = nullptr;
            }
        return 0;
        }

    int cipline_tech_object::check_LS(device*& outdev, int parno)
        {
        auto dev_no = (u_int)rt_par_float[parno];
        char devname[MAX_DEV_NAME * UNICODE_MULTIPLIER] = { 0 };
        device* dev;
        if (dev_no > 0)
            {
            sprintf(devname, "LINE%dLS%d", nmr, dev_no);
            dev = (device*)LS(devname);
            if (dev->get_serial_n() > 0)
                {
                outdev = dev;
                }
            else
                {
                sprintf(devname, "LINE%dDI%d", nmr, dev_no);
                dev = (device*)DI(devname);
                if (dev->get_serial_n() > 0)
                    {
                    outdev = dev;
                    }
                else
                    {
                    outdev = nullptr;
                    return -1;
                    }
                }
            }
        else
            {
            outdev = nullptr;
            }
        return 0;
        }

    int cipline_tech_object::check_M(device*& outdev, int parno)
        {
        auto dev_no = (u_int)rt_par_float[parno];
        char devname[MAX_DEV_NAME * UNICODE_MULTIPLIER] = { 0 };
        device* dev;
        if (dev_no > 0)
            {
            sprintf(devname, "LINE%dM%d", nmr, dev_no);
            dev = (device*)M(devname);
            if (dev->get_serial_n() > 0)
                {
                outdev = dev;
                }
            else
                {
                dev = (device*)(M(dev_no));
                if (dev->get_serial_n() > 0)
                    {
                    outdev = dev;
                    }
                else
                    {
                    dev = DEVICE(dev_no);
                    if (dev->get_serial_n() > 0 && dev->get_type() == device::DT_M)
                        {
                        outdev = dev;
                        }
                    else
                        {
                        outdev = nullptr;
                        return -1;
                        }
                    }
                }
            }
        else
            {
            outdev = nullptr;
            }
        return 0;
        }

cipline_tech_object* cipline_tech_object::Mdls[10] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};

int cipline_tech_object::EvalCipReadySignal()
    {
    if (loadedRecipe != lastloadedRecipe)
        {
        if (dev_upr_cip_ready)
            {
            dev_upr_cip_ready->off();
            dev_upr_cip_ready = nullptr;
            }
        lastloadedRecipe = loadedRecipe;
        if (loadedRecipe >= 0)
            {
            u_int dev_no;
            char devname[20] = {0};
            device* dev;
            dev_no = (u_int)rt_par_float[P_SIGNAL_CIP_READY];
            if (dev_no > 0)
                {
                sprintf(devname, "LINE%dDO%d", nmr, dev_no);
                dev = (device*)DO(devname);
                if (dev->get_serial_n() > 0)
                    {
                    dev_upr_cip_ready = dev;
                    }
                else
                    {
                    dev = (device*)(DO(dev_no));
                    if (dev->get_serial_n() > 0)
                        {
                        dev_upr_cip_ready = dev;
                        }
                    else
                        {
                        dev = DEVICE(dev_no);
                        if (dev->get_serial_n() > 0 && dev->get_type() == device::DT_DO)
                            {
                            dev_upr_cip_ready = dev;
                            }
                        else
                            {
                            dev_upr_cip_ready = nullptr;
                            return -1;
                            }
                        }
                    }
                }
            else
                {
                dev_upr_cip_ready = nullptr;
                }
            if (dev_upr_cip_ready)
                {
                dev_upr_cip_ready->on();
                }
            }
        }
    return 0;
    }

int cipline_tech_object::SCInitPumping( int what, int from, int where, int whatdrainage, int step, int f )
    {

    V05->off();

    switch (from)
        {
        case WATER:
            V01->on();
            V02->off();
            V03->off();
            V04->off();
            break;
        case TANK_S:
            V01->off();
            V02->off();
            V03->on();
            V04->off();
            break;
        case TANK_K:
            V01->off();
            V02->on();
            V03->off();
            V04->off();
            break;
        case TANK_W:
            V01->off();
            V02->off();
            V03->off();
            V04->on();
            break;
        default:
            V01->off();
            V02->off();
            V03->off();
            V04->off();
            break;
        }

    switch (where)
        {
        case KANAL:
            V06->on();
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->on();
            V12->off();
            VWMG->off();
            VKMG->off();
            VSMG->off();
            break;
        case NEUTRO:
            V06->on();
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            if (no_neutro) {V11->on();} else {V12->on();}
            VWMG->off();
            VKMG->off();
            VSMG->off();
            break;
        case TANK_W:
            V06->on();
            V07->on();
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            VWMG->off();
            VKMG->off();
            VSMG->off();
            break;
        case TANK_W_MG:
            V06->on();
            if (scoldvalves != 0)
                {
                V07->off();
                }
            else
                {
                V07->on();
                }
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            VWMG->on();
            VKMG->off();
            VSMG->off();
            break;
        case TANK_K:
            V06->on();
            V07->off();
            V08->on();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            VWMG->off();
            VKMG->off();
            VSMG->off();
            break;
        case TANK_K_MG:
            V06->on();
            V07->off();
            if (scoldvalves != 0)
                {
                V08->off();
                }
            else
                {
                V08->on();
                }
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            VWMG->off();
            VKMG->on();
            VSMG->off();
            break;
        case TANK_S:
            V06->on();
            V07->off();
            V08->off();
            V09->on();
            V10->off();
            V11->off();
            V12->off();
            VWMG->off();
            VKMG->off();
            VSMG->off();
            break;
        case TANK_S_MG:
            V06->on();
            V07->off();
            V08->off();
            if (scoldvalves != 0)
                {
                V09->off();
                }
            else
                {
                V09->on();
                }
            V10->off();
            V11->off();
            V12->off();
            VWMG->off();
            VKMG->off();
            VSMG->on();
            break;
        default:
            V06->off();
            V07->off();
            V08->off();
            V09->off();
            V10->off();
            V11->off();
            V12->off();
            VWMG->off();
            VKMG->off();
            VSMG->off();
            break;
        }

    VSDREN->off();
    VKDREN->off();
    VWDREN->off();
    switch (whatdrainage)
        {
        case TANK_W_DREN:
            VWDREN->on();
            break;
        case TANK_S_DREN:
            VSDREN->on();
            break;
        case TANK_K_DREN:
            VKDREN->on();
            break;
        case TANK_SK_DREN:
            VSDREN->on();
            VKDREN->on();
            break;
        case TANK_SW_DREN:
            VSDREN->on();
            VWDREN->on();
            break;
        case TANK_SKW_DREN:
            VSDREN->on();
            VKDREN->on();
            VWDREN->on();
            break;
        case TANK_KW_DREN:
            VKDREN->on();
            VWDREN->on();
            break;
        default:
            VSDREN->off();
            VKDREN->off();
            VWDREN->off();
            break;
        }

    float operV = 0, operT = 0, operFlow;
    operFlow = scparams[0][SCP_FLOW];
    if (0 == operFlow)
        {
        operFlow = 10;
        }


    switch (step)
        {
        case 151:
            operT = parpar[0][P_VTANKW] / operFlow * 5;
            break;
        case 152:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TW];
            break;
        case 153:
            operV = scparams[0][SCP_V_TW_PREDV];
            break;
        case 154:
            operT = scparams[0][SCP_T_TW_DRAIN];
            break;
        case 155:
            operT = scparams[0][SCP_T_SCH_CIRC_PREDV];
            break;
        case 156:
            operT = parpar[0][P_VTANKS] / operFlow * 5;
            break;
        case 157:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TS];
            operT = operV / operFlow * 3.6f * 1.2f;
            break;
        case 160:
            operV = scparams[0][SCP_V_PROM_TS];
            break;
        case 161:
            operT = scparams[0][SCP_T_TW_CIRC];
            break;
        case 162:
            operT= scparams[0][SCP_T_K_CIRC_PREDV];
            break;
        case 163:
            operT = parpar[0][P_VTANKK] / operFlow * 5;
            break;
        case 164:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TK];
            operT = operV / operFlow * 3.6f * 1.2f;
            break;
        case 167:
            operV = scparams[0][SCP_V_PROM_TK];
            break;
        case 168:
            operT = scparams[0][SCP_T_TS_CIRC];
            break;
        case 169:
            operT = parpar[0][P_VTANKW] / operFlow * 5;
            break;
        case 170:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TW];
            operT = operV / operFlow * 3.6f * 1.2f;
            break;
        case 173:
            operV = scparams[0][SCP_V_PROM_TW];
            break;
        case 174:
            operT = scparams[0][SCP_T_TK_CIRC];
            break;
        case 175:
            operT = parpar[0][P_VTANKK] / operFlow * 5;
            break;
        case 176:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TK];
            operT = operV / operFlow * 3.6f * 1.2f;
            break;
        case 179:
            operV = scparams[0][SCP_V_PROM_TS];
            break;
        case 180:
            operT = scparams[0][SCP_T_TW_CIRC];
            break;
        case 181:
            operT = parpar[0][P_VTANKK] / operFlow * 5;
            break;
        case 182:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TK];
            break;
        case 183:
            operT = parpar[0][P_VTANKW] / operFlow * 5;
            break;
        case 184:
            operV = scparams[0][SCP_LITERS_AFTER_LL_TW];
            break;
        case 185:
            operV = scparams[0][SCP_V_CLEAN_TS];
            break;
        case 186:
            operV = scparams[0][SCP_V_CLEAN_TK];
            break;
        case 187:
            operV = scparams[0][SCP_V_CLEAN_TW];
            break;
        case 188:
            operT = scparams[0][SCP_T_TW_DRAIN];
            break;
        default:
            break;
        }

    if (from >= 0)
        {
        NP->on();
        nplaststate = true;
        PIDF->on();
        }
    else
        {
        PIDF->off();
        NP->off();
        nplaststate = false;
        operFlow = -1;
        }

    rt_par_float[P_ZAD_FLOW] = operFlow;
    rt_par_float[P_VRAB] = operV;
    rt_par_float[P_OP_TIME_LEFT] = 0;
    if ( 0 == operT )
        {
        if ( operFlow <= 0 )
            {
            operFlow = 1;
            }
        rt_par_float[ P_MAX_OPER_TM ] = 5 * operV / operFlow;
        }
    else
        {
        rt_par_float[P_MAX_OPER_TM] = operT;
        }
    T[TMR_OP_TIME]->set_countdown_time(rt_par_float[P_MAX_OPER_TM] * 1000);
    T[TMR_OP_TIME]->start();

    rt_par_float[P_CONC] = 0;

    cnt->start();
    return 0;
    }

int cipline_tech_object::SCPumping( int what, int from, int where, int whatdrainage )
    {

    switch (what)
        {
        case SHCH:
            rt_par_float[P_CONC] = GetConc(SHCH);
            break;
        case KISL:
            rt_par_float[P_CONC] = GetConc(KISL);
            break;
        default:
            //par->setParamM(P_CONC, 0);
            break;
        }

    switch (from)
        {
        case WATER:
            if (!LM->is_active())
                {
                V00->on();
                }
            if (LH->is_active())
                {
                V00->instant_off();
                }
            break;
        default:
            break;
        }


    switch (where)
        {
        case TANK_W:
            if (LWH->is_active() && from != TANK_W)
                {
                V07->off();
                VWMG->off();
                V11->off();
                if (no_neutro) {V11->on();} else {V12->on();}
                }
            else
                {
                V07->on();
                VWMG->off();
                V11->off();
                V12->off();
                }
            break;
        case TANK_W_MG:
            if (LWH->is_active() && from != TANK_W)
                {
                V07->off();
                VWMG->off();
                V11->off();
                if (no_neutro) {V11->on();} else {V12->on();}
                }
            else
                {
                if (scoldvalves != 0)
                    {
                    V07->off();
                    }
                else
                    {
                    V07->on();
                    }
                VWMG->on();
                V11->off();
                V12->off();
                }
            break;
        case TANK_K:
            if (LKH->is_active() && from != TANK_K)
                {
                V08->off();
                VKMG->off();
                V11->off();
                if (no_neutro) {V11->on();} else {V12->on();}
                }
            else
                {
                V08->on();
                VKMG->off();
                V11->off();
                V12->off();
                }
            break;
        case TANK_K_MG:
            if (LKH->is_active() && from != TANK_K)
                {
                V08->off();
                VKMG->off();
                V11->off();
                if (no_neutro) {V11->on();} else {V12->on();}
                }
            else
                {
                if (scoldvalves != 0)
                    {
                    V08->off();
                    }
                else
                    {
                    V08->on();
                    }
                VKMG->on();
                V11->off();
                V12->off();
                }
            break;
        case TANK_S:
            if (LSH->is_active() && from != TANK_S)
                {
                V09->off();
                VSMG->off();
                V11->off();
                if (no_neutro) {V11->on();} else {V12->on();}
                }
            else
                {
                V09->on();
                VSMG->off();
                V11->off();
                V12->off();
                }
            break;
        case TANK_S_MG:
            if (LSH->is_active() && from != TANK_S)
                {
                V09->off();
                VSMG->off();
                V11->off();
                if (no_neutro) {V11->on();} else {V12->on();}
                }
            else
                {
                if (scoldvalves != 0)
                    {
                    V09->off();
                    }
                else
                    {
                    V09->on();
                    }
                VSMG->on();
                V11->off();
                V12->off();
                }
            break;
        default:
            break;
        }

    switch (curstep)
        {
        case 151:
        case 169:
        case 183:
            if (!LWL->is_active()) return 1;
            break;
        case 152:
        case 157:
        case 164:
        case 170:
        case 176:
        case 182:
        case 184:
            if (volumePassed() || timeIsOut()) return 1;
            break;
        case 153:
        case 160:
        case 167:
        case 173:
        case 179:
        case 185:
        case 186:
        case 187:
            if (volumePassed()) return 1;
            break;
        case 154:
        case 155:
        case 161:
        case 162:
        case 168:
        case 174:
        case 180:
        case 188:
            if (timeIsOut()) return 1;
            break;
        case 156:
        case 175:
            if (!LSL->is_active()) return 1;
            break;
        case 163:
        case 181:
            if (!LKL->is_active()) return 1;
            break;

        default:
            break;
        }

    rt_par_float[P_OP_TIME_LEFT] = (unsigned long)(T[TMR_OP_TIME]->get_work_time()/1000);
    rt_par_float[P_SUM_OP] = cnt->get_quantity();

    return 0;
    }

int cipline_tech_object::timeIsOut()
    {
    if ( T[ TMR_OP_TIME ]->get_work_time() >=
        static_cast<uint32_t>( rt_par_float[ P_MAX_OPER_TM ] * 1000 ) &&
        rt_par_float[ P_MAX_OPER_TM ] > 0 )
        {
        return 1;
        }
    return 0;
    }

int cipline_tech_object::volumePassed()
    {
    if (rt_par_float[P_VRAB] > 0 && cnt->get_quantity() >= rt_par_float[P_VRAB])
        {
        return 1;
        }
    return 0;
    }

float cipline_tech_object::get_selfclean_par( int parno )
    {
    if (scparams)
        {
        return scparams[0][parno];
        }
    else
        {
        return 0;
        }
    }

void cipline_tech_object::set_selfclean_par( int parno, float newval )
    {
    if (scparams)
        {
        scparams->save(parno, newval);
        }
    }

int cipline_tech_object::InitCustomStep( int what, int from, int where, int how, int step, int f )
    {
    int luares = 0;


    if (2 == is_InitCustomStep_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "init_custom_step" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, step);
        lua_pushinteger(L, f);
        if (0 == lua_pcall(L, 3, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling init_custom_step: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }

    return luares;
    }

int cipline_tech_object::EvalCustomStep( int what, int from, int where, int how )
    {
    int luares = 0;

    if (2 == is_DoCustomStep_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "do_custom_step" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, curstep);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling do_custom_step: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }

    return luares;
    }

void cipline_tech_object::DateToChar(char* buff)
    {
    tm curdate = get_time();
    sprintf(buff, "%02d-%02d-%04d %02d:%02d", curdate.tm_mday, curdate.tm_mon + 1, curdate.tm_year + 1900, curdate.tm_hour, curdate.tm_min);
    }

void cipline_tech_object::resetCarNumber()
    {
    ncar1[ 0 ] = 0;
    ncar2[ 0 ] = 0;
    ncar3[ 0 ] = 0;
    ncar4[ 0 ] = 0;
    }

int cipline_tech_object::getSwitch( int switchNO ) const
    {
    switch (switchNO)
        {
        case 1:
            return switch1;
        case 2:
            return switch2;
        case 3:
            return switch3;
        case 4:
            return switch4;
        }
    return 0;
    }

void cipline_tech_object::setSwitch( int switchNO, int value )
    {
    if (state) return;
    switch (switchNO)
        {
        case 1:
            switch1 = value;
            break;
        case 2:
            switch2 = value;
            break;
        case 3:
            switch3 = value;
            break;
        case 4:
            switch4 = value;
            break;
        }
    }


//Функции вызывающие обработчики LUA
int cipline_tech_object::GoToStep( int cur, int param )
    {
    int luares = 0;

    if (2 == is_GoToStep_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_GoToStep" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, cur);
        lua_pushinteger(L, param);
        if (0 == lua_pcall(L, 3, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_GoToStep: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _GoToStep(cur, param);
        }

    return luares;
    }

int cipline_tech_object::DoStep( int step_to_do )
{
    int luares = 0;

    if (2 == is_DoStep_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_DoStep" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, step_to_do);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_DoStep: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _DoStep(step_to_do);
        }

    return luares;
    }

int cipline_tech_object::InitStep( int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitStep_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitStep" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 3, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitStep: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitStep(step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::LoadProgram( )
    {
    int luares = 0;

    if (2 == is_LoadProgram_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_LoadProgram" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        if (0 == lua_pcall(L, 1, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_LoadProgram: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _LoadProgram();
        }

    return luares;
    }

void cipline_tech_object::StopDev( )
    {
    if (2 == is_StopDev_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_StopDev" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        if (0 != lua_pcall(L, 1, 0, 0))
            {
            printf("Error in calling cip_StopDev: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        _StopDev();
        }

    }

void cipline_tech_object::ResetLinesDevicesBeforeReset( )
    {
    if (2 == is_ResetLinesDevicesBeforeReset_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_ResetLinesDevicesBeforeReset" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        if (0 != lua_pcall(L, 1, 0, 0))
            {
            printf("Error in calling cip_ResetLinesDevicesBeforeReset: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    _ResetLinesDevicesBeforeReset();

    return;
    }

int cipline_tech_object::OporCIP( int where )
    {
    int luares = 0;

    if (2 == is_OporCIP_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_OporCIP" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, where);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_OporCIP: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _OporCIP(where);
        }

    return luares;
    }

int cipline_tech_object::InitOporCIP( int where, int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitOporCIP_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitOporCIP" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, where);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 4, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitOporCIP: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitOporCIP(where, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::CheckErr( )
    {
    int luares = 0;

    if (2 == is_CheckErr_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_CheckErr" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        if (0 == lua_pcall(L, 1, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_CheckErr: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _CheckErr();
        }

    return luares;
    }

int cipline_tech_object::InitCirc( int what, int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitCirc_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitCirc" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, what);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 4, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitCirc: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitCirc(what, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::Circ( int what )
    {
    int luares = 0;

    if (2 == is_Circ_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_Circ" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, what);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_Circ: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _Circ( what );
        }

    return luares;
    }

int cipline_tech_object::InitToObject( int from, int where, int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitToObject_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitToObject" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, from);
        lua_pushinteger(L, where);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 5, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitToObject: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitToObject(from, where, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::InitFromObject( int what, int where, int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitFromObject_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitFromObject" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, what);
        lua_pushinteger(L, where);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 5, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitFromObject: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitFromObject(what, where, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::InitFilCirc( int with_what, int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitFillCirc_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitFillCirc" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, with_what);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 4, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitFillCirc: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitFilCirc(with_what, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::InitOporCirc( int where, int step_to_init, int not_first_call )
    {
    int luares = 0;

    if (2 == is_InitOporCirc_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitOporCirc" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, where);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 4, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitOporCirc: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitOporCirc(where, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::ToObject( int from, int where )
    {
    int luares = 0;

    if (2 == is_ToObject_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_ToObject" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, from);
        lua_pushinteger(L, where);
        if (0 == lua_pcall(L, 3, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_ToObject: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _ToObject(from, where);
        }

    return luares;
    }

int cipline_tech_object::FromObject( int what, int where )
    {
    int luares = 0;

    if (2 == is_FromObject_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_FromObject" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, what);
        lua_pushinteger(L, where);
        if (0 == lua_pcall(L, 3, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_FromObject: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _FromObject(what, where);
        }

    return luares;
    }

int cipline_tech_object::FillCirc( int with_what )
    {
    int luares = 0;

    if (2 == is_FillCirc_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_FillCirc" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, with_what);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_FillCirc: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _FillCirc( with_what );
        }

    return luares;
    }

int cipline_tech_object::OporCirc( int where )
    {
    int luares = 0;

    if (2 == is_OporCirc_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_OporCirc" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, where);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_OporCirc: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _OporCirc( where );
        }

    return luares;
    }

void cipline_tech_object::RT( )
    {
    if (2 == is_RT_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_RT" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        if (0 != lua_pcall(L, 1, 0, 0))
            {
            printf("Error in calling cip_RT: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        _RT();
        }
    }

void cipline_tech_object::Stop( int step_to_stop )
    {
    if (2 == is_Stop_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_Stop" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, step_to_stop);
        if (0 != lua_pcall(L, 2, 0, 0))
            {
            printf("Error in calling cip_Stop: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        _Stop( step_to_stop );
        }
    }

int cipline_tech_object::InitDoseRR( int what, int step_to_init, int not_first_call )
{
    int luares = 0;

    if (2 == is_InitDoseRR_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_InitDoseRR" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, what);
        lua_pushinteger(L, step_to_init);
        lua_pushinteger(L, not_first_call);
        if (0 == lua_pcall(L, 4, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_InitDoseRR: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _InitDoseRR(what, step_to_init, not_first_call);
        }

    return luares;
    }

int cipline_tech_object::DoseRR( int what )
    {
    int luares = 0;

    if (2 == is_DoseRR_func)
        {
        lua_State* L = lua_manager::get_instance()->get_Lua();
        lua_getglobal( L, name_Lua );
        lua_getfield( L, -1, "cip_DoseRR" );
        lua_remove( L, -2 );  // Stack: remove OBJECT.
        lua_getglobal( L, name_Lua );
        lua_pushinteger(L, what);
        if (0 == lua_pcall(L, 2, 1, 0))
            {
            luares = lua_tointeger(L, -1);
            lua_pop(L, 1);
            }
        else
            {
            printf("Error in calling cip_DoseRR: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
            }
        }
    else
        {
        luares = _DoseRR(what);
        }

    return luares;
    }

i_DO_device* cipline_tech_object::VWDREN = nullptr;

i_DO_device* cipline_tech_object::VWMG = nullptr;

i_DO_device* cipline_tech_object::VKDREN = nullptr;

i_DO_device* cipline_tech_object::VKMG = nullptr;

i_DO_device* cipline_tech_object::VSDREN = nullptr;

i_DO_device* cipline_tech_object::VSMG = nullptr;

int cipline_tech_object::scenabled = 0;

int cipline_tech_object::scoldvalves = 0;

saved_params<float, true>* cipline_tech_object::scparams = nullptr;

int cipline_tech_object::scline = 0;

int cipline_tech_object::MdlsCNT = 0;

int cipline_tech_object::msa_number = 0;

int cipline_tech_object::blockAlarm = 0;

saved_params<float, true>* cipline_tech_object::parpar = nullptr;

bool cipline_tech_object::waterTankIsEmpty( )
    {
    auto ret = !LWL->is_active( );
    if ( ret || dont_use_water_tank) return true;
    if ( auto lowLevelExtreme = parpar[ 0 ][ P_MIN_BULK_FOR_WATER ];  lowLevelExtreme > 0 )
        {
        auto currentWaterLevel = LTW->get_value( );
        if ( waterTankLastEmptyState )
            {
            if ( currentWaterLevel >= lowLevelExtreme + parpar[ 0 ][ P_MIN_BULK_DELTA ] )
                {
                ret = false;
                waterTankLastEmptyState = false;
                } else
                {
                ret = true;
                }
            } else
            {
            if ( currentWaterLevel < lowLevelExtreme )
                {
                ret = true;
                waterTankLastEmptyState = true;
                } else
                {
                ret = false;
                }
            }

        }
    return ret;
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MSAPID::MSAPID(run_time_params_float* par, int startpar, int taskpar, i_AO_device* ao /*= 0*/, i_AI_device* ai /*= 0*/,  i_counter* ai2 /*= 0 */, int outmaxrecalcpar /*= 0*/ ):
    uk_1( 0 ),
    ek_1( 0 ),
    ek_2( 0 ),
    q0{},
    q1{},
    q2{},
    Uk{},
    dUk{},
    start_time( get_millisec() ),
    last_time( get_millisec() ),
    prev_manual_mode( 0 ),
    is_down_to_inaccel_mode( 0 ),
    par( par ),
    state( STATE_OFF ),
    start_value( 0 ),
    out_max_recalc_offset(outmaxrecalcpar)
    {
    input = ai;
    input2 = ai2;
    output = ao;
    task_par_offset = taskpar;
    pid_par_offset = startpar;
    lastEvalInOnState = get_millisec();
    HI = 0;
    }

void MSAPID::eval()
    {
    if ( 1 == get_state() )
        {
        lastEvalInOnState = get_millisec();
        set( par[0][task_par_offset] );
        if (input)
            {
            output->set_value( pid_eval(input->get_value()));
            }
        else
            {
            output->set_value( pid_eval(input2->get_flow()));
            }
        }
    }

void MSAPID::eval(float inputvalue, float task)
    {
    if (1 == get_state())
        {
        lastEvalInOnState = get_millisec();
        set(task);
        par[0][task_par_offset] = task;
        if (0 == task)
            {
            output->set_value(0);
            }
        else
            {
            output->set_value(pid_eval(inputvalue));
            }
        }
    }

void MSAPID::reset()
    {
    if (get_delta_millisec(lastEvalInOnState) > 3000L)
        {
        pid_reset();
        }
    }

void MSAPID::on( int accel /*= 0 */ )
    {
    pid_on(accel);
    }

void MSAPID::off()
    {
    pid_off();
    output->set_value(0);
    }

void MSAPID::pid_on( char is_down_to_inaccel_mode /*= 0 */ )
    {
    if ( state != STATE_ON )
        {
        state = STATE_ON;
        start_time = get_millisec(); // Для разгона регулятора.
        last_time  = get_millisec(); // Интервал пересчета значений.
        this->is_down_to_inaccel_mode = is_down_to_inaccel_mode;
        }
    }

void MSAPID::pid_off()
    {
    if ( state != STATE_OFF )
        {
        state = STATE_OFF;
        }
    }

void MSAPID::pid_reset()
    {
    uk_1 = 0;
    ek_1 = 0;
    ek_2 = 0;

    Uk = 0;
    if ( is_down_to_inaccel_mode )
        {
        Uk = 100;
        }

    par[ 0 ][ pid_par_offset + P_U ] = 0;
    par[ 0 ][ pid_par_offset + P_is_manual_mode ] = 0;

    start_time = get_millisec(); //Для разгона регулятора.
    }

float MSAPID::pid_eval( float current_value, int delta_sign /*= 1 */ )
    {
    if ( STATE_OFF == state )
        {
        if ( this->is_down_to_inaccel_mode ) return 100;
        else return 0;
        }

    float K = par[ 0 ][ pid_par_offset + P_k ];
    float TI = par[ 0 ][ pid_par_offset + P_Ti ];
    float TD = par[ 0 ][ pid_par_offset + P_Td ];

    float dt = par[ 0 ][ pid_par_offset + P_dt ] / 1000;
    float dmax = par[ 0 ][ pid_par_offset + P_max ];
    float dmin = par[ 0 ][ pid_par_offset + P_min ];

    if ( dmax == dmin )
        {
        dmax = dmin + 1;
        if ( G_DEBUG )
            {
            printf( "Error! PID::eval dmax == dmin!\n" );
            }
        }

    float ek = delta_sign * 100 * ( par[ 0 ][ pid_par_offset + P_Z ] - current_value ) /
        ( dmax - dmin );

    if ( G_DEBUG )
        {
        if ( dt == 0 )
            {
            printf( "Error! PID::eval dt = 0!\n" );
            }
        if ( TI == 0 )
            {
            printf( "Error! PID::eval TI = 0!\n" );
            }
        }

    if ( dt == 0 ) dt = 1;
    if ( TI == 0 ) TI = 0.0001f;

    if ( get_delta_millisec( last_time ) > static_cast<uint32_t>( dt * 1000 ) )
        {
        q0 = K * ( 1 + TD / dt );
        q1 = K * ( -1 - 2 * TD / dt + 2 * dt / TI );
        q2 = K * TD / dt;

        dUk = q0 * ek + q1 * ek_1 + q2 * ek_2;
        Uk = uk_1 + dUk;
        if ( Uk > 100 ) Uk = 100;
        if ( Uk < 0 ) Uk = 0;

        uk_1 = Uk;
        ek_2 = ek_1;
        ek_1 = ek;

        //-Зона разгона.
        if ( get_delta_millisec( start_time ) <
            par[ 0 ][ pid_par_offset + P_acceleration_time ] * 1000 )
            {
            acceleration( par[ 0 ][ pid_par_offset + P_acceleration_time ] );
            }
        //-Зона разгона.-!>

        last_time = get_millisec();
        } // if ( get_millisec() - last_time > dt*1000 )

    //-Мягкий пуск.
    // Включили ручной режим.
    if ( par[ 0 ][ pid_par_offset + P_is_manual_mode ] && 0 == prev_manual_mode )
        {
        prev_manual_mode = 1;
        par[ 0 ][ pid_par_offset + P_U_manual ] = par[ 0 ][ pid_par_offset + P_U ];
        }

    // Выключили ручной режим.
    if ( par[ 0 ][ pid_par_offset + P_is_manual_mode ] == 0 && 1 == prev_manual_mode )
        {
        prev_manual_mode = 0;
        reset();

        // Начинаем заново разгон регулятора.
        start_time = get_millisec();

        // Устанавливаем начальное значение для разгона регулятора.
        start_value = par[ 0 ][ pid_par_offset + P_U_manual ];

        return par[ 0 ][ pid_par_offset + P_U_manual ];
        }
    //-Мягкий пуск.-!>

    float pidRet = Uk;
    if (par[ 0 ][ out_max_recalc_offset ] >= 0.1)
        {
        pidRet = pidRet * par[ 0 ][ out_max_recalc_offset ] / 100;
        }

    par[ 0 ][ pid_par_offset + P_U ] = pidRet;

    if ( 1 == par[ 0 ][ pid_par_offset + P_is_manual_mode ] )
        {
        return par[ 0 ][ pid_par_offset + P_U_manual ];
        }

    return pidRet;
    }

void MSAPID::acceleration( float accel_time )
    {
    float deltaTime = ( float ) get_delta_millisec( start_time ) / 1000;
    float res = 100 * deltaTime / accel_time;

    if ( is_down_to_inaccel_mode )
        {
        res = 100 - res + start_value;
        if ( Uk < res ) Uk = res;
        }
    else
        {
        if ( Uk > res ) Uk = res + start_value;
        }
    }

void MSAPID::set( float new_z )
    {
    par[ 0 ][ pid_par_offset + P_Z ] = new_z;
    }

u_int_4 MSAPID::get_state() const
    {
    return state;
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TSav::TSav( )
    {
    n = 0;
    cn = 0;
    integrator = 0;
    }

TSav::~TSav( ) = default;

void TSav::Add( float val, unsigned long inegr )
    {
    unsigned long delta, i;
    if ( inegr > integrator )
        {
        delta = inegr - integrator;
        integrator = inegr;
        for ( i = 0; i < delta; i++ )
            {
            n++;
            cn = (cn * (n - 1) + val) / n;
            }
        }
    }

void TSav::R( )
    {
    n = 0;
    cn = 0;
    integrator = 0;
    }

float TSav::Q( ) const
    {
    return cn;
    }


#ifdef WIN_OS
#pragma warning(pop)
#endif //WIN_OS
