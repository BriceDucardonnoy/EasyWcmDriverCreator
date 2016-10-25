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
    : isLeaf(true), isReadOnly(true), min(0), max(0)
{
    this->name = name;
    this->oid = oid;
    rowItems << new QStandardItem(name);
    rowItems << new QStandardItem(oid);
    rowItems << new QStandardItem(description);

    foreach (QStandardItem* item, rowItems) {
        item->setEditable(false);
    }
}

const QList<QStandardItem *> MibItem::createOrUpdateItems()
{
    rowItems.at(0)->setText(name);
    rowItems.at(1)->setText(oid);
    rowItems.at(2)->setText(description);
    // Really much faster to declare icons each time here rather than in constructor
//    QIcon iconLeaf(":/icons/leaf");
//    QIcon iconFolder(":/icons/folder_closed");
//    iconFolder.addFile(":/icons/folder_open", QSize(), QIcon::Normal, QIcon::On);
//    rowItems.at(0)->setIcon(isLeaf ? iconLeaf : iconFolder);
    QIcon icon;
    if(isLeaf)
    {
        if(isCurrent)
        {
            switch(this->type)
            {
            case Trap:
                icon.addFile(":/icons/trap");
                break;
            case Gauge:
                icon.addFile(":/icons/gauge");
                break;
            case U32:
            case S32:
            case EnumInt:
                icon.addFile(":/icons/enum");
                break;
            case OctetString:
                icon.addFile(":/icons/text");
                break;
            default:
                icon.addFile(":/icons/leaf");
            }
        }
        else
        {
            icon.addFile(":/icons/obsolete");
        }
    }
    else
    {
        icon.addFile(":/icons/folder_closed");
        icon.addFile(":/icons/folder_open", QSize(), QIcon::Normal, QIcon::On);
    }
    rowItems.at(0)->setIcon(icon);
    return getItems();
}

MibItem::AsnBasicType MibItem::getAsnBasicType() const
{
    return type;
}

void MibItem::setAsnBasicType(const AsnBasicType &value)
{
    type = value;
}

bool MibItem::getIsCurrent() const
{
    return isCurrent;
}

void MibItem::setIsCurrent(bool value)
{
    isCurrent = value;
}

const QList<QStandardItem *> MibItem::getItems() {
    return rowItems;
}

void MibItem::addChild(MibItem *child)
{
    int id = 0;//rowItems.size();
    isLeaf = false;
    child->createOrUpdateItems();
    qInfo() << "Add children" << child->toString() << "to parent" << toString() << "at row" << rowItems.at(id)->rowCount() << "at address" << child;
    rowItems.at(id)->appendRow(child->getItems());
//    for(int i = 0 ; i < /*rowItems.size()*/rowItems.at(id)->columnCount() ; i++)
//    {
//        rowItems.at(id)->setChild(idx, i, child->getItems().at(i));
//    }

}

QString MibItem::toString() const
{
    return getOid() + "->" + getName()
            + (isLeaf ?
                ("[" + QString::number(getMin()) + "," + QString::number(getMax())
                + "] read-only: " + QString::number(getIsReadOnly()) + " current: " + QString::number(getIsCurrent()) + " ASN type: " + QString::number(getAsnBasicType()))
            : "");
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

bool MibItem::getIsReadOnly() const
{
    return isReadOnly;
}

void MibItem::setIsReadOnly(bool value)
{
    isReadOnly = value;
}

int MibItem::getMin() const
{
    return min;
}

void MibItem::setMin(int value)
{
    min = value;
}

int MibItem::getMax() const
{
    return max;
}

void MibItem::setMax(int value)
{
    max = value;
}

QString MibItem::getDescription() const
{
    return description;
}

void MibItem::setDescription(const QString &value)
{
    description = value;
}
