/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC-GUI-Qt Project 2007-2012, See GPL.txt
 */
#ifndef DNODETABLES_H
#define DNODETABLES_H

#include "DNode.h"

class DNodeTables : public DNode
{
public:
    DNodeTables( DNodeWidget *pNodeWidget, DNode *pNode, const QString &stringSchema );
    virtual ~DNodeTables();

    void doLoad() {}
    void doClear() {}

protected:
    QString stringSchema;
};

#endif

