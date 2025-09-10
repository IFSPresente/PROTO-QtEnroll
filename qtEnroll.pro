QT += core gui widgets sql

CONFIG += c++17
CONFIG += moc
CONFIG += link_pkgconfig

PKGCONFIG += glib-2.0 gio-2.0

SOURCES += \
   main.cpp \
   mainwindow.cpp \
   passworddialog.cpp \
   utilities.c \
   storage.c \
   bioscanner.c

HEADERS += \
   mainwindow.h \
   passworddialog.h \
   utilities.h \
   storage.h
   bioscanner.h
   
LIBS += -lfprint-2
   

FORMS += \
