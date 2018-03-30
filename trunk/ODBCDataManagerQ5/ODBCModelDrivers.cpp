/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#include "ODBCModelDrivers.h"

#include "ODBCModelDriver.h"

#include "Drivers48.xpm"

ODBCModelDrivers::ODBCModelDrivers( OQGEnvironment *pHandle, ODBCModel *pmodelParent )
    : ODBCModel( pHandle, pmodelParent )
{
    setObjectName( "Drivers" );
}

ODBCModelDrivers::~ODBCModelDrivers()
{
}

QVariant ODBCModelDrivers::data( const QModelIndex &index, int nRole ) const
{
    Q_UNUSED(index)
    Q_UNUSED(nRole)

    // no props
    return QVariant();
}

bool ODBCModelDrivers::setData( const QModelIndex &index, const QVariant &variantValue, int nRole )
{
    Q_UNUSED(index)
    Q_UNUSED(variantValue)
    Q_UNUSED(nRole)

    // no props
    return false;
}

QIcon ODBCModelDrivers::getIcon()
{
    return QIcon( QPixmap(xpmDrivers48) );
}

bool ODBCModelDrivers::doLoad()
{
    OQGEnvironment *pEnvironment = (OQGEnvironment *)(getHandle()->getParent( OQHandle::Env ));

    QStringList vectorDriverNames = pEnvironment->getDrivers();

    for ( int n = 0; n < vectorDriverNames.count(); n++ )
    {
        new ODBCModelDriver( pEnvironment, this, vectorDriverNames.at( n ) );
    }

    bLoaded = true;
    return true;
}

bool ODBCModelDrivers::doClear()
{
    bLoaded = false;
    return true;
}
