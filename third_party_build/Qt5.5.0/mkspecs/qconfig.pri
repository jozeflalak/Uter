#configuration
CONFIG +=  shared qpa no_mocdepend release qt_no_framework
host_build {
    QT_ARCH = i386
    QT_TARGET_ARCH = i386
} else {
    QT_ARCH = i386
    QMAKE_DEFAULT_LIBDIRS  = 
    QMAKE_DEFAULT_INCDIRS  = 
}
QT_CONFIG +=  minimal-config small-config medium-config large-config full-config build_all debug_and_release gtk2 gtkstyle fontconfig evdev xlib xrender xcb-plugin xcb-qt xcb-xlib xcb-sm xkbcommon-qt accessibility-atspi-bridge linuxfb c++11 accessibility opengl shared qpa reduce_exports reduce_relocations clock-gettime clock-monotonic posix_fallocate mremap getaddrinfo ipv6ifname getifaddrs inotify eventfd png system-freetype harfbuzz zlib nis cups iconv glib dbus openssl xcb xinput2 rpath alsa icu concurrent audio-backend debug release

#versioning
QT_VERSION = 5.5.0
QT_MAJOR_VERSION = 5
QT_MINOR_VERSION = 5
QT_PATCH_VERSION = 0

#namespaces
QT_LIBINFIX = 
QT_NAMESPACE = 

QT_EDITION = OpenSource
QT_LICHECK = licheck32
QT_RELEASE_DATE = 2015-06-26

QMAKE_RPATHDIR +=  "/home/yaozhongkai/Qt5.5.0/5.5/gcc"
QT_GCC_MAJOR_VERSION = 4
QT_GCC_MINOR_VERSION = 9
QT_GCC_PATCH_VERSION = 2
