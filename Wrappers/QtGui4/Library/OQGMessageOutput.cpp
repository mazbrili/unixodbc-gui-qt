/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#include "OQGMessageOutput.h"

#include <OQMessage.h>
#include <OQDiagnostic.h>

#include <OQGSystem.h>

#include "Information16.xpm"
#include "Warning16.xpm"
#include "Error16.xpm"

OQGMessageOutput::OQGMessageOutput( QWidget *pwidgetParent )
    : QTableWidget( 0, 4, pwidgetParent )
{
    QStringList stringlistHeaderLabels;

    setColumnCount( 4 );
    setHorizontalHeaderLabels( stringlistHeaderLabels << tr( "" ) << tr( "SQLSTATE" ) << tr( "Native Code" ) << tr( "Message" ) );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    verticalHeader()->setVisible( false );

    nWaitingMessages = 0;
}

OQGMessageOutput::~OQGMessageOutput()
{
}

void OQGMessageOutput::setWaitingMessages( int n ) 
{ 
    nWaitingMessages = n; 
    emit signalWaitingMessages( this );
}


/*!
    slotMessage
    
    Append the message to our list of messages.
*/
void OQGMessageOutput::slotMessage( OQMessage Message )
{
    int nRows = rowCount() + 1;
    int nRow  = nRows - 1;

    // add row
    setRowCount( nRows );

    // column 1
    switch ( Message.getType() )
    {
        case OQMessage::Info:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( xpmInformation16 ), "" ) );
            break;
        case OQMessage::Error:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( xpmError16 ), "" ) );
            break;
        case OQMessage::Warning:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( xpmWarning16 ), "" ) );
            break;
    }

    // column 2
    setItem( nRow, 1, new QTableWidgetItem( Message.getRequest() ) );

    // column 3
    setItem( nRow, 2, new QTableWidgetItem( QString::number( Message.getNumeric() ) ) );

    // column 4
    setItem( nRow, 3, new QTableWidgetItem( Message.getText() ) );

    nWaitingMessages++;
    emit signalWaitingMessages( this );
}

void OQGMessageOutput::slotDiagnostic( OQDiagnostic Diagnostic )
{
    int nRows = rowCount() + 1;
    int nRow  = nRows - 1;

    // add row
    setRowCount( nRows );

    // column 1
    switch ( Diagnostic.getReturncode() )
    {
        case SQL_SUCCESS:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( xpmInformation16 ), "" ) );
            break;
        case SQL_SUCCESS_WITH_INFO:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( xpmWarning16 ), "" ) );
            break;
        default:
            setItem( nRow, 0, new QTableWidgetItem( QIcon( xpmError16 ), "" ) );
            break;
    }

    // column 2

    // column 3

    // column 4

    nWaitingMessages++;
    emit signalWaitingMessages( this );
}


