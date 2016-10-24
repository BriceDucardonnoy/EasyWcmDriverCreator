#include <qdebug.h>
#include <qicon.h>

#include "mibitem.h"

MibItem::~MibItem()
{
    foreach (QStandardItem *item, rowItems) {
        delete(item);
    }
}

MibItem::MibItem(QString name, QString oid)
    : isLeaf(true), isReadOnly(true)
{
    this->name = name;
    this->oid = oid;
    rowItems << new QStandardItem(name);
    rowItems << new QStandardItem(oid);

    foreach (QStandardItem* item, rowItems) {
        item->setEditable(false);
    }
}

const QList<QStandardItem *> MibItem::createOrUpdateItems()
{
    rowItems.at(0)->setText(name);
    rowItems.at(1)->setText(oid);
    QIcon iconLeaf(":/icons/leaf");
    QIcon iconFolder(":/icons/folder_closed");
    iconFolder.addFile(":/icons/folder_open", QSize(), QIcon::Normal, QIcon::On);

    rowItems.at(0)->setIcon(isLeaf ? iconLeaf : iconFolder);
    return getItems();
}

const QList<QStandardItem *> MibItem::getItems() {
    return rowItems;
}

void MibItem::addChild(MibItem *child)
{
    int idx = rowItems.at(0)->rowCount();
    child->createOrUpdateItems();
//    qInfo() << "Add children" << child->toString() << "to parent" << toString();
    for(int i = 0 ; i < rowItems.size() ; i++)
    {
        rowItems.at(0)->setChild(idx, i, child->getItems().at(i));
    }

    isLeaf = false;
}

QString MibItem::toString() const
{
    return getOid() + " -> " + getName();
}

QString MibItem::getName() const
{
    return name;
}

void MibItem::setName(const QString &value)
{
    name = value;
}

bool MibItem::getIsLeaf() const
{
    return isLeaf;
}

void MibItem::setIsLeaf(bool value)
{
    isLeaf = value;
}

QString MibItem::getOid() const
{
    return oid;
}

void MibItem::setOid(const QString &value)
{
    oid = value;
}

QString MibItem::getType() const
{
    return type;
}

void MibItem::setType(const QString &value)
{
    type = value;
}

bool MibItem::getIsReadOnly() const
{
    return isReadOnly;
}

void MibItem::setIsReadOnly(bool value)
{
    isReadOnly = value;
}

QString MibItem::getKind() const
{
    return kind;
}

void MibItem::setKind(const QString &value)
{
    kind = value;
}

int MibItem::getSize() const
{
    return size;
}

void MibItem::setSize(int value)
{
    size = value;
}

QString MibItem::getDescription() const
{
    return description;
}

void MibItem::setDescription(const QString &value)
{
    description = value;
}
