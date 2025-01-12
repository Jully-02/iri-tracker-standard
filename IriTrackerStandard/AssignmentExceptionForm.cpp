#include "AssignmentExceptionForm.h"
#include <QIcon>
#include "DatabaseHelper.h"
#include "Exception.h"

AssignmentExceptionForm::AssignmentExceptionForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowTitle(tr("Assignment Exception"));

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	loadException();
}

AssignmentExceptionForm::~AssignmentExceptionForm()
{}


void AssignmentExceptionForm::loadException() {
	ui.tableExceptions->setRowCount(0);
	ui.tableExceptions->verticalHeader()->setVisible(false);
	ui.tableExceptions->setColumnHidden(0, true);

	QList<Exception> exceptions = DatabaseHelper::getDatabaseInstance()->getExceptionRepository()->selectAll();

	for (const Exception& exception : exceptions) {
		int row = ui.tableExceptions->rowCount();
		ui.tableExceptions->insertRow(row);


		qint64 seconds = exception.getPaidHours();
		// Thêm dữ liệu vào bảng
		ui.tableExceptions->setItem(row, 0, new QTableWidgetItem((QString::number(exception.getExceptionId()))));
		ui.tableExceptions->setItem(row, 1, new QTableWidgetItem(exception.getName()));
		ui.tableExceptions->setItem(row, 2, new QTableWidgetItem(QTime(seconds / 3600, (seconds % 3600) / 60).toString("HH:mm")));
		ui.tableExceptions->setItem(row, 3, new QTableWidgetItem(QString::number(exception.getWorkCoefficient(), 'f', 2)));

		// Tô màu cho hàng
		QColor rowColor = (row % 2 == 0) ? QColor("#e3e9f1") : QColor("#dbdbd8");
		for (int col = 0; col < ui.tableExceptions->columnCount(); ++col) {
			QTableWidgetItem* item = ui.tableExceptions->item(row, col);
			if (item) {
				item->setBackground(QBrush(rowColor));
			}
		}
	}

	ui.tableExceptions->setSelectionBehavior(QAbstractItemView::SelectRows);

	// Kết nối sự kiện khi click vào ô
	connect(ui.tableExceptions, &QTableWidget::cellClicked, this, [this](int row, int column) {
		ui.tableExceptions->selectRow(row);
		ui.tableExceptions->setCurrentItem(ui.tableExceptions->item(row, column));
		});
}
