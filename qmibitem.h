#ifndef MIBITEM_H
#define MIBITEM_H

#include <qlist.h>
#include <QStandardItem>

class QMibItem : public QObject//, public QStandardItem
{
    Q_OBJECT
public:
    ~QMibItem();
    QMibItem(QString name = "enterprises", QString oid = ".1.3.6.1.4.1");

    QString getName() const;
    void setName(const QString &value);

    bool getIsLeaf() const;
    void setIsLeaf(bool value);

    QString getOid() const;
    void setOid(const QString &value);

    bool getIsReadOnly() const;
    void setIsReadOnly(bool value);

    int getMin() const;
    void setMin(int value);

    int getMax() const;
    void setMax(int value);

    QString getDescription() const;
    void setDescription(const QString &value);

    void addChild(QMibItem *child);
    QString toString() const;

    const QList<QStandardItem *> getItems();
    const QList<QStandardItem *> createOrUpdateItems();

    enum AsnBasicType {Leaf, Trap, /*Group, */EnumInt, S32, U32, Gauge, OctetString, Sequence, Entry};
//    Q_ENUM(Type)

    AsnBasicType getAsnBasicType() const;
    void setAsnBasicType(const AsnBasicType &value);
    bool getIsCurrent() const;
    void setIsCurrent(bool value);
    QMibItem *findChildByName(const QString &);
    QMibItem *getParent() const;
    void setParent(QMibItem *value);
    void setCheckStateRecursive(Qt::CheckState state);
    Qt::CheckState getCheckState();
    QString getUnit() const;
    void setUnit(const QString &value);

protected:
    void updateStateAscending();

public slots:

private:
    QString name;
    bool isLeaf;
    QString oid;
    AsnBasicType type;
    bool isReadOnly;
    bool isCurrent;// Or obsolete in the other case
    int min;
    int max;// FIXME BDY: can be s32, or u32
    QString description;
    QString unit;
    QList<QStandardItem *> rowItems;
    QList<QMibItem*> children;
    QMibItem* parent;
};

#endif // MIBITEM_H
