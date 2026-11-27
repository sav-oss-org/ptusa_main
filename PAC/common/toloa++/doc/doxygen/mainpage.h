///@mainpage
///
///@section main Brief description
///@if rus_dox
/// Управляющая логика реализуется в виде скрипта Lua. В ходе выполнения программы
/// управление передается в отдельные специальные функции Lua, которые отвечают
/// за отдельные этапы.
///@endif
///
///@if eng_dox
/// Control logic is written as Lua script. During program execution special Lua
/// functions realize user logic.
///@endif
///
///
///@section intro Lua and main program
///@if rus_dox
/// Общую схему работы управляющей программы смотри на рис. 1.<br>
/// @image html main.jpg "Рис.1 Работа управляющей программы"
///
/// Управляющий цикл выглядит так (рис. 2):<br>
/// @image html control_cycle.jpg "Рис.2 Управляющий цикл"
///
/// При включении/выключении режима происходит следующее (рис. 3):<br>
/// @image html tech_object__set_mode.jpg "Рис.3 Включение режима"
///
/// При выполнении команды происходит следующее (рис. 4):<br>
/// @image html tech_object__exec_cmd.jpg "Рис.4 Выполнение команды"
///@endif
