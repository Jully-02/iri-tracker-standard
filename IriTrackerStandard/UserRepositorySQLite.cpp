#include "UserRepositorySQLite.h"

bool UserRepositorySQLite::insert(const User& user) {
	QSqlQuery query;
	query.prepare("INSERT INTO user (user_id, first_name, last_name, is_password, password, date_of_birth, "
		"start_working_date, is_active, avatar, email, phone, cell_phone, address, eye_right, eye_left, role_id, department_id) "
		"VALUES (:user_id, :first_name, :last_name, :is_password, :password, :date_of_birth, "
		":start_working_date, :is_active, :avatar, :email, :phone, :cell_phone, :address, :eye_right, :eye_left, :role_id, :department_id)");

	query.bindValue(":user_id", user.getUserId());
	query.bindValue(":first_name", user.getFirstName());
	query.bindValue(":last_name", user.getLastName());
	query.bindValue(":is_password", user.getIsPassword());
	query.bindValue(":password", user.getPassword());
	query.bindValue(":date_of_birth", user.getDateOfBirth());
	query.bindValue(":start_working_date", user.getStartWorkingDate());
	query.bindValue(":is_active", 1);
	query.bindValue(":avatar", user.getAvatar());
	query.bindValue(":email", user.getEmail());
	query.bindValue(":phone", user.getPhone());
	query.bindValue(":cell_phone", user.getCellPhone());
	query.bindValue(":address", user.getAddress());
	query.bindValue(":eye_right", user.getEyeRight());
	query.bindValue(":eye_left", user.getEyeLeft());
	query.bindValue(":role_id", user.getRoleId());
	query.bindValue(":department_id", user.getDepartmentId());

	if (!query.exec()) {
		qDebug() << "Error inserting user:" << query.lastError().text();
		return false;
	}

	return true;
}

bool UserRepositorySQLite::update(const User& user) {
	QSqlQuery query;
	query.prepare("UPDATE user SET first_name = :first_name, last_name = :last_name, password = :password, "
		"date_of_birth = :date_of_birth, start_working_date = :start_working_date, "
		"is_active = :is_active, avatar = :avatar, email = :email, phone = :phone, "
		"cell_phone = :cell_phone, address = :address, department_id = :department_id, eye_right = :eye_right, eye_left = :eye_left "
		"WHERE user_id = :user_id");

	query.bindValue(":user_id", user.getUserId());
	query.bindValue(":first_name", user.getFirstName());
	query.bindValue(":last_name", user.getLastName());
	query.bindValue(":date_of_birth", user.getDateOfBirth());
	query.bindValue(":start_working_date", user.getStartWorkingDate());
	query.bindValue(":is_active", user.getIsActive());
	query.bindValue(":avatar", user.getAvatar());
	query.bindValue(":email", user.getEmail());
	query.bindValue(":password", user.getPassword());
	query.bindValue(":phone", user.getPhone());
	query.bindValue(":cell_phone", user.getCellPhone());
	query.bindValue(":address", user.getAddress());
	query.bindValue(":department_id", user.getDepartmentId());
	query.bindValue(":eye_right", user.getEyeRight());
	query.bindValue(":eye_left", user.getEyeLeft());

	if (!query.exec()) {
		qDebug() << "Error updating user:" << query.lastError().text();
		return false;
	}

	return true;
}
bool UserRepositorySQLite::deleteItem(QString id) {
	QSqlQuery query;
	query.prepare("UPDATE user SET is_active = false WHERE user_id = :user_id");
	query.bindValue(":user_id", id);
	if (!query.exec()) {
		qDebug() << "Error deleting user:" << query.lastError().text();
		return false;
	}

	return true;
}
QList<User> UserRepositorySQLite::selectAll(bool condition) {
	QList<User> users;
	QSqlQuery query("SELECT user_id, first_name, last_name, is_password, password, date_of_birth, start_working_date, "
		"is_active, avatar, email, phone, cell_phone, address, eye_right, eye_left, created_at, updated_at, "
		"department_id, role_id FROM user WHERE role_id != 1 AND is_active = true");

	if (!query.exec()) {
		qDebug() << "Error selecting users:" << query.lastError().text();
		return users;
	}

	while (query.next()) {
		User user(
			query.value(0).toString(),         // user_id
			query.value(1).toString(),         // first_name
			query.value(2).toString(),         // last_name
			query.value(3).toBool(),           // is_password
			query.value(4).toString(),         // password
			query.value(5).toString(),         // date_of_birth
			query.value(6).toLongLong(),       // start_working_date
			query.value(7).toBool(),           // is_active
			query.value(8).toByteArray(),      // avatar
			query.value(9).toString(),         // email
			query.value(10).toString(),        // phone
			query.value(11).toString(),        // cell_phone
			query.value(12).toString(),        // address
			query.value(13).toByteArray(),        // eye_right
			query.value(14).toByteArray(),        // eye_left
			query.value(15).toDateTime(),      // created_at
			query.value(16).toDateTime(),      // updated_at
			query.value(17).toInt(),           // department_id
			query.value(18).toInt()            // role_id
		);
		users.append(user);
	}

	return users;

}

User UserRepositorySQLite::selectById(QString userId) {
	QSqlQuery query;
	User user;

	query.prepare("SELECT first_name, last_name, email, phone, cell_phone, address, is_password, password, "
		"date_of_birth, start_working_date, is_active, avatar, department_id, eye_right, eye_left "
		"FROM user WHERE user_id = :user_id");
	query.bindValue(":user_id", userId);

	if (!query.exec()) {
		qDebug() << "Error executing selectUserById:" << query.lastError().text();
		return user;
	}

	if (query.next()) {
		user.setUserId(userId);
		user.setFirstName(query.value(0).toString());
		user.setLastName(query.value(1).toString());
		user.setEmail(query.value(2).toString());
		user.setPhone(query.value(3).toString());
		user.setCellPhone(query.value(4).toString());
		user.setAddress(query.value(5).toString());
		user.setIsPassword(query.value(6).toBool());
		user.setPassword(query.value(7).toString());
		user.setDateOfBirth(query.value(8).toString());
		user.setStartWorkingDate(query.value(9).toLongLong());
		user.setIsActive(query.value(10).toBool());
		user.setAvatar(query.value(11).toByteArray());
		user.setDepartmentId(query.value(12).toInt());
		user.setEyeRight(query.value(13).toByteArray());
		user.setEyeLeft(query.value(14).toByteArray());
	}
	else {
		qDebug() << "No user found with user_id:" << userId;
	}

	return user;
}

QList<QVariantList> UserRepositorySQLite::selectByDepartmentId(int departmentId) {
	QList<QVariantList> users;
	QSqlQuery queryUsers;

	queryUsers.prepare("SELECT user_id, first_name, last_name FROM user WHERE department_id = :department_id AND role_id = 2 AND is_active = true");

	queryUsers.bindValue(":department_id", departmentId);

	if (!queryUsers.exec()) {
		qDebug() << "Error selecting users by department:" << queryUsers.lastError().text();
		return users;
	}

	while (queryUsers.next()) {
		QVariantList user;
		user.append(queryUsers.value(0)); 
		user.append(queryUsers.value(1));
		user.append(queryUsers.value(2)); 

		users.append(user);
	}

	return users;
}

bool UserRepositorySQLite::checkInOut(const QString& userId, const QString& password) {
	QSqlQuery query;

	query.prepare("SELECT password FROM user WHERE user_id = :user_id");
	query.bindValue(":user_id", userId);

	if (!query.exec()) {
		qDebug() << "Error executing checkInOut query:" << query.lastError().text();
		return false;
	}

	if (query.next()) {
		bool isPasswordCorrect = query.value("password").toString() == password;
		if (!isPasswordCorrect) {
			qDebug() << "Invalid password for user_id:" << userId;
			return false;
		}
		else {
			return true;
		}
	}
	else {
		return false;
	}
}

User UserRepositorySQLite::checkIfAdminExist() {
	QSqlQuery query;

	query.prepare("SELECT user_id, first_name, last_name, password, role_id FROM user WHERE role_id = 1 AND is_active = true");

	User user;
	if (!query.exec()) {
		qDebug() << "Error selecting admin:" << query.lastError().text();
		return user;
	}

	if (query.next()) {
		user.setUserId(query.value(0).toString());
		user.setFirstName(query.value(1).toString());
		user.setLastName(query.value(2).toString());
		user.setPassword(query.value(3).toString());
		user.setRoleId(query.value(4).toInt());
	}
	else {
		qDebug() << "No user found with role admin";
	}

	return user;
}

bool UserRepositorySQLite::changePasswordAdmin(const QString& oldPassword, const QString& newPassword) {
	QSqlQuery query;
	User admin = selectById("admin");
	if (oldPassword != admin.getPassword()) {
		return false;
	}
	else {
		admin.setPassword(newPassword);
		update(admin);
		return true;
	}
}

QList<QPair<QString, QPair<QByteArray, QByteArray>>> UserRepositorySQLite::selectAllEyes() {
	QList<QPair<QString, QPair<QByteArray, QByteArray>>> userEyes;

	QSqlQuery query("SELECT user_id, eye_right, eye_left FROM user WHERE (eye_right IS NOT NULL OR eye_left IS NOT NULL) AND is_active = true");

	if (!query.exec()) {
		qDebug() << "Error selecting eyes:" << query.lastError().text();
		return userEyes;
	}

	while (query.next()) {
		QString userId = query.value(0).toString(); 
		QByteArray eyeRight = query.value(1).toByteArray();  
		QByteArray eyeLeft = query.value(2).toByteArray();  

		if (!eyeRight.isEmpty() || !eyeLeft.isEmpty()) {
			userEyes.append(qMakePair(userId, qMakePair(eyeRight, eyeLeft)));
		}
	}

	return userEyes;
}
