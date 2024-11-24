#pragma once

#include <QString>
#include <QCryptographicHash>
#include <QByteArray>
#include <QFile>
#include <QDebug>

class FunctionPublic
{
public:
    // Hàm dùng để mã hóa mật khẩu
    static QString hashPassword(const QString& password);
    static bool deleteFile(const QString& filePath);
};
