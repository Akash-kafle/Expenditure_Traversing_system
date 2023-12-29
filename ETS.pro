QT       += core gui winextras sql charts axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17

VERSION = 1.0.0.0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
INCLUDEPATH += C:\Users\aakas\OneDrive\Desktop\folders\programming\C++,C\Uni_project\ETS\header

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#win32: RC_FILE +=

SOURCES += \
    src/forgotpassword.cpp \
    src/linegraph.cpp \
    src/main.cpp \
    src/login.cpp \
    src/mainwindow.cpp \
    src/qcustomplot.cpp \
    src/register.cpp

HEADERS += \
    header/forgotpassword.h \
    header/linegraph.h \
    header/login.h \
    header/mainwindow.h \
    header/qcustomplot.h \
    header/register.h

FORMS += \
    ui/forgotpassword.ui \
    ui/linegraph.ui \
    ui/login.ui \
    ui/mainwindow.ui \
    ui/register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
