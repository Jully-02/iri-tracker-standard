#include "CustomLineEdit.h"
#include <QFocusEvent>
#include <QMouseEvent>
#include <QKeyEvent>

CustomLineEdit::CustomLineEdit(QWidget* parent) : QLineEdit(parent) {
    setupIcons();
    setReadOnly(true);
    applyStyle();
}

void CustomLineEdit::applyStyle() {
    setStyleSheet(
        "QLineEdit {"
        "    width: 170px;"
        "    background-color: #5f96d8;"
        "    color: #fff;"
        "    border: none;"
        "    padding: 5px;"
        "}"
    );
}

void CustomLineEdit::setupIcons() {
    employeeIcon = new QAction(QIcon("../icons/employee-search.png"), "", this);
    addAction(employeeIcon, QLineEdit::LeadingPosition);

    searchIcon = new QAction(QIcon("../icons/search.png"), "", this);
    addAction(searchIcon, QLineEdit::TrailingPosition);

    clearIcon = new QAction(QIcon("../icons/clear.png"), "", this);
    addAction(clearIcon, QLineEdit::TrailingPosition);
    clearIcon->setVisible(false);

    connect(clearIcon, &QAction::triggered, this, &CustomLineEdit::onClearText);
    connect(searchIcon, &QAction::triggered, this, &CustomLineEdit::onSearch);
}

void CustomLineEdit::onClearText() {
    employeeIcon->setVisible(true);
    searchIcon->setVisible(true);
    clearIcon->setVisible(false);
    setReadOnly(true);
    emit searchCleared();
}

void CustomLineEdit::onSearch() {
    clear();
    employeeIcon->setVisible(false);
    searchIcon->setVisible(false);
    clearIcon->setVisible(true);
    setReadOnly(false);
}

void CustomLineEdit::keyPressEvent(QKeyEvent* event) {
    if (isReadOnly()) {
        event->ignore();
        return;
    }
    QLineEdit::keyPressEvent(event);
}

void CustomLineEdit::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        int actionWidth = 24;

        if (clearIcon->isVisible() && event->pos().x() >= width() - actionWidth) {
            emit clearIcon->triggered();
            return;
        }

        if (searchIcon->isVisible() && event->pos().x() >= width() - 2 * actionWidth) {
            emit searchIcon->triggered();
            return;
        }
    }

    QLineEdit::mousePressEvent(event);
}

void CustomLineEdit::setEmployeeCount(int count) {
    setText(QString("Employee(%1)").arg(count));
}