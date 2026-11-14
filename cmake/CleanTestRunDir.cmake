# Очищает и заново создает рабочую директорию теста.
# Вызывается как: cmake -Ddir=<путь> -P CleanTestRunDir.cmake
if (NOT DEFINED dir)
    message(FATAL_ERROR "Переменная dir не задана")
endif()

file(REMOVE_RECURSE "${dir}")
file(MAKE_DIRECTORY "${dir}")
