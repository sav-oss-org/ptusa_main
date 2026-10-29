# Прямой доступ к локальной шине используется отдельным процессом ptusa_main.
# Компонент PLCnext Engineer продолжает использовать свои настройки ввода-вывода.
if(ARP_DEVICE MATCHES "^AXCF(2152|3152)$")
    option(PTUSA_WITH_AXIOBUS "Поддержка локальной шины Axiobus" ON)
else()
    option(PTUSA_WITH_AXIOBUS "Поддержка локальной шины Axiobus" OFF)
endif()

# Оба транспорта входят в одну сборку; выбор задаёт ntype в main.io.lua.
unset(PTUSA_AXIOBUS_BACKEND CACHE)

if(PTUSA_WITH_AXIOBUS)
    if(NOT LINUX OR NOT ARP_DEVICE MATCHES "^AXCF(2152|3152)$")
        message(FATAL_ERROR "PTUSA_WITH_AXIOBUS требует SDK AXCF2152 или AXCF3152")
    endif()
    message(STATUS "Axiobus: LIBRARY и IOCTL, выбор по типу узла")
    if(ARP_DEVICE STREQUAL "AXCF2152")
        set(axiobus_arch arm)
    else()
        set(axiobus_arch x64)
    endif()
    set(axiobus_root "${CMAKE_CURRENT_SOURCE_DIR}/plcnext-io-drivers-cpp")
    set(axiobus_library "${axiobus_root}/lib/shared/${axiobus_arch}/libAxiobus.so.11.2")
    if(NOT EXISTS "${axiobus_library}")
        message(FATAL_ERROR "Инициализируйте сабмодуль plcnext-io-drivers-cpp")
    endif()
    add_library(axiobus SHARED IMPORTED)
    set_target_properties(axiobus PROPERTIES
        IMPORTED_LOCATION "${axiobus_library}"
        INTERFACE_INCLUDE_DIRECTORIES "${axiobus_root}/include")
    target_compile_definitions(ptusa_main PRIVATE PTUSA_AXIOBUS PTUSA_AXIO_IOCTL)
    target_link_libraries(ptusa_main PRIVATE axiobus)
    set_target_properties(ptusa_main PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE INSTALL_RPATH "$ORIGIN")
    add_custom_command(TARGET ptusa_main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${axiobus_library}"
            "$<TARGET_FILE_DIR:ptusa_main>/libAxiobus.so.11")
    string(REGEX REPLACE "^.*\\(([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+).*$"
        "\\1" axiobus_device_version "${ARP_DEVICE_VERSION}")
    install(FILES "${axiobus_library}"
        DESTINATION "${ARP_DEVICE}_${axiobus_device_version}/$<CONFIG>/bin"
        RENAME libAxiobus.so.11)
endif()
