/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2003
 * \license Copyright unixODBC-GUI-Qt Project 2003-2012, See LGPL.txt
 */
#include "OQEnvironment.h"

OQEnvironment::OQEnvironment( OQSystem *pSystem )
    : OQHandle( Env, pSystem )
{
    setObjectName( QString::fromLocal8Bit("OQEnvironment") );

    // echo up the object hierarchy
    connect( this, SIGNAL(signalMessage(OQMessage)), pSystem, SIGNAL(signalMessage(OQMessage)) );
    connect( this, SIGNAL(signalDiagnostic(OQDiagnostic)), pSystem, SIGNAL(signalDiagnostic(OQDiagnostic)) );
}

OQEnvironment::~OQEnvironment()
{
}

/*! 
 * \brief   Sets SQL_ATTR_CONNECTION_POOLING attribute.
 * 
 *          See \sa getAttrConnectionPooling for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \note    Connection pooling not fully implemented here - yet. This does does set
 *          this attribute but does not follow up with the remaining, internal, bits 
 *          we need to do.
 *
 * \param   n   Input. One of \sa OQEnvironment::AttrConnectionPoolingTypes.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrConnectionPooling
 */
SQLRETURN OQEnvironment::setAttrConnectionPooling( AttrConnectionPoolingTypes n )
{
    return setEnvAttr( SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)n );
}

/*! 
 * \brief   Sets SQL_ATTR_CP_MATCH attribute.
 * 
 *          See \sa getAttrCPMatch for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \param   n   Input. One of \sa OQEnvironment::AttrCPMatchTypes.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrCPMatch
 */
SQLRETURN OQEnvironment::setAttrCPMatch( AttrCPMatchTypes n )
{
    return setEnvAttr( SQL_ATTR_CP_MATCH, (SQLPOINTER)n );
}

/*! 
 * \brief   Sets SQL_ATTR_ODBC_VERSION attribute.
 * 
 *          See \sa getAttrODBCVersion for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \param   n   Input. One of \sa OQEnvironment::AttrODBCVersionTypes.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrODBCVersion
 */
SQLRETURN OQEnvironment::setAttrODBCVersion( AttrODBCVersionTypes n )
{
    return setEnvAttr( SQL_ATTR_ODBC_VERSION, (SQLPOINTER)n );
}

/*! 
 * \brief   Sets SQL_ATTR_OUTPUT_NTS attribute.
 * 
 *          See \sa getAttrOutputNTS for information about this attribute.
 *
 *          This is a convenience method which maps to SQLSetEnvAttr.
 *
 * \param   b   Input. Either true or false.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrOutputNTS
 */
SQLRETURN OQEnvironment::setAttrOutputNTS( bool b )
{
    return setEnvAttr( SQL_ATTR_OUTPUT_NTS, (SQLPOINTER)b );
}

/*! 
 * \brief   Get SQL_ATTR_CONNECTION_POOLING attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  OQEnvironment::AttrConnectionPoolingTypes
 *
 * \sa      setAttrConnectionPooling
 */
OQEnvironment::AttrConnectionPoolingTypes OQEnvironment::getAttrConnectionPooling( SQLRETURN *pnReturn )
{
    AttrConnectionPoolingTypes n = CPOff;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_CONNECTION_POOLING, &n );
    if ( pnReturn ) *pnReturn = nReturn;

    return n;
}

/*! 
 * \brief   Get SQL_ATTR_CP_MATCH attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  OQEnvironment::AttrCPMatchTypes
 *
 * \sa      setAttrCPMatch
 */
OQEnvironment::AttrCPMatchTypes OQEnvironment::getAttrCPMatch( SQLRETURN *pnReturn )
{
    AttrCPMatchTypes n = CPStrictMatch;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_CP_MATCH, &n );
    if ( pnReturn ) *pnReturn = nReturn;

    return n;
}

/*! 
 * \brief   Get SQL_ATTR_ODBC_VERSION attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  OQEnvironment::AttrODBCVersionTypes
 *
 * \sa      setAttrODBCVersion
 */
OQEnvironment::AttrODBCVersionTypes OQEnvironment::getAttrODBCVersion( SQLRETURN *pnReturn )
{
    AttrODBCVersionTypes n = OVOdbc3;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_ODBC_VERSION, &n );
    if ( pnReturn ) *pnReturn = nReturn;

    return n;
}

/*! 
 * \brief   Get SQL_ATTR_OUTPUT_NTS attribute.
 * 
 *          This method is a convenience method which calls SQLGetEnvAttr.
 *
 *          Any error messages generated during this call will be captured to
 *          the message list.
 *
 *          A default value will be returned in the event of an error. The default value
 *          will often be a valid representation of the facts - but not always. The caller
 *          is relying more on the extreme likelihood that no error will occur for this call.
 * 
 * \param   pnReturn    Input. Pointer to mem to return SQLRETURN value. Ignored if NULL.
 *
 * \return  bool
 *
 * \sa      setAttrOutputNTS
 */
bool OQEnvironment::getAttrOutputNTS( SQLRETURN *pnReturn )
{
    bool b = true;

    SQLRETURN nReturn = getEnvAttr( SQL_ATTR_OUTPUT_NTS, &b );
    if ( pnReturn ) *pnReturn = nReturn;

    return b;
}

/*!
 * \brief   Get a list of the installed drivers. 
 *  
 *          This can be accomplished by OQSystem as well but they use
 *          different underlying SQL calls.
 * 
 * \author pharvey (3/27/18)
 * 
 * \param pnReturn Return value. (optional)
 * 
 * \return QStringList List of driver names.
 */
QStringList OQEnvironment::getDrivers( SQLRETURN *pnReturn )
{
    QStringList     stringlistDrivers;
    SQLRETURN       nReturn;
    SQLWCHAR        szDRV[100];
    SQLSMALLINT     nDrvChars;
    SQLWCHAR        szAttribute[300];
    SQLSMALLINT     nAttrChars;

    nReturn = doDrivers( SQL_FETCH_FIRST, szDRV, 100, &nDrvChars, szAttribute, 300, &nAttrChars );
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        stringlistDrivers.append( QString::fromUtf16( szDRV, nDrvChars ) );
        nReturn = doDrivers( SQL_FETCH_NEXT, szDRV, 100, &nDrvChars, szAttribute, 300, &nAttrChars );
    }

    if ( pnReturn )
        *pnReturn = nReturn;

    return stringlistDrivers;
}

/*!
 * \brief   Get a list of Data Source Names (DSN).
 * 
 *          This can be accomplished by OQSystem as well but they use
 *          different underlying SQL calls.
 *  
 * \author pharvey (3/27/18)
 * 
 * \param bUser User scope.
 * \param bSystem System scope.
 * \param pnReturn Return value. (optional)
 * 
 * \return QStringList List of Data Source Names.
 */
QStringList OQEnvironment::getDataSources( OQSystem::enumFetch nScope, SQLRETURN *pnReturn )
{
    QStringList     stringlistDataSources;
    SQLRETURN       nReturn;

    SQLSMALLINT     nDSNChars = 100;
    SQLWCHAR        szDSN[nDSNChars];
    SQLSMALLINT     nDSNCharsAvail = 0;

    SQLSMALLINT      nDescChars = 200;
    SQLWCHAR        szDesc[nDescChars];
    SQLSMALLINT     nDescCharsAvail = 0;

    nReturn = doDataSources( (SQLUSMALLINT)nScope, szDSN, nDSNChars, &nDSNCharsAvail, szDesc, nDescChars, &nDescCharsAvail );
    while ( SQL_SUCCEEDED( nReturn ) )
    {
        stringlistDataSources << QString::fromUtf16( szDSN );
        nReturn = doDataSources( SQL_FETCH_NEXT, szDSN, nDSNChars, &nDSNCharsAvail, szDesc, nDescChars, &nDescCharsAvail );
    }

    if ( pnReturn )
        *pnReturn = nReturn;

    return stringlistDataSources;
}

/*! 
 * \brief   Set an environment attribute.
 *
 *          Use of this method of setting an attribute is discouraged. Please consider using
 *          the more convenient methods such as \sa setAttrConnectionPooling etc. This method
 *          can be used to set non-standard environment attributes.
 * 
 * \param   nAttribute      Input. A viable environment attribute such as; SQL_ATTR_CONNECTION_POOLING.
 * \param   pValue          Input. The value (pointer *is* the value for numerics).
 * \param   nStringLength   Input. Number of chars at pValue in case where pValue points to a string.
 * 
 * \return  SQLRETURN
 * 
 * \sa      setAttrConnectionPooling
 *          setAttrCPMatch
 *          setAttrODBCVersion
 *          setAttrOutputNTS
 */
SQLRETURN OQEnvironment::setEnvAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLRETURN   nReturn = SQLSetEnvAttr( hHandle, nAttribute, pValue, 0 );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Get an environment attribute.
 *
 *          Use of this method of getting an environment attribute is discouraged. Please consider using
 *          the more convenient methods such as \sa getAttrConnectionPooling etc. This method
 *          can be used to get non-standard environment attributes.
 * 
 * \param   nAttribute      Input. A viable environment attribute such as; SQL_ATTR_CONNECTION_POOLING.
 * \param   pValue          Output. Pointer to a buffer in which to get the desired value.
 * \param   nBufferLength   Input. The number of bytes we can use at pValue.
 * \param   pnStringLength  Output. The number of chars available if value is a string.
 * 
 * \return  SQLRETURN
 *
 * \sa      getAttrConnectionPooling
 *          getAttrCPMatch
 *          getAttrODBCVersion
 *          getAttrOutputNTS
 */
SQLRETURN OQEnvironment::getEnvAttr( SQLINTEGER nAttribute, SQLPOINTER pValue )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    SQLINTEGER  nBufferLength   = 0;
    SQLINTEGER  nStringLength   = 0;
    SQLRETURN   nReturn = SQLGetEnvAttr( hHandle, nAttribute, pValue, nBufferLength, &nStringLength );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Gets driver information.
 *
 *          This call maps to SQLDriver.
 * 
 * \param   nDirection
 * \param   pszDriverDescription
 * \param   nBufferLength1
 * \param   pnDescriptionLengthPtr
 * \param   pszDriverAttributes
 * \param   nBufferLength2
 * \param   pnAttributesLengthPtr
 * 
 * \return  SQLRETURN
 */
SQLRETURN OQEnvironment::doDrivers( SQLUSMALLINT nDirection, SQLWCHAR *pszDriverDescription, SQLSMALLINT nBufferLength1, SQLSMALLINT *pnDescriptionLengthPtr, SQLWCHAR *pszDriverAttributes, SQLSMALLINT nBufferLength2, SQLSMALLINT *pnAttributesLengthPtr )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    //
    SQLRETURN nReturn = SQLDriversW( hHandle, nDirection, pszDriverDescription, nBufferLength1, pnDescriptionLengthPtr, pszDriverAttributes, nBufferLength2, pnAttributesLengthPtr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

/*! 
 * \brief   Get data source information.
 *
 *          This call maps to SQLDataSources.
 * 
 * \param   nDirection
 * \param   pszServerName
 * \param   nBufferLength1
 * \param   pnNameLength1Ptr
 * \param   pszDescription
 * \param   nBufferLength2
 * \param   pnNameLength2Ptr
 * 
 * \return  SQLRETURN
 */
SQLRETURN OQEnvironment::doDataSources( SQLUSMALLINT nDirection, SQLWCHAR *pszServerName, SQLSMALLINT nBufferLength1, SQLSMALLINT *pnNameLength1Ptr, SQLWCHAR *pszDescription, SQLSMALLINT nBufferLength2, SQLSMALLINT *pnNameLength2Ptr )
{
    if ( !isAlloc() )
        return SQL_ERROR;

    //
    SQLRETURN nReturn = SQLDataSourcesW( hHandle, nDirection, pszServerName, nBufferLength1, pnNameLength1Ptr, pszDescription, nBufferLength2, pnNameLength2Ptr );
    switch ( nReturn )
    {
        case SQL_SUCCESS:
        case SQL_NO_DATA:
            break;
        case SQL_SUCCESS_WITH_INFO:
            eventDiagnostic();
            break;
        case SQL_ERROR:
            eventDiagnostic();
            break;
        case SQL_INVALID_HANDLE:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QString::fromLocal8Bit("SQL_INVALID_HANDLE") ) );
            break;
        default:
            eventMessage( OQMessage( OQMessage::Error, QString::fromLocal8Bit(__FUNCTION__), QObject::tr("Unexpected SQLRETURN value."), nReturn ) );
            break;
    }

    return nReturn;
}

