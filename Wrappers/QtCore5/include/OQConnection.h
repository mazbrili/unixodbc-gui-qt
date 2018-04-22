/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See LGPL.txt
 */
#pragma once

#include "OQEnvironment.h"

class OQStatement;

/*! 
 * \class   OQConnection
 * \brief   An ODBC connection.
 *
 *          This class wraps an ODBC Connection handle.
 *
 *          \li uses Qt data types where possible
 *          \li provides several Qt signals
 *          \li inherits QObject to assist in parent/child relationships using rtti and using signals/slots
 *
 *          This class does not provide any User Interface code - see QtGui5 layer for those bits.
 */
class OQConnection : public OQHandle
{
    Q_OBJECT
public:
    enum AttributeTypes
    {
        AttrAccessMode          = SQL_ATTR_ACCESS_MODE,         /*!< This can be used to optimize locking strategies, transaction management, or other areas as appropriate to the driver or data source.   */
        AttrAsyncEnable         = SQL_ATTR_ASYNC_ENABLE,        /*!< Specifies whether a function called with a statement on the specified connection is executed asynchronously.                           */
        AttrAutoIpd             = SQL_ATTR_AUTO_IPD,            /*!< ReadOnly. Specifies whether automatic population of the parameter descriptions after a call to prepare statement is supported.         */
        AttrAutocommit          = SQL_ATTR_AUTOCOMMIT,          /*!< Specifies whether to use autocommit or manual-commit mode.                                                                             */
        AttrConnectionDead      = SQL_ATTR_CONNECTION_DEAD,     /*!< ReadOnly. Indicates the state of the connection (or at least the connect/disconnect part of state).                                    */
        AttrConnectionTimeout   = SQL_ATTR_CONNECTION_TIMEOUT,  /*!< The number of seconds to wait for any request on the connection to complete before returning to the application.                       */
        AttrCurrentCatalog      = SQL_ATTR_CURRENT_CATALOG,     /*!< A character string containing the name of the catalog to be used by the data source.                                                   */
        AttrLoginTimeout        = SQL_ATTR_LOGIN_TIMEOUT,       /*!< The number of seconds to wait for a login request to complete before returning to the application.                                     */
        AttrMetadataId          = SQL_ATTR_METADATA_ID,         /*!< Determines how the string arguments of catalog functions are treated.                                                                  */
        AttrOdbcCursors         = SQL_ATTR_ODBC_CURSORS,        /*!< Specifies how the Driver Manager uses the ODBC cursor library.                                                                         */
        AttrPacketSize          = SQL_ATTR_PACKET_SIZE,         /*!< Specifying the network packet size in bytes.                                                                                           */
        AttrQuietMode           = SQL_ATTR_QUIET_MODE,          /*!< The driver uses this handle to display dialog boxes.                                                                                   */
        AttrTrace               = SQL_ATTR_TRACE,               /*!< Tells the Driver Manager whether to perform tracing.                                                                                   */
        AttrTracefile           = SQL_ATTR_TRACEFILE,           /*!< String containing the name of the trace file.                                                                                          */
        AttrTranslateLib        = SQL_ATTR_TRANSLATE_LIB,       /*!< String containing the name of a lib containing the functions that the driver accesses to perform tasks such as char set translation.   */
        AttrTranslateOption     = SQL_ATTR_TRANSLATE_OPTION,    /*!< Value that is passed to the translation DLL.                                                                                           */
        AttrTxnIsolation        = SQL_ATTR_TXN_ISOLATION        /*!< Sets the transaction isolation level for the current connection.                                                                       */
    };

    enum AttrAccessModeTypes
    {
        ModeReadOnly        = SQL_MODE_READ_ONLY,               /*!< Used by the driver/data source as an indicator that the connection is not required to support SQL that cause updates to occur.         */
        ModeReadWrite       = SQL_MODE_READ_WRITE               /*!< Default. Driver/Data Source should support SQL which cause updates to occur.                                                           */
    };

    enum AttrAsyncEnableTypes
    {
        AsyncEnableOff      = SQL_ASYNC_ENABLE_OFF,             /*!< Default. All calls are synchronous.                                                                                                    */
        AsyncEnableOn       = SQL_ASYNC_ENABLE_ON               /*!< Enables asynchronous execution for all future statement handles allocated on this connection.                                          */
    };

    enum AttrAutocommitTypes
    {
        AutocommitOff   = SQL_AUTOCOMMIT_OFF,                   /*!< Driver uses manual-commit mode, and the application must explicitly commit or roll back transactions with SQLEndTran.                  */
        AutocommitOn    = SQL_AUTOCOMMIT_ON                     /*!< Default. Driver uses autocommit mode. Each statement is committed immediately after it is executed.                                    */
    };

    enum AttrOdbcCursorsTypes
    {
        CurUseIfNeeded      = SQL_CUR_USE_IF_NEEDED,            /*!< Driver Manager uses the ODBC cursor library only if it is needed.                                                                      */
        CurUseOdbc          = SQL_CUR_USE_ODBC,                 /*!< Driver Manager uses the ODBC cursor library.                                                                                           */
        CurUseDriver        = SQL_CUR_USE_DRIVER                /*!< Default. Driver Manager uses the scrolling capabilities of the driver.                                                                 */
    };

    enum AttrTraceTypes
    {
        OptTraceOff = SQL_OPT_TRACE_OFF,                        /*!< Default. Tracing off.                                                                                                                  */
        OptTraceOn  = SQL_OPT_TRACE_ON                          /*!< Tracing on. When tracing is on, the Driver Manager writes each ODBC function call to the trace file.                                   */
    };

    enum DriverPromptTypes
    {
        DriverPrompt = SQL_DRIVER_PROMPT, 
        DriverComplete = SQL_DRIVER_COMPLETE, 
        DriverCompleteRequired = SQL_DRIVER_COMPLETE_REQUIRED, 
        DriverNoprompt = SQL_DRIVER_NOPROMPT
    };

    OQConnection( OQEnvironment *penvironment );
    virtual ~OQConnection();

    // SETTERS
    virtual SQLRETURN setAttrAccessMode( AttrAccessModeTypes nAttrAccessMode = ModeReadWrite );
    virtual SQLRETURN setAttrAsyncEnable( AttrAsyncEnableTypes nAttrAsyncEnable = AsyncEnableOff ); 
    virtual SQLRETURN setAttrAutocommit( AttrAutocommitTypes nAttrAutocommit = AutocommitOn );
    virtual SQLRETURN setAttrConnectionTimeout( SQLUINTEGER nAttrConnectionTimeout = 0 );
    virtual SQLRETURN setAttrCurrentCatalog( const QString &stringAttrCurrentCatalog );
    virtual SQLRETURN setAttrLoginTimeout( SQLUINTEGER nAttrLoginTimeout = 0 );
    virtual SQLRETURN setAttrMetadataId( bool bAttrMetadataId = false );
    virtual SQLRETURN setAttrOdbcCursors( AttrOdbcCursorsTypes nAttrOdbcCursors = CurUseDriver );
    virtual SQLRETURN setAttrPacketSize( SQLUINTEGER nAttrPacketSize = 1024 );
    virtual SQLRETURN setAttrQuietMode( SQLHWND hWnd );
    virtual SQLRETURN setAttrTrace( AttrTraceTypes nAttrTrace = OptTraceOff );
    virtual SQLRETURN setAttrTracefile( const QString &stringAttrTracefile );
    virtual SQLRETURN setAttrTranslateLib( const QString &stringAttrTranslateLib );
    virtual SQLRETURN setAttrTranslateOption( SQLUINTEGER nAttrTranslateOption );
    virtual SQLRETURN setAttrTxnIsolation( SQLUINTEGER nAttrTxnIsolation );

    // GETTERS
    virtual AttrAccessModeTypes     getAttrAccessMode( SQLRETURN *pn = NULL );
    virtual AttrAsyncEnableTypes    getAttrAsyncEnable( SQLRETURN *pn = NULL ); 
    virtual bool                    getAttrAutoIpd( SQLRETURN *pn = NULL );
    virtual AttrAutocommitTypes     getAttrAutocommit( SQLRETURN *pn = NULL );
    virtual bool                    getAttrConnectionDead( SQLRETURN *pn = NULL );
    virtual SQLUINTEGER             getAttrConnectionTimeout( SQLRETURN *pn = NULL );
    virtual QString                 getAttrCurrentCatalog( SQLRETURN *pn = NULL );
    virtual SQLUINTEGER             getAttrLoginTimeout( SQLRETURN *pn = NULL );
    virtual bool                    getAttrMetadataId( SQLRETURN *pn = NULL );
    virtual AttrOdbcCursorsTypes    getAttrOdbcCursors( SQLRETURN *pn = NULL );
    virtual SQLUINTEGER             getAttrPacketSize( SQLRETURN *pn = NULL );
    virtual SQLHWND                 getAttrQuietMode( SQLRETURN *pn = NULL );
    virtual AttrTraceTypes          getAttrTrace( SQLRETURN *pn = NULL );
    virtual QString                 getAttrTracefile( SQLRETURN *pn = NULL );
    virtual QString                 getAttrTranslateLib( SQLRETURN *pn = NULL );
    virtual SQLUINTEGER             getAttrTranslateOption( SQLRETURN *pn = NULL );
    virtual SQLUINTEGER             getAttrTxnIsolation( SQLRETURN *pn = NULL );

    virtual OQStatement *getExecute( const QString &stringStatement );
    virtual OQStatement *getCatalogs();
    virtual OQStatement *getSchemas( const QString &stringCatalog = QString::null );
    virtual OQStatement *getTables( const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null, const QString &stringType = QString::fromLocal8Bit( "TABLE" ) );
    virtual OQStatement *getViews( const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null, const QString &stringType = QString::fromLocal8Bit( "VIEW" ) );
    virtual OQStatement *getColumns( const QString &stringTable = QString::null, const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null, const QString &stringType = QString::fromLocal8Bit( "TABLE" ) );
    virtual OQStatement *getIndexs( const QString &stringTable = QString::null, const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null );
    virtual OQStatement *getPrimaryKeys( const QString &stringTable = QString::null, const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null );
    virtual OQStatement *getForeignKeys( const QString &stringTable = QString::null, const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null );
    virtual OQStatement *getSpecialColumns( const QString &stringTable = QString::null, const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null );
    virtual OQStatement *getProcedures( const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null );
    virtual OQStatement *getProcedureColumns( const QString &stringProcedure = QString::null, const QString &stringSchema = QString::null, const QString &stringCatalog = QString::null );
    virtual OQStatement *getDataTypes();

    virtual QString   getBrowseConnect( const QString &stringIn, SQLRETURN *pnReturn = NULL );
    virtual QString   getDriverConnect( QWidget *pwidgetParent, const QString &stringIn, DriverPromptTypes nPrompt, SQLRETURN *pnReturn = NULL );

    // DOERS
    virtual SQLRETURN doConnect( const QString &stringServerName = QString::null, const QString &stringUserName = QString::null, const QString &stringAuthentication = QString::null );
    virtual SQLRETURN doDisconnect();

    virtual bool isConnected();

signals:
    void signalConnected();
    void signalDisconnected();

protected:
    bool bConnected;             /*!< cached connect state - need to see if this is needed                   */

    // 2 arg calls for setting attributes (they call the 3 arg version)
    // these have extra validation to check data type for attribute
    // note: some calls, such as SQL_ATTR_ENLIST_IN_DTC/SQL_ATTR_QUIET_MODE, can not be done by the 2 arg version
    virtual SQLRETURN setConnectAttr( SQLINTEGER nAttribute, SQLUINTEGER n );
    virtual SQLRETURN setConnectAttr( SQLINTEGER nAttribute, SQLINTEGER n );
    virtual SQLRETURN setConnectAttr( SQLINTEGER nAttribute, const QString &stringValue );
    virtual SQLRETURN setConnectAttr( SQLINTEGER nAttribute, QByteArray d );

    // 3 arg call for setting attributes
    // does not check data type for attribute
    virtual SQLRETURN setConnectAttr( SQLINTEGER nAttribute, SQLPOINTER p, SQLINTEGER n );

    // 2 arg calls for getting attribute (they call the 3 arg version)
    // these have extra validation to check data type for attribute
    // note: some calls, such as SQL_ATTR_ENLIST_IN_DTC/SQL_ATTR_QUIET_MODE, can not be done by the 2 arg version
    virtual SQLRETURN getConnectAttr( SQLINTEGER nAttribute, SQLUINTEGER *pn );
    virtual SQLRETURN getConnectAttr( SQLINTEGER nAttribute, SQLINTEGER *pn );
    virtual SQLRETURN getConnectAttr( SQLINTEGER nAttribute, QString *pValue );
    virtual SQLRETURN getConnectAttr( SQLINTEGER nAttribute, QByteArray *pd );

    // 4 arg call for getting attribute
    // does not check data type for attribute
    virtual SQLRETURN getConnectAttr( SQLINTEGER nAttribute, SQLPOINTER pValue, SQLINTEGER nBufferLength, SQLINTEGER *pnStringLength );

    // these have the same args as the corresponding CLI but have some validation/error reporting
    // used by convenience methods
    virtual SQLRETURN doConnect( SQLWCHAR *pszServerName = NULL, SQLSMALLINT nLength1 = SQL_NTS, SQLWCHAR *pszUserName = NULL, SQLSMALLINT nLength2 = SQL_NTS, SQLWCHAR *pszAuthentication = NULL, SQLSMALLINT nLength3 = SQL_NTS );
    virtual SQLRETURN doDriverConnect( SQLHWND hWnd, SQLWCHAR *pszIn, SQLSMALLINT nLengthIn, SQLWCHAR *pszOut, SQLSMALLINT nLengthOut, SQLSMALLINT *pnLengthOut, SQLUSMALLINT nPrompt );
    virtual SQLRETURN doBrowseConnect( SQLWCHAR *szInConnectionString, SQLSMALLINT nStringLength1, SQLWCHAR *szOutConnectionString, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength2Ptr );
    virtual SQLRETURN doGetInfo( SQLUSMALLINT nInfoType, SQLPOINTER pInfoValue, SQLSMALLINT nBufferLength, SQLSMALLINT *pnStringLength );
};
