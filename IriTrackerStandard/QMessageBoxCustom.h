#pragma once

#include <QDialog>
#include "ui_QMessageBoxCustom.h"

class QMessageBoxCustom : public QDialog
{
	Q_OBJECT

public:
	~QMessageBoxCustom();
	explicit QMessageBoxCustom(QWidget* parent = nullptr);
	void setQuestionLabel(const QString& text);
	void setWindowTitleCustom(const QString& title);

private:
	void btnOkClicked();
	void btnCancelClicked();

private:
	Ui::QMessageBoxCustomClass ui;
};
