TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        menu.cpp \
        migration.cpp

HEADERS += \
    menu.h \
    migration.h

DISTFILES += \
    .gitignore \
    README.md
