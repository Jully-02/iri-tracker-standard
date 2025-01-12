#include "AttendanceEventDeleteForm.h"
#include "AttendanceEvent.h"
#include <QDateTime>
#include <QDebug>
#include "DatabaseHelper.h"

AttendanceEventDeleteForm::AttendanceEventDeleteForm(QWidget *parent)
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


    connect(ui.btnOk, &QPushButton::clicked, this, &AttendanceEventDeleteForm::btnOkClicked);
    connect(ui.btnCancel, &QPushButton::clicked, this, &AttendanceEventDeleteForm::btnCancelClicked);

}

AttendanceEventDeleteForm::~AttendanceEventDeleteForm()
{}

void AttendanceEventDeleteForm::handleFormAction(const QString& userId, int id) {
    selectUserId = userId;
    eventId = id;

    std::pair<AttendanceEvent, AttendanceEvent> eventPair = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->getInOutPairByUserIdAndEventId(selectUserId, eventId);
    AttendanceEvent inEvent = eventPair.first;
    AttendanceEvent outEvent = eventPair.second;

    // Check if inEvent and outEvent have valid values
    if (inEvent.getAttendanceEventId() != 0) {
        // Convert timestamp from inEvent to QDateTime
        QDateTime inDateTime = QDateTime::fromSecsSinceEpoch(inEvent.getDate());

        // String format for date and time
        QString inDateStr = inDateTime.date().toString("MM/dd/yyyy");
        QString inTimeStr = inDateTime.time().toString("hh:mm:ss AP");

        // Fill the checkin fields with data
        ui.checkinDateLabel->setText(inDateStr);
        ui.checkinTimeLabel->setText(inTimeStr);
    }
    else {
        ui.checkinDateLabel->setText("");
        ui.checkinTimeLabel->setText("");
    }

    if (outEvent.getAttendanceEventId() != 0) {
        QDateTime outDateTime = QDateTime::fromSecsSinceEpoch(outEvent.getDate());

        QString outDateStr = outDateTime.date().toString("MM/dd/yyyy");
        QString outTimeStr = outDateTime.time().toString("hh:mm:ss AP");

        ui.checkoutDateLabel->setText(outDateStr);
        ui.checkoutTimeLabel->setText(outTimeStr);
    }
    else {
        ui.checkoutDateLabel->setText("");
        ui.checkoutTimeLabel->setText("");
    }
}


void AttendanceEventDeleteForm::btnOkClicked() {
    bool isSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->deleteInOutPairByUserIdAndEventId(selectUserId, eventId);
    if (!isSuccess) {
        qDebug() << "Error deleting attendance events for user:" << selectUserId;
    }
    else {
        emit attendanceEventDeleteChanged();
        this->close();
    }
}

void AttendanceEventDeleteForm::btnCancelClicked() {
    this->close();
}
