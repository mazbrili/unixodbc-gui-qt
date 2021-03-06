/*********************************************************************
 *
 * Written by Nick Gorham
 * (nick@lurcher.org).
 *
 * copyright (c) 1999 Nick Gorham
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 **********************************************************************
 *
 * 2008-12-10   Code adopted by unixODBC-GUI-Qt project. Heavily altered
 * pharvey      to be Qt4 only code.
 * 
 **********************************************************************/

#include "DlgEnvAllocHandle.h"
#include "OdbcTest.h"

void DlgEnvAllocHandle::slotDone()
{
	SQLHANDLE 	out_handle, in_handle = SQL_NULL_HANDLE;
	SQLRETURN 	ret;
	SQLINTEGER 	type;
	const char *handle;

	switch( types->currentIndex() )
	{
		case 0:
			type = SQL_HANDLE_ENV;
			handle = "SQL_HANDLE_ENV=1";
			break;

		case 1:
			type = SQL_HANDLE_DBC;
			handle = "SQL_HANDLE_DBC=2";
			break;

		case 2:
			type = SQL_HANDLE_STMT;
			handle = "SQL_HANDLE_STMT=3";
			break;

		case 3:
			type = SQL_HANDLE_DESC;
			handle = "SQL_HANDLE_DESC=4";
			break;
	}

	/*
	 * get input handle
	 */

    OdbcHandle *hand = pOdbcTest->extract_handle_list( -1, handles );

	if ( hand )
		in_handle = hand->getHandle();

	/*
	 * set output handle to something
	 */
	out_handle = 0;

	if ( valid->isChecked() )
		ret = SQLAllocHandle( type, in_handle, SQL_NULL_HANDLE );
	else
		ret = SQLAllocHandle( type, in_handle, &out_handle );

	pOdbcTest->out_win->append( "SQLAllocHandle():" );
	pOdbcTest->out_win->append( "  In:" );
	txt.sprintf( "    Handle Type: %s", handle );
	pOdbcTest->out_win->append( txt );
	if ( in_handle )
	    txt.sprintf( "    InputHandle: %p", in_handle );
	else
	    txt.sprintf( "    InputHandle: SQL_NULL_HANDLE" );
	pOdbcTest->out_win->append( txt );
	if ( valid->isChecked() )
		txt.sprintf( "    OutputHandle: SQL_NULL_HANDLE" );
	else
		txt.sprintf( "    OutputHandle: %p", &out_handle );
	pOdbcTest->out_win->append( txt );

	pOdbcTest->out_win->append( "  Return:" );
	txt.sprintf( "    %s=%d", pOdbcTest->return_as_text( ret ), ret );
	pOdbcTest->out_win->append( txt );

	pOdbcTest->out_win->append( "  Out:" );
	if ( out_handle )
		txt.sprintf( "    *OutputHandle: %p", out_handle );
	else
		txt.sprintf( "    *OutputHandle: <unmodified>" );
	pOdbcTest->out_win->append( txt );
	pOdbcTest->out_win->append( "" );

	if ( SQL_SUCCEEDED( ret ))
	{
	    pOdbcTest->listHandle.append( new OdbcHandle( type, out_handle, pOdbcTest->listHandle ));
	}

	accept();
}

void DlgEnvAllocHandle::out_handle_ptr_clkd()
{
	if ( valid->isChecked() )
	    valid->setText( "OutputHandlePtr: SQL_NULL_HANDLE" );
	else
	    valid->setText( "OutputHandlePtr: VALID" );
}

DlgEnvAllocHandle::DlgEnvAllocHandle( OdbcTest *pOdbcTest, QString name )
        : QDialog( pOdbcTest )
{
	setWindowTitle( name );

	this->pOdbcTest = pOdbcTest;

	QVBoxLayout *playoutTop = new QVBoxLayout( this );
	QGridLayout *pLayout 	= new QGridLayout;

	playoutTop->addLayout( pLayout );

	l_handle = new QLabel( "InputHandle:", this );
	handles = new QComboBox( this );
	pOdbcTest->fill_handle_list( -1, handles );
	pLayout->addWidget( l_handle, 0, 0 );
	pLayout->addWidget( handles, 0, 1 );

	l_types = new QLabel( "HandleType:", this );
	types = new QComboBox( this );
	types->insertItem( 0, "SQL_HANDLE_ENV=1 (3.0)" );
	types->insertItem( 1, "SQL_HANDLE_DBC=2 (3.0)" );
	types->insertItem( 2, "SQL_HANDLE_STMT=3 (3.0)" );
	types->insertItem( 3, "SQL_HANDLE_DESC=3 (3.0)" );
	pLayout->addWidget( l_types, 1, 0 );
	pLayout->addWidget( types, 1, 1 );

	valid = new QCheckBox( "OutputHandlePtr: VALID", this );
	pLayout->addWidget( valid, 2, 1 );

	pDialogButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help, Qt::Horizontal, this );
	playoutTop->addWidget( pDialogButtonBox );

	connect( valid, SIGNAL( clicked()), this, SLOT( out_handle_ptr_clkd()));
	connect( pDialogButtonBox, SIGNAL(accepted()), this, SLOT(slotDone()) );
	connect( pDialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()) );
//	connect( pDialogButtonBox, SIGNAL(helpRequested()), this, SLOT(slotHelp()) );
}

DlgEnvAllocHandle::~DlgEnvAllocHandle()
{
	delete types;
	delete handles;
	delete l_handle;
	delete l_types;
	delete valid;
}


