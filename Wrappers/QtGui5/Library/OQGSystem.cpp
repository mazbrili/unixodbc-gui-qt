/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#include "OQGSystem.h"

OQGSystem::OQGSystem()
    : OQSystem()
{
    setObjectName( QString::fromLocal8Bit("OQGSystem") );
}

OQGSystem::~OQGSystem()
{
}

/*! 
 *  \f$     doManageDataSources
 *  \brief  Wrapper for SQLManageDataSources. Invoke a GUI (an ODBC Administrator).
 *  
 *  Q_OS_MACOS  We execute an external program 'ODBC Administrator.app'. In this case
 *              hWnd is ignored.
 *  
 *  Q_OS_UNIX   We assume unixODBC is being used.
 *              We use an ODBCINSTWND to ask unixODBC to use the odbcinstQ5 GUI plugin.
 *  
 *  Q_OS_WIN    We use the underlying windows handle for QWidget as parent for GUI and let
 *              Windows invoke its interface.
 *  
 */
#if defined(Q_OS_MACOS)
bool OQGSystem::doManageDataSources( QWidget * )
{
    if ( !OQSystem::doManageDataSources( NULL ) )
    {
        QMessageBox::warning( 0, metaObject()->className(),  tr( "Failed to execute the ODBC Administrator.\nIt should have been in /Applications/Utilities." ), QMessageBox::Ok );
        return false;
    }

	return true;
}
#elif defined(Q_OS_UNIX)
bool OQGSystem::doManageDataSources( QWidget *pwidgetParent )
{
    ODBCINSTWND odbcinstwnd;
    strcpy( odbcinstwnd.szUI, "odbcinstQ5" );
    odbcinstwnd.hWnd = (HWND)pwidgetParent;

    if ( !OQSystem::doManageDataSources( (HWND)(&odbcinstwnd) ) )
    {
        const QMetaObject *p = metaObject();
        const char *psz = p->className();
        QMessageBox::warning( pwidgetParent, QString::fromLocal8Bit(psz),  tr( "Failed to execute a Qt based ODBC Config tool. Is the unixODBC libodbcinstQ5 plugin installed?." ), QMessageBox::Ok );
        return false;
    }

    return true;
}
#elif defined(Q_OS_WIN)
bool OQGSystem::doManageDataSources( QWidget *pwidget )
{
    HWND hWnd = ( pwidget ? (HWND)(pwidget->winId()) : NULL );

    if ( !OQSystem::doManageDataSources( hWnd ) )
    {
        QMessageBox::warning( 0, metaObject()->className(),  tr( "Call to SQLManageDataSources returned false." ), QMessageBox::Ok );
        return false;
    }

    return true;
}
#else
    #error Platform not supported.
#endif

