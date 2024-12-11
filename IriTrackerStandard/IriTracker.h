#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <QString>
#include <QObject>

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
    bool compare_templates_custom(int dataSize, unsigned char* data);
    void changedScreen(int index);
    
private:
    int currentScreenIndex = 5;
};
