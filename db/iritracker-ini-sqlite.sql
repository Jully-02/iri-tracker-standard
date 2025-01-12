BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "attendance_event" (
	"attendance_event_id"	INTEGER,
	"type"	TEXT NOT NULL,
	"date"	INTEGER NOT NULL,
	"user_id"	TEXT NOT NULL,
	PRIMARY KEY("attendance_event_id" AUTOINCREMENT),
	FOREIGN KEY("user_id") REFERENCES "user"("user_id")
);
CREATE TABLE IF NOT EXISTS "bulletin_board" (
	"bulletin_board_id"	INTEGER NOT NULL UNIQUE,
	"title"	TEXT NOT NULL,
	"to_employee" TEXT NOT NULL,
	"content"	TEXT NOT NULL,
	"start_date"	INTEGER,
	"end_date"	INTEGER,
	"is_active"	INTEGER,
	"is_high_priority"	INTEGER,
	"created_at"	TEXT,
	"updated_at"	TEXT,
	PRIMARY KEY("bulletin_board_id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "department" (
	"department_id"	INTEGER NOT NULL UNIQUE,
	"name"	TEXT NOT NULL UNIQUE,
	"description"	TEXT,
	"created_at"	TEXT,
	"updated_at"	TEXT,
	"is_active"	INTEGER,
	PRIMARY KEY("department_id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "exception" (
	"exception_id"	INTEGER NOT NULL UNIQUE,
	"name"	TEXT NOT NULL UNIQUE,
	"paid_hours"	INTEGER NOT NULL,
	"paid_coefficient"	REAL NOT NULL,
	"work_coefficient"	REAL NOT NULL,
	"is_overtime"	INTEGER NOT NULL,
	"created_at"	TEXT,
	"updated_at"	TEXT,
	PRIMARY KEY("exception_id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "role" (
	"role_id"	INTEGER NOT NULL UNIQUE,
	"name"	TEXT NOT NULL UNIQUE,
	"description"	TEXT,
	"created_at"	TEXT,
	"updated_at"	TEXT,
	PRIMARY KEY("role_id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "user" (
	"user_id"	TEXT NOT NULL UNIQUE,
	"first_name"	TEXT,
	"last_name"	TEXT,
	"is_password"	INTEGER,
	"password"	TEXT,
	"department_id"	INTEGER,
	"date_of_birth"	TEXT,
	"start_working_date"	INTEGER,
	"is_active"	INTEGER NOT NULL,
	"avatar"	BLOB,
	"email"	TEXT,
	"phone"	TEXT,
	"cell_phone"	TEXT,
	"address"	TEXT,
	"eye_right"	BLOB,
	"eye_left"	BLOB,
	"role_id"	INTEGER NOT NULL,
	"created_at"	TEXT,
	"updated_at"	TEXT,
	PRIMARY KEY("user_id"),
	FOREIGN KEY("department_id") REFERENCES "department"("department_id"),
	FOREIGN KEY("role_id") REFERENCES "role"("role_id")
);
CREATE TABLE IF NOT EXISTS "user_exception" (
	"user_exception_id"	INTEGER,
	"user_id"	TEXT NOT NULL,
	"exception_id"	INTEGER NOT NULL,
	"start_date"	INTEGER NOT NULL,
	"end_date"	INTEGER NOT NULL,
	"is_all_date"	INTEGER NOT NULL,
	"day_of_week"	NUMERIC NOT NULL,
	PRIMARY KEY("user_exception_id" AUTOINCREMENT),
	FOREIGN KEY("exception_id") REFERENCES "exception"("exception_id"),
	FOREIGN KEY("user_id") REFERENCES "user"("user_id")
);

CREATE TABLE IF NOT EXISTS "user_bulletin_board" (
    "id" INTEGER NOT NULL UNIQUE,
    "user_id" TEXT NOT NULL,
    "bulletin_board_id" INTEGER NOT NULL,
    PRIMARY KEY("id" AUTOINCREMENT),
    FOREIGN KEY("user_id") REFERENCES "user"("user_id"),
    FOREIGN KEY("bulletin_board_id") REFERENCES "bulletin_board"("bulletin_board_id")
);
INSERT INTO "department" ("department_id","name","description","created_at","updated_at","is_active") VALUES (1, 'Others','Other', datetime('now'), datetime('now'), 1);
INSERT INTO "exception" ("exception_id","name","paid_hours","paid_coefficient","work_coefficient","is_overtime","created_at","updated_at") VALUES (1, 'Sickly',28800,1.0,1.0,1, datetime('now'), datetime('now')),
 (2,'Vacation',28800,1.0,1.0,1, datetime('now'), datetime('now')),
 (3,'Holiday',28800,1.0,1.0,1, datetime('now'), datetime('now'));
INSERT INTO "role" ("role_id","name","description","created_at","updated_at") VALUES (1,'Admin','admin', datetime('now'), datetime('now')),
 (2,'User','user', datetime('now'), datetime('now'));
COMMIT;
