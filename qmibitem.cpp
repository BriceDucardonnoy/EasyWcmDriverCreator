#include <qdebug.h>
#include <qicon.h>

#include "qmibitem.h"

QMibItem::~QMibItem()
{
    foreach (QStandardItem *item, rowItems)
    {
        delete(item);
    }
    foreach(QMibItem *child, children)
    {
        delete(child);
    }
}

QMibItem::QMibItem(QString name, QString oid)
    : isLeaf(true), isReadOnly(true), min(0), max(0)
{
    this->name = name;
    this->oid = oid;
    rowItems << new QStandardItem(name);
    rowItems << new QStandardItem(oid);
    rowItems << new QStandardItem(description);

    rowItems[0]->setCheckable(true);
    rowItems[0]->setTristate(true);
    foreach (QStandardItem* item, rowItems)
    {
        item->setEditable(false);
    }
}

const QList<QStandardItem *> QMibItem::createOrUpdateItems()
{
    QStandardItem *first = rowItems.at(0);
    first->setText(name);
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
    first->setIcon(icon);
//    connect(first, SIGNAL(itemChanged(QStandardItem*)), first, SLOT(checkItemStates(QStandardItem*)));

    return getItems();
}

void QMibItem::checkItemStates(QStandardItem *item)
{
    qInfo() << "Hello" << getName() << item->text();
}

void QMibItem::connectCheck()
{
//    connect(rowItems.at(0)->model(), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(checkItemStates(QStandardItem*)));
//    if(isLeaf)
//    {
//        connect(rowItems.at(0)->model(), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(checkItemStates(QStandardItem*)));
//    }
//    foreach(QMibItem *child, children)
//    {
//        child->connectCheck();
//    }
}

QMibItem *QMibItem::findChildByName(const QString &name)
{
    if(getName().compare(name) == 0) return this;
    QMibItem *node = NULL;
    foreach(QMibItem *child, children)
    {
        node = child->findChildByName(name);
        if(node != NULL) break;
    }
    return node;
}

void QMibItem::addChild(QMibItem *child)
{
    int id = 0;//rowItems.size();
    isLeaf = false;
    child->createOrUpdateItems();
    qInfo() << "Add children" << child->toString() << "to parent" << toString() << "at row" << rowItems.at(id)->rowCount() << "at address" << child;
    rowItems.at(id)->appendRow(child->getItems());
    children.append(child);
}

QMibItem::AsnBasicType QMibItem::getAsnBasicType() const
{
    return type;
}

void QMibItem::setAsnBasicType(const AsnBasicType &value)
{
    type = value;
}

bool QMibItem::getIsCurrent() const
{
    return isCurrent;
}

void QMibItem::setIsCurrent(bool value)
{
    isCurrent = value;
}

const QList<QStandardItem *> QMibItem::getItems() {
    return rowItems;
}

QString QMibItem::toString() const
{
    return getOid() + "->" + getName()
            + (isLeaf ?
                ("[" + QString::number(getMin()) + "," + QString::number(getMax())
                + "] read-only: " + QString::number(getIsReadOnly()) + " current: " + QString::number(getIsCurrent()) + " ASN type: " + QString::number(getAsnBasicType()))
            : "");
}

QString QMibItem::getName() const
{
    return name;
}

void QMibItem::setName(const QString &value)
{
    name = value;
}

bool QMibItem::getIsLeaf() const
{
    return isLeaf;
}

void QMibItem::setIsLeaf(bool value)
{
    isLeaf = value;
}

QString QMibItem::getOid() const
{
    return oid;
}

void QMibItem::setOid(const QString &value)
{
    oid = value;
}

bool QMibItem::getIsReadOnly() const
{
    return isReadOnly;
}

void QMibItem::setIsReadOnly(bool value)
{
    isReadOnly = value;
}

int QMibItem::getMin() const
{
    return min;
}

void QMibItem::setMin(int value)
{
    min = value;
}

int QMibItem::getMax() const
{
    return max;
}

void QMibItem::setMax(int value)
{
    max = value;
}

QString QMibItem::getDescription() const
{
    return description;
}

void QMibItem::setDescription(const QString &value)
{
    description = value;
}
