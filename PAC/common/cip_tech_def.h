#ifndef CIP_TECH_DEF
#define CIP_TECH_DEF
#include "mcaRec.h"
#include "tech_def.h"
#include "g_device.h"
#include "PID.h"
#include "modbus_serv.h"
#include "device/device.h"
#include "device/manager.h"
#include "cip_stats.h"
#include <unordered_set>
#include <string>
#include <sstream>
#include <stdint.h>


#define NOCONC 0.1f

#define TECH_TYPE_SELF_CLEAN          112
#define TECH_TYPE_CAR_WASH            113
#define TECH_TYPE_CAR_WASH_SELF_CLEAN 114

///@brief Максимальная длина списка рецептов
#define PROGRAM_LIST_MAX_LEN 600
///@brief Максимальная длина названия программы мойки
#define PROGRAM_MAX_LEN 24
///@brief Задержка при окончании, сбросе мойки (для протоколирования)
#define WASH_END_DELAY 3000
///@brief Минимальная задержка переключения при сортировке растворов
#define SORT_SWITCH_DELAY 3000
///@brief Минимальное время соблюдения условий для включения запорного клапана пара
#define STEAM_VALVE_MIN_DELAY 1200L
///@brief Количество параметров станции
#define STATION_PAR_COUNT 50
///@brief Количество параметров для программы самоочистки
#define SELFCLEAN_PAR_COUNT 20
///@brief Максимальная длина номера машины
#define CAR_NAME_MAX_LENGTH 15

#define MAX_DEV_NAME 30

#define TMR_CNT 10
#define SAV_CNT 1

#define ON  1
#define OFF 0

//timers for operations
#define FLIS 1
#define FLNO 0

#define SAV_CONC 0

#define TMR_OP_TIME  0
#define TMR_RETURN   1
#define TMR_CHK_CONC 2
#define TMR_NO_FLOW  9

#define MAX_OP_TIME 2

///@brief минимальный расход в м3/час, при котором включается подогрев
#define MIN_FLOW_FOR_HEATING 5

//parameters for main station
#define P_CZAD_S 1   //концентрация рабочего раствора щелочи
#define P_CMIN_S 2   //минимальная концентрация рабочего раствора щелочи
#define P_CKANAL_S \
    3   //максимальная концентрация раствора щелочи для канализации
#define P_CZAD_K 4   //концентрация рабочего раствора кислоты
#define P_CMIN_K 5   //минимальная концентрация рабочего раствора кислоты
#define P_CKANAL_K \
    6   //максимальная концентрация раствора кислоты для канализации
#define P_NAV_OVERREGULATE \
    7  //Перерегулирование концентрации при наведении растворов
#define P_NAV_TOLERANCE 8  //Допуск концентрации растворов при наведении

#define P_BLOCK_ERRORS 10  //Набор флагов для блокировки ошибок модулей мойки
#define P_ALFK         11  //коеффициент температурной зависимости кислоты
#define P_ALFS         12  //коэффициент температурной зависимости щелочи
#define P_K_S          13  //концентрация концентрированной щелочи
#define P_K_K          14  //концентрация концентрированной кислоты
#define P_RO_S         15  //плотность концентрированной щелочи
#define P_RO_K         16  //плотность концентрированной кислоты
#define P_CONC25S      17  //концентрация щелочи в заданной точке
#define P_MS25S        18  //проводимость щелочи в заданной точке
#define P_CONC25K      19  //концентрация кислоты в заданной точке
#define P_MS25K        20  //проводимость кислоты в заданной точке
#define P_MS25W        21  //проводимость воды в заданной точке
#define P_TM_CHKC      22  //время измерения концентрации
#define P_TM_CIRC_RR   23  //время циркуляции перед изменением концентрации
#define P_T_RR         24  //температура растворов при наведении
#define P_VTANKS       25  //объем танка щелочи
#define P_VTANKK       26  //объем танка кислоты
#define P_VTANKW       27  //не используется
#define P_PDNK         28  //производительность дозатора кислоты
#define P_PDNS         29  //производительность дозатора щелочи
#define P_FLOW_RR      30  //производительность при перемешивании растворов

#define P_MAX_BULK_FOR_CAUSTIC \
    31  //максимальный аналоговый уровень при наполнении танка щелочи при наведении
#define P_MAX_BULK_FOR_ACID \
    32  //максимальный аналоговый уровень при наполнении танка кислоты при наведении
#define P_CAUSTIC_TYPE      33  //Тип щелочного раствора
#define P_ACID_TYPE         34  //Тип кислотного раствора
#define P_DEZINFECTION_TYPE 35  //Тип дезинфицирующего раствора
#define P_CAUSTIC_SELECTED  36  //Выбранный рецепт щелочного раствора
#define P_ACID_SELECTED     37  //Выбранный рецепт кислотного раствора
#define P_END_WASH_DELAY    38  //Длительность 555 операции(завершение мойки)
#define P_MIN_BULK_FOR_WATER \
    39  //Минимальный аналоговый уровень в танке воды, при котором считать, что его нет
#define P_MIN_BULK_DELTA 40  //Отклонение уровня в танке вторичной воды

//programms of moika
#define PRG_SELFCLEAN 11
#define PRG_ASO       10
#define PRG_NAVS      9
#define PRG_NAVK      8
#define PRG_PRO       7
#define PRG_CIRC      6
#define PRG_S         5
#define PRG_K         4
#define PRG_S1        3
#define PRG_D         2
#define PRG_SANITIZER 1
#define PRG_OKO       0

#define CIRC_DEFAULT_FEED_COUNT 3
#define CIRC_STOP_PUMP_HOTWATER 256L
#define CIRC_STOP_PUMP_MEDIUM   128L
#define CIRC_TANK_S             64L
#define CIRC_TANK_K             32L
#define CIRC_PODP_SCHC          4L
#define CIRC_PODP_KISL          2L
#define CIRC_PODP_HOTWATER      1L

enum selectablePrograms
    {
    SPROG_NO_PROGRAM = 0,
    SPROG_RINSING_CLEAN = 1,
    SPROG_SANITIZER = 3,
    SPROG_HOTWATER = 5,
    SPROG_RINSING = 128,
    SPROG_RINSING_HOTWATER = 133,
    SPROG_ACID = 144,
    SPROG_ACID_SANITIZER = 147,
    SPROG_ACID_HOTWATER = 149,
    SPROG_CAUSTIC = 160,
    SPROG_CAUSTIC_SANITIZER = 163,
    SPROG_CAUSTIC_HOTWATER = 165,
    SPROG_CAUSTIC_ACID = 176,
    SPROG_CAUSTIC_ACID_SANITIZER = 179,
    SPROG_CAUSTIC_ACID_HOTWATER = 181,
    SPROG_ACID_PREPARATION = 256,
    SPROG_CAUSTIC_PREPARATION = 512,
    SPROG_REMOTE = 1024,
    SPROG_SELF_CLEAN = 2048,
    SPROG_AP_RC_KANAL = 9000,
    SPROG_AP_RC_SW = 9001,
    SPROG_OPTION_DISABLE_RINSE = 9010,
    };

#define KS_MASK 56

///@brief Команды модуля
enum MODULE_COMMANDS
    {
    MCMD_NEXT = 1,
    MCMD_PAUSE,
    MCMD_EVALUATE,
    MCMD_RESET,
    MCMD_RESET_ALARM,
    MCMD_LOCK_MODULE = 10,
    MCMD_UNLOCK_MODULE,
    MCMD_LOAD_RECIPE = 20,
    MCMD_NEXT_RECIPE,
    MCMD_PREV_RECIPE,
    MCMD_CAUSTIC_NEXT_RECIPE,
    MCMD_CAUSTIC_PREV_RECIPE,
    MCMD_ACID_NEXT_RECIPE,
    MCMD_ACID_PREV_RECIPE,
    MCMD_FORCE_RET_ON = 30,  //принудительное включение возвратного насоса
    MCMD_FORCE_RET_OFF,  //принудетельное выключение возвратного насоса
    MCMD_RELOAD_RECIPES = 256,
    MCMD_RELOAD_ALL_RECIPES = 257,
    MCMD_RELOAD_CAUSTIC_RECIPES = 258,
    MCMD_RELOAD_ACID_RECIPES = 259,
    MCMD_RELOAD_WASH_STATS = 260,
    };


enum MODULE_CONSTANTS
    {
    TANK_W,
    TANK_S,
    TANK_K,
    WATER,
    KANAL,
    WITH_WATER,
    WITH_RETURN,
    SHCH,
    KISL,
    HOT_WATER,
    SANITIZER,
    //+++для самоочистки+++
    NEUTRO,
    TANK_W_MG,
    TANK_W_DREN,
    TANK_S_MG,
    TANK_S_DREN,
    TANK_K_MG,
    TANK_K_DREN,
    TANK_KW_DREN,
    TANK_SW_DREN,
    TANK_SK_DREN,
    TANK_SKW_DREN,
    //---для самоочистки---
    };

///@brief CIP_ERROR_CODES
const int ERR_UNKNOWN_STEP = -2;
const int ERR_POSSIBLE_NO_MEDIUM = -9;
const int ERR_NO_ACID = -10;
const int ERR_NO_ALKALINE = -11;
const int ERR_NO_RETURN = -12;
const int ERR_NO_CONC = -13;
const int ERR_IS_CONC = -14;
const int ERR_WRONG_RET = -15;
const int ERR_PUMP = -16;
const int ERR_NO_FLOW = -17;
const int ERR_AIR = -18;
const int ERR_OS = -19;
const int ERR_CIP_OBJECT = -20;
const int ERR_WRONG_OS_OR_RECIPE_ERROR = -30;
const int ERR_VALVES_ARE_IN_CONFLICT = -31;
const int ERR_ACID_WASH_REQUIRED = -32;
const int ERR_LEVEL_BACHOK = -35;
const int ERR_LEVEL_TANK_S = -36;
const int ERR_LEVEL_TANK_K = -37;
const int ERR_LEVEL_TANK_W = -38;
const int ERR_SUPPLY_TEMP_SENSOR = -39;
const int ERR_RETURN_TEMP_SENSOR = -40;
const int ERR_CONCENTRATION_SENSOR = -41;
const int ERR_WATCHDOG = -42;

const int ERR_NO_DESINFECTION_MEDIUM = -71;
const int ERR_DESINFECTION_MEDIUM_MAX_TIME = -72;
const int ERR_DESINFECTION_MEDIUM_INSUFFICIENT_TIME = -73;

const int ERR_RET = -100;
///---CIP_ERROR_CODES

extern const std::map<int, const char*> ERR_MSG;
extern const char* const UNKNOWN_ERR_MSG;

//блокирование ошибок
enum BLOCK_ERRORS
    {
    BE_ERR_LEVEL_BACHOK = 0,
    BE_ERR_LEVEL_TANK_S,
    BE_ERR_LEVEL_TANK_K,
    BE_ERR_LEVEL_TANK_W,
    BE_ERR_SUPPLY_TEMP_SENSOR,
    BE_ERR_RETURN_TEMP_SENSOR,
    BE_ERR_CONCENTRATION_SENSOR,
    };


enum workParameters
    {
    P_CONC_RATE = 1,  //расход концентрата при наведении
    P_ZAD_PODOGR,  //текущее задание подогрева
    P_ZAD_FLOW,  //текущее задание расхода
    P_VRAB,  //заданный объем операции
    P_MAX_OPER_TM,  //максимальное время текущей операции
    P_OP_TIME_LEFT,  //прошло времени текущей операции
    P_CONC,  //текущая концентрация
    P_SUM_OP,  //литров с начала операции
    P_ZAD_CONC,  //заданная концентрация
    P_LOADED_RECIPE,  //загруженный рецепт

    P_PROGRAM,  //программа мойки

    P_CUR_REC,  //текущий выбранный рецепт
    P_RET_STATE,  //состояние возвратного насоса
    P_SELECT_REC,  //загрузка рецепта
    P_SELECT_PRG,  //загрузка программы

    PV1,  //объем V1 (от модуля до объекта)
    PV2,  //объем V2 (от объекта до модуля)
    P_OBJ_TYPE,  //тип объекта (танк, линия и пр.)
    P_FLOW,  //расход
    P_PODP_CIRC,  //подпитывать ли бачок во время циркуляции
    P_DELTA_TR,  //разница температур на подаче и возврате (если меньше заданной, то начинается отсчет времени циркуляции)

    P_T_WP,  //температура предварительного ополаскивания
    P_T_WSP,  //температура промежуточной промывки после щелочи
    P_T_WKP,  //температура промежуточной промывки после кислоты
    P_T_WOP,  //температура окончательного ополаскивания
    P_T_S,  //температура мойки щелочью
    P_T_K,  //температура мойки кислотой
    P_T_D,  //температура дезинфекции на воде

    P_T_SANITIZER,  //температура дезинфекции дезинфицирующим средством
    P_DOP_V_PR_OP,  //дополнительный объем предварительного ополаскивания
    P_DOP_V_AFTER_S,  //дополнительный объем ополаскивания после щелочи
    P_DOP_V_AFTER_K,  //дополнительный объем ополаскивания после кислоты
    P_DOP_V_OK_OP,  //дополнительный объем окончательного ополаскивания

    P_RET_STOP,  //объем до конца V1 для остановки возвратного насоса
    P_V_RAB_ML,  //количество литров выше среднего уровня при котором заканчивается заполнение бачка циркуляции
    PV_RET_DEL,  //через сколько литров после начала подачи V2 включать возвратный насос
    P_SIGNAL_PUMP_RUNNING,  //номер DO сигнала "подающий насос включен" (0 - сигнал не используется)
    PTM_S,  //время циркуляции на щелочи
    PTM_K,  //время циркуляции на кислоте
    PTM_S_SK,  //время циркуляции на щелочи при мойке кислота+щелочь
    PTM_K_SK,  //время циркуляции на кислоте при мойке кислота+щелочь
    PTM_D,  //время дезинфекции на воде
    PTM_SANITIZER,  //время дезинфекции дезинфицирующим средством
    PTM_SANITIZER_INJECT,  //время внесения дезинфицирующего средства при дезинфекции

    P_N_RET,  //номер возвратного насоса
    P_N_UPR,  //номер обратной связи
    P_OS,  //обратная связь
    P_OBJ_EMPTY,  //сигнал "объект опорожнен"
    P_PROGRAM_MASK,  //маска режимов мойки
    P_T_CLEAN_RINSING,  //Температура ополаскивания чистой водой
    P_V_CLEAN_RINSING,  //объем ополаскивания чистой водой

    P_T_SANITIZER_RINSING,  //Температура ополаскивания после мойки дезинфецирующим средством
    P_V_SANITIZER_RINSING,  //Объем ополаскивания после мойки дезинфицирующим средством
    P_TM_MAX_TIME_OPORBACHOK,  //Максимальное время от пропадания нижнего уровня в бачке до завершении операции опорожнения бачка

    P_TM_RET_IS_EMPTY,  //время отсутствия протока на возврате для окончания опорожнения объекта
    P_V_LL_BOT,  //количество литров от пропадания нижнего уровня в бачке до завершения операции "опорожнение бачка"
    P_R_NO_FLOW,  //минимальный расход при котором считать, что его нет
    P_TM_R_NO_FLOW,  //время возникновения ошибки "нет расхода на подаче"

    P_TM_NO_FLOW_R,  //время появления ошибки "нет расхода на возврате"
    P_TM_NO_CONC,  //время появления ошибки "нет концентрации в возвратной трубе"


    //end of parameters
    //-PID1
    PIDP_Z,  //Требуемое значение.
    PIDP_k,  //Параметр k.
    PIDP_Ti,  //Параметр Ti.
    PIDP_Td,  //Параметр Td.
    PIDP_dt,  //Интервал расчёта
    PIDP_dmax,  //Мax значение входной величины.
    PIDP_dmin,  //Мin значение входной величины.
    PIDP_AccelTime,  //Время выхода на режим регулирования.
    PIDP_IsManualMode,  //Ручной режим.
    PIDP_UManual,  //Заданное ручное значение выходного сигнала.
    PIDP_Uk,  //Выход ПИД.
    //-PID1-!>
    //-PID2
    PIDF_Z,  //Требуемое значение.
    PIDF_k,  //Параметр k.
    PIDF_Ti,  //Параметр Ti.
    PIDF_Td,  //Параметр Td.
    PIDF_dt,  //Интервал расчёта
    PIDF_dmax,  //Мax значение входной величины.
    PIDF_dmin,  //Мin значение входной величины.
    PIDF_AccelTime,  //Время выхода на режим регулирования.
    PIDF_IsManualMode,  //Ручной режим.
    PIDF_UManual,  //Заданное ручное значение выходного сигнала.
    PIDF_Uk,  //Выход ПИД.
    //-PID2-!>
    P_TM_MAX_TIME_OPORCIP,  //Максимальное время операции "Опорожнение объекта CIP"
    P_SIGNAL_MEDIUM_CHANGE,  //Сигнал out "Смена среды"
    P_SIGNAL_CAUSTIC,  //Сигнал out"Щелочь в трубе"
    P_SIGNAL_ACID,  //Сигнал out"Кислота в трубе"
    P_SIGNAL_CIP_IN_PROGRESS,  //Сигнал out"Мойка идет"
    P_SIGNAL_CIPEND,  //Сигнал out"Мойка окончена"
    P_SIGNAL_CIP_READY,  //Сигнал out "Мойка готова"
    P_SIGNAL_OBJECT_READY,  //Сигнал in "Готовность объекта к мойке"
    P_SIGNAL_SANITIZER_PUMP,  //Сигнал out управления дозатором дезинфицирующего средства
    P_RESUME_CIP_ON_SIGNAL,  //Автоматическое возобновление мойки при пропадании ошибки объекта CIP
    P_SIGNAL_PUMP_CONTROL,  //Сигнал in analog "Управление производительностью подающего насоса"
    P_SIGNAL_DESINSECTION,  //Сигнал out "Дезинфекция"
    P_SIGNAL_OBJECT_PAUSE,  //Сигнал in "Пауза"
    P_SIGNAL_CIRCULATION,  //Сигнал out циркуляция.
    P_SIGNAL_PUMP_CAN_RUN,  //Сигнал in "Возможно включение подающего насоса"
    P_SIGNAL_PUMP_CONTROL_FEEDBACK,  //Сигнал in analog "Контролируемый уровень для насоса подачи"
    P_SIGNAL_RET_PUMP_SENSOR,  //Сигнал датчика уровня для работы возвратного насоса
    P_RET_PUMP_SENSOR_DELAY,  //Задержка выключения возвратного насоса при пропадании сигнала датчика уровня
    P_SIGNAL_IN_CIP_READY,  //Сигнал in "Мойка идет"
    P_SIGNAL_CIPEND2,  //Сигнал "Мойка окончена 2"
    P_SIGNAL_CAN_CONTINUE,  //Сигнал можно продолжать мойку для операций циркуляции и промывки
    P_SIGNAL_WATER,  //Сигнал вода в трубе
    P_SIGNAL_PRERINSE,  //Сигнал "предварительное ополаскивание"
    P_SIGNAL_INTERMEDIATE_RINSE,  //Сигнал "промежуточная промывка"
    P_SIGNAL_POSTRINSE,  //Сигнал "окончательная промывка"
    P_SIGNAL_PUMP_STOPPED,  //Сигнал "подающий насос остановлен и нет потока"
    P_SIGNAL_FLOW_TASK,  //Сигнал "задание потока"
    P_SIGNAL_TEMP_TASK,  //Сигнал "задание температуры"
    P_SIGNAL_WASH_ABORTED,  //Сигнал "мойка закончена некорректно"
    P_PRESSURE_CONTROL,  //Задание давления для регулятора
    P_DONT_USE_WATER_TANK,  //Не использовать вторичную воду при мойке
    P_PIDP_MAX_OUT,  //Верхняя граница пересчета выхода ПИД-регулятора подогрева
    P_PIDF_MAX_OUT,  //Верхняя граница пересчета выхода ПИД-регулятора потока

    P_WATCHDOG,  //Сторожевой таймер связи с проектом объекта CIP.

    P_RESERV_START,  //начало резервных параметров


    STP_QAVS = 119,  //средняя концентрация щелочи
    STP_QAVK,  //средняя концентрация кислоты
    STP_WC,  //чистая вода
    STP_WS,  //вторичная вода
    STP_LV,  //промежуточный параметр
    STP_WC_INST_WS,  //чистая вода, использованная по причине нехватки вторичной
    STP_WASH_START,  //время начала мойки (используется сервером для хранения)
    STP_STEPS_OVER,  //количество пропущенных операций
    STP_RESETSTEP,  //шаг, на котором сбросили
    STP_ERRCOUNT,  //количество ошибок
    STP_USED_CAUSTIC,  //количество использованной щелочи
    STP_USED_ACID,  //количество использованной кислоты
    STP_LAST_STEP_COUNTER,  //показания счетчика на предыдущей операции
    STP_LAST_STEP,  //предыдущая операция
    STP_USED_HOTWATER,  //количество воды, использованной на дезинфекции
    STP_PODP_CAUSTIC,  //количество подпиток на щелочи
    STP_PODP_ACID,  //количество подпиток на кислоте
    STP_PODP_WATER,  //количество подпиток на воде
    };

//+++Параметры для самоочистки+++
enum SELFCLEAN_PARAMS
    {
    SCP_FLOW = 1,  //Заданный расход мойки танков
    SCP_V_TW_PREDV,  //Объем предварительного ополаскивания танка воды
    SCP_T_TS_DRAIN,  //Время опорожнения танка щелочи (самотеком)
    SCP_T_TK_DRAIN,  //Время опорожнения танка кислоты (самотеком)
    SCP_T_TW_DRAIN,  //Время опорожнения танка воды (самотеком)
    SCP_T_SCH_CIRC_PREDV,  //Время предварительной циркуляции щелочи в танке щелочи
    SCP_T_K_CIRC_PREDV,  //Время предварительной циркуляции кислоты в ТК
    SCP_T_TS_CIRC,  //Время циркуляции раствора в ТЩ через моющую головку
    SCP_T_TK_CIRC,  //Время циркуляции раствора в ТК через моющую головку
    SCP_T_TW_CIRC,  //Время циркуляции раствора в ТВ через моющую головку
    SCP_LITERS_AFTER_LL_TS,  //Кол-во л для откачивания после исчезновения НУ в ТЩ
    SCP_LITERS_AFTER_LL_TK,  //Кол-во л для откачивания после исчезновения НУ в ТК
    SCP_LITERS_AFTER_LL_TW,  //Кол-во л для откачивания после исчезновения НУ в ТВ
    SCP_V_CLEAN_TS,  //Объем окончательного ополаскивания ТЩ чистой водой
    SCP_V_CLEAN_TK,  //Объем окончательного ополаскивания ТК чистой водой
    SCP_V_CLEAN_TW,  //Объем окончательного ополаскивания ТВ чистой водой
    SCP_V_PROM_TS,  //Объем промежуточного ополаскивания ТЩ водой
    SCP_V_PROM_TK,  //Объем промежуточного ополаскивания ТК водой
    SCP_V_PROM_TW,  //Объем промежуточного ополаскивания ТВ водой
    };
//---Параметры для самоочистки---

class MSAPID
    {
    private:
        i_AO_device* output;
        i_AI_device* input;
        i_counter* input2;
        uint32_t lastEvalInOnState;

    public:
        int HI;
        int task_par_offset;
        int pid_par_offset;
        int out_max_recalc_offset;
        MSAPID( run_time_params_float* par, int startpar, int taskpar,
            i_AO_device* ao = nullptr, i_AI_device* ai = nullptr,
            i_counter* ai2 = nullptr, int outmaxrecalcpar = 0 );
        void eval();
        void eval( float input, float task );
        void reset();
        void on( int accel = 0 );
        void off();


    private:
        enum STATES
            {
            STATE_OFF,
            STATE_ON,
            };

        float uk_1;
        float ek_1;
        float ek_2;
        float q0;
        float q1;
        float q2;
        float Uk;
        float dUk;
        uint32_t start_time;
        uint32_t last_time;

        char prev_manual_mode;

        /// @brief Надо ли при старте регулятора уменьшать, а не увеличивать
        /// выходную величину.
        char is_down_to_inaccel_mode;

        run_time_params_float* par;

        u_int_4 state;

        void acceleration( float accel_time );

        float start_value;

    public:
        enum PARAM
            {
            P_Z = 0,  ///< Требуемое значение.
            P_k,  ///< Параметр k.
            P_Ti,  ///< Параметр Ti.
            P_Td,  ///< Параметр Td.
            P_dt,  ///< Интервал расчёта
            P_max,  ///< Мax значение входной величины.
            P_min,  ///< Мin значение входной величины.
            P_acceleration_time,  ///< Время выхода на режим регулирования.
            P_is_manual_mode,  ///< Ручной режим.
            P_U_manual,  ///< Заданное ручное значение выходного сигнала.
            P_U,  ///< Выход ПИД.
            };

        /// @brief Включение ПИД.
        void pid_on( char is_down_to_inaccel_mode = 0 );

        /// @brief Выключение ПИД.
        void pid_off();

        /// @brief Сброс ПИД
        void pid_reset();

        /// @brief Работа ПИД.
        float pid_eval( float current_value, int delta_sign = 1 );

        /// @brief Установка нового задания ПИД.
        void set( float new_z );

        /// @brief Получение задания ПИД.
        float get_assignment();

        /// @brief Состояние регулятора
        u_int_4 get_state() const;
    };

class TSav
    {
    private:
        float cn;
        unsigned long n;
        unsigned long integrator;

    public:
        TSav();
        ~TSav();
        void Add( float val, unsigned long inegr );
        void R();
        float Q() const;
    };

// Данная линия также содержит параметры станции CIP.
constexpr int FIRST_CIPLINE_OBJECT_NUMBER = 1;

class cipline_tech_object : public tech_object
    {
    protected:
        int ncmd;
        bool is_reset;  //Идет сброс программы мойки.
        bool
            is_ready_to_end{};  //Операция должна закончиться но ожидает сигнала готовности от объекта

        unsigned int tech_type;  //подтип объекта
        int ret_circ_flag;  //флаг управления возвратным насосом при циркуляции
        uint32_t
            ret_circ_delay;  //задержка обратного включения возвратного насоса

        uint32_t
            steam_valve_delay;  //таймер задержки включения отсечного клапана пара (чтобы не включался/выключался на граничных значениях температуры)
        uint32_t
            bachok_lvl_err_delay;  //задержка для появления ошибки уровня бачка

        int pumpflag;  //флаг ошибки подающего насоса
        uint32_t pumptimer;  //таймер ошибки подающего насоса

        int sort_last_destination;  //куда последний раз направлялось при сортировке
        uint32_t
            sort_delay;  //таймер задержки при сортировке растворов (чтобы не переключалось хаотически)
        int tankempty;  //флаг пустого танка для задержек переключений
        uint32_t tankemptytimer;  //пауза между переключениями при пустом танке
        int tankfull;  //флаг полного танка для задержек переключений
        uint32_t tankfulltimer;  //пауза между переключениями при полном танке

        int forcesortrr;  //флаг принудительной сортировки растворов без учета таймаута переключений

        bool
            waterTankLastEmptyState{};  //последнее состояние танка вторичной воды

        uint32_t enddelayTimer;
        int valvesAreInConflict{};
        void resetProgramName();
        void resetRecipeName();
        void resetProgramList( unsigned long programmask = 0xB00 );
        void formProgramList( unsigned long programmask );
        void addProgramToList( const char* programName, int programNmr );
        void loadProgramFromList( int selectedPrg );
        void closeLineValves() const;
        int isTank();
        int isLine();
        int getValvesConflict() const;

        void resetCarNumber();

        //для ошибки "возможно отсутствует концентрированный раствор"
        int no_liquid_is_warning;
        int no_liquid_phase;
        float no_liquid_last_time;


        //Обработчики LUA
        int is_in_evaluate_func;
        int is_in_error_func;
        int is_InitCustomStep_func;
        int is_DoCustomStep_func;
        int is_GoToStep_func;
        int is_DoStep_func;
        int is_InitStep_func;
        int is_LoadProgram_func;
        int is_StopDev_func;
        int is_ResetLinesDevicesBeforeReset_func;
        int is_OporCIP_func;
        int is_InitOporCIP_func;
        int is_CheckErr_func;
        int is_Circ_func;
        int is_InitCirc_func;
        int is_ToObject_func;
        int is_InitToObject_func;
        int is_FromObject_func;
        int is_InitFromObject_func;
        int is_FillCirc_func;
        int is_InitFillCirc_func;
        int is_OporCirc_func;
        int is_InitOporCirc_func;
        int is_RT_func;
        int is_Stop_func;
        int is_DoseRR_func;
        int is_InitDoseRR_func;
        int is_On_Resume_func;
        int is_ConfigureLine_func;

    public:
        cipline_tech_object( const char* name, u_int number, u_int type,
            const char* name_Lua, u_int states_count, u_int timers_count,
            u_int par_float_count, u_int runtime_par_float_count,
            u_int par_uint_count, u_int runtime_par_uint_count );


        ~cipline_tech_object() override;

        //Флаг формата описания устройств
        char is_old_definition{};
        //В программе мойки есть мойка кислотой
        bool program_has_acid;
        //В программе мойки есть мойка щелочью
        bool program_has_caustic;
        //В программе мойки есть дезинфекция
        bool program_has_desinfection;
        //В программе мойки есть холодная дезинфекция
        bool program_has_cold_desinfection;
        //Программа была завершена некорректно
        bool program_was_terminated;

        //работа с параметрами
        static saved_params<float, true>* parpar;
        static float get_station_par( int parno );
        static void set_station_par( int parno, float newval );

        //Статистика по объектам мойки
        static cip_stats* statsbase;
        static std::unordered_set<int> steps_v2_supply;
        static std::unordered_set<int> steps_additional_rinse;
        static std::unordered_set<int> steps_circulation;
        static std::unordered_set<int> steps_caustic;
        static std::unordered_set<int> steps_acid;

        //Константы
        static const uint32_t toTankSwitchTimeMs{ 60000L };
        static const uint32_t circMaxFeedTimeMs{ 60000L };

        cip_object_stats* objectstats;
        cip_object_stats* emptystats;

        ///Флаг отсутствия нейтрализации
        char no_neutro;
        ///Флаг использования танка для вторичной воды
        char dont_use_water_tank;
        int ret_overrride;  //флаг принудительного включения/выключения возвратного насооса
        int return_ok;  //есть расход на возврате
        int concentration_ok;  //есть концентрация на возврате
        int enable_ret_pump;  //используется для того, чтобы определить, нужно ли отключать возвратный насос
        ///Ручное управление ПИД-регулятором подачи из скрипта
        bool pidf_override;
        ///Последнее состояние подающего насоса
        bool nplaststate;
        bool flagnplaststate;
        ///Максимальное количество моек щелочью без кислоты.
        int no_acid_wash_max;
        bool use_internal_medium_recipes;

        //Флаг использования сигнала "циркуляция" на подаче объема V2
        bool use_circulation_on_v2_supply;

        //Переменные для циркуляции
        char circ_tank_s;  //Циркулировать ли через танк со щелочью
        char circ_tank_k;  //Циркулировать ли через танк с кислотой
        char circ_podp_water;  //Подпитывать ли бачок на воде
        char circ_podp_s;  //Подпитывать ли бачок на щелочи
        char circ_podp_k;  //Подпитывать ли бачок на кислоте
        char circ_podp_count;  //текущее количество подпиток на операции
        char
            circ_podp_max_count;  //максимальное количество подпиток на операции
        char
            circ_water_no_pump_stop;  //не останавливать насос при поялении верхнего уровня в бачке
        char
            circ_medium_no_pump_stop;  //не останавливать насос при поялении верхнего уровня в бачке на щелочи и кислоте
        char circ_was_feed{};  //флаг факта подпитки
        uint32_t circ_max_timer;  //таймер подпитки
        char
            circ_temp_reached{};  //флаг достижения заданной температуры на возврате
        uint32_t circ_return_timer;

        //Рецепты
        TRecipeManager* lineRecipes;
        int loadedRecipe;
        int lastloadedRecipe;
        int loadedProgram;
        char* loadedRecName;
        std::string programList{};
        char* currentProgramName;

        //Выбор моющего средства
        static TMediumRecipeManager* causticRecipes;
        static char* causticName;
        static int causticLoadedRecipe;
        static TMediumRecipeManager* acidRecipes;
        static char* acidName;
        static int acidLoadedRecipe;

        //Номер машины(или что-нибудь другое)
        char* ncar1;
        char* ncar2;
        char* ncar3;
        char* ncar4;
        int switch1;
        int switch2;
        int switch3;
        int switch4;
        int getSwitch( int switchNO ) const;
        void setSwitch( int switchNO, int value );


        //Список программ для панели
        static const int panelMaxPrograms = 16;
        static const int panelProgramLength = 24;
        std::vector<std::string> prgArray{};
        int prgNumber[ panelMaxPrograms ]{};
        int prgListLen{};


        int blocked;

        int opcip;
        int curstep;
        int state;
        int curprg;
        int nmr;

        int cip_in_error;

        unsigned long
            default_programlist;  //список доступных программ мойки по-умолчанию
        int disable_tank_heating;  //отключение подогрева при начале подачи растворов в танк(для МСА со старыми регулирующими клапанами)
        int clean_water_rinsing_return;  //Куда возвращать на операции окончательного ополаскивания
        bool disable_final_rinsing;  //Не ополаскивать после дезинфекции

        uint32_t ret_pums_ls_timer{};

        static int blockAlarm;
        static cipline_tech_object* Mdls[ 10 ];
        static int MdlsCNT;

        //+++функции самоочистки+++
        static i_DO_device* VSMG;
        static i_DO_device* VSDREN;
        static i_DO_device* VKMG;
        static i_DO_device* VKDREN;
        static i_DO_device* VWMG;
        static i_DO_device* VWDREN;
        static int scline;
        static int scenabled;
        static int scoldvalves;
        static saved_params<float, true>* scparams;
        static float get_selfclean_par( int parno );
        static void set_selfclean_par( int parno, float newval );
        int SCInitPumping(
            int what, int from, int where, int whatdrainage, int step, int f );
        int SCPumping( int what, int from, int where, int whatdrainage );
        int timeIsOut();
        int volumePassed();
        //---функции самоочистки---
        i_DO_device* V00{};
        i_DO_device* V01{};
        i_DO_device* V02{};
        i_DO_device* V03{};
        i_DO_device* V04{};
        i_DO_device* V05{};
        i_DO_device* V06{};
        i_DO_device* V07{};
        i_DO_device* V08{};
        i_DO_device* V09{};
        i_DO_device* V10{};
        i_DO_device* V11{};
        i_DO_device* V12{};
        i_DO_device* V13{};

        i_AI_device* LTK{};
        i_AI_device* LTS{};
        i_AI_device* LTW{};

        i_DO_AO_device* NP{};
        i_DO_AO_device* NK{};
        i_DO_AO_device* NS{};
        i_DI_device* LL{};
        i_DI_device* LM{};
        i_DI_device* LH{};
        i_DI_device* LWL{};
        i_DI_device* LWH{};
        i_DI_device* LSL{};
        i_DI_device* LSH{};
        i_DI_device* LKL{};
        i_DI_device* LKH{};
        i_AI_device* TP{};
        i_AI_device* TR{};
        i_AI_device* Q{};
        i_AO_device* ao{};
        i_AO_device* PUMPFREQ{};
        i_AI_device* PRESSURE{};
        i_DI_device* FL{};
        timer* T[ TMR_CNT ]{};
        TSav* SAV[ SAV_CNT ]{};
        i_counter* cnt{};
        //-------------------

        MSAPID* PIDF{};
        MSAPID* PIDP{};
        void initline();


        //Устройства для непосредственного объекта мойки
        device* dev_upr_ret;  //Сигнал управления возвратным насосом
        device* dev_m_ret;  //Возвратный насос на моечной станции
        device* dev_os_object;  //Обратная связь объекта мойки
        device* dev_os_object_ready;  //Обратная связь объекта мойки 2
        device* dev_os_object_pause;  //Обратная связь объекта сигнал "пауза"
        device* dev_os_object_empty;  //Сигнал "объект опорожнен"
        device* dev_upr_medium_change;  //Сигнал "смена среды"
        device* dev_upr_caustic;  //Сигнал "щелочь"
        device* dev_upr_acid;  //Сигнал "кислота"
        device* dev_upr_water;  //Сигнал "вода в трубе"
        device* dev_upr_desinfection;  //Сигнал "дезинфекция"
        device* dev_upr_cip_ready;  //Сигнал "готовность к мойке"
        device* dev_upr_cip_in_progress;  //Сигнал "готовность к мойке"
        device* dev_upr_cip_finished;  //Сигнал "мойка окончена"
        device* dev_upr_cip_finished2;  //Сигнал "мойка окончена 2"
        device* dev_ai_pump_frequency;  //Задание частоты подающего насоса
        device* dev_ai_pump_feedback;  //Уровень для контроля подающего насоса
        device*
            dev_upr_sanitizer_pump;  //Управление насосом подачи дезинфицирующего средства
        device* dev_upr_circulation;  //Сигнал "Циркуляция"
        device*
            dev_os_pump_can_run;  //Сигнал, запрещающий включение подающего насоса.
        device* dev_ls_ret_pump;  //Сигнал уровня перед возвратным насосом
        device* dev_os_cip_ready;  //Сигнал "мойка готова" от объекта
        device*
            dev_os_can_continue;  //Сигнал "можно переходить на другой шаг" на операциях циркуляции и доп. ополаскиваниии

        device* dev_upr_prerinse;  //Сигнал "предварительное ополаскивание"
        device* dev_upr_intermediate_rinse;  //Сигнал "промежуточная промывка"
        device* dev_upr_postrinse;  //Сигнал "окончательная промывка"
        device*
            dev_upr_pump_stopped;  //Сигнал "подающий насос остановлен и нет потока"
        device* dev_upr_pump_running;  //Сигнал "подающий насос включен"
        device* dev_ao_flow_task;  //Сигнал "задание потока"
        device* dev_ao_temp_task;  //Сигнал "задание температуры"
        device* dev_upr_wash_aborted;  //Сигнал "мойка закончена некорректно"

        device* dev_watchdog = nullptr;  //Watchdog связь объекта мойки.

        int
        init_object_devices();  //Функция для инициализации устройств объекта мойки

        int check_device( device*& outdev, int parno, device::DEVICE_TYPE );

        int check_DI( device*& outdev, int parno );
        int check_DO( device*& outdev, int parno );
        int check_AI( device*& outdev, int parno );
        int check_AO( device*& outdev, int parno );
        int check_LS( device*& outdev, int parno );
        int check_M( device*& outdev, int parno );
        //----------------------------------------------

        static int msa_number;

        //overriden members
        int save_device( char* buff ) const override;
        int set_cmd( const char* prop, u_int idx, double val ) override;
        int set_cmd( const char* prop, u_int idx, const char* val );
        int evaluate() override;
        int init_params() override;
        int init_runtime_params() override;

        ////-------------------
        void RHI();
        void PauseTimers();
        int SetCommand( int command );
        void ResetWP();
        int SetRet( int val );
        int GetRetState() const;
        int HasRet() const;
        int ForceRet( int val );
        void ResetStat();
        bool waterTankIsEmpty();
        ///Базовые функции. При необходимости могут вызываться из Lua. Могут быть переопределены.
        int _DoStep( int step_to_do );  //Выполнение шага, заданного параметром.
        int _GoToStep( int cur, int param );  //Переход к следующему шагу.
        int _InitStep( int step_to_init,
            int not_first_call );  //cip_InitStep(steptoinit, param)
        int _LoadProgram();
        void _StopDev();
        void _ResetLinesDevicesBeforeReset();
        int _OporCIP( int where );
        int _InitOporCIP( int where, int step_to_init, int not_first_call );
        int _CheckErr();
        int _Circ( int what );
        int _InitCirc( int what, int step_to_init, int not_first_call );
        int _InitToObject( int from, int where, int step_to_init, int f );
        int _InitFromObject( int what, int where, int step_to_init, int f );
        int _InitFilCirc( int with_what, int step_to_init, int f );
        int _InitOporCirc( int where, int step_to_init, int not_first_call );
        int _ToObject( int from, int where );
        int _FromObject( int what, int where );
        int _FillCirc( int with_what );
        int _OporCirc( int where );
        void _RT();
        void _Stop( int step_to_stop );
        int _InitDoseRR( int what, int step_to_init, int not_first_call );
        int _DoseRR( int what );
        ///-----------------------------------------------
        ////Функции, вызывающие обработчики на Lua. При отсутствии обработчиков вызываются стандартные функции.
#ifdef PTUSA_TEST
        virtual
#endif
            int DoStep( int step_to_do );  //cip_DoStep(step)
        int GoToStep( int cur, int param );  //cip_GoToStep(currentstep,param)
        int InitStep( int step_to_init,
            int not_first_call );  //cip_InitStep(steptoinit, param)
        int LoadProgram();  //cip_LoadProgram()
        void StopDev();  //cip_StopDev()
        void ResetLinesDevicesBeforeReset();
        int OporCIP( int where );
        int InitOporCIP( int where, int step_to_init, int not_first_call );
        int CheckErr();
        int Circ( int what );
        int InitCirc( int what, int step_to_init, int not_first_call );
        int InitToObject(
            int from, int where, int step_to_init, int not_first_call );
        int InitFromObject(
            int what, int where, int step_to_init, int not_first_call );
        int InitFilCirc( int with_what, int step_to_init, int not_first_call );
        int InitOporCirc( int where, int step_to_init, int not_first_call );
        int ToObject( int from, int where );
        int FromObject( int what, int where );
        int FillCirc( int with_what );
        int OporCirc( int where );
        void RT();
        void Stop( int step_to_stop );
        int InitDoseRR( int what, int step_to_init, int not_first_call );
        int DoseRR( int what );
        ////--------------------------------------------
        ////-----error service-------
        void ResetErr();
        ////-----for main station----
        void SortRR( int where, int forcetotank = 0 );
        float GetConc( int what ) const;
        int InitFilRR( int where );
        int InitCircRR( int where );
        int InitCheckConc( int where );
        int InitAddRR( int where, int step, int first_init_flag );
        int InitOpolRR( int where );
        int FilRR( int where );
        int CircRR( int where );
        int CheckConc( int where );
        int AddRR( int where );
        int OpolRR( int where );
        ////-------------------
        int EvalBlock();
        ////-------------------
        int EvalPIDS();
        int EvalCommands();
        int EvalRecipes();
        int EvalCipInProgress();
        int EvalCipInError();
        int EvalCipReadySignal();
        ////------------------------------
        int InitCustomStep(
            int what, int from, int where, int how, int step, int f );
        int EvalCustomStep( int what, int from, int where, int how );
        ////Вспомогательные функции
        static void DateToChar( char* buff );
    };


#endif  //CIP_TECH_DEF
