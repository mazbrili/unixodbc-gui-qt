/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2003-2009, LGPL
 */
#ifndef OQGLOGIN_H
#define OQGLOGIN_H

//
#include <sys/types.h>

// Qt
#include <QtGui>
#include <QtWidgets>
#include <QDialog>

//
#ifndef Q_OS_WIN
#include <unistd.h>
#include <pwd.h>
#endif

//
class OQGSystem;
class OQGEnvironment;
class OQGConnection;
class OQMessage;
class OQGTabOutput;

class OQGLogin: public QDialog
{
	Q_OBJECT
public:
    OQGLogin( QWidget *pwidgetParent, OQGConnection *pconnection );
    ~OQGLogin();

    // SETTERS
    virtual void setShowDriver( bool b );
    virtual void setShowDataSourceName( bool b );
    virtual void setShowUserID( bool b );
    virtual void setShowPassword( bool b );
    virtual void setDriver( const QString &string );
    virtual void setDataSourceName( const QString &string );
    virtual void setUserID( const QString &string );
    virtual void setPassword( const QString &string );

    // GETTERS
    QString getDriver() { return pcomboboxDriver->currentText(); }
    QString getDataSourceName() { return pcomboboxDataSourceName->currentText(); }
    QString getUserID() { return plineeditUserID->text(); }
    QString getPassword() { return plineeditPassword->text(); }

protected:
    OQGEnvironment *        penvironment;
    QLabel *                plabelDriver;
    QComboBox *             pcomboboxDriver;
    QLabel *                plabelDataSourceName;
    QComboBox *             pcomboboxDataSourceName;
    QLabel *                plabelUserID;
	QLineEdit *             plineeditUserID;
    QLabel *                plabelPassword;
	QLineEdit *             plineeditPassword;
    OQGTabOutput *          pmessageoutput;

    virtual void loadDrivers();
    virtual void loadDataSourceNames();
};

#endif

