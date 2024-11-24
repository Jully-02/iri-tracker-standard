#include "AttendanceEventForm.h"
#include <QDebug>
#include "AttendanceEvent.h"
#include <QMessageBox>
#include "DatabaseHelper.h"

AttendanceEventForm::AttendanceEventForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    QLinearGradient gradient(0, 0, 0, this->height());
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor("#87A8D2"));

    QPalette palette;
    palette.setBrush(QPalette::Window, gradient);
    this->setPalette(palette);
    this->setAutoFillBackground(true);


    connect(ui.btnOk, &QPushButton::clicked, this, &AttendanceEventForm::btnOkClicked);
    connect(ui.btnCancel, &QPushButton::clicked, this, &AttendanceEventForm::btnCancelClicked);

    ui.checkinDateEdit->setDateTime(QDateTime::currentDateTime());
    ui.checkoutDateEdit->setDateTime(QDateTime::currentDateTime());

    connect(ui.inRadio, &QRadioButton::toggled, this, &AttendanceEventForm::updateDateTimeFields);
    connect(ui.outRadio, &QRadioButton::toggled, this, &AttendanceEventForm::updateDateTimeFields);
    connect(ui.inOutRadio, &QRadioButton::toggled, this, &AttendanceEventForm::updateDateTimeFields);

    connect(ui.checkinDateEdit, &QDateEdit::dateChanged, this, &AttendanceEventForm::onDateOrTimeChanged);
    connect(ui.checkinTimeEdit, &QTimeEdit::timeChanged, this, &AttendanceEventForm::onDateOrTimeChanged);
    connect(ui.checkoutDateEdit, &QDateEdit::dateChanged, this, &AttendanceEventForm::onDateOrTimeChanged);
    connect(ui.checkoutTimeEdit, &QTimeEdit::timeChanged, this, &AttendanceEventForm::onDateOrTimeChanged);
}

AttendanceEventForm::~AttendanceEventForm()
{}

void AttendanceEventForm::updateDateTimeFields() {
    if (ui.inRadio->isChecked()) {
        ui.checkinDateEdit->setEnabled(true);
        ui.checkinTimeEdit->setEnabled(true);
        ui.checkoutDateEdit->setDisabled(true);
        ui.checkoutTimeEdit->setDisabled(true);
        ui.valueHoursLabel->setText("");
    }
    else if (ui.outRadio->isChecked()) {
        ui.checkinDateEdit->setDisabled(true);
        ui.checkinTimeEdit->setDisabled(true);
        ui.checkoutDateEdit->setEnabled(true);
        ui.checkoutTimeEdit->setEnabled(true);
        ui.valueHoursLabel->setText("");
    }
    else if (ui.inOutRadio->isChecked()) {
        ui.checkinDateEdit->setEnabled(true);
        ui.checkinTimeEdit->setEnabled(true);
        ui.checkoutDateEdit->setEnabled(true);
        ui.checkoutTimeEdit->setEnabled(true);
    }
}

void AttendanceEventForm::onDateOrTimeChanged() {
    QDate checkinDate = ui.checkinDateEdit->date();
    QTime checkinTime = ui.checkinTimeEdit->time();
    QDate checkoutDate = ui.checkoutDateEdit->date();
    QTime checkoutTime = ui.checkoutTimeEdit->time();

    QDateTime checkinDateTime(checkinDate, checkinTime, Qt::LocalTime);
    QDateTime checkoutDateTime(checkoutDate, checkoutTime, Qt::LocalTime);

    qint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
    qint64 checkinTimestamp = checkinDateTime.toSecsSinceEpoch();
    qint64 checkoutTimestamp = checkoutDateTime.toSecsSinceEpoch();


    QDateTime checkoutDateC(checkoutDate, QTime(0, 0));
    qint64 checkoutDateCompare = checkoutDateC.toSecsSinceEpoch();

    QDateTime checkinDateC(checkinDate, QTime(0, 0));
    qint64 checkinDateCompare = checkinDateC.toSecsSinceEpoch();


    // Reset nhãn thông báo lỗi
    ui.valueHoursLabel->setText(""); // Xóa thông báo lỗi trước khi kiểm tra
    ui.errorLabel->setText("");
    ui.errorLabel->setStyleSheet("background-color: transparent;");

    bool isErrStartWorkingIn = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isEventBeforeStartWorkingDate(userIdEvent, checkinDateCompare);
    bool isErrStartWorkingOut = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isEventBeforeStartWorkingDate(userIdEvent, checkoutDateCompare);

    if (ui.inOutRadio->isChecked()) { // Nếu inOutRadio được chọn
        // Kiểm tra nếu ngày "In" và ngày "Out" không trùng nhau
        ui.errorLabel->setText("");
        ui.errorLabel->setStyleSheet("background-color: transparent;");
        if (checkinDate != checkoutDate) {
            ui.valueHoursLabel->setText("The \"IN\" event and the \"OUT\" event must be on the same date.");
            ui.valueHoursLabel->setStyleSheet("color: red;");
            return;
        }

        // Kiểm tra nếu thời gian "Out" trước thời gian "In"
        if (checkoutTime < checkinTime) {
            ui.valueHoursLabel->setText("The \"IN\" event must be followed by \"OUT\" event.");
            ui.valueHoursLabel->setStyleSheet("color: red;");
            return;
        }

        // Lấy thời gian hiện tại
        QTime currentTime = QTime::currentTime();
        QDate currentDate = QDate::currentDate();

        // Kiểm tra thời gian "In"
        if (checkinDate < currentDate || (checkinDate == currentDate && checkinTime < currentTime)) {
            // "In" là thời gian trong quá khứ
        }
        else {
            ui.valueHoursLabel->setText("Cannot add event in a future time.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
            return;
        }

        // Kiểm tra thời gian "Out"
        if (checkoutDate < currentDate || (checkoutDate == currentDate && checkoutTime < currentTime)) {
            // "Out" là thời gian trong quá khứ
        }
        else {
            ui.valueHoursLabel->setText("Cannot add event in a future time.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
            return;
        }

        if (isErrStartWorkingIn) {
            ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
            return;
        }
        
        if (isErrStartWorkingOut) {
            ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
            return;
        }
        // Nếu không có lỗi, tính toán giá trị giờ
        int secondsDifference = checkinTime.secsTo(checkoutTime);
        double hoursDecimal = secondsDifference / 3600.0; // Chia cho 3600 để chuyển đổi sang giờ

        // Cập nhật nhãn với giá trị hours
        ui.valueHoursLabel->setText(QString("%1").arg(hoursDecimal, 0, 'f', 2)); // 2 chữ số thập phân
        ui.valueHoursLabel->setStyleSheet("color: black;"); // Đổi màu chữ thành xanh nếu không có lỗi
    }
    else { // Nếu là inRadio hoặc outRadio
     // Lấy thời gian hiện tại
        QTime currentTime = QTime::currentTime();
        QDate currentDate = QDate::currentDate();
        ui.valueHoursLabel->setText("");
        // Nếu là inRadio
        if (ui.inRadio->isChecked()) {
            if (checkinDate > currentDate || (checkinDate == currentDate && checkinTime > currentTime)) {
                ui.valueHoursLabel->setText("Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
                return;
            }

            if (checkinTimestamp > currentTimestamp) {
                ui.valueHoursLabel->setText(" Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
                return;
            }

            if (isErrStartWorkingIn) {
                ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
                return;
            }
        }

        // Nếu là outRadio
        if (ui.outRadio->isChecked()) {
            if (checkoutDate > currentDate || (checkoutDate == currentDate && checkoutTime > currentTime)) {
                ui.valueHoursLabel->setText("Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
                return;
            }

            if (checkinTimestamp > currentTimestamp) {
                ui.valueHoursLabel->setText(" Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
                return;
            }

            if (isErrStartWorkingOut) {
                ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); // Đổi màu chữ thành màu đỏ
                return;
            }

        }
        ui.valueHoursLabel->setText("");
    }
}




void AttendanceEventForm::handleFormAction(const QString& action, const QString& userId, int id) {
    currentAction = action;
    userIdEvent = userId;
    attendanceEventId = id;

    updateDateTimeFields();

    if (action == "Add") {
        
    }
    else if (action == "Edit") {
        std::pair<AttendanceEvent, AttendanceEvent> eventPair = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->getInOutPairByUserIdAndEventId(userIdEvent, attendanceEventId);
        AttendanceEvent inEvent = eventPair.first;
        AttendanceEvent outEvent = eventPair.second;
        checkinTimestampDB = inEvent.getDate();
        checkoutTimestampDB = outEvent.getDate();

        // Kiểm tra xem inEvent và outEvent có giá trị hợp lệ không
        if (inEvent.getAttendanceEventId() != 0) {
            // Chuyển đổi timestamp từ inEvent thành QDate và QTime
            QDateTime inDateTime = QDateTime::fromSecsSinceEpoch(inEvent.getDate());
            QDate inDate = inDateTime.date();
            QTime inTime = inDateTime.time();

            // Đổ dữ liệu vào các trường checkin
            ui.checkinDateEdit->setDate(inDate);
            ui.checkinTimeEdit->setTime(inTime);
        }

        if (outEvent.getAttendanceEventId() != 0) {
            // Chuyển đổi timestamp từ outEvent thành QDate và QTime
            QDateTime outDateTime = QDateTime::fromSecsSinceEpoch(outEvent.getDate());
            QDate outDate = outDateTime.date();
            QTime outTime = outDateTime.time();

            // Đổ dữ liệu vào các trường checkout
            ui.checkoutDateEdit->setDate(outDate);
            ui.checkoutTimeEdit->setTime(outTime);
        }

        QDate checkinDate = ui.checkinDateEdit->date();
        QTime checkinTime = ui.checkinTimeEdit->time();
        QDate checkoutDate = ui.checkoutDateEdit->date();
        QTime checkoutTime = ui.checkoutTimeEdit->time();

        if (inEvent.getAttendanceEventId() != 0 && outEvent.getAttendanceEventId() != 0) {
            ui.inRadio->setDisabled(true);
            ui.outRadio->setDisabled(true);
            ui.inOutRadio->setChecked(true);
            int secondsDifference = checkinTime.secsTo(checkoutTime);
            double hoursDecimal = secondsDifference / 3600.0; // Chia cho 3600 để chuyển đổi sang giờ

            // Cập nhật nhãn với giá trị hours
            ui.valueHoursLabel->setText(QString("%1").arg(hoursDecimal, 0, 'f', 2)); // 2 chữ số thập phân
            ui.valueHoursLabel->setStyleSheet("color: black;"); // Đổi màu chữ thành xanh nếu không có lỗi
        }
        else if (inEvent.getAttendanceEventId() != 0 && outEvent.getAttendanceEventId() == 0) {
            ui.outRadio->setDisabled(true);
            ui.inOutRadio->setDisabled(true);
            ui.inRadio->setChecked(true);
        }
        else if (inEvent.getAttendanceEventId() == 0 && outEvent.getAttendanceEventId() != 0) {
            ui.inRadio->setDisabled(true);
            ui.inOutRadio->setDisabled(true);
            ui.outRadio->setChecked(true);
        }
    }
}

bool AttendanceEventForm::handleErrorInRadio(qint64 checkinTimestamp, qint64 currentTimestamp ) {
    ui.errorLabel->setText("");
    if (checkinTimestamp > currentTimestamp) {
        ui.errorLabel->setText(" Cannot add event in a future time.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }
    
    bool checkForOrphanInEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEvents(userIdEvent);
    bool isCheckinBeforeExistingEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isCheckinBeforeExistingEvents(userIdEvent, checkinTimestamp);
    bool isEventBeforeStartWorkingDate = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isEventBeforeStartWorkingDate(userIdEvent, checkinTimestamp);

    if (currentAction == "Add") {
        if (!checkForOrphanInEvents) {
            ui.errorLabel->setText(" Cannot add this event. The \"IN\" event must be followed by \"OUT\" event.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            return true;
        }
    }

    if (isCheckinBeforeExistingEvents) {
        ui.errorLabel->setText(" The current event should be later than the previous normal event.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    if (isEventBeforeStartWorkingDate) {
        ui.errorLabel->setText(" Event time must be later than the start of working date.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    return false;
}

bool AttendanceEventForm::handleErrorOutRadio(qint64 checkoutTimestamp, qint64 currentTimestamp) {
    ui.errorLabel->setText("");
    if (checkoutTimestamp > currentTimestamp) {
        ui.errorLabel->setText(" Cannot add event in a future time.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    bool checkForOrphanInEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEvents(userIdEvent);
    bool checkForOrphanInEventsHasTime = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEventsHasTime(userIdEvent, checkoutTimestamp);
    bool isCheckinBeforeExistingEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isCheckinBeforeExistingEvents(userIdEvent, checkoutTimestamp);

    if (checkForOrphanInEvents) {
        ui.errorLabel->setText(" Cannot add this event. The \"IN\" event must be followed by \"OUT\" event.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }
    else if (checkForOrphanInEventsHasTime) {
        ui.errorLabel->setText(" The current event should be later than the previous normal event.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    if (isCheckinBeforeExistingEvents) {
        qDebug() << "Hello, Bug Here!";
        ui.errorLabel->setText(" The current event should be later than the previous normal event.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    return false;
}

bool AttendanceEventForm::handleErrorInOutRadio(qint64 checkinTimestamp, qint64 checkoutTimestamp, qint64 currentTimestamp) {
    // Kiểm tra thời gian "In"
    if (checkinTimestamp >= currentTimestamp) {
        ui.errorLabel->setText(" Cannot add event in a future time.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    // Kiểm tra thời gian "Out"
    if (checkoutTimestamp >= currentTimestamp) {
        ui.errorLabel->setText(" Cannot add event in a future time.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    bool checkForOrphanInEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEvents(userIdEvent);
    bool checkForDuplicateEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForDuplicateEvents(userIdEvent, checkinTimestamp, checkoutTimestamp);
    bool checkForOverlappingEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOverlappingEvents(userIdEvent, checkinTimestamp, checkoutTimestamp);


    if (!checkForOrphanInEvents) {
        ui.errorLabel->setText(" Cannot add this event. The \"IN\" event exists.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    if (checkinTimestampDB != checkinTimestamp || checkoutTimestampDB != checkoutTimestamp) {
        if (checkForDuplicateEvents) {
            ui.errorLabel->setText(" Time duplicates with other event.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            return true;
        }

        if (checkForOverlappingEvents) {
            ui.errorLabel->setText(" Event time is overlapped with other events.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            return true;
        }
    }

    return false;
}


void AttendanceEventForm::btnOkClicked() {
    QDate checkinDate = ui.checkinDateEdit->date();
    QTime checkinTime = ui.checkinTimeEdit->time();
    QDate checkoutDate = ui.checkoutDateEdit->date();
    QTime checkoutTime = ui.checkoutTimeEdit->time();

    // Reset nhãn thông báo lỗi
    ui.valueHoursLabel->setText("");
    ui.errorLabel->setText(""); // Xóa thông báo lỗi trước khi kiểm tra

    // Chuyển đổi ngày và giờ thành QDateTime và sau đó chuyển đổi sang Unix timestamp
    QDateTime checkinDateTime(checkinDate, checkinTime, Qt::LocalTime);
    QDateTime checkoutDateTime(checkoutDate, checkoutTime, Qt::LocalTime);

    qint64 checkinTimestamp = checkinDateTime.toSecsSinceEpoch();
    qint64 checkoutTimestamp = checkoutDateTime.toSecsSinceEpoch();

    bool hasError = false; // Biến theo dõi lỗi

    AttendanceEvent inEvent;
    inEvent.setAttendanceEventId(attendanceEventId);
    inEvent.setType("In");
    inEvent.setDate(checkinTimestamp);
    inEvent.setUserId(userIdEvent);

    AttendanceEvent outEvent;
    outEvent.setAttendanceEventId(attendanceEventId);
    outEvent.setType("Out");
    outEvent.setDate(checkoutTimestamp);
    outEvent.setUserId(userIdEvent);

    if (ui.inOutRadio->isChecked()) { // Nếu inOutRadio được chọn
        // Kiểm tra nếu ngày "In" và ngày "Out" không trùng nhau
        ui.errorLabel->setText("");
        if (checkinDate != checkoutDate) {
            ui.errorLabel->setText(" The \"IN\" event and the \"OUT\" event must be on the same date.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            hasError = true;
        }

        // Kiểm tra nếu thời gian "Out" trước thời gian "In"
        if (checkoutTime < checkinTime) {
            ui.errorLabel->setText(" The \"IN\" event must be followed by \"OUT\" event.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            hasError = true;
        }

        // Lấy thời gian hiện tại
        qint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();

        hasError = handleErrorInOutRadio(checkinTimestamp, checkoutTimestamp, currentTimestamp);

        // Nếu không có lỗi, tính toán giá trị giờ
        if (!hasError) {
            int secondsDifference = checkinTime.secsTo(checkoutTime);
            double hoursDecimal = secondsDifference / 3600.0; // Chia cho 3600 để chuyển đổi sang giờ

            // Cập nhật nhãn với giá trị hours
            ui.valueHoursLabel->setText(QString("%1").arg(hoursDecimal, 0, 'f', 2)); // 2 chữ số thập phân
            ui.valueHoursLabel->setStyleSheet("color: black;"); // Đổi màu chữ thành xanh nếu không có lỗi

            bool inSuccess, outSuccess;

            if (currentAction == "Add") {
                inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(inEvent);
                outSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(outEvent);
                if (!inSuccess || !outSuccess) {
                    qDebug() << "Error adding attendance events for user:" << userIdEvent;
                }
                else {
                    emit attendanceEventChanged();
                    this->close();
                }
            }
            else if (currentAction == "Edit") {
                inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->update(inEvent);
                outSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->update(outEvent);
                if (!inSuccess || !outSuccess) {
                    qDebug() << "Error updating attendance events for user:" << userIdEvent;
                }
                else {
                    emit attendanceEventChanged();
                    this->close();
                }
            }
        }
    }
    else { // Nếu là inRadio hoặc outRadio

        qint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();

        // Nếu là inRadio
        if (ui.inRadio->isChecked()) {
            hasError = handleErrorInRadio(checkinTimestamp, currentTimestamp);
            if (!hasError) {
                bool inSuccess;

                if (currentAction == "Add") {
                    inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(inEvent);
                    if (!inSuccess) {
                        qDebug() << "Error adding attendance events for user:" << userIdEvent;
                    }
                    else {
                        emit attendanceEventChanged();
                        this->close();
                    }
                }
                else if (currentAction == "Edit") {
                    inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->update(inEvent);
                    if (!inSuccess) {
                        qDebug() << "Error updating attendance events for user:" << userIdEvent;
                    }
                    else {
                        emit attendanceEventChanged();
                        this->close();
                    }
                }
                
            }
        }

        // Nếu là outRadio
        if (ui.outRadio->isChecked()) {
            hasError = handleErrorOutRadio(checkoutTimestamp, currentTimestamp);
            if (!hasError) {
                bool inSuccess;
                if (currentAction == "Add") {
                    inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(outEvent);
                    if (!inSuccess) {
                        qDebug() << "Error adding attendance events for user:" << userIdEvent;
                    }
                    else {
                        emit attendanceEventChanged();
                        this->close();
                    }
                }
                else if (currentAction == "Edit") {
                    inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->update(outEvent);
                    //inSuccess = AttendanceEvent::updateAttendanceEvent(attendanceEventId, "In", checkoutTimestamp);
                    if (!inSuccess) {
                        qDebug() << "Error updating attendance events for user:" << userIdEvent;
                    }
                    else {
                        emit attendanceEventChanged();
                        this->close();
                    }
                }
            }

        }
    }
}



void AttendanceEventForm::btnCancelClicked() {
    this->close();
}
