QT.webengine.VERSION = 5.5.0
QT.webengine.MAJOR_VERSION = 5
QT.webengine.MINOR_VERSION = 5
QT.webengine.PATCH_VERSION = 0
QT.webengine.name = QtWebEngine
QT.webengine.libs = $$QT_MODULE_LIB_BASE
QT.webengine.rpath = /home/yaozhongkai/Qt5.5.0/5.5/gcc/lib
QT.webengine.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtWebEngine
QT.webengine.bins = $$QT_MODULE_BIN_BASE
QT.webengine.libexecs = $$QT_MODULE_LIBEXEC_BASE
QT.webengine.plugins = $$QT_MODULE_PLUGIN_BASE
QT.webengine.imports = $$QT_MODULE_IMPORT_BASE
QT.webengine.qml = $$QT_MODULE_QML_BASE
QT.webengine.depends = core gui qml quick
QT.webengine.run_depends = webenginecore quick_private gui_private core_private
QT.webengine.module_config =
QT.webengine.DEFINES = QT_WEBENGINE_LIB
QT_MODULES += webengine
