#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <QString>
#include <QObject>
#include <QEventLoop>

class IriTracker : public QObject
{
    Q_OBJECT 

signals:
    void imageProcessed(unsigned char* imageData,
        int imageLen,
        int imageWidth,
        int imageHeight);   
    
    void imageProcessedForInOut(unsigned char* imageData,
        int imageLen,
        int imageWidth,
        int imageHeight);

    void imageResult(unsigned char* imageData,
        int imageLen,
        int imageWidth,
        int imageHeight);

    void resultTemplate(unsigned char* data, int size);
    void resultTemplateForInOut();

    void onOpenDevice(bool isDevice);

public:
    IriTracker();
    void run(bool bDefaultParams = false, bool bMultiple = true, bool bProcessResult = true);
    void get_device();
    QPair<QString, bool> compare_templates_custom(bool checkDup = false);
    void changedScreen(int index);
    QPair<QString, bool> checkTemplates();

    bool isCancel = false;

    bool isDisconnectInOut = false;
    
private:
    int currentScreenIndex = 5;
};
