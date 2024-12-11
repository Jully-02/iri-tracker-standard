#include "EmployeeBulletins.h"
#include <QIcon>
#include "DatabaseHelper.h"
#include "BulletinBoard.h"

EmployeeBulletins::EmployeeBulletins(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowTitle("Employee Bulletins");

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);
}

EmployeeBulletins::~EmployeeBulletins()
{}

void EmployeeBulletins::handleEmit(const QString& userId) {
    ui.tableBulletins->setRowCount(0);
    ui.tableBulletins->verticalHeader()->setVisible(false);
    ui.tableBulletins->setColumnHidden(0, true);

    ui.tableBulletins->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableBulletins->setSelectionMode(QAbstractItemView::NoSelection);
    ui.tableBulletins->resizeColumnsToContents();
    ui.tableBulletins->horizontalHeader()->setStretchLastSection(true);
    ui.tableBulletins->setSelectionBehavior(QAbstractItemView::SelectRows);

    QList<BulletinBoard> bulletins = DatabaseHelper::getDatabaseInstance()->getBulletinBoardRepository()->selectBulletinsForUser(userId);

    for (const BulletinBoard& bulletin : bulletins) {
        int row = ui.tableBulletins->rowCount();
        ui.tableBulletins->insertRow(row);

        ui.tableBulletins->setItem(row, 0, new QTableWidgetItem(QString::number(bulletin.getBulletinBoardId())));
        ui.tableBulletins->setItem(row, 1, new QTableWidgetItem(bulletin.getTitle()));

        QDateTime startDateTime = QDateTime::fromSecsSinceEpoch(bulletin.getStartDate());
        QDate startDate = startDateTime.date();
        ui.tableBulletins->setItem(row, 2, new QTableWidgetItem(startDate.toString("MM/dd/yyyy")));

        QDateTime endDateTime = QDateTime::fromSecsSinceEpoch(bulletin.getEndDate());
        QDate endDate = endDateTime.date();
        ui.tableBulletins->setItem(row, 3, new QTableWidgetItem(endDate.toString("MM/dd/yyyy")));

        QColor rowColor = (row % 2 == 0) ? QColor("#e3e9f1") : QColor("#dbdbd8");
        for (int col = 0; col < ui.tableBulletins->columnCount(); ++col) {
            QTableWidgetItem* item = ui.tableBulletins->item(row, col);
            if (item) {
                item->setBackground(QBrush(rowColor));
            }
        }
    }

    ui.tableBulletins->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui.tableBulletins, &QTableWidget::cellClicked, this, [this, bulletins](int row, int column) {
        ui.tableBulletins->selectRow(row);
        ui.tableBulletins->setCurrentItem(ui.tableBulletins->item(row, column));

        if (row >= 0 && row < bulletins.size()) {
            const BulletinBoard& selectedBulletin = bulletins.at(row);
            ui.titleLabel->setText(selectedBulletin.getTitle());
            ui.contentTextEdit->setText(selectedBulletin.getContent());
        }
        });

    if (!bulletins.isEmpty()) {
        ui.tableBulletins->selectRow(0);
        ui.tableBulletins->setCurrentItem(ui.tableBulletins->item(0, 1));

        ui.titleLabel->setText(bulletins[0].getTitle());
        ui.contentTextEdit->setText(bulletins[0].getContent());
    }
}
