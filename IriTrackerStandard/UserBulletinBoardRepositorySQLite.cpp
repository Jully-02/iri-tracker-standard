#include "UserBulletinBoardRepositorySQLite.h"


bool UserBulletinBoardRepositorySQLite::insert(const UserBulletinBoard& user_bulletin_board) {
    QSqlQuery query;
    query.prepare("INSERT INTO user_bulletin_board (user_id, bulletin_board_id) VALUES (:user_id, :bulletin_board_id)");
    query.bindValue(":user_id", user_bulletin_board.getUserId());
    query.bindValue(":bulletin_board_id", user_bulletin_board.getBulletinBoardId());

    if (!query.exec()) {
        qDebug() << "Error inserting user_bulletin_board:" << query.lastError().text();
        return false;
    }
    return true;
}

bool UserBulletinBoardRepositorySQLite::update(const UserBulletinBoard& user_bulletin_board) {
    QSqlQuery query;
    query.prepare("UPDATE user_bulletin_board SET user_id = :user_id, bulletin_board_id = :bulletin_board_id WHERE id = :id");
    query.bindValue(":id", user_bulletin_board.getUserBulletinBoardId());
    query.bindValue(":user_id", user_bulletin_board.getUserId());
    query.bindValue(":bulletin_board_id", user_bulletin_board.getBulletinBoardId());

    if (!query.exec()) {
        qDebug() << "Error updating user_bulletin_board:" << query.lastError().text();
        return false;
    }
    return true;
}

bool UserBulletinBoardRepositorySQLite::deleteByBulletinBoardId(int bulletinBoardId) {
    QSqlQuery query;
    query.prepare("DELETE FROM user_bulletin_board WHERE bulletin_board_id = :bulletin_board_id");
    query.bindValue(":bulletin_board_id", bulletinBoardId);

    if (!query.exec()) {
        qDebug() << "Error deleting user_bulletin_board:" << query.lastError().text();
        return false;
    }
    return true;
}


QList<UserBulletinBoard> UserBulletinBoardRepositorySQLite::selectByUserId(QString userId) {
    QList<UserBulletinBoard> user_bulletin_boards;
    QSqlQuery query;


    query.prepare("SELECT id, user_id, bulletin_board_id FROM user_bulletin_board WHERE user_id = :user_id");

 
    query.bindValue(":user_id", userId);


    if (!query.exec()) {
        qDebug() << "Error selecting user_bulletin_boards:" << query.lastError().text();
        return user_bulletin_boards;
    }

    while (query.next()) {
        int id = query.value("id").toInt();
        QString userId = query.value("user_id").toString();
        int bulletinBoardId = query.value("bulletin_board_id").toInt();

      
        user_bulletin_boards.append(UserBulletinBoard(id, userId, bulletinBoardId));
    }

    return user_bulletin_boards;
}

QList<UserBulletinBoard> UserBulletinBoardRepositorySQLite::selectByBulletinBoardId(int bulletinBoardId) {
    QList<UserBulletinBoard> user_bulletin_boards;
    QSqlQuery query;

   
    query.prepare("SELECT id, user_id, bulletin_board_id FROM user_bulletin_board WHERE bulletin_board_id = :bulletin_board_id");

    query.bindValue(":bulletin_board_id", bulletinBoardId);

    // Thực thi câu truy vấn
    if (!query.exec()) {
        qDebug() << "Error selecting user_bulletin_boards:" << query.lastError().text();
        return user_bulletin_boards;
    }

    while (query.next()) {
        int id = query.value("id").toInt();
        QString userId = query.value("user_id").toString();
        int bulletinBoardId = query.value("bulletin_board_id").toInt();

        user_bulletin_boards.append(UserBulletinBoard(id, userId, bulletinBoardId));
    }

    return user_bulletin_boards;
}
