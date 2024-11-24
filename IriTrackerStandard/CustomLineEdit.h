#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QAction>

class CustomLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit CustomLineEdit(QWidget* parent = nullptr);
    void setEmployeeCount(int count);
    void applyStyle();

signals:
    void searchCleared();

private slots:
    void onClearText();
    void onSearch();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QAction* employeeIcon;
    QAction* searchIcon;
    QAction* clearIcon;

    void setupIcons();
};

#endif 