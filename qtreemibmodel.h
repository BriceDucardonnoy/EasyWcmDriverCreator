#ifndef TREEMIBMODEL_H
#define TREEMIBMODEL_H

#include <QWidget>
#include <qfile.h>
#include <qtextstream.h>
#include <qstandarditemmodel.h>

#include "qmibitem.h"

//class TreeMibModel : public QFileSystemModel
class QTreeMibModel : public QStandardItemModel
{
    Q_OBJECT
public:
    QTreeMibModel(QObject *parent = 0);
//    TreeMibModel::TreeMibModel();
    QTreeMibModel(QString mibfile);
    ~QTreeMibModel();

    void createModel(QTextStream *stream, QTreeMibModel *parent);
    void createModel(QTextStream *stream, QMibItem *parent);
    /**
     * @brief QTreeMibModel::getMibNodeFromIndex Find the MibNode from its QModelIndex
     * @param index The QModelIndex
     * @return The MibNode* or NULL if not found
     */
    QMibItem *getMibNodeFromIndex(const QModelIndex &index);
    QString getModuleName() const;

    QString getVendor() const;
    void setVendor(const QString &value);

protected:
    /**
     * @brief TreeMibModel::createModel Recursive method to create the tree model
     * @param stream The QTextStream of the MIB file
     * @param parent The parent node to look children for
     */
    void createModel(QTextStream *stream, const QMibItem *parent);
    bool createModel(QFile *mibfile);
    void findNode(QTextStream *stream, QTreeMibModel *parent);
private slots:
    void checkItemStates(QStandardItem *);
private:
    QMibItem *root;
    QMibItem *moduleIdentity;
    QString moduleIdentityParentName;
    QString moduleName;
    QString vendor;
};

#endif // TREEMIBMODEL_H
