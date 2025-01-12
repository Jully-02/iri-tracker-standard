#pragma once

#include <QString>
#include <QCryptographicHash>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <cstring> 
#include <utility>

class FunctionPublic
{
public:
    static QString hashPassword(const QString& password);
    static QByteArray templateConvertToByte(const unsigned char* buffer, int size);
    static std::pair<unsigned char*, int> convertByteArrayToUnsignedChar(const QByteArray& byteArray);
    static QByteArray readFileToByteArray(const QString& filePath);
};
