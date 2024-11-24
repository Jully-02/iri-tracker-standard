#pragma once

#include <QSortFilterProxyModel>
#include <QString>
#include <QRegularExpression>

class CustomFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit CustomFilterProxyModel(QObject* parent = nullptr);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
};