#ifndef CUSTOMITEM_H
#define CUSTOMITEM_H

#include <QObject>
#include <QTreeWidgetItem>
#include <QTreeWidget>

class CustomItem: public QTreeWidgetItem
{
public:
    int i, j;
    CustomItem(int i = -1, int j = -1, QTreeWidget* parent = nullptr);
};

#endif // CUSTOMITEM_H
