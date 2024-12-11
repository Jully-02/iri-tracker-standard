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

QByteArray FunctionPublic::templateConvertToByte(const unsigned char* buffer, int size) {
    QByteArray byteArray(reinterpret_cast<const char*>(buffer), size);
    return byteArray;
}

std::pair<unsigned char*, int> FunctionPublic::convertByteArrayToUnsignedChar(const QByteArray& byteArray) {
    // Cấp phát bộ nhớ cho buffer
    unsigned char* buffer = new unsigned char[byteArray.size()];

    // Sao chép dữ liệu từ QByteArray vào buffer
    memcpy(buffer, byteArray.data(), byteArray.size());

    // Trả về một std::pair chứa buffer và kích thước của nó
    return std::make_pair(buffer, byteArray.size());
}