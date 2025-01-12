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
    // Cấp phát bộ nhớ cho buffer
    unsigned char* buffer = new unsigned char[byteArray.size()];

    // Sao chép dữ liệu từ QByteArray vào buffer
    memcpy(buffer, byteArray.data(), byteArray.size());

    // Trả về một std::pair chứa buffer và kích thước của nó
    return std::make_pair(buffer, byteArray.size());
}

QByteArray FunctionPublic::readFileToByteArray(const QString& filePath) {
    // Mở file với chế độ đọc nhị phân
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Không thể mở file" << filePath;
        return QByteArray();  // Trả về QByteArray rỗng nếu không thể mở file
    }

    // Đọc nội dung file vào QByteArray
    QByteArray byteArray = file.readAll();

    // Đóng file sau khi đọc xong
    file.close();

    return byteArray;
}