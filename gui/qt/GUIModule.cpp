#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"
#include <QGraphicsObject>
#include "GUIModule.h"


const char* GUIModule::myName="GUI";
void GUIModule::initialize(uint32& flag){
    int args=0;
    QScopedPointer<QApplication> app(createApplication(args,NULL));

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/gui/main.qml"));
    viewer.showFullScreen();
    QObject* vlbl = viewer.rootObject()->findChild<QObject*>("voltageLabel");
    if(vlbl)
        vlbl->setProperty("text",QVariant("Voltage:ihateyou"));
    app->exec();
}
