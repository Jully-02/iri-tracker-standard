#include "BulletinBoardRepositoryMySQL.h"

BulletinBoard BulletinBoardRepositoryMySQL::existByName(const QString& title) {
	QSqlQuery query;
	query.prepare("SELECT bulletinBoard_id, title, to_employee, content, start_date, end_date, is_high_priority, is_active, user_id, created_at, updated_at FROM bulletinBoard WHERE title = :title");
	query.bindValue(":title", title);
	BulletinBoard bulletinBoard;
	if (!query.exec()) {
		qDebug() << "Error checking if bulletinBoard exists:" << query.lastError().text();
		return bulletinBoard;
	}

	if (query.next()) {
		bulletinBoard.setBulletinBoardId(query.value("bulletinBoard_id").toInt());
		bulletinBoard.setTitle(query.value("title").toString());
		bulletinBoard.setToEmployee(query.value("to_employee").toString());
		bulletinBoard.setContent(query.value("content").toString());
		bulletinBoard.setStartDate(query.value("start_date").toInt());
		bulletinBoard.setEndDate(query.value("end_date").toInt());
		bulletinBoard.setIsHighPriority(query.value("is_high_priority").toInt());
		bulletinBoard.setIsActive(query.value("is_active").toInt());
		bulletinBoard.setUserId(query.value("user_id").toString());
		bulletinBoard.setCreatedAt(query.value("created_at").toDateTime());
		bulletinBoard.setUpdatedAt(query.value("updated_at").toDateTime());
	}

	return bulletinBoard;
}



BulletinBoard BulletinBoardRepositoryMySQL::insert(const BulletinBoard& bulletinBoard) {
	BulletinBoard existingBulletinBoard = existByName(bulletinBoard.getTitle());

	if (existingBulletinBoard.getBulletinBoardId() != 0) {
		return BulletinBoard(); // Trả về đối tượng rỗng nếu đã tồn tại
	}

	QSqlQuery query;
	query.prepare("INSERT INTO bulletin_board (title, to_employee, content, start_date, end_date, is_high_priority, is_active, user_id, created_at) "
		"VALUES (:title, :to_employee, :content, :start_date, :end_date, :is_high_priority, :is_active, :user_id, :created_at)");
	query.bindValue(":title", bulletinBoard.getTitle());
	query.bindValue(":to_employee", bulletinBoard.getToEmployee());
	query.bindValue(":content", bulletinBoard.getContent());
	query.bindValue(":start_date", bulletinBoard.getStartDate());
	query.bindValue(":end_date", bulletinBoard.getEndDate());
	query.bindValue(":is_high_priority", bulletinBoard.getIsHighPriority());
	query.bindValue(":is_active", bulletinBoard.getIsActive());
	query.bindValue(":user_id", bulletinBoard.getUserId());
	query.bindValue(":created_at", QDateTime::currentDateTime());

	if (!query.exec()) {
		qDebug() << "Error inserting bulletinBoard:" << query.lastError().text();
		return BulletinBoard(); // Trả về đối tượng rỗng nếu có lỗi
	}

	// Sau khi insert thành công, lấy bulletin_board_id vừa được sinh ra
	int bulletinBoardId = query.lastInsertId().toInt();

	// Tạo và trả về đối tượng BulletinBoard với thông tin đã được lưu và ID mới
	return BulletinBoard(
		bulletinBoardId,
		bulletinBoard.getTitle(),
		bulletinBoard.getToEmployee(),
		bulletinBoard.getContent(),
		bulletinBoard.getStartDate(),
		bulletinBoard.getEndDate(),
		bulletinBoard.getIsActive(),
		bulletinBoard.getIsHighPriority(),
		QDateTime::currentDateTime(), // Thời gian tạo mới
		QDateTime(), // Giả sử thời gian cập nhật ban đầu là rỗng
		bulletinBoard.getUserId()
	);
}

bool BulletinBoardRepositoryMySQL::update(const BulletinBoard& bulletinBoard) {

	QSqlQuery query;
	query.prepare("UPDATE bulletinBoard SET title = :title, to_employee = :to_employee, content = :content, start_date = :start_date, end_date = :end_date, is_high_priority = :is_high_priority, is_active = :is_active, user_id = :user_id, updated_at = :updated_at WHERE bulletinBoard_id = :id");
	query.bindValue(":id", bulletinBoard.getBulletinBoardId());
	query.bindValue(":title", bulletinBoard.getTitle());
	query.bindValue(":to_employee", bulletinBoard.getToEmployee());
	query.bindValue(":content", bulletinBoard.getContent());
	query.bindValue(":start_date", bulletinBoard.getStartDate());
	query.bindValue(":end_date", bulletinBoard.getEndDate());
	query.bindValue(":is_high_priority", bulletinBoard.getIsHighPriority());
	query.bindValue(":is_active", bulletinBoard.getIsActive());
	query.bindValue(":user_id", bulletinBoard.getUserId());
	query.bindValue(":updated_at", QDateTime::currentDateTime());

	if (!query.exec()) {
		qDebug() << "Error updating bulletinBoard:" << query.lastError().text();
		return false;
	}
	return true;
}

BulletinBoard BulletinBoardRepositoryMySQL::selectById(int id) {
	QSqlQuery query;
	query.prepare("SELECT bulletinBoard_id, title, to_employee, content, start_date, end_date, is_high_priority, is_active, user_id, created_at, updated_at FROM bulletinBoard WHERE bulletinBoard_id = :id");
	query.bindValue(":id", id);

	if (!query.exec()) {
		qDebug() << "Error executing query:" << query.lastError().text();
		return BulletinBoard(); // Trả về một đối tượng rỗng nếu có lỗi
	}

	BulletinBoard bulletinBoard;
	if (query.next()) {
		bulletinBoard.setBulletinBoardId(id);
		bulletinBoard.setTitle(query.value("title").toString());
		bulletinBoard.setTitle(query.value("to_employee").toString());
		bulletinBoard.setContent(query.value("content").toString());
		bulletinBoard.setStartDate(query.value("start_date").toInt());
		bulletinBoard.setEndDate(query.value("end_date").toInt());
		bulletinBoard.setIsHighPriority(query.value("is_high_priority").toInt());
		bulletinBoard.setIsActive(query.value("is_active").toInt());
		bulletinBoard.setUserId(query.value("user_id").toString());
		bulletinBoard.setCreatedAt(query.value("created_at").toDateTime());
		bulletinBoard.setUpdatedAt(query.value("updated_at").toDateTime());
	}
	else {
		qDebug() << "No bulletinBoard found with ID:" << id;
	}

	return bulletinBoard;
}

bool BulletinBoardRepositoryMySQL::deleteItem(int id) {
	QSqlQuery query;
	query.prepare("DELETE FROM bulletinBoard WHERE bulletinBoard_id = :id");
	query.bindValue(":id", id);

	if (!query.exec()) {
		qDebug() << "Error deleting bulletinBoard:" << query.lastError().text();
		return false;
	}
	return true;
}

QList<BulletinBoard> BulletinBoardRepositoryMySQL::selectAll() {
	QList<BulletinBoard> bulletinBoards;
	QSqlQuery query;

	query.prepare("SELECT bulletinBoard_id, title, to_employee, start_date, end_date, is_high_priority, is_active, user_id, created_at, updated_at FROM bulletinBoard");


	if (!query.exec()) {
		qDebug() << "Error selecting bulletinBoards:" << query.lastError().text();
		return bulletinBoards;
	}

	while (query.next()) {
		int bulletinBoardId = query.value("bulletinBoard_id").toInt();
		QString title = query.value("title").toString();
		QString toEmployee = query.value("to_employee").toString();
		QString content = query.value("content").toString();
		qint64 startDate = query.value("start_date").toInt();
		qint64 endDate = query.value("end_date").toInt();
		int isHighPriority = query.value("is_high_priority").toInt();
		int isActive = query.value("is_active").toInt();
		QString userId = query.value("user_id").toString();
		QDateTime createdAt = query.value("created_at").toDateTime();
		QDateTime updatedAt = query.value("updated_at").toDateTime();

		bulletinBoards.append(BulletinBoard(bulletinBoardId, title, toEmployee, content, startDate, endDate, isHighPriority, isActive, createdAt, updatedAt, userId));
	}

	return bulletinBoards;
}

QList<BulletinBoard> BulletinBoardRepositoryMySQL::selectBulletinsForUser(QString userId) {
	QList<BulletinBoard> bulletins;
	QSqlQuery query;

	// Chuẩn bị câu truy vấn SQL cho MySQL
	query.prepare(R"(
        SELECT b.bulletin_board_id, b.title, b.to_employee, b.content, b.start_date, b.end_date, 
               b.is_active, b.is_high_priority, b.user_id, b.created_at, b.updated_at
        FROM bulletin_board b
        JOIN user_bulletin_board ubb ON b.bulletin_board_id = ubb.bulletin_board_id
        WHERE ubb.user_id = :user_id
        AND CURDATE() BETWEEN FROM_UNIXTIME(b.start_date, '%Y-%m-%d') AND FROM_UNIXTIME(b.end_date, '%Y-%m-%d')
        AND b.is_active = 1
    )");

	// Gán giá trị cho userId vào câu truy vấn
	query.bindValue(":user_id", userId);

	// Thực thi câu truy vấn
	if (!query.exec()) {
		qDebug() << "Error selecting bulletins for user:" << query.lastError().text();
		return bulletins;
	}

	// Lặp qua các kết quả và thêm vào danh sách
	while (query.next()) {
		int bulletinBoardId = query.value("bulletin_board_id").toInt();
		QString title = query.value("title").toString();
		QString toEmployee = query.value("to_employee").toString();
		QString content = query.value("content").toString();
		qint64 startDate = query.value("start_date").toLongLong();
		qint64 endDate = query.value("end_date").toLongLong();
		bool isActive = query.value("is_active").toBool();
		bool isHighPriority = query.value("is_high_priority").toBool();
		QString userId = query.value("user_id").toString();
		QDateTime createdAt = query.value("created_at").toDateTime();
		QDateTime updatedAt = query.value("updated_at").toDateTime();

		// Tạo đối tượng BulletinBoard và thêm vào danh sách
		bulletins.append(BulletinBoard(
			bulletinBoardId, title, toEmployee, content,
			startDate, endDate, isActive, isHighPriority,
			createdAt, updatedAt, userId
		));
	}

	return bulletins;
}
