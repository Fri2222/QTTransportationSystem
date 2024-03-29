QT       += core gui sql webchannel network  webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    accountmangement.cpp \
    basicinformation.cpp \
    cargoinformation.cpp \
    driver.cpp \
    enroll.cpp \
    login_window.cpp \
    main.cpp \
    main_interface.cpp \
    maphtml.cpp \
    neworder.cpp \
    orderandaccount.cpp \
    orderconfirmation.cpp \
    truck.cpp \
    yardinformation.cpp

HEADERS += \
    User.h \
    accountmangement.h \
    basicinformation.h \
    cargoinformation.h \
    driver.h \
    enroll.h \
    login_window.h \
    main_interface.h \
    maphtml.h \
    neworder.h \
    orderandaccount.h \
    orderconfirmation.h \
    truck.h \
    yardinformation.h

FORMS += \
    accountmangement.ui \
    basicinformation.ui \
    cargoinformation.ui \
    driver.ui \
    enroll.ui \
    login_window.ui \
    main_interface.ui \
    maphtml.ui \
    neworder.ui \
    orderandaccount.ui \
    orderconfirmation.ui \
    truck.ui \
    yardinformation.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resourrces.qrc
