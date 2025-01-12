#include "AttendanceEventForm.h"
#include <QDebug>
#include "AttendanceEvent.h"
#include <QMessageBox>
#include "DatabaseHelper.h"

AttendanceEventForm::AttendanceEventForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    QIcon logoIcon("../icons/logo.png");
    this->setWindowIcon(logoIcon);

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


    // Reset error message labels
    ui.valueHoursLabel->setText(""); 
    ui.errorLabel->setText("");
    ui.errorLabel->setStyleSheet("background-color: transparent;");

    bool isErrStartWorkingIn = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isEventBeforeStartWorkingDate(userIdEvent, checkinDateCompare);
    bool isErrStartWorkingOut = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->isEventBeforeStartWorkingDate(userIdEvent, checkoutDateCompare);

    if (ui.inOutRadio->isChecked()) {
        // Check if the "In" date and "Out" date do not overlap
        ui.errorLabel->setText("");
        ui.errorLabel->setStyleSheet("background-color: transparent;");
        if (checkinDate != checkoutDate) {
            ui.valueHoursLabel->setText("The \"IN\" event and the \"OUT\" event must be on the same date.");
            ui.valueHoursLabel->setStyleSheet("color: red;");
            return;
        }

        // Check if "Out" time is before "In" time
        if (checkoutTime < checkinTime) {
            ui.valueHoursLabel->setText("The \"IN\" event must be followed by \"OUT\" event.");
            ui.valueHoursLabel->setStyleSheet("color: red;");
            return;
        }

        // Get the current time
        QTime currentTime = QTime::currentTime();
        QDate currentDate = QDate::currentDate();

        // Check time "In"
        if (checkinDate < currentDate || (checkinDate == currentDate && checkinTime < currentTime)) {
            // "In" is time in the past
        }
        else {
            ui.valueHoursLabel->setText("Cannot add event in a future time.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); 
            return;
        }

        // Check time "Out"
        if (checkoutDate < currentDate || (checkoutDate == currentDate && checkoutTime < currentTime)) {
            // "Out" is time in the past
        }
        else {
            ui.valueHoursLabel->setText("Cannot add event in a future time.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); 
            return;
        }

        if (isErrStartWorkingIn) {
            ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
            ui.valueHoursLabel->setStyleSheet("color: red;"); 
            return;
        }
        
        if (isErrStartWorkingOut) {
            ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
            ui.valueHoursLabel->setStyleSheet("color: red;");
            return;
        }
        // If there are no errors, calculate the hour value
        int secondsDifference = checkinTime.secsTo(checkoutTime);
        double hoursDecimal = secondsDifference / 3600.0; // Divide by 3600 to convert to hours

        // Cập nhật nhãn với giá trị hours
        ui.valueHoursLabel->setText(QString("%1").arg(hoursDecimal, 0, 'f', 2)); 
        ui.valueHoursLabel->setStyleSheet("color: black;"); 
    }
    else { // If inRadio or outRadio
        QTime currentTime = QTime::currentTime();
        QDate currentDate = QDate::currentDate();
        ui.valueHoursLabel->setText("");

        // inRadio
        if (ui.inRadio->isChecked()) {
            if (checkinDate > currentDate || (checkinDate == currentDate && checkinTime > currentTime)) {
                ui.valueHoursLabel->setText("Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); 
                return;
            }

            if (checkinTimestamp > currentTimestamp) {
                ui.valueHoursLabel->setText(" Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); 
                return;
            }

            if (isErrStartWorkingIn) {
                ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); 
                return;
            }
        }

        // outRadio
        if (ui.outRadio->isChecked()) {
            if (checkoutDate > currentDate || (checkoutDate == currentDate && checkoutTime > currentTime)) {
                ui.valueHoursLabel->setText("Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); 
                return;
            }

            if (checkinTimestamp > currentTimestamp) {
                ui.valueHoursLabel->setText(" Cannot add event in a future time.");
                ui.valueHoursLabel->setStyleSheet("color: red;"); 
                return;
            }

            if (isErrStartWorkingOut) {
                ui.valueHoursLabel->setText(" Event time must be later than the start of working date.");
                ui.valueHoursLabel->setStyleSheet("color: red;");
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

        // Check if inEvent and outEvent have valid values
        if (inEvent.getAttendanceEventId() != 0) {
            QDateTime inDateTime = QDateTime::fromSecsSinceEpoch(inEvent.getDate());
            QDate inDate = inDateTime.date();
            QTime inTime = inDateTime.time();

           
            ui.checkinDateEdit->setDate(inDate);
            ui.checkinTimeEdit->setTime(inTime);
        }

        if (outEvent.getAttendanceEventId() != 0) {
            QDateTime outDateTime = QDateTime::fromSecsSinceEpoch(outEvent.getDate());
            QDate outDate = outDateTime.date();
            QTime outTime = outDateTime.time();

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
            double hoursDecimal = secondsDifference / 3600.0; 

            // Update the label with the value hours
            ui.valueHoursLabel->setText(QString("%1").arg(hoursDecimal, 0, 'f', 2)); 
            ui.valueHoursLabel->setStyleSheet("color: black;"); 
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
    // Check time "In"
    if (checkinTimestamp >= currentTimestamp) {
        ui.errorLabel->setText(" Cannot add event in a future time.");
        ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
        return true;
    }

    // Check time "Out"
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

    ui.valueHoursLabel->setText("");
    ui.errorLabel->setText(""); 

    QDateTime checkinDateTime(checkinDate, checkinTime, Qt::LocalTime);
    QDateTime checkoutDateTime(checkoutDate, checkoutTime, Qt::LocalTime);

    qint64 checkinTimestamp = checkinDateTime.toSecsSinceEpoch();
    qint64 checkoutTimestamp = checkoutDateTime.toSecsSinceEpoch();

    bool hasError = false; 

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

    if (ui.inOutRadio->isChecked()) { // If inOutRadio is selected
        // Check if the "In" date and "Out" date do not overlap
        ui.errorLabel->setText("");
        if (checkinDate != checkoutDate) {
            ui.errorLabel->setText(" The \"IN\" event and the \"OUT\" event must be on the same date.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            hasError = true;
        }

        // Check if "Out" time is before "In" time
        if (checkoutTime < checkinTime) {
            ui.errorLabel->setText(" The \"IN\" event must be followed by \"OUT\" event.");
            ui.errorLabel->setStyleSheet("background-color: darkred; color: white;");
            hasError = true;
        }

        // Get the current time
        qint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();

        hasError = handleErrorInOutRadio(checkinTimestamp, checkoutTimestamp, currentTimestamp);

        // If there are no errors, calculate the hour value
        if (!hasError) {
            int secondsDifference = checkinTime.secsTo(checkoutTime);
            double hoursDecimal = secondsDifference / 3600.0; // Divide by 3600 to convert to hours

            // Update the label with the value hours
            ui.valueHoursLabel->setText(QString("%1").arg(hoursDecimal, 0, 'f', 2)); 
            ui.valueHoursLabel->setStyleSheet("color: black;");

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
    else { // If inRadio or outRadio

        qint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();

        // inRadio
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

        // outRadio
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
