#include "FunctionPublic.h"
#include "SingletonManager.h"

QString FunctionPublic::hashPassword(const QString& password) {
    QByteArray byteArray = password.toUtf8();
    QByteArray hashed = QCryptographicHash::hash(byteArray, QCryptographicHash::Sha256); 
    return QString(hashed.toHex()); 
}

QByteArray FunctionPublic::templateConvertToByte(const unsigned char* buffer, int size) {
    QByteArray byteArray(reinterpret_cast<const char*>(buffer), size);
    return byteArray;
}

std::pair<unsigned char*, int> FunctionPublic::convertByteArrayToUnsignedChar(const QByteArray& byteArray) {
    unsigned char* buffer = new unsigned char[byteArray.size()];

    memcpy(buffer, byteArray.data(), byteArray.size());

    return std::make_pair(buffer, byteArray.size());
}

QByteArray FunctionPublic::readFileToByteArray(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Không thể mở file" << filePath;
        return QByteArray();  
    }

    QByteArray byteArray = file.readAll();

    file.close();

    return byteArray;
}