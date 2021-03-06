/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef ODBCMODELENVIRONMENT_H
#define ODBCMODELENVIRONMENT_H

#include "ODBCModel.h"
#include "ODBCModelDrivers.h"

/*!
 * \class   ODBCModelEnvironment
 * \brief   This model encapsulates an ODBC environment.
 * \author  pharvey (8/7/2008)
 */
class ODBCModelEnvironment : public ODBCModel
{
    Q_OBJECT
public:
    explicit ODBCModelEnvironment( ODBCQGEnvironment *pEnvironment );
    ~ODBCModelEnvironment();

    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    QIcon               getIcon();
    ODBCQGEnvironment * getEnvironment();

    bool doLoad();
    bool doClear();

private:
    ODBCMetaInfoAttr *  pEnvAttr;
    ODBCQGEnvironment * pEnvironment;
};

#endif

