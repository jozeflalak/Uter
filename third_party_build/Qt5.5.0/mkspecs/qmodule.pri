CONFIG +=  compile_examples qpa largefile precompile_header enable_new_dtags sse2 sse3 ssse3 sse4_1 sse4_2 avx avx2 pcre
QT_BUILD_PARTS += libs tools
QT_NO_DEFINES =  EGL EGLFS EGL_X11 IMAGEFORMAT_JPEG LIBPROXY OPENVG PULSEAUDIO TABLET TSLIB XINPUT
QT_QCONFIG_PATH = 
host_build {
    QT_CPU_FEATURES.i386 = 
} else {
    QT_CPU_FEATURES.i386 = 
}
QT_COORD_TYPE = double
QT_CFLAGS_MYSQL   = -I/usr/include/mysql
QT_LFLAGS_MYSQL   = -rdynamic -L/usr/lib/mysql -lmysqlclient_r -lz -lpthread -lcrypt -lnsl -lm -lpthread -lssl -lcrypto
QT_LFLAGS_ODBC   = -lodbc
QMAKE_CFLAGS = -m32
QMAKE_CXXFLAGS = -m32
QMAKE_LFLAGS = -m32 -L/usr/lib
styles += mac fusion windows
DEFINES += QT_NO_MTDEV
CONFIG += use_libmysqlclient_r
QT_CFLAGS_GLIB = -pthread -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
QT_LIBS_GLIB = -lgthread-2.0 -pthread -lrt -lglib-2.0
QT_CFLAGS_QGTKSTYLE = -pthread -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/atk-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/libpng12
QT_LIBS_QGTKSTYLE = -lgobject-2.0 -lgthread-2.0 -pthread -lrt -lglib-2.0
QT_CFLAGS_QGTK2 = -pthread -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/pango-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/atk-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/libpng12
QT_LIBS_QGTK2 = -lgtk-x11-2.0 -lgdk-x11-2.0 -lpangocairo-1.0 -latk-1.0 -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lpangoft2-1.0 -lpango-1.0 -lgobject-2.0 -lgthread-2.0 -pthread -lrt -lgmodule-2.0 -pthread -lrt -lglib-2.0 -lfreetype -lfontconfig
QMAKE_INCDIR_OPENGL =  "/usr/include/libdrm"
QMAKE_LIBDIR_OPENGL = 
QMAKE_LIBS_OPENGL =  "-lGL"
QMAKE_CFLAGS_OPENGL = 
QMAKE_CFLAGS_FONTCONFIG = -I/usr/include/freetype2
QMAKE_LIBS_FONTCONFIG = -lfontconfig -lfreetype
DEFINES += QT_NO_LIBUDEV
DEFINES += QT_NO_TSLIB
DEFINES += QT_NO_LIBINPUT
QMAKE_LIBXI_VERSION_MAJOR = 1
QMAKE_LIBXI_VERSION_MINOR = 7
QMAKE_LIBXI_VERSION_PATCH = 2
QMAKE_X11_PREFIX = /usr
QMAKE_XKB_CONFIG_ROOT = /usr/share/X11/xkb
QMAKE_CFLAGS_XCB = 
QMAKE_LIBS_XCB = -lxcb
INCLUDEPATH += 
LIBS +=
QMAKE_RPATHDIR +=  "/home/yaozhongkai/Qt5.5.0/5.5/gcc"
sql-drivers = 
sql-plugins =  mysql psql sqlite
