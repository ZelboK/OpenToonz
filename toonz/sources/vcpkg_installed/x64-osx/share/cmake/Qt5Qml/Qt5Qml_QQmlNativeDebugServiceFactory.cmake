
add_library(Qt5::QQmlNativeDebugServiceFactory MODULE IMPORTED)

set(_Qt5QQmlNativeDebugServiceFactory_MODULE_DEPENDENCIES "Qml;PacketProtocol;Core")

foreach(_module_dep ${_Qt5QQmlNativeDebugServiceFactory_MODULE_DEPENDENCIES})
    if(NOT Qt5${_module_dep}_FOUND)
        find_package(Qt5${_module_dep}
            1.0.0 ${_Qt5Qml_FIND_VERSION_EXACT}
            ${_Qt5Qml_DEPENDENCIES_FIND_QUIET}
            ${_Qt5Qml_FIND_DEPENDENCIES_REQUIRED}
            PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
        )
    endif()
endforeach()

if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/libqmldbg_nativedebugger.prl")
    _qt5_Qml_process_prl_file(
        "${_qt5Qml_install_prefix}/plugins/qmltooling/libqmldbg_nativedebugger.prl" RELEASE
        _Qt5QQmlNativeDebugServiceFactory_STATIC_RELEASE_LIB_DEPENDENCIES
        _Qt5QQmlNativeDebugServiceFactory_STATIC_RELEASE_LINK_FLAGS
    )
endif()

if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/libqmldbg_nativedebugger_debug.prl")
    _qt5_Qml_process_prl_file(
        "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/libqmldbg_nativedebugger_debug.prl" DEBUG
        _Qt5QQmlNativeDebugServiceFactory_STATIC_DEBUG_LIB_DEPENDENCIES
        _Qt5QQmlNativeDebugServiceFactory_STATIC_DEBUG_LINK_FLAGS
    )
endif()

set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY INTERFACE_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Qt5Qml_QQmlNativeDebugServiceFactory_Import.cpp"
)

if(EXISTS "${_qt5Qml_install_prefix}/plugins/qmltooling/libqmldbg_nativedebugger.a")
    _populate_Qml_plugin_properties(QQmlNativeDebugServiceFactory RELEASE "qmltooling/libqmldbg_nativedebugger.a" FALSE)
endif()
if(EXISTS "${_qt5Qml_install_prefix}/debug/plugins/qmltooling/libqmldbg_nativedebugger_debug.a")
    _populate_Qml_plugin_properties(QQmlNativeDebugServiceFactory DEBUG "qmltooling/libqmldbg_nativedebugger_debug.a" FALSE)
endif()

list(APPEND Qt5Qml_PLUGINS Qt5::QQmlNativeDebugServiceFactory)
set_property(TARGET Qt5::Qml APPEND PROPERTY QT_ALL_PLUGINS_qmltooling Qt5::QQmlNativeDebugServiceFactory)
# $<GENEX_EVAL:...> wasn't added until CMake 3.12, so put a version guard around it
if(CMAKE_VERSION VERSION_LESS "3.12")
    set(_manual_plugins_genex "$<TARGET_PROPERTY:QT_PLUGINS>")
    set(_plugin_type_genex "$<TARGET_PROPERTY:QT_PLUGINS_qmltooling>")
    set(_no_plugins_genex "$<TARGET_PROPERTY:QT_NO_PLUGINS>")
else()
    set(_manual_plugins_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_PLUGINS>>")
    set(_plugin_type_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_PLUGINS_qmltooling>>")
    set(_no_plugins_genex "$<GENEX_EVAL:$<TARGET_PROPERTY:QT_NO_PLUGINS>>")
endif()
set(_user_specified_genex
    "$<IN_LIST:Qt5::QQmlNativeDebugServiceFactory,${_manual_plugins_genex};${_plugin_type_genex}>"
)
set(_user_specified_genex_versionless
    "$<IN_LIST:Qt::QQmlNativeDebugServiceFactory,${_manual_plugins_genex};${_plugin_type_genex}>"
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
                
                "$<STREQUAL:$<TARGET_PROPERTY:Qt5::QQmlNativeDebugServiceFactory,QT_PLUGIN_EXTENDS>,>"
            ">,"
            "$<NOT:$<IN_LIST:Qt5::QQmlNativeDebugServiceFactory,${_no_plugins_genex}>>,"
            "$<NOT:$<IN_LIST:Qt::QQmlNativeDebugServiceFactory,${_no_plugins_genex}>>"
        ">"
    ">:Qt5::QQmlNativeDebugServiceFactory>"
)
set_property(TARGET Qt5::Qml APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    ${_plugin_genex}
)
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    "Qt5::Qml;Qt5::PacketProtocol;Qt5::Core"
)
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY QT_PLUGIN_TYPE "qmltooling")
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY QT_PLUGIN_EXTENDS "")
set_property(TARGET Qt5::QQmlNativeDebugServiceFactory PROPERTY QT_PLUGIN_CLASS_NAME "QQmlNativeDebugServiceFactory")
