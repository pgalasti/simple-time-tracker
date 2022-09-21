#-------------------------------------------------
#
# Project created by QtCreator 2016-07-12T11:02:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTimeTracker
TEMPLATE = app


SOURCES += main.cpp \
        taskmodel.cpp \
        mainwindow.cpp \
    timetask.cpp \
    configurationdialog.cpp \
    about.cpp


HEADERS  += taskmodel.h \
    mainwindow.h \
    timetask.h \
    taskdate.h \
    configurationdialog.h \
    about.h


FORMS    += mainwindow.ui \
    timetask.ui \
    configurationdialog.ui \
    about.ui

DISTFILES +=

RESOURCES += \
    resources.qrc

RC_ICONS = resources/hourglass_icon.ico
