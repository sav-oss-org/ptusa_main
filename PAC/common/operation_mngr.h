/// @file operation_mngr.h
/// @brief Содержит описания классов, которые используются для организации шагов
/// при выполнении операций танка, линий.
///
/// Класс @ref step, служащий для организации работы с шагом, содержит всю
/// необходимую информацию - список открываемых\закрываемых устройств,
/// параметры шага, связанный с ним маршрут. Шаг - представляет ход
/// протекания технологического процесса, включение\выключение которого
/// происходит автоматически, без непосредственного участия оператора. В
/// отличие от операции, который оператор включает\выключает сам. Для хранения
/// всех операций объекта служит класс @ref mode_manager.
/// Класс @ref action содержит списки устройств и выполняет над ними
/// определенные действия.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef MODE_MNGR
#define MODE_MNGR

#include <string>
#include <array>
#include <chrono>

#include "dtime.h"

#include "device/device.h"
#include "PAC_info.h"
#include "param_ex.h"

#include "i_tech_def.h"

class operation_manager;
class operation_state;
struct lua_State;

//-----------------------------------------------------------------------------
/// @brief Действие над устройствами (включение, выключение и т.д.).
class action
    {
    public:
        action( std::string name, u_int subgropups_cnt = 1 );

        virtual ~action() = default;

        /// @brief Проверка на отсутствие устройств.
        ///
        /// @return true  Есть устройства, над которыми что-то делается.
        /// @return false Нет устройств, над которыми что-то делается.
        virtual bool is_empty() const;

        virtual void print( const char* prefix = "", bool new_line = true ) const;

        /// @brief Проверка действия.
        ///
        /// @param [out] reason Пояснение, почему нельзя выполнить действие.
        virtual int check( char* reason, unsigned int max_len ) const
            {
            reason[ 0 ] = 0;
            return 0;
            }

        /// @brief Инициализация действия.
        virtual void init()
            {
            // По умолчанию ничего не делаем.
            }

        /// @brief Выполнение действия.
        virtual void evaluate()
            {
            // По умолчанию ничего не делаем.
            }

        /// @brief Завершения действия.
        virtual void finalize();

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Группа устройства.
        /// @param [in] subgroup Подгруппа устройств.
        virtual void add_dev( device *dev, u_int group = MAIN_GROUP,
            u_int subgroup = MAIN_SUBGROUP );

        /// @brief Поиск устройства с какой-либо ошибкой.
        ///
        /// @param [out] err_description Устройства, находящиеся в ошибке.
        /// @param [in] size Дополнительный параметр.
        /// @return 0 - нет ошибок, >0 - есть ошибки, длина строки описания.
        int check_devices( char* err_description, int size ) const;

        /// @brief Установка параметров для действия.
        ///
        /// @param [in] par Параметры.
        virtual void set_params( const saved_params_float *new_par )
            {
            par = new_par;
            }

        /// @brief Задание индексов используемых параметров к действию.
        ///
        /// @param [in] position Позиция параметра.
        /// @param [in] idx Индекс параметра.
        void set_param_idx( unsigned int position, int idx )
            {
            while ( position >= par_idx.size() )
                {
                par_idx.push_back( 0 );
                }

            par_idx[ position ] = idx;
            }

        /// @brief Задание булевского свойства (настраивается пользователем
        /// при описании проекта).
        ///
        /// @param [in] name Название свойства.
        /// @param [in] value Значение свойства.
        virtual int set_bool_property( const char* prop_name, bool value );

        /// @brief Задание числового свойства (настраивается пользователем
        /// при описании проекта).
        ///
        /// @param [in] name Название свойства.
        /// @idx [in] index Индекс свойства.
        /// @param [in] value Значение свойства.
        virtual int set_int_property( const char* prop_name, size_t idx,
            int value );

        enum CONSTANTS
            {
            MAIN_GROUP = 0,
            MAIN_SUBGROUP = 0
            };

        /// @brief Очистка устройств действия.
        virtual void clear_dev();

    protected:
        u_int subgropups_cnt;

        // Устройства.
        std::vector < std::vector< std::vector< device* > > > devices;
        std::string name;                           ///< Имя действия.

        const saved_params_float* par = nullptr;    ///< Параметры действия.
        std::vector< int >        par_idx;  ///< Индексы параметров действия.
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Включение устройств.
/// </summary>
class on_action: public action
    {
    public:
        on_action();

        void evaluate() override;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Включение устройств с задержкой.
/// </summary>
class delay_on_action : public action
    {
    public:
        delay_on_action();

        void init() override;

        void evaluate() override;

    private:
        uint32_t start_time;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Включение реверсных устройств.
/// </summary>
class on_reverse_action : public action
    {
    public:
        on_reverse_action() : action( "Включать реверс" )
            {
            }

        void evaluate() override;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Выключение устройств.
/// </summary>
class off_action: public action
    {
    public:
        off_action(): action( "Выключать" )
            {
            }

        void evaluate() override;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Выключение устройств с задержкой.
/// </summary>
class delay_off_action : public action
    {
    public:
        delay_off_action();

        void init() override;

        void evaluate() override;

    private:
        uint32_t start_time;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Промывка седел.
/// </summary>
class open_seat_action: public action
    {
    public:
        open_seat_action( bool is_mode, operation_state *owner );

        void init() override;
        void evaluate() override;
        void finalize() override;

#ifdef PTUSA_TEST
        void set_wait_time( int wait_time );
        int get_wait_time();
#endif

        /// @brief Добавление устройства к действию.
        ///
        /// @param [in] dev Устройство.
        /// @param [in] group Дополнительный параметр.
        /// @param [in] seat_type Дополнительный параметр.
        void add_dev( device *dev, u_int group, u_int seat_type ) override;

        virtual void print( const char* prefix = "", bool new_line = true ) const;

        bool is_empty() const override;

    private:
        enum PHASES
            {
            P_WAITING,
            P_OPEN_UPPER,
            P_OPEN_LOWER,
            };

        PHASES phase;      ///< Текущий этап.
        PHASES next_phase; ///< Следующий этап.

        u_int active_group_n;  ///< Номер промываемой сейчас группы.

        uint32_t wait_time;      ///< Время ожидания перед промыванием седел.

        /// Седла.
        std::vector< std::vector< device* > > wash_upper_seat_devices;
        std::vector< std::vector< device* > > wash_lower_seat_devices;

        uint32_t start_cycle_time; ///< Время старта цикла (ожидания или промывки).

        bool is_mode;             ///< Является ли шагом операции.
        operation_state* owner;

        void switch_off( std::vector< std::vector< device* > > devices,
            bool is_check = false );

        void switch_off_group( std::vector< device* > group,
            bool is_check = false );

        void switch_on_group( std::vector< device* > group, valve::VALVE_STATE st );
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Пары DI->DO.
/// </summary>
class DI_DO_action: public action
    {
    public:
        explicit DI_DO_action( std::string name = "Группы DIs->DOs",
            bool is_inverted_logic = false );

        int check( char* reason, unsigned int max_len ) const override;

        void evaluate() override;

        void finalize() override;

        void print( const char* prefix = "",
            bool new_line = true ) const override;

        /// @brief Установка свойства для группы DI->DO.
        ///
        /// Используется для конфигурирования логики обработки сигналов DI
        /// (типа логики OR/AND) при управлении соответствующими DO.
        int set_int_property( const char* prop_name, size_t idx,
            int value ) override;

        bool is_di_device_type( device::DEVICE_TYPE device_type ) const;

        void add_dev( device* dev, u_int group = MAIN_GROUP,
            u_int subgroup = MAIN_SUBGROUP ) override;

        void clear_dev() override;

    private:
        enum class LOGIC_TYPE
            {
            OR = 0,  // Default: any DI active turns on all DO.
            AND = 1  // All DI must be active to turn on all DO.
            };

        std::vector < LOGIC_TYPE > logic_type;

        virtual void evaluate_DO( const std::vector< device* >& devices,
            LOGIC_TYPE logic_t );

        bool is_inverted{ false };
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Пары AI->AO.
/// </summary>
class AI_AO_action : public action
    {
    public:
        AI_AO_action();

        int check( char* reason, unsigned int max_len ) const override;

        void evaluate() override;

        void finalize() override;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Проверка дискретных входных сигналов.
/// </summary>
class required_DI_action: public action
    {
    public:
        required_DI_action(): action( "Сигналы для включения" )
            {
            }

        int check( char* reason, unsigned int max_len ) const override;

        void finalize() override;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Проверка устройств во время выполнения шага.
/// </summary>
class checked_devices_action final : public action
    {
    public:
        checked_devices_action();

        void finalize() override;

        /// @brief Инициализация действия.
        ///
        /// Если в устройствах присутствуют счётчики, то для них выполняем
        /// вначале метод `start()` для запуска счёта (после паузы или аварии).
        void init() override;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Мойка линии.
/// </summary>
class wash_action: public action
    {
    public:
        wash_action(): action( "Устройства DI's DO's DEV's R_DEV's AI", G_SUBGROUPS_CNT )
            {
            }

        void evaluate() override;

        virtual void print( const char* prefix = "", bool new_line = true ) const;

        void finalize() override;

    private:
        enum GROUPS
            {
            G_DI = 0,       //Входные сигналы запроса включения устройств.
            G_DO,           //Выходные сигналы "ОК".
            G_DEV,          //Устройства, включаемые по запросу.
            G_REV_DEV,      //Реверсные устройства, включаемые по запросу.
            G_PUMP_FREQ,    //Задания производительности.

            G_SUBGROUPS_CNT,   //Количество групп.
            };
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Проверка устройств на нахождение в активном (включены) и в пассивном
/// (отключены) состоянии.
/// </summary>
class jump_if_action : public action
    {
    public:
        explicit jump_if_action( const char* name );

        bool is_jump( int &next, std::string &reason );

        int set_int_property( const char* prop_name, size_t idx,
            int value ) override;

        int get_int_property( const char* prop_name, size_t idx ) ;

        /// @brief Завершения действия.
        void finalize() override;

        void print( const char* prefix = "",
            bool new_line = true ) const override;

    private:
        bool check( const std::vector< device* > &checked_devices,
            bool check_is_opened ) const;

        enum GROUPS
            {
            G_ON_DEVICES = 0,   //Устройства, которые должны быть включены.
            G_OFF_DEVICES,      //Устройства, которые должны быть отключены.

            G_GROUPS_CNT,       //Количество групп.
            };

        // Устройства.
        std::vector < int > next_n;
    };
//-----------------------------------------------------------------------------
/// <summary>
/// Проверка сигналов для дальнейшего автоматического включения/выключения
/// шага.
/// </summary>
class enable_step_by_signal : public action
    {
    public:
        enable_step_by_signal();

        bool is_any_group_active() const;

        bool should_turn_off() const;

        int set_bool_property( const char* prop_name, bool value ) override;

    private:
        bool turn_off_flag = true;
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об устройствах, которые входят в шаг (открываются/
/// закрываются).
///
/// У операции может быть активным (выполняться) только один шаг.
/// Также дополнительно можно включать 1 либо более шагов параллельно.
class step
    {
    friend class operation_manager;
    public:
        enum ACTIONS
            {
            A_CHECKED_DEVICES = 0,
            A_ON,
            A_ON_REVERSE,
            A_OFF,
            A_UPPER_SEATS_ON,
            A_LOWER_SEATS_ON = A_UPPER_SEATS_ON,

            A_REQUIRED_FB,
            A_DI_DO,
            A_INVERTED_DI_DO,
            A_AI_AO,
            A_WASH,

            A_ENABLE_STEP_BY_SIGNAL,
            A_DELAY_ON,
            A_DELAY_OFF,
            A_JUMP_IF,
            };

        step( std::string name, operation_state *owner, bool is_mode = false );

        ~step();

        /// @brief Получение действия через операцию индексирования.
        ///
        /// @param idx - индекс действия.
        ///
        /// @return - значение действия с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение 0.
        action* operator[] ( int idx );

        /// @brief Проверка возможности выполнения шага.
        ///
        /// @return > 0 - нельзя выполнить.
        /// @return   0 - ок.
        int check( char* reason, unsigned int max_len ) const;

        void init();

        void evaluate() const;

        void finalize();

        /// Получение времени выполнения шага.
        u_long get_eval_time() const;

        /// Получение времени выполнения шага со времени последней паузы/запуска.
        u_long get_latest_eval_time() const;

        /// Установление времени начала шага.
        void set_start_time( u_int_4 start_time );

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;

        /// @brief Проверка на отсутствие устройств.
        ///
        /// @return true  Есть устройства, над которыми что-то делается.
        /// @return false Нет устройств, над которыми что-то делается.
        bool is_empty() const;

        int check_devices( char* err_dev_name, int str_len );

        const char* get_name() const
            {
            return name.c_str();
            }

        bool is_active() const
            {
            return active;
            }
    private:
        std::vector< action* > actions; ///< Действия.
        action action_stub;             ///< Фиктивное действие.
        uint32_t start_time;             ///< Время старта шага.

        bool is_mode;     ///< Выполняется ли все время во время операции.
        std::string name; ///< Имя.

        bool active;
        uint32_t dx_time;                ///< Время шага, отработанное до паузы.

        int tag = -1;   ///< Тег (индекс связанного объекта).

    public:
        /// Добавление времени для возобновления после паузы.
        void set_dx_time( u_int_4 dx_time );

        ///< Получение тега (индекс связанного объекта).
        int get_tag() const
            {
            return tag;
            }

        ///< Установка тега (индекс связанного объекта).
        void set_tag( int new_tag )
            {
            tag = new_tag;
            }
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об операции, состоящей из шагов.
///
/// У объекта (танк, линия, ...) может быть включена только одна операция.
class operation_state
    {
    friend class operation_manager;
    public:
        operation_state( const char* name, operation_manager *owner, int n );

        ~operation_state();

        step* add_step( const char* step_name, int next_step_n = -1,
            int step_duration_par_n = -1, int step_max_duration_par_n = -1 );

        /// @brief Получение операции через операцию индексирования.
        ///
        /// @param idx - индекс операции.
        ///
        /// @return - значение операции с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки - поля @ref
        /// mode::step_stub.
        step* operator[] ( int idx );

        int check_on( char* reason, unsigned int max_len ) const;

        void init( u_int start_step = 1 );

        void evaluate();

        void finalize();

        void reset_eval_time()
            {
            dx_step_time = 0;
            }

        /// @brief Переход к заданному шагу.
        ///
        /// @param new_step - номер шага (с единицы).
        /// @param cooperative_time - время совместной работы (сек).
        void to_step( u_int new_step, uint32_t cooperative_time = 0 );

        /// @brief Переход к следующему шагу.
        void to_next_step();

        /// @brief Отключить текущий активный шаг.
        void turn_off_active_step();

        u_long evaluation_time();

        u_long active_step_evaluation_time() const;
        u_long get_active_step_set_time() const;

        u_int active_step() const;
        u_int steps_count() const;

        /// Выводит на консоль объект.
        void print( const char* prefix = "" ) const;

        const char* get_name() const;

        int check_devices( char* err_dev_name, unsigned int str_len );

        int check_steps_params( char* err_dev_name, unsigned int str_len );

        int check_max_step_time( char* err_dev_name, unsigned int str_len );

        /// @brief Проверка на отсутствие устройств.
        ///
        /// @return true  Есть устройства, над которыми что-то делается.
        /// @return false Нет устройств, над которыми что-то делается.
        bool is_empty() const;

        bool is_goto_next_state( int& next_state, std::string& reason ) const;

        void set_step_cooperate_time_par_n( int step_cooperate_time_par_number );
    private:
        std::string name;
        std::vector< step* > steps;

        step* mode_step;

        int active_step_n;           ///< Активный шаг.
        int active_step_time;        ///< Время активного шага.
        int active_step_max_time;    ///< Максимальное время активного шага.
        int active_step_next_step_n; ///< Следующий шаг.

        /// @brief Номера параметров времен шагов.
        std::vector< int > step_duration_par_ns;

        /// @brief Номера параметров максимального времени шагов.
        std::vector< int > step_max_duration_par_ns;

        /// @brief Следующие шаги.
        std::vector< int > next_step_ns;

        uint32_t start_time; ///< Время начала операции.
        step step_stub;     ///< Шаг-заглушка.

        operation_manager *owner;
        int operation_number;   /// Номер операции.

        /// Время выполнения активного шага, для возобновления после паузы.
        uint32_t dx_step_time;


        /// Номер параметра совместного времени выполнения шагов.
        int step_cooperate_time_par_n{ -1 };

    public:
        /// Добавление времени выполнения активного шага при возобновлении
        /// после паузы.
        void add_dx_step_time();

#ifndef __GNUC__
#pragma region Поддержка более чем одного активного шага.
#endif

        void save();
        void load();

        int on_extra_step( int step_idx );
        int on_extra_step( int step_idx, uint32_t step_time, bool is_print_time );

        int off_extra_step( int step_idx );

        /// @brief Выключение заданного активного шага и включение другого.
        ///
        /// @param off_step - номер выключаемого шага (с единицы).
        /// @param on_step - номер включаемого шага (с единицы).
        int switch_active_extra_step( int off_step, int on_step );

        /// @brief Определение активности заданного шага.
        ///
        /// @param step_idx - номер выключаемого шага (с единицы).
        bool is_active_extra_step( int step_idx ) const;

    private:
        /// Активные шаги. Может быть 1 или более дополнительных активных шагов.
        std::vector< int > active_steps;
        std::vector< uint32_t > active_steps_duration;
        std::vector< uint32_t > active_steps_start_time;

        std::vector< int > saved_active_steps;

#ifndef __GNUC__
#pragma endregion
#endif

    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об операции.
class operation
    {
    friend class operation_manager;
    public:
        operation( const char* name, operation_manager *owner, int n );

        ~operation();

#ifndef __GNUC__
#pragma region Совместимость со старой версией.
#endif
        int check_devices_on_run_state( char* err_dev_name, unsigned int str_len );

        int check_on_run_state( char* reason, unsigned int max_len ) const;

        u_long evaluation_time();

        void evaluate();

        void finalize();

        int check_steps_params( char* err_dev_name, unsigned int str_len );

        int check_max_step_time( char* err_dev_name, unsigned int str_len );

        u_int active_step() const;
        int get_run_step() const;

        u_int get_run_steps_count() const;
        u_int get_run_active_step() const;

        const char* get_name() const;

        void print( const char* prefix /*= "" */ ) const;

        u_long active_step_evaluation_time() const;
        u_long get_active_step_set_time() const;

        /// @brief Переход к заданному  шагу.
        ///
        /// @param new_step - номер шага (с единицы).
        /// @param cooperative_time - время совместной работы (сек).
        void to_step( unsigned int new_step, uint32_t cooperative_time = 0 );

        /// @brief Переход к следующему шагу.
        ///
        /// Данный метод упрощает реализацию, когда необходимо использовать
        /// функцию to_step, передавая в качестве параметра номер текущего шага,
        /// увеличенного на единицу.
        void to_next_step();

        /// @brief Отключение текущего активного шага.
        void turn_off_active_step();
#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region Новая функциональность по состояниям операции.
#endif

        enum state_idx
            {
            IDLE = 0,// Ожидание.
            RUN,     // Выполнение.
            PAUSE,   // Пауза.
            STOP,    // Остановлен.

            STARTING = 10,  //Запускается.
            PAUSING,        //Становится в паузу.
            UNPAUSING,      //Выходит из паузы.
            STOPPING,       //Останавливается.
            COMPLETING,     //Завершается.

			STATES_MAX,
            };

        static constexpr const std::array <const char* const, STATES_MAX> state_str =
            {
            "Отключен",
            "Выполнение",
            "Пауза",
            "Остановлен",
            "",
            "",
            "",
            "",
            "",
            "",
            "Запускается",
            "Становится в паузу",
            "Выходит из паузы",
            "Останавливается",
            "Завершается",
            };

        static constexpr const std::array <const char* const, STATES_MAX> en_state_str =
            {
            "OFF",
            "RUN",
            "PAUSE",
            "STOP",
            "",
            "",
            "",
            "",
            "",
            "",
            "STARTING",
            "PAUSING",
            "UNPAUSING",
            "STOPPING",
            "COMPLETING"
            };

        state_idx get_state() const;

        int pause();

        int stop();

        int start();

        int switch_off();

        int start( int new_run_step );

        void to_run_state( int new_run_step );

#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region Загрузка описания.
#endif
        /// @brief Получение операции через индексирование.
        ///
        /// @param idx - индекс операции.
        operation_state* operator[] ( int idx )
            {
            if ( idx < static_cast<int>( state_idx::STATES_MAX ) )
                {
                return states[ idx ];
                }
            else
                {
                if ( G_DEBUG )
                    {
                    printf( "Error operation_state* operation::operator[] "
                        "( int idx ) - idx %d > count %d.\n",
                        idx, state_idx::STATES_MAX );
                    }
                return &stub;
                }
            }

        step* add_step( const char* step_name, int next_step_n = -1,
            int step_duration_par_n = -1, int step_max_duration_par_n = -1,
            state_idx s_idx = state_idx::RUN );

#ifndef __GNUC__
#pragma endregion
#endif

#ifndef __GNUC__
#pragma region Поддержка более чем одного активного шага.
#endif

        int on_extra_step( int step_idx );

        int off_extra_step( int step_idx );

        /// @brief Выключение заданного активного шага и включение другого.
        ///
        /// @param off_step - номер выключаемого шага (с единицы).
        /// @param on_step - номер включаемого шага (с единицы).
        int switch_active_extra_step( int off_step, int on_step );

        /// @brief Определение активности заданного шага для выполнения
        /// операции.
        ///
        /// @param step_idx - номер выключаемого шага (с единицы).
        bool is_active_run_extra_step( int step_idx ) const;

        /// @brief Определение активности заданного шага.
        ///
        /// @param step_idx - номер выключаемого шага (с единицы).
        bool is_active_extra_step( int step_idx ) const;

#ifndef __GNUC__
#pragma endregion
#endif


        /// @brief Установка номера параметра со временем переходного
        /// переключения шагов.
        void set_step_cooperate_time_par_n( int step_cooperate_time_par_n );

    private:
        int process_auto_switch_on( const std::string& reason );

        int process_new_state_from_run( int next_state,
            const std::string& reason );

        /// @brief Обработка перехода по умолчанию - переход к остановке
        /// или к следующему состоянию.
        ///
        /// @param next_state - новое состояние.
        /// @param def_state - новое состояние по умолчанию.
        int default_process_new_state( state_idx next_state, state_idx def_state );

        state_idx current_state = IDLE;

        std::vector< operation_state* > states;

        std::string name;
        operation_manager *owner;
        int operation_num;                                /// Номер операции у владельца.

        operation_state stub;

        /// Шаг для состояния Выполнение. Нужен для запуска после паузы,
        /// остановки и т.д.
        int run_step = -1;

        u_int run_time = 0;  /// Время выполнения операции (состояние run).

        uint32_t start_warn = 0;
        uint32_t start_wait = 0;
        bool is_first_goto_next_state = true;
        bool was_fail = false;
    };
//-----------------------------------------------------------------------------
/// @brief Содержит информацию об операциях какого-либо объекта (танк,
/// линия, ...).
///
/// У объекта (танк, ...) может быть включена одна операция.
///
class operation_manager
    {
    public:
        // A reload must not destroy objects still referenced by Lua code.
        bool has_lua_references( lua_State* L ) const;
        size_t size() const { return operations.size(); }
        /// @brief Конструктор с параметрами.
        ///
        /// @param i_tech_object - техобъект-владелец.
        explicit operation_manager( i_tech_object *owner );

        ~operation_manager();

        //NOTE Поддержка старой версии.
        operation* add_mode( const char* name )
            {
            return add_operation( name );
            }

        operation* add_operation( const char* name );

        /// @brief Получение операции через индексирование.
        ///
        /// @param idx - индекс операции.
        ///
        /// @return - значение операции с заданным индексом. Если индекс
        /// выходит за диапазон, возвращается значение заглушки.
        operation* operator[] ( unsigned int idx );

        /// @brief Время бездействия (нет включенных операций) в секундах.
        ///
        /// @return - время системы без активных операций.
        unsigned long get_idle_time_sec() const;

        /// @brief Время бездействия (нет включенных операций) в миллисекундах.
        ///
        /// @return - время системы без активных операций.
        unsigned long long get_idle_time() const;

        void reset_active_operation_or_idle_time()
            {
            start_time_point = std::chrono::steady_clock::now();
            }

        /// @brief Отладочный вывод объекта в консоль.
        void print();

        void off_mode( int mode ) const
            {
            owner->set_mode( mode, 0 );
            }

        float get_step_param( u_int idx ) const
            {
            return owner->get_step_param( idx );
            }

        const saved_params_float* get_params() const
            {
            return owner->get_params();
            }

        i_tech_object *owner;              ///Техобъект-владелец.

    private:
        std::vector< operation* > operations; ///< Операции.

        ///< Операция-заглушка.
        operation oper_stub{ "Операция-заглушка", this, -1 };

        /// @brief Время активной операции (или бездействия).
        std::chrono::steady_clock::time_point start_time_point =
            std::chrono::steady_clock::now();
    };
//-----------------------------------------------------------------------------
#endif // MODE_MNGR
