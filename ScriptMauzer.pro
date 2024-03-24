#sudo apt install qt5-qmake qtbase5-dev qt5-qmake qtbase5-dev-tools qttools5-dev qtscript5-dev libqt5svg5-dev libqt5xmlpatterns5-dev

#  ./linuxdeploy/linuxdeployqt-continuous-x86_64.AppImage release/linux/ScriptMauzer -always-overwrite -appimage -unsupported-allow-new-glibc -no-strip

TEMPLATE = app
TARGET = ScriptMauzer
QT += script xmlpatterns core xml svg

CONFIG   += qt console
QTPLUGIN += qjpeg qgif qmng qsvg qtiff qico qtga qwebp qwbmp

SOURCES += main.cpp
#INCLUDEPATH += ./quazip/quazip

ICON = icons/Robot.ico
HEADERS += Classes.h \
           QMyDomDocument.h
win32{
    RC_FILE += icons/win32Script.rc
    CONFIG(debug, debug|release) {
        DESTDIR = debug/win
        LIBS += -lole32 -loleaut32 -luuid -lStrmiids -lQuartz -lVersion
    } else {
       DESTDIR = release/win
       LIBS += -lole32 -loleaut32 -luuid -lStrmiids -lQuartz -lVersion
    }    
}
unix{
    CONFIG(debug, debug|release){
       DESTDIR = debug/linux
    } else {
       DESTDIR = release/linux
    }
}



