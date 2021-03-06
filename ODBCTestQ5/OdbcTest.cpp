/*********************************************************************
 *
 * Written by Nick Gorham
 * (nick@lurcher.org).
 *
 * copyright (c) 1999 Nick Gorham
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 **********************************************************************
 *
 * 2008-12-10   Code adopted by unixODBC-GUI-Qt project. Heavily altered
 * pharvey      to be Qt4 only code.
 * 
 **********************************************************************/

#include "OdbcTest.h"
#include "ODBCTestQ5-48.xpm"

#include "DlgEnvAllocHandle.h"
#include "DlgEnvDataSources.h"
#include "DlgEnvDrivers.h"
#include "DlgEnvEndTran.h"
#include "DlgEnvFreeHandle.h"
#include "DlgEnvAllocEnv.h"
#include "DlgEnvFreeEnv.h"
#include "DlgEnvTransact.h"

OdbcTest::OdbcTest( QWidget *parent )
: QMainWindow( parent )
{
    setWindowTitle( "ODBC Test" );
    setWindowIcon( QPixmap( xpmODBCTestQ5_48 ) );

    /*! 
     *  Open the test configuration file once and allow all supporting code to use it. Let Qt
     *  delete the object when this (its parent) deconstructs. The settings will automatically
     *  sync to disk at that time.
     *  
     *  \note   Using QSettings to read/write ini files imposes some limitations. The main
     *          one is that slashs in directory names need to be escaped. There may also be
     *          issues with the order in which things are read.
     */
    pSettings = new QSettings( QDir::homePath() + "/Gator.ini", QSettings::IniFormat, this );

    createActions();
    createMenus();

    this->setGeometry( 0, 0, 500, 250 );

    split = new QSplitter( Qt::Vertical, this );
    split->setOpaqueResize( FALSE );
    setCentralWidget( split );
    in_win  = new QTextEdit( split );
    out_win = new QTextEdit( split );
    out_win->setReadOnly( TRUE );

    readApplicationState();
}

OdbcTest::~OdbcTest()
{
    writeApplicationState();

    // delete all handles
    while ( !listHandle.isEmpty() )
        delete listHandle.takeFirst();
}

void OdbcTest::slotExit()
{
    qApp->quit();
}

const char * OdbcTest::return_as_text( int ret )
{
    switch ( ret )
    {
        case SQL_SUCCESS:
            return "SQL_SUCCESS";

        case SQL_SUCCESS_WITH_INFO:
            return "SQL_SUCCESS_WITH_INFO";

        case SQL_ERROR:
            return "SQL_ERROR";

        case SQL_INVALID_HANDLE:
            return "SQL_INVALID_HANDLE";

        case SQL_STILL_EXECUTING:
            return "SQL_STILL_EXECUTING";

        case SQL_NEED_DATA:
            return "SQL_NEED_DATA";

        case SQL_NO_DATA:
            return "SQL_NO_DATA";

        default:
            return "Unknown Return ?";
    }
}

void OdbcTest::fill_list_box( attr_value *attr, QComboBox *lst )
{
    char txt[ 128 ];

    for ( int i = 0; attr[ i ].text; i ++ )
    {
        if ( strlen( attr[ i ].text ) > 0 )
        {
            if ( attr[ i ].version )
            {
                sprintf( txt, "%s=%d (%s)", attr[ i ].text, 
                         attr[ i ].value, attr[ i ].version );
            }
            else
            {
                sprintf( txt, "%s=%d", attr[ i ].text, 
                         attr[ i ].value );
            }
        }
        else
        {
            sprintf( txt, "%d", attr[ i ].value );
        }
        lst->insertItem( i, txt );
    }
}

void OdbcTest::fill_list_box( attr_options *attr, QComboBox *lst )
{
    char txt[ 128 ];

    for ( int i = 0; attr[ i ].text; i ++ )
    {
        if ( strlen( attr[ i ].text ) > 0 )
        {
            if ( attr[ i ].version )
            {
                sprintf( txt, "%s=%d (%s)", attr[ i ].text, 
                         attr[ i ].attr, attr[ i ].version );
            }
            else
            {
                sprintf( txt, "%s=%d", attr[ i ].text, 
                         attr[ i ].attr );
            }
        }
        else
        {
            sprintf( txt, "%d", attr[ i ].attr );
        }
        lst->insertItem( i, txt );
    }
}

SQLHANDLE OdbcTest::get_handle( int type )
{
    QListIterator<OdbcHandle*> it( listHandle );

    while ( it.hasNext() )
    {
        OdbcHandle *hand = it.next();
        if ( hand && hand->getType() == type )
        {
            return hand->getHandle();
        }
    }

    return SQL_NULL_HANDLE;
}

OdbcHandle * OdbcTest::fill_handle_list( int type, QComboBox *lst )
{
    QListIterator<OdbcHandle*>  it( listHandle );
    OdbcHandle *                match   = NULL;
    int                         i       = 0;
    char                        txt[128];

    while ( it.hasNext() )
    {
        OdbcHandle *hand = it.next();
        if ( !hand  )
            break;

        hand->toStr( txt );
        if ( type < 0 )
        {
            lst->insertItem( i++, txt );
            match = hand;
        }
        else if ( hand->getType() == type )
        {
            lst->insertItem( i++, txt );
            match = hand;
        }
    }

    lst->insertItem( i, "SQL_NULL_HANDLE" );

    return match;
}

OdbcHandle *OdbcTest::extract_handle_list( int type, QComboBox *lst )
{
    QListIterator<OdbcHandle*>  it( listHandle );
    int                         index   = lst->currentIndex(); 
    int                         i       = 0;
    OdbcHandle *                hand    = NULL;

    while ( it.hasNext() )
    {
        hand = it.next();
        if ( !hand  )
            break;
        if ( type < 0 || hand->getType() == type )
        {
            if ( i == index )
            {
                break;
            }
            i++;
        }
    }

    return hand;
}

void OdbcTest::fill_dsn_list( QComboBox *box )
{
    SQLHANDLE henv;
    SQLRETURN ret;
    SQLCHAR dsn_txt[ 128 ];

    box -> clear();

    ret = SQLAllocEnv( &henv );

    if ( !SQL_SUCCEEDED( ret ))
    {
        out_win -> append( "SQLAllocHandle fails allocating a environment handle" );
        return;
    }

    ret = SQLDataSources( henv,
                          SQL_FETCH_FIRST,
                          dsn_txt,
                          sizeof( dsn_txt ),
                          NULL,
                          NULL,
                          0,
                          NULL );

    while ( SQL_SUCCEEDED( ret ))
    {
        box -> addItem( (char*)dsn_txt );
        ret = SQLDataSources( henv,
                              SQL_FETCH_NEXT,
                              dsn_txt,
                              sizeof( dsn_txt ),
                              NULL,
                              NULL,
                              0,
                              NULL );
    }

    SQLFreeEnv( henv );
}

void OdbcTest::set_dsn_list( QComboBox *box, const QString &stringItem )
{
    int index;

    for ( index = 0; index < box->count(); index ++ )
    {        
        if ( box->itemText( index ) == stringItem )
        {
            box->setCurrentIndex( index );
            break;
        }
    }
}

void OdbcTest::dumpError( int type, SQLHANDLE hnd )
{
    SQLRETURN ret;
    SQLINTEGER count, i;
    char handle[ 30 ];

    switch ( type )
    {
        case SQL_HANDLE_ENV:
            sprintf( handle, "henv %p", hnd );
            break;

        case SQL_HANDLE_DBC:
            sprintf( handle, "hdbc %p", hnd );
            break;

        case SQL_HANDLE_STMT:
            sprintf( handle, "hstmt %p", hnd );
            break;

        case SQL_HANDLE_DESC:
            sprintf( handle, "hdesc %p", hnd );
            break;

        default:
            strcpy( handle, "unknown handle" );
            break;
    }

    ret = SQLGetDiagField( type, hnd, 0, SQL_DIAG_NUMBER,
                           &count, 0, NULL );

    if ( !SQL_SUCCEEDED( ret ))
    {
        out_win->append( "SQLGetDiagField( SQL_DIAG_NUMBER ) failed" );
        return;
    }

    for ( i = 1; i <= count; i ++ )
    {
        SQLCHAR sqlstate[ 6 ];
        SQLINTEGER native;
        SQLCHAR message_text[ SQL_MAX_MESSAGE_LENGTH ];
        char txt[ SQL_MAX_MESSAGE_LENGTH + 50 ];

        ret = SQLGetDiagRec( type, hnd, i, sqlstate,
                             &native, message_text, sizeof( message_text ),
                             NULL );

        if ( !SQL_SUCCEEDED( ret ))
        {
            out_win->append( "SQLGetDiagRec() failed" );
            return;
        }

        sprintf( txt, "Diag(%s):%s:%d:%s",
                 handle, sqlstate, native, message_text );

        out_win->append( txt );
    }
}

void OdbcTest::slotHelpApi()
{
    QMessageBox::about( this, "ODBC Test",
                        "Not yet implemented" );
}

void OdbcTest::slotHelpTest()
{
    QMessageBox::about( this, "ODBC Test",
                        "Not yet implemented" );
}

void OdbcTest::slotHelpAbout()
{
    /*! 
     * \todo
     *
     *  1. create unixODBC.adp
     *  2. get unixODBC.adp and associated doc to install in various install scenarios
     *  3. tweek the way we get the path to the doc so its more portable (currently just Linux friendly)
     *
     * \sa
     *
     *  slotHelp
     */
    QProcess *process = new QProcess( this );
    QString app = QLibraryInfo::location( QLibraryInfo::BinariesPath ) + QLatin1String( "/assistant" );

    process->start( app, QStringList() << QLatin1String( "-enableRemoteControl" ) );
    if ( !process->waitForStarted() )
    {
        QMessageBox::critical(this, tr("ODBC Test"), tr("Could not start Qt Assistant from %1.").arg( app ) );
        return;
    }

    // show index page
    // "SetSource qthelp://unixODBC-GUI-Qt/doc/index.html"
    QTextStream str( process );
    str << QLatin1String( "SetSource qthelp://org.unixODBC.doc/doc/ODBCTestQ4/index.html" )
    << QLatin1Char('\0') << endl;
}

void OdbcTest::resizeEvent( QResizeEvent * )
{
    // label->setGeometry( 20, rect().center().y()-20, width()-40, 40 );
}

const char *OdbcTest::int_type_as_string( SQLINTERVAL itype )
{
    switch ( itype )
    {
        case SQL_IS_YEAR:
            return "SQL_IS_YEAR";

        case SQL_IS_MONTH:
            return "SQL_IS_MONTH";

        case SQL_IS_DAY:
            return "SQL_IS_DAY";

        case SQL_IS_HOUR:
            return "SQL_IS_HOUR";

        case SQL_IS_MINUTE:
            return "SQL_IS_MINUTE";

        case SQL_IS_SECOND:
            return "SQL_IS_SECOND";

        case SQL_IS_YEAR_TO_MONTH:
            return "SQL_IS_YEAR_TO_MONTH";

        case SQL_IS_DAY_TO_HOUR :
            return "SQL_IS_DAY_TO_HOUR";

        case SQL_IS_DAY_TO_MINUTE:
            return "SQL_IS_DAY_TO_MINUTE";

        case SQL_IS_DAY_TO_SECOND:
            return "SQL_IS_DAY_TO_SECOND";

        case SQL_IS_HOUR_TO_MINUTE:
            return "SQL_IS_HOUR_TO_MINUTE";

        case SQL_IS_HOUR_TO_SECOND:
            return "SQL_IS_HOUR_TO_SECOND";

        case SQL_IS_MINUTE_TO_SECOND:
            return "SQL_IS_MINUTE_TO_SECOND";
    }

    return "Unknown Type";
}   

void OdbcTest::createActions()
{
    pactionExit                             = new QAction( tr("E&xit"), this );    
    pactionGetDiagRec                       = new QAction( tr("SQLGetDiag&Rec..."), this );    
    pactionGetDiagField                     = new QAction( tr("SQLGetDiag&Field..."), this );    
    pactionError                            = new QAction( tr("SQL&Error..."), this );    
    pactionErrorsAll                        = new QAction( tr("Errors &All..."), this );    
    pactionAllocEnv                         = new QAction( tr("SQL&AllocEnv..."), this );    
    pactionAllocHandle                      = new QAction( tr("SQL&AllocHandle..."), this );    
    pactionDataSources                      = new QAction( tr("SQLDataS&ources..."), this );    
    pactionDrivers                          = new QAction( tr("S&QLDrivers..."), this );    
    pactionFreeEnv                          = new QAction( tr("SQL&FreeEnv..."), this );    
    pactionFreeHandle                       = new QAction( tr("SQLF&reeHandle..."), this );    
    pactionEndTran                          = new QAction( tr("SQLE&ndTran..."), this );    
    pactionTransact                         = new QAction( tr("SQL&Transact..."), this );    
    pactionDataSourcesAll                   = new QAction( tr("Data &Sources All..."), this );    
    pactionDriversAll                       = new QAction( tr("Dri&vers All..."), this );    
    pactionAllocConnect                     = new QAction( tr("SQL&AllocConnect..."), this );    
    pactionBrowseConnect                    = new QAction( tr("SQL&BrowseConnect..."), this );    
    pactionConnect                          = new QAction( tr("SQL&Connect..."), this );    
    pactionDriverConnect                    = new QAction( tr("SQLD&riverConnect..."), this );    
    pactionDisconnect                       = new QAction( tr("SQL&Disconnect..."), this );    
    pactionFreeConnect                      = new QAction( tr("SQL&FreeConnect..."), this );    
    pactionGetFunctions                     = new QAction( tr("S&QLGetFunctions..."), this );    
    pactionGetInfo                          = new QAction( tr("SQLGet&Info..."), this );    
    pactionNativeSql                        = new QAction( tr("SQLN&ativeSQL..."), this );    
    pactionFullConnect                      = new QAction( tr("F&ull Connect..."), this );    
    pactionFullDisconnect                   = new QAction( tr("Fu&ll Disconnect..."), this );    
    pactionGetInfoAll                       = new QAction( tr("G&et Info All..."), this );    
    pactionGetFunctionsAll                  = new QAction( tr("Get &Functions All..."), this );    
    pactionCopyDesc                         = new QAction( tr("SQLCop&yDesc..."), this );    
    pactionGetDescField                     = new QAction( tr("SQLGetDesc&Field..."), this );    
    pactionGetDescRec                       = new QAction( tr("SQLG&etDescRec..."), this );    
    pactionSetDescField                     = new QAction( tr("SQLSetDescF&ield..."), this );    
    pactionSetDescRec                       = new QAction( tr("SQLSetDescRec..."), this );    
    pactionGetDescAll                       = new QAction( tr("Get Desc All..."), this );    
    pactionAllocStmt                        = new QAction( tr("SQLAllocStmt..."), this );    
    pactionBindParam                        = new QAction( tr("SQLBindParam,..."), this );    
    pactionBindParameter                    = new QAction( tr("SQLBindParameter..."), this );    
    pactionCancel                           = new QAction( tr("SQLCancel..."), this );    
    pactionCloseCursor                      = new QAction( tr("SQLCloseCursor..."), this );    
    pactionDescribeParam                    = new QAction( tr("SQLDescribeParam..."), this );    
    pactionExecute                          = new QAction( tr("SQLExecute..."), this );    
    pactionExecDirect                       = new QAction( tr("SQLExecDirect..."), this );    
    pactionFreeStmt                         = new QAction( tr("SQLFreeStmt..."), this );    
    pactionGetCursorName                    = new QAction( tr("SQLGetCursorName..."), this );    
    pactionNumParams                        = new QAction( tr("SQLNumParams..."), this );    
    pactionParamData                        = new QAction( tr("SQLParamData..."), this );    
    pactionParamOptions                     = new QAction( tr("SQLParamOptions..."), this );    
    pactionPrepare                          = new QAction( tr("SQLPrepare..."), this );    
    pactionPutData                          = new QAction( tr("SQLPutData..."), this );    
    pactionSetCursorName                    = new QAction( tr("SQLSetCursorName..."), this );    
    pactionFillParam                        = new QAction( tr("Fill Param..."), this );    
    pactionShowParam                        = new QAction( tr("Show Param..."), this );    
    pactionShowCursorSettings               = new QAction( tr("Show Cursor Settings..."), this );    
    pactionSetStmtAttr                      = new QAction( tr("SQLSetStmtAttr..."), this );    
    pactionGetStmtAttr                      = new QAction( tr("SQLGetStmtAttr..."), this );    
    pactionSetStmtOption                    = new QAction( tr("SQLSetStmtOption..."), this );    
    pactionGetStmtOption                    = new QAction( tr("SQLGetStmtOption..."), this );    
    pactionSetConnectAttr                   = new QAction( tr("SQLSetConnectAttr..."), this );    
    pactionGetConnectAttr                   = new QAction( tr("SQLGetConnectAttr..."), this );    
    pactionSetConnectOption                 = new QAction( tr("SQLSetConnectOption..."), this );    
    pactionGetConnectOption                 = new QAction( tr("SQLGetConnectOption..."), this );    
    pactionSetEnvAttr                       = new QAction( tr("SQLSetEnvAttr..."), this );    
    pactionGetEnvAttr                       = new QAction( tr("SQLGetEnvAttr..."), this );    
    pactionSetCursorAttributes              = new QAction( tr("Set Cursor Attributes..."), this );    
    pactionBindCol                          = new QAction( tr("SQLBindCol..."), this );    
    pactionBulkOperations                   = new QAction( tr("SQLBulkOperations..."), this );    
    pactionColAttributes                    = new QAction( tr("SQLColAttributes..."), this );    
    pactionColAttribute                     = new QAction( tr("SQLColAttribute..."), this );    
    pactionDescribeCol                      = new QAction( tr("SQLDescribeCol..."), this );    
    pactionExtendedFetch                    = new QAction( tr("SQLExtendedFetch..."), this );    
    pactionFetch                            = new QAction( tr("SQLFetch..."), this );    
    pactionFetchScroll                      = new QAction( tr("SQLFetchScroll..."), this );    
    pactionGetData                          = new QAction( tr("SQLGetData..."), this );    
    pactionMoreResults                      = new QAction( tr("SQLMoreResults..."), this );    
    pactionNumResultCols                    = new QAction( tr("SQLNumResultCols..."), this );    
    pactionRowCount                         = new QAction( tr("SQLRowCount..."), this );    
    pactionSetPos                           = new QAction( tr("SQLSetPos..."), this );    
    pactionSetScrollOptions                 = new QAction( tr("SQLSetScrollOptions..."), this );    
    pactionBindColAll                       = new QAction( tr("Bind Col All..."), this );    
    pactionDescribeColAll                   = new QAction( tr("Describe Col All..."), this );    
    pactionFetchAll                         = new QAction( tr("Fetch All..."), this );    
    pactionGetDataAll                       = new QAction( tr("Get Data All..."), this );    
    pactionShowBoundCols                    = new QAction( tr("Show Bound Cols..."), this );    
    pactionDisplayRowSet                    = new QAction( tr("Display Row Set..."), this );    
    pactionColumns                          = new QAction( tr("SQLColumns..."), this );    
    pactionColumnPrivileges                 = new QAction( tr("SQLColumnPrivileges..."), this );    
    pactionGetTypeInfo                      = new QAction( tr("SQLGetTypeInfo..."), this );    
    pactionForeignKeys                      = new QAction( tr("SQLForeignKeys..."), this );    
    pactionPrimaryKeys                      = new QAction( tr("SQLPrimaryKeys..."), this );    
    pactionProcedures                       = new QAction( tr("SQLProcedures..."), this );    
    pactionProcedureColumns                 = new QAction( tr("SQLProcedureColumns..."), this );    
    pactionSpecialColumns                   = new QAction( tr("SQLSpecialColumns..."), this );    
    pactionStatistics                       = new QAction( tr("SQLStatistics..."), this );    
    pactionTables                           = new QAction( tr("SQLTables..."), this );    
    pactionTablePrivileges                  = new QAction( tr("SQLTablePrivileges..."), this );    
    pactionManageDataSources                = new QAction( tr("SQLManageDataSources..."), this );    
    pactionRemoveDefaultDataSource          = new QAction( tr("SQLRemoveDefaultDataSource..."), this );    
    pactionConfigDataSource                 = new QAction( tr("SQLConfigDataSource..."), this );    
    pactionCreateDataSource                 = new QAction( tr("SQLCreateDataSource..."), this );    
    pactionValidDSN                         = new QAction( tr("SQLValidDSN..."), this );    
    pactionRemoveDSNFromIni                 = new QAction( tr("SQLRemoveDSNFromIni..."), this );    
    pactionWriteDSNToIni                    = new QAction( tr("SQLWriteDSNToIni..."), this );    
    pactionRemoveDrivers                    = new QAction( tr("SQLRemoveDrivers..."), this );    
    pactionConfigDrivers                    = new QAction( tr("SQLConfigDrivers..."), this );    
    pactionInstallDriver                    = new QAction( tr("SQLInstallDriver..."), this );    
    pactionInstallDriverEx                  = new QAction( tr("SQLInstallDriverEx..."), this );    
    pactionGetInstalledDrivers              = new QAction( tr("SQLGetInstalledDrivers..."), this );    
    pactionRemoveDriverManager              = new QAction( tr("SQLRemoveDriverManager..."), this );    
    pactionInstallDriverManager             = new QAction( tr("SQLInstallDriverManager..."), this );    
    pactionReadFileDSN                      = new QAction( tr("SQLReadFileDSN..."), this );    
    pactionWriteFileDSN                     = new QAction( tr("SQLWriteFileDSN..."), this );    
    pactionWritePrivateProfileString        = new QAction( tr("SQLWritePrivateProfileString..."), this );    
    pactionGetPrivateProfileString          = new QAction( tr("SQLGetPrivateProfileString..."), this );    
    pactionInstallTranslator                = new QAction( tr("SQLInstallTranslator..."), this );    
    pactionInstallTranslatorEx              = new QAction( tr("SQLInstallTranslatorEx..."), this );    
    pactionRemoveTranslator                 = new QAction( tr("SQLRemoveTranslator..."), this );    
    pactionGetTranslator                    = new QAction( tr("SQLGetTranslator..."), this );    
    pactionSetConfigMode                    = new QAction( tr("SQLSetConfigMode..."), this );    
    pactionGetConfigMode                    = new QAction( tr("SQLGetConfigMode..."), this );    
    pactionOptions                          = new QAction( tr("Options..."), this );    
    pactionTrace                            = new QAction( tr("Trace..."), this );    
    pactionManageTestSources                = new QAction( tr("Manage Test Sources..."), this );    
    pactionManageAutoTests                  = new QAction( tr("Manage Auto Tests..."), this );    
    pactionManageTestGroups                 = new QAction( tr("Manage Test Groups..."), this );    
    pactionRunAutoTests                     = new QAction( tr("Run Auto Tests..."), this );    
    pactionHelpTest                         = new QAction( tr("&ODBC Test..."), this );    
    pactionHelpApi                          = new QAction( tr("A&PI..."), this );    
    pactionHelpAbout                        = new QAction( tr("About..."), this );    

    connect( pactionExit                     , SIGNAL(triggered()), this, SLOT(slotExit                     ()) );
    connect( pactionGetDiagRec               , SIGNAL(triggered()), this, SLOT(slotGetDiagRec               ()) );
    connect( pactionGetDiagField             , SIGNAL(triggered()), this, SLOT(slotGetDiagField             ()) );
    connect( pactionError                    , SIGNAL(triggered()), this, SLOT(slotError                    ()) );
    connect( pactionErrorsAll                , SIGNAL(triggered()), this, SLOT(slotErrorsAll                ()) );
    connect( pactionAllocEnv                 , SIGNAL(triggered()), this, SLOT(slotAllocEnv                 ()) );
    connect( pactionAllocHandle              , SIGNAL(triggered()), this, SLOT(slotAllocHandle              ()) );
    connect( pactionDataSources              , SIGNAL(triggered()), this, SLOT(slotDataSources              ()) );
    connect( pactionDrivers                  , SIGNAL(triggered()), this, SLOT(slotDrivers                  ()) );
    connect( pactionFreeEnv                  , SIGNAL(triggered()), this, SLOT(slotFreeEnv                  ()) );
    connect( pactionFreeHandle               , SIGNAL(triggered()), this, SLOT(slotFreeHandle               ()) );
    connect( pactionEndTran                  , SIGNAL(triggered()), this, SLOT(slotEndTran                  ()) );
    connect( pactionTransact                 , SIGNAL(triggered()), this, SLOT(slotTransact                 ()) );
    connect( pactionDataSourcesAll           , SIGNAL(triggered()), this, SLOT(slotDataSourcesAll           ()) );
    connect( pactionDriversAll               , SIGNAL(triggered()), this, SLOT(slotDriversAll               ()) );
    connect( pactionAllocConnect             , SIGNAL(triggered()), this, SLOT(slotAllocConnect             ()) );
    connect( pactionBrowseConnect            , SIGNAL(triggered()), this, SLOT(slotBrowseConnect            ()) );
    connect( pactionConnect                  , SIGNAL(triggered()), this, SLOT(slotConnect                  ()) );
    connect( pactionDriverConnect            , SIGNAL(triggered()), this, SLOT(slotDriverConnect            ()) );
    connect( pactionDisconnect               , SIGNAL(triggered()), this, SLOT(slotDisconnect               ()) );
    connect( pactionFreeConnect              , SIGNAL(triggered()), this, SLOT(slotFreeConnect              ()) );
    connect( pactionGetFunctions             , SIGNAL(triggered()), this, SLOT(slotGetFunctions             ()) );
    connect( pactionGetInfo                  , SIGNAL(triggered()), this, SLOT(slotGetInfo                  ()) );
    connect( pactionNativeSql                , SIGNAL(triggered()), this, SLOT(slotNativeSql                ()) );
    connect( pactionFullConnect              , SIGNAL(triggered()), this, SLOT(slotFullConnect              ()) );
    connect( pactionFullDisconnect           , SIGNAL(triggered()), this, SLOT(slotFullDisconnect           ()) );
    connect( pactionGetInfoAll               , SIGNAL(triggered()), this, SLOT(slotGetInfoAll               ()) );
    connect( pactionGetFunctionsAll          , SIGNAL(triggered()), this, SLOT(slotGetFunctionsAll          ()) );
    connect( pactionCopyDesc                 , SIGNAL(triggered()), this, SLOT(slotCopyDesc                 ()) );
    connect( pactionGetDescField             , SIGNAL(triggered()), this, SLOT(slotGetDescField             ()) );
    connect( pactionGetDescRec               , SIGNAL(triggered()), this, SLOT(slotGetDescRec               ()) );
    connect( pactionSetDescField             , SIGNAL(triggered()), this, SLOT(slotSetDescField             ()) );
    connect( pactionSetDescRec               , SIGNAL(triggered()), this, SLOT(slotSetDescRec               ()) );
    connect( pactionGetDescAll               , SIGNAL(triggered()), this, SLOT(slotGetDescAll               ()) );
    connect( pactionAllocStmt                , SIGNAL(triggered()), this, SLOT(slotAllocStmt                ()) );
    connect( pactionBindParam                , SIGNAL(triggered()), this, SLOT(slotBindParam                ()) );
    connect( pactionBindParameter            , SIGNAL(triggered()), this, SLOT(slotBindParameter            ()) );
    connect( pactionCancel                   , SIGNAL(triggered()), this, SLOT(slotCancel                   ()) );
    connect( pactionCloseCursor              , SIGNAL(triggered()), this, SLOT(slotCloseCursor              ()) );
    connect( pactionDescribeParam            , SIGNAL(triggered()), this, SLOT(slotDescribeParam            ()) );
    connect( pactionExecute                  , SIGNAL(triggered()), this, SLOT(slotExecute                  ()) );
    connect( pactionExecDirect               , SIGNAL(triggered()), this, SLOT(slotExecDirect               ()) );
    connect( pactionFreeStmt                 , SIGNAL(triggered()), this, SLOT(slotFreeStmt                 ()) );
    connect( pactionGetCursorName            , SIGNAL(triggered()), this, SLOT(slotGetCursorName            ()) );
    connect( pactionNumParams                , SIGNAL(triggered()), this, SLOT(slotNumParams                ()) );
    connect( pactionParamData                , SIGNAL(triggered()), this, SLOT(slotParamData                ()) );
    connect( pactionParamOptions             , SIGNAL(triggered()), this, SLOT(slotParamOptions             ()) );
    connect( pactionPrepare                  , SIGNAL(triggered()), this, SLOT(slotPrepare                  ()) );
    connect( pactionPutData                  , SIGNAL(triggered()), this, SLOT(slotPutData                  ()) );
    connect( pactionSetCursorName            , SIGNAL(triggered()), this, SLOT(slotSetCursorName            ()) );
    connect( pactionFillParam                , SIGNAL(triggered()), this, SLOT(slotFillParam                ()) );
    connect( pactionShowParam                , SIGNAL(triggered()), this, SLOT(slotShowParam                ()) );
    connect( pactionShowCursorSettings       , SIGNAL(triggered()), this, SLOT(slotShowCursorSettings       ()) );
    connect( pactionSetStmtAttr              , SIGNAL(triggered()), this, SLOT(slotSetStmtAttr              ()) );
    connect( pactionGetStmtAttr              , SIGNAL(triggered()), this, SLOT(slotGetStmtAttr              ()) );
    connect( pactionSetStmtOption            , SIGNAL(triggered()), this, SLOT(slotSetStmtOption            ()) );
    connect( pactionGetStmtOption            , SIGNAL(triggered()), this, SLOT(slotGetStmtOption            ()) );
    connect( pactionSetConnectAttr           , SIGNAL(triggered()), this, SLOT(slotSetConnectAttr           ()) );
    connect( pactionGetConnectAttr           , SIGNAL(triggered()), this, SLOT(slotGetConnectAttr           ()) );
    connect( pactionSetConnectOption         , SIGNAL(triggered()), this, SLOT(slotSetConnectOption         ()) );
    connect( pactionGetConnectOption         , SIGNAL(triggered()), this, SLOT(slotGetConnectOption         ()) );
    connect( pactionSetEnvAttr               , SIGNAL(triggered()), this, SLOT(slotSetEnvAttr               ()) );
    connect( pactionGetEnvAttr               , SIGNAL(triggered()), this, SLOT(slotGetEnvAttr               ()) );
    connect( pactionSetCursorAttributes      , SIGNAL(triggered()), this, SLOT(slotSetCursorAttributes      ()) );
    connect( pactionBindCol                  , SIGNAL(triggered()), this, SLOT(slotBindCol                  ()) );
    connect( pactionBulkOperations           , SIGNAL(triggered()), this, SLOT(slotBulkOperations           ()) );
    connect( pactionColAttributes            , SIGNAL(triggered()), this, SLOT(slotColAttributes            ()) );
    connect( pactionColAttribute             , SIGNAL(triggered()), this, SLOT(slotColAttribute             ()) );
    connect( pactionDescribeCol              , SIGNAL(triggered()), this, SLOT(slotDescribeCol              ()) );
    connect( pactionExtendedFetch            , SIGNAL(triggered()), this, SLOT(slotExtendedFetch            ()) );
    connect( pactionFetch                    , SIGNAL(triggered()), this, SLOT(slotFetch                    ()) );
    connect( pactionFetchScroll              , SIGNAL(triggered()), this, SLOT(slotFetchScroll              ()) );
    connect( pactionGetData                  , SIGNAL(triggered()), this, SLOT(slotGetData                  ()) );
    connect( pactionMoreResults              , SIGNAL(triggered()), this, SLOT(slotMoreResults              ()) );
    connect( pactionNumResultCols            , SIGNAL(triggered()), this, SLOT(slotNumResultCols            ()) );
    connect( pactionRowCount                 , SIGNAL(triggered()), this, SLOT(slotRowCount                 ()) );
    connect( pactionSetPos                   , SIGNAL(triggered()), this, SLOT(slotSetPos                   ()) );
    connect( pactionSetScrollOptions         , SIGNAL(triggered()), this, SLOT(slotSetScrollOptions         ()) );
    connect( pactionBindColAll               , SIGNAL(triggered()), this, SLOT(slotBindColAll               ()) );
    connect( pactionDescribeColAll           , SIGNAL(triggered()), this, SLOT(slotDescribeColAll           ()) );
    connect( pactionFetchAll                 , SIGNAL(triggered()), this, SLOT(slotFetchAll                 ()) );
    connect( pactionGetDataAll               , SIGNAL(triggered()), this, SLOT(slotGetDataAll               ()) );
    connect( pactionShowBoundCols            , SIGNAL(triggered()), this, SLOT(slotShowBoundCols            ()) );
    connect( pactionDisplayRowSet            , SIGNAL(triggered()), this, SLOT(slotDisplayRowSet            ()) );
    connect( pactionColumns                  , SIGNAL(triggered()), this, SLOT(slotColumns                  ()) );
    connect( pactionColumnPrivileges         , SIGNAL(triggered()), this, SLOT(slotColumnPrivileges         ()) );
    connect( pactionGetTypeInfo              , SIGNAL(triggered()), this, SLOT(slotGetTypeInfo              ()) );
    connect( pactionForeignKeys              , SIGNAL(triggered()), this, SLOT(slotForeignKeys              ()) );
    connect( pactionPrimaryKeys              , SIGNAL(triggered()), this, SLOT(slotPrimaryKeys              ()) );
    connect( pactionProcedures               , SIGNAL(triggered()), this, SLOT(slotProcedures               ()) );
    connect( pactionProcedureColumns         , SIGNAL(triggered()), this, SLOT(slotProcedureColumns         ()) );
    connect( pactionSpecialColumns           , SIGNAL(triggered()), this, SLOT(slotSpecialColumns           ()) );
    connect( pactionStatistics               , SIGNAL(triggered()), this, SLOT(slotStatistics               ()) );
    connect( pactionTables                   , SIGNAL(triggered()), this, SLOT(slotTables                   ()) );
    connect( pactionTablePrivileges          , SIGNAL(triggered()), this, SLOT(slotTablePrivileges          ()) );
    connect( pactionManageDataSources        , SIGNAL(triggered()), this, SLOT(slotManageDataSources        ()) );
    connect( pactionRemoveDefaultDataSource  , SIGNAL(triggered()), this, SLOT(slotRemoveDefaultDataSource  ()) );
    connect( pactionConfigDataSource         , SIGNAL(triggered()), this, SLOT(slotConfigDataSource         ()) );
    connect( pactionCreateDataSource         , SIGNAL(triggered()), this, SLOT(slotCreateDataSource         ()) );
    connect( pactionValidDSN                 , SIGNAL(triggered()), this, SLOT(slotValidDSN                 ()) );
    connect( pactionRemoveDSNFromIni         , SIGNAL(triggered()), this, SLOT(slotRemoveDSNFromIni         ()) );
    connect( pactionWriteDSNToIni            , SIGNAL(triggered()), this, SLOT(slotWriteDSNToIni            ()) );
    connect( pactionRemoveDrivers            , SIGNAL(triggered()), this, SLOT(slotRemoveDrivers            ()) );
    connect( pactionConfigDrivers            , SIGNAL(triggered()), this, SLOT(slotConfigDrivers            ()) );
    connect( pactionInstallDriver            , SIGNAL(triggered()), this, SLOT(slotInstallDriver            ()) );
    connect( pactionInstallDriverEx          , SIGNAL(triggered()), this, SLOT(slotInstallDriverEx          ()) );
    connect( pactionGetInstalledDrivers      , SIGNAL(triggered()), this, SLOT(slotGetInstalledDrivers      ()) );
    connect( pactionRemoveDriverManager      , SIGNAL(triggered()), this, SLOT(slotRemoveDriverManager      ()) );
    connect( pactionInstallDriverManager     , SIGNAL(triggered()), this, SLOT(slotInstallDriverManager     ()) );
    connect( pactionReadFileDSN              , SIGNAL(triggered()), this, SLOT(slotReadFileDSN              ()) );
    connect( pactionWriteFileDSN             , SIGNAL(triggered()), this, SLOT(slotWriteFileDSN             ()) );
    connect( pactionWritePrivateProfileString, SIGNAL(triggered()), this, SLOT(slotWritePrivateProfileString()) );
    connect( pactionGetPrivateProfileString  , SIGNAL(triggered()), this, SLOT(slotGetPrivateProfileString  ()) );
    connect( pactionInstallTranslator        , SIGNAL(triggered()), this, SLOT(slotInstallTranslator        ()) );
    connect( pactionInstallTranslatorEx      , SIGNAL(triggered()), this, SLOT(slotInstallTranslatorEx      ()) );
    connect( pactionRemoveTranslator         , SIGNAL(triggered()), this, SLOT(slotRemoveTranslator         ()) );
    connect( pactionGetTranslator            , SIGNAL(triggered()), this, SLOT(slotGetTranslator            ()) );
    connect( pactionSetConfigMode            , SIGNAL(triggered()), this, SLOT(slotSetConfigMode            ()) );
    connect( pactionGetConfigMode            , SIGNAL(triggered()), this, SLOT(slotGetConfigMode            ()) );
    connect( pactionOptions                  , SIGNAL(triggered()), this, SLOT(slotOptions                  ()) );
    connect( pactionTrace                    , SIGNAL(triggered()), this, SLOT(slotTrace                    ()) );
    connect( pactionManageTestSources        , SIGNAL(triggered()), this, SLOT(slotManageTestSources        ()) );
    connect( pactionManageAutoTests          , SIGNAL(triggered()), this, SLOT(slotManageAutoTests          ()) );
    connect( pactionManageTestGroups         , SIGNAL(triggered()), this, SLOT(slotManageTestGroups         ()) );
    connect( pactionRunAutoTests             , SIGNAL(triggered()), this, SLOT(slotRunAutoTests             ()) );
    connect( pactionHelpTest                 , SIGNAL(triggered()), this, SLOT(slotHelpTest                 ()) );
    connect( pactionHelpApi                  , SIGNAL(triggered()), this, SLOT(slotHelpApi                  ()) );
    connect( pactionHelpAbout                , SIGNAL(triggered()), this, SLOT(slotHelpAbout                ()) );

}

void OdbcTest::createMenus()
{
    // File...
    pmenuFile               = menuBar()->addMenu( tr("&File") );
    pmenuFile->addAction( pactionExit );

    // Diagnostic...	
    pmenuDiagnostic         = menuBar()->addMenu( tr("Dia&gnostic") );
    pmenuDiagnostic->addAction( pactionGetDiagRec   );
    pmenuDiagnostic->addAction( pactionGetDiagField );
    pmenuDiagnostic->addAction( pactionError        );
    pmenuDiagnostic->addSeparator();
    pmenuDiagnostic->addAction( pactionErrorsAll    );

    // Environment...	
    pmenuEnvironment        = menuBar()->addMenu( tr("E&nvironment") );
    pmenuEnvironment->addAction( pactionAllocEnv       );
    pmenuEnvironment->addAction( pactionAllocHandle    );
    pmenuEnvironment->addAction( pactionDataSources    );
    pmenuEnvironment->addAction( pactionDrivers        );
    pmenuEnvironment->addAction( pactionFreeEnv        );
    pmenuEnvironment->addAction( pactionFreeHandle     );
    pmenuEnvironment->addAction( pactionEndTran        );
    pmenuEnvironment->addAction( pactionTransact       );
    pmenuEnvironment->addSeparator();
    pmenuEnvironment->addAction( pactionDataSourcesAll );
    pmenuEnvironment->addAction( pactionDriversAll     );

    // Connection...
    pmenuConnection         = menuBar()->addMenu( tr("&Connection") );
    pmenuConnection->addAction( pactionAllocConnect    );
    pmenuConnection->addAction( pactionBrowseConnect   );
    pmenuConnection->addAction( pactionConnect         );
    pmenuConnection->addAction( pactionDriverConnect   );
    pmenuConnection->addAction( pactionDisconnect      );
    pmenuConnection->addAction( pactionFreeConnect     );
    pmenuConnection->addAction( pactionGetFunctions    );
    pmenuConnection->addAction( pactionGetInfo         );
    pmenuConnection->addAction( pactionNativeSql       );
    pmenuConnection->addSeparator();
    pmenuConnection->addAction( pactionFullConnect     );
    pmenuConnection->addAction( pactionFullDisconnect  );
    pmenuConnection->addAction( pactionGetInfoAll      );
    pmenuConnection->addAction( pactionGetFunctionsAll );

    // Descriptor...
    pmenuDescriptor         = menuBar()->addMenu( tr("&Descriptor") );
    pmenuDescriptor->addAction( pactionCopyDesc     );
    pmenuDescriptor->addAction( pactionGetDescField );
    pmenuDescriptor->addAction( pactionGetDescRec   );
    pmenuDescriptor->addAction( pactionSetDescField );
    pmenuDescriptor->addAction( pactionSetDescRec   );
    pmenuDescriptor->addSeparator();
    pmenuDescriptor->addAction( pactionGetDescAll   );

    // Statement...
    pmenuStatement          = menuBar()->addMenu( tr("&Statement") );
    pmenuStatement->addAction( pactionAllocStmt          );
    pmenuStatement->addAction( pactionBindParam          );
    pmenuStatement->addAction( pactionBindParameter      );
    pmenuStatement->addAction( pactionCancel             );
    pmenuStatement->addAction( pactionCloseCursor        );
    pmenuStatement->addAction( pactionDescribeParam      );
    pmenuStatement->addAction( pactionExecute            );
    pmenuStatement->addAction( pactionExecDirect         );
    pmenuStatement->addAction( pactionFreeStmt           );
    pmenuStatement->addAction( pactionGetCursorName      );
    pmenuStatement->addAction( pactionNumParams          );
    pmenuStatement->addAction( pactionParamData          );
    pmenuStatement->addAction( pactionParamOptions       );
    pmenuStatement->addAction( pactionPrepare            );
    pmenuStatement->addAction( pactionPutData            );
    pmenuStatement->addAction( pactionSetCursorName      );
    pmenuStatement->addSeparator();
    pmenuStatement->addAction( pactionFillParam          );
    pmenuStatement->addAction( pactionShowParam          );
    pmenuStatement->addAction( pactionShowCursorSettings );

    // Attributes...
    pmenuAttributes         = menuBar()->addMenu( tr("&Attributes") );
    pmenuAttributes->addAction( pactionSetStmtAttr         );
    pmenuAttributes->addAction( pactionGetStmtAttr         );
    pmenuAttributes->addAction( pactionSetStmtOption       );
    pmenuAttributes->addAction( pactionGetStmtOption       );
    pmenuAttributes->addSeparator();
    pmenuAttributes->addAction( pactionSetConnectAttr      );
    pmenuAttributes->addAction( pactionGetConnectAttr      );
    pmenuAttributes->addAction( pactionSetConnectOption    );
    pmenuAttributes->addAction( pactionGetConnectOption    );
    pmenuAttributes->addSeparator();
    pmenuAttributes->addAction( pactionSetEnvAttr          );
    pmenuAttributes->addAction( pactionGetEnvAttr          );
    pmenuAttributes->addAction( pactionSetCursorAttributes );

    // Results...
    pmenuResults            = menuBar()->addMenu( tr("&Results") );
    pmenuResults->addAction( pactionBindCol          );
    pmenuResults->addAction( pactionBulkOperations   );
    pmenuResults->addAction( pactionColAttributes    );
    pmenuResults->addAction( pactionColAttribute     );
    pmenuResults->addAction( pactionDescribeCol      );
    pmenuResults->addAction( pactionExtendedFetch    );
    pmenuResults->addAction( pactionFetch            );
    pmenuResults->addAction( pactionFetchScroll      );
    pmenuResults->addAction( pactionGetData          );
    pmenuResults->addAction( pactionMoreResults      );
    pmenuResults->addAction( pactionNumResultCols    );
    pmenuResults->addAction( pactionRowCount         );
    pmenuResults->addAction( pactionSetPos           );
    pmenuResults->addAction( pactionSetScrollOptions );
    pmenuResults->addSeparator();
    pmenuResults->addAction( pactionBindColAll       );
    pmenuResults->addAction( pactionDescribeColAll   );
    pmenuResults->addAction( pactionFetchAll         );
    pmenuResults->addAction( pactionGetDataAll       );
    pmenuResults->addAction( pactionShowBoundCols    );
    pmenuResults->addAction( pactionDisplayRowSet    );

    // Catalog...
    pmenuCatalog            = menuBar()->addMenu( tr("Cata&log") );
    pmenuCatalog->addAction( pactionColumns          );
    pmenuCatalog->addAction( pactionColumnPrivileges );
    pmenuCatalog->addAction( pactionGetTypeInfo      );
    pmenuCatalog->addAction( pactionForeignKeys      );
    pmenuCatalog->addAction( pactionPrimaryKeys      );
    pmenuCatalog->addAction( pactionProcedures       );
    pmenuCatalog->addAction( pactionProcedureColumns );
    pmenuCatalog->addAction( pactionSpecialColumns   );
    pmenuCatalog->addAction( pactionStatistics       );
    pmenuCatalog->addAction( pactionTables           );
    pmenuCatalog->addAction( pactionTablePrivileges  );

    // Installer...
    pmenuInstaller          = menuBar()->addMenu( tr("&Installer") );

    // Installer->Data Sources...
    pmenuDataSources        = pmenuInstaller->addMenu( tr("DataSources") );
    pmenuDataSources->addAction( pactionManageDataSources       );
    pmenuDataSources->addAction( pactionRemoveDefaultDataSource );
    pmenuDataSources->addAction( pactionConfigDataSource        );
    pmenuDataSources->addAction( pactionCreateDataSource        );
    pmenuDataSources->addAction( pactionValidDSN                );
    pmenuDataSources->addAction( pactionRemoveDSNFromIni        );
    pmenuDataSources->addAction( pactionWriteDSNToIni           );

    // Installer->Drivers...
    pmenuDrivers            = pmenuInstaller->addMenu( tr("Drivers") );
    pmenuDrivers->addAction( pactionRemoveDrivers       );
    pmenuDrivers->addAction( pactionConfigDrivers       );
    pmenuDrivers->addAction( pactionInstallDriver       );
    pmenuDrivers->addAction( pactionInstallDriverEx     );
    pmenuDrivers->addAction( pactionGetInstalledDrivers );

    // Installer->DriverManager...
    pmenuDriverManager      = pmenuInstaller->addMenu( tr("DriverManager") );
    pmenuDriverManager->addAction( pactionRemoveDriverManager  );
    pmenuDriverManager->addAction( pactionInstallDriverManager );

    // Installer->DataSourceName...
    pmenuFileDataSourceName = pmenuInstaller->addMenu( tr("FileDataSourceName") );
    pmenuFileDataSourceName->addAction( pactionReadFileDSN  );
    pmenuFileDataSourceName->addAction( pactionWriteFileDSN );

    // Installer->ProfileStrings...
    pmenuProfileStrings     = pmenuInstaller->addMenu( tr("ProfileStrings") );
    pmenuProfileStrings->addAction( pactionWritePrivateProfileString  );
    pmenuProfileStrings->addAction( pactionGetPrivateProfileString );

    // Installer->Translator...
    pmenuTranslator         = pmenuInstaller->addMenu( tr("Translator") );
    pmenuTranslator->addAction( pactionInstallTranslator   );
    pmenuTranslator->addAction( pactionInstallTranslatorEx );
    pmenuTranslator->addAction( pactionRemoveTranslator    );
    pmenuTranslator->addAction( pactionGetTranslator       );

    // Installer->ConfigMode...
    pmenuConfigMode         = pmenuInstaller->addMenu( tr("ConfigMode") );
    pmenuConfigMode->addAction( pactionSetConfigMode );
    pmenuConfigMode->addAction( pactionGetConfigMode );

    // Tools...
    pmenuTools              = menuBar()->addMenu( tr("&Tools") );
    pmenuTools->addAction( pactionOptions           );
    pmenuTools->addAction( pactionTrace             );
    pmenuTools->addSeparator();
    pmenuTools->addAction( pactionManageTestSources );
    pmenuTools->addAction( pactionManageAutoTests   );
    pmenuTools->addAction( pactionManageTestGroups  );
    pmenuTools->addAction( pactionRunAutoTests      );

    // Help...
    pmenuHelp               = menuBar()->addMenu( tr("&Help") );
    pmenuHelp->addAction( pactionHelpTest );
    pmenuHelp->addAction( pactionHelpApi );
    pmenuHelp->addAction( pactionHelpAbout );
}

void OdbcTest::readApplicationState()
{
    QSettings settings( "unixODBC-GUI-Qt", "OdbcTestQ4" );

    settings.beginGroup( "MainWindow" );
    resize(settings.value( "size", QSize( 400, 400 ) ).toSize() );
    move(settings.value( "pos", QPoint( 200, 200 ) ).toPoint() );
    settings.endGroup();
}

void OdbcTest::writeApplicationState()
{
    QSettings settings( "unixODBC-GUI-Qt", "OdbcTestQ4" );

    settings.beginGroup( "MainWindow" );
    settings.setValue( "size", size() );
    settings.setValue( "pos", pos() );
    settings.endGroup();
}

void OdbcTest::slotAllocHandle()
{
	DlgEnvAllocHandle *dlg = new DlgEnvAllocHandle( this, "SQLAllocHandle" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotDataSources()
{
	DlgEnvDataSources *dlg = new DlgEnvDataSources( this, "SQLDataSources" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotDrivers()
{
	DlgEnvDrivers *dlg = new DlgEnvDrivers( this, "SQLDrivers" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotEndTran()
{
	DlgEnvEndTran *dlg = new DlgEnvEndTran( this, "SQLEndTran" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotFreeHandle()
{
	DlgEnvFreeHandle *dlg = new DlgEnvFreeHandle( this, "SQLFreeHandle" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotAllocEnv()
{
	DlgEnvAllocEnv *dlg = new DlgEnvAllocEnv( this, "SQLAllocEnv" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotFreeEnv()
{
	DlgEnvFreeEnv *dlg = new DlgEnvFreeEnv( this, "SQLFreeEnv" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotTransact()
{
	DlgEnvTransact *dlg = new DlgEnvTransact( this, "SQLTransact" );

	dlg->exec();

	delete dlg;
}

void OdbcTest::slotDataSourcesAll()
{
    QMessageBox::about( this, "ODBC Test",
			"Not yet implemented" );
}

void OdbcTest::slotDriversAll()
{
    QMessageBox::about( this, "ODBC Test",
			"Not yet implemented" );
}

