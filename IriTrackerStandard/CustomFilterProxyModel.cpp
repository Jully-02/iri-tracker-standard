#include "CustomFilterProxyModel.h"

CustomFilterProxyModel::CustomFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent) {}

bool CustomFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    QModelIndex departmentIndex = sourceModel()->index(source_row, 0, source_parent);
    QString departmentName = sourceModel()->data(departmentIndex).toString();

    int rowCount = sourceModel()->rowCount(departmentIndex);
    QRegularExpression regex(filterRegExp().pattern(), QRegularExpression::CaseInsensitiveOption);

    for (int i = 0; i < rowCount; ++i) {
        QModelIndex employeeIndex = sourceModel()->index(i, 0, departmentIndex);
        QString employeeName = sourceModel()->data(employeeIndex).toString();

        if (regex.match(employeeName).hasMatch() || regex.match(departmentName).hasMatch()) {
            return true;
        }
    }

    return regex.match(departmentName).hasMatch();
}
