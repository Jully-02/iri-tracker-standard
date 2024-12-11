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
    // Hàm dùng để mã hóa mật khẩu
    static QString hashPassword(const QString& password);
    static bool deleteFile(const QString& filePath);
    static QByteArray templateConvertToByte(const unsigned char* buffer, int size);
    static std::pair<unsigned char*, int> convertByteArrayToUnsignedChar(const QByteArray& byteArray);
};
