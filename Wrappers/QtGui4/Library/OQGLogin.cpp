/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#include "../include/OQGLogin.h"

#include <OQMessage.h>

#include "../include/OQGEnvironment.h"
#include "../include/OQGMessageOutput.h"

#include "ODBC64.xpm"

OQGLogin::OQGLogin( QWidget *pwidgetParent, OQGEnvironment *penvironment )
    : QDialog( pwidgetParent )
{
    setObjectName( "OQGLogin" );
    this->penvironment = penvironment;

    QVBoxLayout *playoutTop = new QVBoxLayout;

    // init fields...
    {
        QGridLayout *playoutFields = new QGridLayout;

        // DRIVER
        plabelDriver = new QLabel( tr( "Driver" ) );
        pcomboboxDriver = new QComboBox;
        playoutFields->addWidget( plabelDriver, 0, 0 );
        playoutFields->addWidget( pcomboboxDriver, 0, 1 );

        // DSN
        plabelDataSourceName = new QLabel( tr( "Data Source Name" ) );
        pcomboboxDataSourceName = new QComboBox;
        playoutFields->addWidget( plabelDataSourceName, 1, 0 );
        playoutFields->addWidget( pcomboboxDataSourceName, 1, 1 );

        // UID
        plabelUserID  = new QLabel( tr( "User ID" ) );
        plineeditUserID = new QLineEdit;
    #ifndef Q_WS_WIN
        plineeditUserID->setText( ((struct passwd *)getpwuid(getuid()))->pw_name );
    #endif
        plineeditUserID->setToolTip( tr( "your User ID\nHINT: you get this from your database administrator" ) );
        playoutFields->addWidget( plabelUserID, 2, 0 );
        playoutFields->addWidget( plineeditUserID, 2, 1 );

        // PWD
        plabelPassword = new QLabel( tr( "Password" ) );
        plineeditPassword = new QLineEdit;
        plineeditPassword->setEchoMode( QLineEdit::Password );
        plineeditPassword->setToolTip( tr( "your password\nHINT: sometimes; you can leave this blank" ) );
        playoutFields->addWidget( plabelPassword, 3, 0 );
        playoutFields->addWidget( plineeditPassword, 3, 1 );

        playoutFields->setRowStretch( 4, 10 );
        playoutTop->addLayout( playoutFields );
    }

    // init dialog buttons...
    {
        QDialogButtonBox *  pDialogButtonBox    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );
        QFrame *            pFrame              = new QFrame;

        pFrame->setFrameStyle( QFrame::HLine );

        connect( pDialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()) );
        connect( pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()) );

        playoutTop->addWidget( pFrame );
        playoutTop->addWidget( pDialogButtonBox );
    }

    // messages
    {
        pmessageoutput = new OQGMessageOutput( this );
        playoutTop->addWidget( pmessageoutput );
//        pmessageoutput->hide();
//        pmessageoutput->setMinimumHeight( 50 );
//        setExtension( pmessageoutput );
//        setOrientation( Qt::Vertical );

        // connect( penvironment, SIGNAL(signalMessage(OQMessage)), pmessageoutput, SLOT(slotMessage(OQMessage)) );

    //    connect( pbMessages, SIGNAL(toggled(bool)), this, SLOT(showExtension(bool)) );
    }

    // 
    setLayout( playoutTop );
    setWindowTitle( tr( "Connect..." ) );
    setWindowIcon( QPixmap( xpmODBC64 ) );
    
#ifndef Q_WS_WIN
    plineeditPassword->setFocus();
#else
	plineeditUserID->setFocus();
#endif

printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
    // do these last so we catch any errors with slotMessage
    loadDrivers();
printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
    loadDataSourceNames();
printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
}

OQGLogin::~OQGLogin()
{
}


void OQGLogin::setShowDriver( bool b )
{
    if ( b )
    {
        plabelDriver->show();
        pcomboboxDriver->show();
    }
    else
    {
        plabelDriver->hide();
        pcomboboxDriver->hide();
    }
}

void OQGLogin::setShowDataSourceName( bool b )
{
    if ( b )
    {
        plabelDataSourceName->show();
        pcomboboxDataSourceName->show();
    }
    else
    {
        plabelDataSourceName->hide();
        pcomboboxDataSourceName->hide();
    }
}

void OQGLogin::setShowUserID( bool b )
{
    if ( b )
    {
        plabelUserID->show();
        plineeditUserID->show();
    }
    else
    {
        plabelUserID->hide();
        plineeditUserID->hide();
    }
}

void OQGLogin::setShowPassword( bool b )
{
    if ( b )
    {
        plabelPassword->show();
        plineeditPassword->show();
    }
    else
    {
        plabelPassword->hide();
        plineeditPassword->hide();
    }
}

void OQGLogin::setDriver( const QString &string )
{
/*
    QListBoxItem *plistboxitem = pcomboboxDriver->listBox()->findItem( string );
    if ( plistboxitem )
        pcomboboxDriver->listBox()->setCurrentItem( plistboxitem );
*/
    pcomboboxDriver->setEditText( string );
}

void OQGLogin::setDataSourceName( const QString &string )
{
    pcomboboxDataSourceName->setEditText( string );
}

void OQGLogin::setUserID( const QString &string )
{
    plineeditUserID->setText( string );
}

void OQGLogin::setPassword( const QString &string )
{
    plineeditPassword->setText( string );
}

void OQGLogin::loadDrivers()
{
printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
    pcomboboxDriver->insertItems( 0, penvironment->getDrivers() );
printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
    pcomboboxDriver->insertItem( 0, "" );
printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
}

void OQGLogin::loadDataSourceNames()
{
    pcomboboxDataSourceName->insertItems( 0, penvironment->getDataSources() );
    pcomboboxDataSourceName->insertItem( 0, "" );
}


