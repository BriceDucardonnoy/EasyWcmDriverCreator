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
    : name(name), isLeaf(true), oid(oid), isReadOnly(true), min(0), max(0), unit(""), attribute("Misc"), refreshFactor(4), factor(1), precision(1), identifierType(0),
      strOperator(0), wcsType(QMibItem::Unset), mib(""), expectedValue(""), fr(""), en(""), es(""), parent(NULL)
{
    rowItems << new QStandardItem(name);
    rowItems << new QStandardItem(oid);
    rowItems << new QStandardItem(description);

    rowItems[0]->setCheckable(true);
    rowItems[0]->setTristate(false);
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
                setWcsType(QMibItem::IdentifierReading);
                break;
            case U32:
            case S32:
            case EnumInt:
                icon.addFile(":/icons/enum");
                setWcsType(QMibItem::Identifier);
                break;
            case OctetString:
                icon.addFile(":/icons/text");
                setWcsType(QMibItem::Identifier);
                break;
            default:
                icon.addFile(":/icons/leaf");
                setWcsType(QMibItem::Identifier);
            }
        }
        else
        {
            icon.addFile(":/icons/obsolete");
        }
    }
    else
    {
        if(type == Sequence)
        {
            icon.addFile(":/icons/table");
        }
        else
        {
            icon.addFile(":/icons/folder_closed");
            icon.addFile(":/icons/folder_open", QSize(), QIcon::Normal, QIcon::On);
        }
    }
    first->setIcon(icon);

    return getItems();
}

QMibItem *QMibItem::getParent() const
{
    return parent;
}

void QMibItem::setParent(QMibItem *value)
{
    parent = value;
}

Qt::CheckState QMibItem::getCheckState()
{
    return rowItems[0]->checkState();
}

void QMibItem::updateStateAscending()
{
    if(isLeaf) return;
    Qt::CheckState childState = children.at(0)->getCheckState();

    foreach(QMibItem *child, children)
    {
        if(childState != child->getCheckState())
        {
            childState = Qt::PartiallyChecked;
            break;
        }
    }
    rowItems[0]->setCheckState(childState);

    if(parent != NULL && parent->getCheckState() != getCheckState())
    {
        parent->updateStateAscending();
    }
}

int QMibItem::getIdentifierType() const
{
    return identifierType;
}

void QMibItem::setIdentifierType(int value)
{
    identifierType = value;
}

QJsonObject QMibItem::write(const QString mibName, QJsonObject &fr, QJsonObject &en, QJsonObject &es) const
{
    QJsonObject me;

    me["attribute"] = attribute;
    me["mib"] = mibName;
    me["name"] = name;
    me["identifier"] = oid;

    fr[name] = this->fr;
    en[name] = this->en;
    es[name] = this->es;

    if(wcsType == Identifier)
    {
        writeId(me);
    }
    else if(wcsType == IdentifierReading)
    {
        writeIdReading(me);
    }

    return me;
}

void QMibItem::writeId(QJsonObject &json) const
{
    json["type"] = identifierType;
    json["operator"] = strOperator;
    json["expectedValue"] = expectedValue;
}

void QMibItem::writeIdReading(QJsonObject &json) const
{
    json["minValue"] = wcsMin;
    json["maxValue"] = wcsMax;
    json["factor"] = factor;
    json["precision"] = precision;
    json["unit"] = unit;
    json["refreshFactor"] = refreshFactor;
}

int QMibItem::getWcsMax() const
{
    return wcsMax;
}

void QMibItem::setWcsMax(int value)
{
    wcsMax = value;
}

int QMibItem::getWcsMin() const
{
    return wcsMin;
}

void QMibItem::setWcsMin(int value)
{
    wcsMin = value;
}

QString QMibItem::getEn() const
{
    return en;
}

void QMibItem::setEn(const QString &value)
{
    en = value;
}

QString QMibItem::getFr() const
{
    return fr;
}

void QMibItem::setFr(const QString &value)
{
    fr = value;
}

QString QMibItem::getEs() const
{
    return es;
}

void QMibItem::setEs(const QString &value)
{
    es = value;
}

QString QMibItem::getMib() const
{
    return mib;
}

void QMibItem::setMib(const QString &value)
{
    mib = value;
}

QMibItem::WcsType QMibItem::getWcsType() const
{
    return wcsType;
}

void QMibItem::setWcsType(const WcsType &value)
{
    wcsType = value;
}

QString QMibItem::getExpectedValue() const
{
    return expectedValue;
}

void QMibItem::setExpectedValue(const QString &value)
{
    expectedValue = value;
}

const QList<QMibItem *> QMibItem::getCheckedNodes() const
{
    QList<QMibItem *> checkedNodes;
    foreach(QMibItem *child, children)
    {
        if(child->isLeaf)
        {
            if(child->getCheckState() == Qt::Checked && child->isCurrent == true)// Don't add obsolete
            {
                checkedNodes.append(child);
            }
        }
        else
        {
            if(child->getCheckState() != Qt::Unchecked)
            {
                checkedNodes.append(child->getCheckedNodes());
            }
        }
    }

    return checkedNodes;
}

int QMibItem::getStrOperator() const
{
    return strOperator;
}

void QMibItem::setStrOperator(int value)
{
    strOperator = value;
}

int QMibItem::getPrecision() const
{
    return precision;
}

void QMibItem::setPrecision(int value)
{
    precision = value;
}

double QMibItem::getFactor() const
{
    return factor;
}

void QMibItem::setFactor(double value)
{
    factor = value;
}

int QMibItem::getRefreshFactor() const
{
    return refreshFactor;
}

void QMibItem::setRefreshFactor(int value)
{
    refreshFactor = value;
}

QString QMibItem::getAttribute() const
{
    return attribute;
}

void QMibItem::setAttribute(const QString &value)
{
    attribute = value;
}

QString QMibItem::getUnit() const
{
    return unit;
}

void QMibItem::setUnit(const QString &value)
{
    unit = value;
}

void QMibItem::setCheckStateRecursive(Qt::CheckState state)
{
    rowItems[0]->setCheckState(state);
    if(!isLeaf)
    {
        foreach(QMibItem *child, children)
        {
            child->setCheckStateRecursive(state);
        }
    }
    if(parent != NULL && parent->getCheckState() != getCheckState())
    {
        parent->updateStateAscending();
    }
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
    rowItems[0]->setTristate(true);// Partially checked possible if it has children
    child->createOrUpdateItems();
    qInfo() << "Add children" << child->toString() << "to parent" << toString() << "at row" << rowItems.at(id)->rowCount()
            << "at address" << child;
    rowItems.at(id)->appendRow(child->getItems());
    children.append(child);
    child->setParent(this);
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

const QList<QStandardItem *> QMibItem::getItems() const {
    return rowItems;
}

QString QMibItem::toString() const
{
    return getOid() + "->" + getName()
            + (isLeaf ?
                ("[" + QString::number(getMin()) + "," + QString::number(getMax())
                + "] read-only: " + QString::number(isReadOnly) + " current: " + QString::number(isCurrent)
                + " ASN type: " + QString::number(getAsnBasicType()))
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

void QMibItem::setNameAndItem(const QString &value)
{
    setName(value);
    rowItems[0]->setText(value);
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
