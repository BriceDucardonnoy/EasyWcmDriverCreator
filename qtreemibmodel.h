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

protected:
    void createModel(QTextStream *stream, const QMibItem *parent);

private slots:
    void checkItemStates(QStandardItem *);

private:
    QMibItem *moduleIdentity;
    QString moduleIdentityParentName;
    bool createModel(QFile *mibfile);
    void findNode(QTextStream *stream, QTreeMibModel *parent);
};

#endif // TREEMIBMODEL_H
