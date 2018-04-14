/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef DDATAMANAGER_H
#define DDATAMANAGER_H

#include <QtGui>
#include <QtWidgets>

#include <OQGEnvironment.h>

#include "ODBCModelSystem.h"
#include "ODBCModelBrowser.h"
#include "ODBCViewBrowser.h"
#include "ODBCDelegateCell.h"

// class DBrowser;
// class DPropWidget;
class OQGTabOutput;
class DEdit;

class DDataManager : public QMainWindow
{
    Q_OBJECT
public:
    DDataManager();
    virtual ~DDataManager();

protected slots:

    void slotAbout();

    void slotSubWindowActivated( QMdiSubWindow *pSubWindow );
    void slotSelected( const QModelIndex &index );

protected:
    ODBCViewBrowser *       pViewBrowser;
    OQGSystem *             pSystem;
    ODBCModelSystem *       pModelSystem;
    ODBCModelBrowser *      pModelBrowser;
    QTableView *            ptableviewProperties; // properties
    ODBCDelegateCell *      pDelegateCell;

    QToolBar *          ptoolbarEditor;
    QMdiArea *          pMdiArea;
    OQGTabOutput *      pTabOutput;

    DEdit *             pEdit; // current editor or NULL

    QMenu * pmenuFile;
    QMenu * pmenuHelp;

    QAction *   pactionQuit;
    QAction *   pactionAbout;

    QAction *   pactionSubmit;

    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
};

#endif

