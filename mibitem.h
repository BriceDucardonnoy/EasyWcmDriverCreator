#ifndef MIBITEM_H
#define MIBITEM_H

#include <qlist.h>
#include <QStandardItem>

class MibItem
{
public:
    ~MibItem();
    MibItem(QString name = "enterprises", QString oid = ".1.3.6.1.4.1");

    QString getName() const;
    void setName(const QString &value);

    bool getIsLeaf() const;
    void setIsLeaf(bool value);

    QString getOid() const;
    void setOid(const QString &value);

    QString getType() const;
    void setType(const QString &value);

    bool getIsReadOnly() const;
    void setIsReadOnly(bool value);

    QString getKind() const;
    void setKind(const QString &value);

    int getSize() const;
    void setSize(int value);

    QString getDescription() const;
    void setDescription(const QString &value);

    void addChild(MibItem *child);
    QString toString() const;

    const QList<QStandardItem *> getItems();
    const QList<QStandardItem *> createOrUpdateItems();
protected:

private:
    QString name;
    bool isLeaf;
    QString oid;
    QString type;
    bool isReadOnly;
    QString kind;
    int size;
    QString description;
    QList<QStandardItem *> rowItems;
};

#endif // MIBITEM_H
