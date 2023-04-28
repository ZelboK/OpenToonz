
add_library(Qt5::QMinimalIntegrationPlugin MODULE IMPORTED)

set(_Qt5QMinimalIntegrationPlugin_MODULE_DEPENDENCIES "EventDispatcherSupport;FontDatabaseSupport;Gui;Gui;Core;Core")

foreach(_module_dep ${_Qt5QMinimalIntegrationPlugin_MODULE_DEPENDENCIES})
    if(NOT Qt5${_module_dep}_FOUND)
        find_package(Qt5${_module_dep}
            1.0.0 ${_Qt5Gui_FIND_VERSION_EXACT}
            ${_Qt5Gui_DEPENDENCIES_FIND_QUIET}
            ${_Qt5Gui_FIND_DEPENDENCIES_REQUIRED}
            PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
        )
    endif()
endforeach()

if(EXISTS "${_qt5Gui_install_prefix}/plugins/platforms/libqminimal.prl")
    _qt5_Gui_process_prl_file(
        "${_qt5Gui_install_prefix}/plugins/platforms/libqminimal.prl" RELEASE
        _Qt5QMinimalIntegrationPlugin_STATIC_RELEASE_LIB_DEPENDENCIES
        _Qt5QMinimalIntegrationPlugin_STATIC_RELEASE_LINK_FLAGS
    )
endif()

if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/platforms/libqminimal_debug.prl")
    _qt5_Gui_process_prl_file(
        "${_qt5Gui_install_prefix}/debug/plugins/platforms/libqminimal_debug.prl" DEBUG
        _Qt5QMinimalIntegrationPlugin_STATIC_DEBUG_LIB_DEPENDENCIES
        _Qt5QMinimalIntegrationPlugin_STATIC_DEBUG_LINK_FLAGS
    )
endif()

set_property(TARGET Qt5::QMinimalIntegrationPlugin PROPERTY INTERFACE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Qt5Gui_QMinimalIntegrationPlugin_Import.cpp"
)

if(EXISTS "${_qt5Gui_install_prefix}/plugins/platforms/libqminimal.a")
    _populate_Gui_plugin_properties(QMinimalIntegrationPlugin RELEASE "platforms/libqminimal.a" FALSE)
endif()
if(EXISTS "${_qt5Gui_install_prefix}/debug/plugins/platforms/libqminimal_debug.a")
    _populate_Gui_plugin_properties(QMinimalIntegrationPlugin DEBUG "platforms/libqminimal_debug.a" FALSE)
endif()

list(APPEND Qt5Gui_PLUGINS Qt5::QMinimalIntegrationPlugin)
set_property(TARGET Qt5::Gui APPEND PROPERTY QT_ALL_PLUGINS_platforms Qt5::QMinimalIntegrationPlugin)
# $<GENEX_EVAL:...> wasn't added until CMake 3.12, so put a version guard around it
if(CMAKE_VERSION VERSION_LESS "3.12")
    set(_manual_plugins_genex "$<TARGET_PROPERTY:QT_PLUGINS>")
    set(_plugin_type_genex "$<TARGET_PROPERTY:QT_PLUGINS_platforms>")
    set(_no_plugins_genex "$<TARGET_PROPERTY:QT_NO_PLUGINS>")
else()
    set(_manual_plugins_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_PLUGINS>>")
    set(_plugin_type_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_PLUGINS_platforms>>")
    set(_no_plugins_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_NO_PLUGINS>>")
endif()
set(_user_specified_genex
    "$<IN_LIST:Qt5::QMinimalIntegrationPlugin,${_manual_plugins_genex};${_plugin_type_genex}>"
)
set(_user_specified_genex_versionless
    "$<IN_LIST:Qt::QMinimalIntegrationPlugin,${_manual_plugins_genex};${_plugin_type_genex}>"
)
string(CONCAT _plugin_genex
    "$<$<OR:"
        # Add this plugin if it's in the list of manually specified plugins or in the list of
        # explicitly included plugin types.
        "${_user_specified_genex},"
        "${_user_specified_genex_versionless},"
        # Add this plugin if all of the following are true:
        # 1) the list of explicitly included plugin types is empty
        # 2) the QT_PLUGIN_EXTENDS property for the plugin is empty or equal to one of the modules
        # 3) the user hasn't explicitly excluded the plugin.
        # TODO: Note that the current implementation of (2) is not entirely correct QTBUG-93501
        "$<AND:"
            "$<STREQUAL:${_plugin_type_genex},>,"
            "$<OR:"
                
                "$<STREQUAL:$<TARGET_PROPERTY:Qt5::QMinimalIntegrationPlugin,QT_PLUGIN_EXTENDS>,>"
            ">,"
            "$<NOT:$<IN_LIST:Qt5::QMinimalIntegrationPlugin,${_no_plugins_genex}>>,"
            "$<NOT:$<IN_LIST:Qt::QMinimalIntegrationPlugin,${_no_plugins_genex}>>"
        ">"
    ">:Qt5::QMinimalIntegrationPlugin>"
)
set_property(TARGET Qt5::Gui APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    ${_plugin_genex}
)
set_property(TARGET Qt5::QMinimalIntegrationPlugin APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    "Qt5::EventDispatcherSupport;Qt5::FontDatabaseSupport;Qt5::Gui;Qt5::Gui;Qt5::Core;Qt5::Core"
)
set_property(TARGET Qt5::QMinimalIntegrationPlugin PROPERTY QT_PLUGIN_TYPE "platforms")
set_property(TARGET Qt5::QMinimalIntegrationPlugin PROPERTY QT_PLUGIN_EXTENDS "-")
set_property(TARGET Qt5::QMinimalIntegrationPlugin PROPERTY QT_PLUGIN_CLASS_NAME "QMinimalIntegrationPlugin")
