/// @file mcaRec.h
/// @brief Реализация класса @ref TRecipeManager для хранения рецептов автоматической моющей станции
/// в энергонезависимой памяти контроллера.
///
/// @author  Рудницкий Артем Иванович
///
/// @par Информация о версии файла
/// @$Rev: 3612 $.\n
/// @$Author: artem $.\n
/// @$Date: 2013-09-17 15:30:53 +0300 (Tue, 17 Sep 2013) $.
#ifndef mcaRecipes_h__
#define mcaRecipes_h__
#include "param_ex.h"
#include "dtime.h"

///@brief Множитель размера строки для кодировки UTF-8
#define UNICODE_MULTIPLIER 3
///@brief Максимальная длина имени для рецепта
#define MAX_REC_NAME_LENGTH 32L
///@brief Размер блока памяти в байтах
#define BLOCK_SIZE 128L
///@brief Интервал проверки изменений в рецепте
#define RECIPE_SAVE_INTERVAL 30000L

///@class TRecipeManager mcaRec.h
///@brief Класс для хранения и работы с рецептами в энергонезависимой памяти контроллера для МСА
class TRecipeManager
    {
    public:
        enum RecipeValues
            {
            RV_IS_USED = 0,
            RV_TO_DEFAULTS,
            RV_V1,  //объем V1 (от модуля до объекта)
            RV_V2,  //объем V2 (от объекта до модуля)
            RV_OBJ_TYPE,  //тип объекта (танк, линия и пр.)
            RV_FLOW,  //расход
            RV_PODP_CIRC,  //подпитывать ли бачок во время циркуляции
            RV_DELTA_TR,  //разница температур на подаче и возврате (если меньше заданной, то начинается отсчет времени циркуляции)
            RV_T_WP,  //температура предварительного ополаскивания
            RV_T_WSP,  //температура промежуточной промывки после щелочи
            RV_T_WKP,  //температура промежуточной промывки после кислоты
            RV_T_WOP,  //температура окончательного ополаскивания
            RV_T_S,  //температура мойки щелочью
            RV_T_K,  //температура мойки кислотой
            RV_T_D,  //температура дезинфекции на воде
            RV_T_DEZSR,  //температура дезинфекции дезинфицирующим средством
            RV_DOP_V_PR_OP,  //дополнительный объем предварительного ополаскивания
            RV_DOP_V_AFTER_S,  //дополнительный объем ополаскивания после щелочи
            RV_DOP_V_AFTER_K,  //дополнительный объем ополаскивания после кислоты
            RV_DOP_V_OK_OP,  //дополнительный объем окончательного ополаскивания
            RV_RET_STOP,  //объем до конца V1 для остановки возвратного насоса
            RV_V_RAB_ML,  //количество литров выше среднего уровня при котором заканчивается заполнение бачка циркуляции
            RV_V_RET_DEL,  //через сколько литров после начала подачи V2 включать возвратный насос
            RV_SIGNAL_PUMP_RUNNING,  //номер DO сигнала "подающий насос включен" (0 - сигнал не используется)
            RV_TM_S,  //время циркуляции на щелочи
            RV_TM_K,  //время циркуляции на кислоте
            RV_TM_S_SK,  //время циркуляции на щелочи при мойке кислота+щелочь
            RV_TM_K_SK,  //время циркуляции на кислоте при мойке кислота+щелочь
            RV_TM_D,  //время дезинфекции на воде
            RV_TM_DEZSR,  //время дезинфекции дезинфицирующим средством
            RV_TM_DEZSR_INJECT,  //время внесения дезинфицирующего средства при дезинфекции
            RV_N_RET,  //номер возвратного насоса
            RV_N_UPR,  //номер сигнала включения возвратного насоса
            RV_OS,  //обратная связь
            RV_OBJ_EMPTY,  //сигнал "объект опорожнен"
            RV_PROGRAM_MASK,  //маска режимов можйки
            RV_T_RINSING_CLEAN,  //температура ополаскивания чистой водой
            RV_V_RINSING_CLEAN,  //объем ополаскивания чистой водой

            RV_T_SANITIZER_RINSING,  //Температура ополаскивания после мойки дезинфецирующим средством
            RV_V_SANITIZER_RINSING,  //Объем ополаскивания после мойки дезинфицирующим средством
            RV_TM_MAX_TIME_OPORBACHOK,  //Максимальное время от пропадания нижнего уровня в бачке до завершении операции опорожнения бачка

            RV_TM_RET_IS_EMPTY,  //время отсутствия протока на возврате для окончания опорожнения объекта
            RV_V_LL_BOT,  //количество литров от пропадания нижнего уровня в бачке до завершения операции "опорожнение бачка"
            RV_R_NO_FLOW,  //минимальный расход при котором считать, что его нет
            RV_TM_R_NO_FLOW,  //время возникновения ошибки "нет расхода на подаче"

            RV_TM_NO_FLOW_R,  //время появления ошибки "нет расхода на возврате"
            RV_TM_NO_CONC,  //время появления ошибки "нет концентрации в возвратной трубе"

            //-PID1
            RV_PIDP_Z = 47,  //Требуемое значение.
            RV_PIDP_k,  //Параметр k.
            RV_PIDP_Ti,  //Параметр Ti.
            RV_PIDP_Td,  //Параметр Td.
            RV_PIDP_dt,  //Интервал расчёта
            RV_PIDP_dmax,  //Мax значение входной величины.
            RV_PIDP_dmin,  //Мin значение входной величины.
            RV_PIDP_AccelTime,  //Время выхода на режим регулирования.
            RV_PIDP_IsManualMode,  //Ручной режим.
            RV_PIDP_UManual,  //Заданное ручное значение выходного сигнала.
            RV_PIDP_Uk,  //Выход ПИД.
            //-PID1-!>
            //-PID2
            RV_PIDF_Z,  //Требуемое значение.
            RV_PIDF_k,  //Параметр k.
            RV_PIDF_Ti,  //Параметр Ti.
            RV_PIDF_Td,  //Параметр Td.
            RV_PIDF_dt,  //Интервал расчёта
            RV_PIDF_dmax,  //Мax значение входной величины.
            RV_PIDF_dmin,  //Мin значение входной величины.
            RV_PIDF_AccelTime,  //Время выхода на режим регулирования.
            RV_PIDF_IsManualMode,  //Ручной режим.
            RV_PIDF_UManual,  //Заданное ручное значение выходного сигнала.
            RV_PIDF_Uk,  //Выход ПИД.
            //-PID2-!>
            RV_TM_MAX_TIME_OPORCIP,  //Максимальное время операции "Опорожнение объекта CIP"
            RV_SIGNAL_MEDIUM_CHANGE,  //Сигнал out "Смена среды"
            RV_SIGNAL_CAUSTIC,  //Сигнал out"Щелочь в трубе"
            RV_SIGNAL_ACID,  //Сигнал out"Кислота в трубе"
            RV_SIGNAL_CIP_IN_PROGRESS,  //Сигнал out"Мойка идет"
            RV_SIGNAL_CIPEND,  //Сигнал out"Мойка окончена"
            RV_SIGNAL_CIP_READY,  //Сигнал out "Мойка готова"
            RV_SIGNAL_OBJECT_READY,  //Сигнал in "Готовность объекта к мойке"
            RV_SIGNAL_SANITIZER_PUMP,  //Сигнал out управления дозатором дезинфицирующего средства
            RV_RESUME_CIP_ON_SIGNAL,  //Автоматическое возобновление мойки при пропадании ошибки объекта CIP
            RV_SIGNAL_PUMP_CONTROL,  //Сигнал in analog "Управление производительностью подающего насоса"
            RV_SIGNAL_DESINSECTION,  //Сигнал out "Дезинфекция"
            RV_SIGNAL_OBJECT_PAUSE,  //Сигнал in "Пауза"
            RV_SIGNAL_CIRCULATION,  //Сигнал out циркуляция.
            RV_SIGNAL_PUMP_CAN_RUN,  //Сигнал in "Возможно включение подающего насоса"
            RV_SIGNAL_PUMP_CONTROL_FEEDBACK,  //Сигнал in analog "Контролируемый уровень для насоса подачи"
            RV_SIGNAL_RET_PUMP_SENSOR,  //Сигнал датчика уровня для работы возвратного насоса
            RV_RET_PUMP_SENSOR_DELAY,  //Задержка выключения возвратного насоса при пропадании сигнала датчика уровня
            RV_SIGNAL_IN_CIP_READY,  //Сигнал in "Мойка идет"
            RV_SIGNAL_CIPEND2,  //Сигнал "Мойка окончена 2"
            RV_SIGNAL_CAN_CONTINUE,  //Сигнал можно продолжать мойку для операций циркуляции и промывки
            RV_SIGNAL_WATER,  //Сигнал "вода в трубе"
            RV_SIGNAL_PRERINSE,  //Сигнал "предварительное ополаскивание"
            RV_SIGNAL_INTERMEDIATE_RINSE,  //Сигнал "промежуточная промывка"
            RV_SIGNAL_POSTRINSE,  //Сигнал "окончательная промывка"
            RV_SIGNAL_PUMP_STOPPED,  //Сигнал "подающий насос остановлен и нет потока"
            RV_SIGNAL_FLOW_TASK,  //Сигнал "задание потока"
            RV_SIGNAL_TEMP_TASK,  //Сигнал "задание температуры"
            RV_SIGNAL_WASH_ABORTED,  //Сигнал "мойка закончена некорректно"
            RV_PRESSURE_CONTROL,  //Задание давления для регулятора
            RV_DONT_USE_WATER_TANK,  //Не использовать вторичную воду при мойке
            RV_PIDP_MAX_OUT,  //Верхняя граница пересчета выхода ПИД-регулятора подогрева
            RV_PIDF_MAX_OUT,  //Верхняя граница пересчета выхода ПИД-регулятора потока

            RV_WATCHDOG,  //Сторожевой таймер

            RV_RESERV_START,  //начало резервных параметров

            RV_WORKCENTER = 104,  //Номер рабочего центра объекта мойки
            RV_FIRSTVALVEON = 105,
            RV_LASTVALVEON = 114,
            RV_FIRSTVALVEOFF = 115,
            RV_LASTVALVEOFF = 119,
            };

    private:
        ///@brief Флаг, сигнализирующий об изменении параметров рецепта
        int recipechanged;
        uint32_t recipechangechecktime;
        int lineNo;
        int currentRecipe;
        int curRecipeStartBlock;
        uint32_t lastEvalTime;
        unsigned long recipeStartAddr;
        void SaveRecipeName();

        void FormRecipeList();
        unsigned long startAddr();
        unsigned long startAddr( int recNo );
        unsigned char* recipeMemory;
        unsigned long recipeMemorySize;
        int ReadMem( unsigned long startaddr, unsigned long length,
            unsigned char* buf, bool is_string = false );
        int WriteMem( unsigned long startaddr, unsigned long length,
            unsigned char* buf, bool is_string = false );

    public:
        ///@brief Имя файла с рецептами
        char* defaultfilename;
        ///@brief Начальный блок для всех экземляров рецептов
        static int startRecipeBlock;
        ///@brief Количество рецептов на линию
        static int recipePerLine;
        ///@brief Длина рецепта в блоках
        static int blocksPerRecipe;
        ///@brief Длина имени рецепта
        static int recipeNameLength;
        ///@brief Относительный адрес начала параметров (от начального адреса рецепта)
        static int startRecipeParamsOffset;
        ///@brief Буфер для копирования рецептов
        static unsigned char* recipeCopyBuffer;
        ///@brief Имя текущего рецепта
        char* currentRecipeName;
        ///@brief Список рецептов для сервера
        char* recipeList;
        /// @fn  int TRecipeManager::LoadRecipeToParams(int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, TParams* par)
        /// @brief Загружает указанное число параметров из указанного рецепта с указанной позиции в указанные параметры
        /// @param recipeNo - номер рецепта
        /// @param recipeStartPos - с данного параметра рецепта начинается загрузка
        /// @param paramsStartPos - начальная позиция в рецепте
        /// @param parQuantity - количество загружаемых параметров
        /// @param par - указатель на класс параметров TParams
        /// @return   int - возвращает 0 при ошибке и !0 при успешном завершении
        int LoadRecipeToParams( int recipeNo, int recipeStartPos,
            int paramsStartPos, int parQuantity, run_time_params_float* par );
        /// @fn  int TRecipeManager::getCurrentRecipe()
        /// @brief Возвращает номер текущего рецепта
        /// @return   int Номер рецепта
        int getCurrentRecipe();
        /// @fn  int TRecipeManager::setCurrentRecipe(int recipeNo)
        /// @brief Задает текущий рецепт
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - заданный номер рецепта > максимального количества рецептов
        int setCurrentRecipe( int recipeNo );
        /// @fn  float TRecipeManager::getRecipeValue(int recNo, int valueNo)
        /// @brief Возвращает значение параметра указанного рецепта
        /// @param recNo Номер рецепта
        /// @param valueNo Номер параметра
        /// @return   float Значение параметра
        float getRecipeValue( int recNo, int valueNo );
        /// @fn  float TRecipeManager::getValue(int valueNo)
        /// @brief Возвращает значение параметра текущего рецепта
        /// @param valueNo Номер параметра
        /// @return   float Значение параметра
        float getValue( int valueNo );
        /// @fn  int TRecipeManager::getRecipeName(int recNO, char* recName)
        /// @brief Получает строку с именем рецепта
        /// @param recNO Номер рецепта
        /// @param recName Указатель на строку, в которую будет записан рецепт
        /// @return   int 0 - ошибка !0 - ОК
        int getRecipeName( int recNO, char* recName );
        /// @fn  int TRecipeManager::setRecipeValue(int recNo, int valueNo, float newValue)
        /// @brief Устанавливает значение заданного параметра заданного рецепта
        /// @param recNo Номер рецепта
        /// @param valueNo Номер параметра
        /// @param newValue Новое значение
        /// @return   int 0 - ошибка !0 - ОК
        int setRecipeValue( int recNo, int valueNo, float newValue );
        /// @fn  int TRecipeManager::setValue(int valueNo, float newValue)
        /// @brief Устанавливает значение заданного параметра текущего рецепта
        /// @param valueNo Номер параметра
        /// @param newValue Новое значение
        /// @return   int 0 - ошибка !0 - ОК
        int setValue( int valueNo, float newValue );
        /// @fn  int TRecipeManager::NextRecipe()
        /// @brief Переход к следующему рецепту
        /// @return   int 0 - ошибка !0 - ОК
        int NextRecipe();
        /// @fn  int TRecipeManager::PrevRecipe()
        /// @brief Переход к предыдущему рецепту
        /// @return   int 0 - ошибка !0 - ОК
        int PrevRecipe();
        /// @fn  int TRecipeManager::ToRecipe(int recNo)
        /// @brief Переход к указанному рецепту
        /// @param recNo - номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int ToRecipe( int recNo );
        /// @fn  int TRecipeManager::OnRecipeDevices(int recipeNo)
        /// @brief Включает устройства рецепта
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int OnRecipeDevices( int recipeNo, int msaline = 1 );
        /// @fn  int TRecipeManager::OffRecipeDevices(int recipeNo)
        /// @brief Выключает устройства рецепта
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int OffRecipeDevices( int recipeNo, int msaline = 1 );
        /// @fn  int TRecipeManager::GetParamsCount()
        /// @brief Возвращает количество параметров для рецепта
        /// @return   int Количество параметров рецепта
        int GetParamsCount();
        /// @fn  int TRecipeManager::ResetRecipeToDefaults(int recipeNo)
        /// @brief Сбрасывает значения параметров указанного рецепта на значения по-умолчанию
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int ResetRecipeToDefaults( int recipeNo );
        /// @fn  void TRecipeManager::LoadRecipeName()
        /// @brief Обновляет имя рецепта из энергонезависимой памяти
        /// @return   void
        void LoadRecipeName();
        /// @fn  void TRecipeManager::EvalRecipe()
        /// @brief Обработка рецептов, периодически сохраняет текущее имя рецепта в энергонезависимую память и формирует список рецептов
        /// @return   void
        void EvalRecipe();
        /// @fn  void TRecipeManager::CopyRecipe()
        /// @brief Копирует текущий рецепт в буфер
        /// @return   void
        void CopyRecipe();
        /// @fn  void TRecipeManager::PasteRecipe()
        /// @brief Переписывает текущий рецепт значениями из буффера
        /// @return   void
        void PasteRecipe();
        /// @fn int TRecipeManager::SaveToFile()
        /// @brief Сохранение рецептов модуля в файл
        /// @return Возвращает 0 в случае успешного завершения
        int SaveToFile( const char* filename );
        /// @fn int TRecipeManager::LoadFromFile()
        /// @brief Загрузка рецептов из сохраненного файла
        /// @return Возвращает 0 в случае успешного завершения
        int LoadFromFile( const char* filename );
        /// @fn  void TRecipeManager::NullifyRecipe()
        /// @brief Обнуляет текущий рецепт
        /// @return   void
        void NullifyRecipe();
        /// @fn   TRecipeManager::TRecipeManager(int lineNo)
        /// @brief Конструктор класса
        /// @param lineNo номер линии мойки, начинается с 0. От него зависит расположение рецептов в памяти
        /// @return
        TRecipeManager( int lineNo );
        ~TRecipeManager();
    };


class TMediumRecipeManager
    {
    public:
        enum RecipeValues
            {
            RV_IS_USED = 0,
            RV_TO_DEFAULTS,
            RV_P_TYPE,  //код раствора
            RV_P_CZAD,  //концентрация рабочего раствора
            RV_P_CMIN,  //минимальная концентрация рабочего раствора
            RV_P_CKANAL,  //максимальная концентрация раствора для канализации
            RV_P_ALF,  //коэффициент температурной зависимости
            RV_P_K,  //концентрация концентрированного раствора
            RV_P_RO,  //плотность концентрированного раствора
            RV_P_CONC25,  //концентрация в заданной точке
            RV_P_MS25,  //проводимость в заданной точке
            };

        enum MediumTypes
            {
            MT_CAUSTIC = 0,
            MT_ACID,
            };

    private:
        ///@brief Флаг, сигнализирующий об изменении параметров рецепта
        int recipechanged;
        int mediumType;
        uint32_t recipechangechecktime;
        int currentRecipe;
        int curRecipeStartBlock;
        uint32_t lastEvalTime;
        unsigned long recipeStartAddr;
        void SaveRecipeName();

        void FormRecipeList();
        unsigned long startAddr();
        unsigned long startAddr( int recNo );
        unsigned char* recipeMemory;
        unsigned long recipeMemorySize;
        int ReadMem( unsigned long startaddr, unsigned long length,
            unsigned char* buf, bool is_string = false );
        int WriteMem( unsigned long startaddr, unsigned long length,
            unsigned char* buf, bool is_string = false );

    public:
        char* defaultfilename;
        ///@brief Начальный блок для всех экземляров рецептов
        static int startRecipeBlock;
        ///@brief Количество рецептов на линию
        static int recipePerLine;
        ///@brief Длина рецепта в блоках
        static int blocksPerRecipe;
        ///@brief Длина имени рецепта
        static int recipeNameLength;
        ///@brief Относительный адрес начала параметров (от начального адреса рецепта)
        static int startRecipeParamsOffset;
        ///@brief Буфер для копирования рецептов
        static unsigned char* recipeCopyBuffer;
        ///@brief Имя текущего рецепта
        char* currentRecipeName;
        ///@brief Список рецептов для сервера
        char* recipeList;
        /// @fn  int TRecipeManager::LoadRecipeToParams(int recipeNo, int recipeStartPos, int paramsStartPos, int parQuantity, TParams* par)
        /// @brief Загружает указанное число параметров из указанного рецепта с указанной позиции в указанные параметры
        /// @param recipeNo - номер рецепта
        /// @param recipeStartPos - с данного параметра рецепта начинается загрузка
        /// @param paramsStartPos - начальная позиция в рецепте
        /// @param parQuantity - количество загружаемых параметров
        /// @param par - указатель на класс параметров TParams
        /// @return   int - возвращает 0 при ошибке и !0 при успешном завершении
        int LoadRecipeToParams( int recipeNo, saved_params<float, true> par );
        /// @fn  int TRecipeManager::getCurrentRecipe()
        /// @brief Возвращает номер текущего рецепта
        /// @return   int Номер рецепта
        int getCurrentRecipe();
        /// @fn  int TRecipeManager::setCurrentRecipe(int recipeNo)
        /// @brief Задает текущий рецепт
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - заданный номер рецепта > максимального количества рецептов
        int setCurrentRecipe( int recipeNo );
        /// @fn  float TRecipeManager::getRecipeValue(int recNo, int valueNo)
        /// @brief Возвращает значение параметра указанного рецепта
        /// @param recNo Номер рецепта
        /// @param valueNo Номер параметра
        /// @return   float Значение параметра
        float getRecipeValue( int recNo, int valueNo );
        /// @fn  float TRecipeManager::getValue(int valueNo)
        /// @brief Возвращает значение параметра текущего рецепта
        /// @param valueNo Номер параметра
        /// @return   float Значение параметра
        float getValue( int valueNo );
        /// @fn  int TRecipeManager::getRecipeName(int recNO, char* recName)
        /// @brief Получает строку с именем рецепта
        /// @param recNO Номер рецепта
        /// @param recName Указатель на строку, в которую будет записан рецепт
        /// @return   int 0 - ошибка !0 - ОК
        int getRecipeName( int recNO, char* recName );
        /// @fn  int TRecipeManager::setRecipeValue(int recNo, int valueNo, float newValue)
        /// @brief Устанавливает значение заданного параметра заданного рецепта
        /// @param recNo Номер рецепта
        /// @param valueNo Номер параметра
        /// @param newValue Новое значение
        /// @return   int 0 - ошибка !0 - ОК
        int setRecipeValue( int recNo, int valueNo, float newValue );
        /// @fn  int TRecipeManager::setValue(int valueNo, float newValue)
        /// @brief Устанавливает значение заданного параметра текущего рецепта
        /// @param valueNo Номер параметра
        /// @param newValue Новое значение
        /// @return   int 0 - ошибка !0 - ОК
        int setValue( int valueNo, float newValue );
        /// @fn  int TRecipeManager::NextRecipe()
        /// @brief Переход к следующему рецепту
        /// @return   int 0 - ошибка !0 - ОК
        int NextRecipe();
        /// @fn  int TRecipeManager::PrevRecipe()
        /// @brief Переход к предыдущему рецепту
        /// @return   int 0 - ошибка !0 - ОК
        int PrevRecipe();
        /// @fn  int TRecipeManager::ToRecipe(int recNo)
        /// @brief Переход к указанному рецепту
        /// @param recNo - номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int ToRecipe( int recNo );
        /// @fn  int TRecipeManager::OnRecipeDevices(int recipeNo)
        /// @brief Включает устройства рецепта
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int OnRecipeDevices( int recipeNo, int msaline = 1 );
        /// @fn  int TRecipeManager::OffRecipeDevices(int recipeNo)
        /// @brief Выключает устройства рецепта
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int OffRecipeDevices( int recipeNo, int msaline = 1 );
        /// @fn  int TRecipeManager::GetParamsCount()
        /// @brief Возвращает количество параметров для рецепта
        /// @return   int Количество параметров рецепта
        int GetParamsCount();
        /// @fn  int TRecipeManager::ResetRecipeToDefaults(int recipeNo)
        /// @brief Сбрасывает значения параметров указанного рецепта на значения по-умолчанию
        /// @param recipeNo Номер рецепта
        /// @return   int 0 - ошибка !0 - ОК
        int ResetRecipeToDefaults( int recipeNo );
        /// @fn  void TRecipeManager::LoadRecipeName()
        /// @brief Обновляет имя рецепта из энергонезависимой памяти
        /// @return   void
        void LoadRecipeName();
        /// @fn  void TRecipeManager::EvalRecipe()
        /// @brief Обработка рецептов, периодически сохраняет текущее имя рецепта в энергонезависимую память и формирует список рецептов
        /// @return   void
        void EvalRecipe();
        /// @fn  void TRecipeManager::CopyRecipe()
        /// @brief Копирует текущий рецепт в буфер
        /// @return   void
        void CopyRecipe();
        /// @fn  void TRecipeManager::PasteRecipe()
        /// @brief Переписывает текущий рецепт значениями из буффера
        /// @return   void
        void PasteRecipe();
        /// @fn int TRecipeManager::SaveToFile()
        /// @brief Сохранение рецептов модуля в файл
        /// @return Возвращает 0 в случае успешного завершения
        int SaveToFile( const char* filename );
        /// @fn int TRecipeManager::LoadFromFile()
        /// @brief Загрузка рецептов из сохраненного файла
        /// @return Возвращает 0 в случае успешного завершения
        int LoadFromFile( const char* filename );
        /// @fn  void TRecipeManager::NullifyRecipe()
        /// @brief Обнуляет текущий рецепт
        /// @return   void
        void NullifyRecipe();
        /// @fn   TRecipeManager::TRecipeManager(int lineNo)
        /// @brief Конструктор класса
        /// @param lineNo номер линии мойки, начинается с 0. От него зависит расположение рецептов в памяти
        /// @return
        TMediumRecipeManager( MediumTypes mType );
        ~TMediumRecipeManager();
    };


#endif  // mcaRecipes_h__
