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

    void imageResult(unsigned char* imageData,
        int imageLen,
        int imageWidth,
        int imageHeight);

    void resultTemplate(QString pathTemplate);

public:
    IriTracker();
    void run();
    static void get_divice();
    
};
