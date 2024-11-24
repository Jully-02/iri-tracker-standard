#include "FunctionPublic.h"

// Cài đặt hàm hashPassword
QString FunctionPublic::hashPassword(const QString& password) {
    QByteArray byteArray = password.toUtf8(); // Chuyển đổi QString thành QByteArray
    QByteArray hashed = QCryptographicHash::hash(byteArray, QCryptographicHash::Sha256); // Mã hóa bằng SHA-256
    return QString(hashed.toHex()); // Trả về chuỗi hex
}

bool FunctionPublic::deleteFile(const QString& filePath) {
    QFile file(filePath);
    if (file.exists()) {
        if (file.remove()) {
            qDebug() << "File deleted successfully:" << filePath;
            return true;
        }
        else {
            qDebug() << "Failed to delete file:" << filePath;
            return false;
        }
    }
    else {
        qDebug() << "File does not exist:" << filePath;
        return false;
    }
}