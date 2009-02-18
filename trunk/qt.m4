# Copyright (C) 2001 David Johnson
# This file is free software; the author gives unlimited permission to  copy
# and/or distribute it, with or without modifications, as long as this notice
# is preserved.

#
# This was pulled from a gettext example on a SuSE 9.1 distro and has since been 
# enhanced to serve the purposes of unixODBC.
#
# Peter Harvey <pharvey@codebydesign.com> 30.AUG.05
#

#
# This was pulled from unixODBC and has since been 
# enhanced to serve the purposes of unixODBC-GUI-Qt.
#
# Peter Harvey <pharvey@codebydesign.com> 05.DEC.08
#


# FUN_CHECK_QT([qt_min_version],[qt_max_version])
# check for qt headers, libs, progs and compilation
# substs QT_CXXFLAGS, QT_LDFLAGS, and QT_LIBS
# substs QTVERSION, MOC and UIC
# MOC and UIC 'precious' variables

AC_DEFUN([FUN_CHECK_QT],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  # some 'precious' variables for configure --help
  AC_ARG_VAR(QTMIN, minimum version of Qt to search for  e.g. export QTMIN=040000)
  AC_ARG_VAR(QTMAX, maximum version of Qt to search for  e.g. export QTMAX=050000)
  AC_ARG_VAR(MOC, QT meta object compiler command)
  AC_ARG_VAR(UIC, Qt UI compiler command)

  AC_CACHE_SAVE

  AC_MSG_NOTICE([checking for Qt])

  # process our args
  if test -z "$1" ; then
    qt_min_version=0
  else
    qt_min_version=$1
  fi
  if test -z "$2" ; then
    qt_max_version=99999
  else
    qt_max_version=$2
  fi
  # adjust for user preferences
  if test "x$QTMIN" != "x" ; then
      qt_min_version=$QTMIN;
  fi
  if test "x$QTMAX" != "x" ; then
    if expr $QTMAX '<' $qt_max_version > /dev/null ; then
      qt_max_version=$QTMAX;
    fi
  fi

  # set up our configuration options
  qt_dir=""
  qt_includes=""
  qt_libraries=""
  qt_programs=""
  AC_ARG_WITH([qt_dir], AC_HELP_STRING([--with-qt-dir=DIR],
                        [where the Qt package is installed]),
    [ qt_dir="$withval"
      qt_includes="$withval"/include
      qt_libraries="$withval"/lib
      qt_programs="$withval"/bin
    ])
  AC_ARG_WITH([qt_includes], AC_HELP_STRING([--with-qt-includes=DIR],
                             [where the Qt includes are installed]),
    [qt_includes="$withval"])
  AC_ARG_WITH([qt_libraries], AC_HELP_STRING([--with-qt-libraries=DIR],
                              [where the Qt libraries are installed]),
    [qt_libraries="$withval"])
  AC_ARG_WITH([qt_programs], AC_HELP_STRING([--with-qt-programs=DIR],
                             [where the Qt programs are installed]),
    [qt_programs="$withval"])

  QTVERSION="00000"

  FUN_QT_HEADERS

  AC_MSG_NOTICE([Found Qt version $QTVERSION])

  FUN_QT_LIBRARIES
  FUN_QT_PROGRAMS
  FUN_QT_COMPILE

  AC_SUBST(QTVERSION)
  AC_SUBST(MOC)
  AC_SUBST(UIC)
  QT_CXXFLAGS="-I$qt_includes -I$qt_includes/QtCore -I$qt_includes/QtGui -I$qt_includes/QtAssistant -I$qt_includes/QtNetwork"
  AC_SUBST(QT_CXXFLAGS)
  QT_LDFLAGS="-L$qt_libraries"
  AC_SUBST(QT_LDFLAGS)
  QT_LIBS="$qt_libs"
  AC_SUBST(QT_LIBS)

  have_qt="yes"

])#FUN_CHECK_QT

# FUN_QT_HEADERS
# helper function for FUN_CHECK_QT
# check for qt headers in standard locations

AC_DEFUN([FUN_QT_HEADERS],
[

  qt_found_version=""
  qt_v4_include=""

  # Have not been told where Qt is so search for it...
  if test "x$qt_includes" = "x" ; then
    # look in standard locations
    qt_found_dirs=""
    qt_include_dirs="
      $QTDIR/include
      /usr/include
      /usr/local/include
      /usr/X11R6/include
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/local/include/qt* 2>/dev/null`
      `ls -dr /usr/X11R6/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/X11R6/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /usr/local/Trolltech/Qt*/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null` "
    for n in $qt_include_dirs ; do
      if test -r "$n/qglobal.h"; then
        qt_found_dirs="$qt_found_dirs $n"
      else
        if test -r "$n/Qt/qglobal.h"; then
          qt_found_dirs="$qt_found_dirs $n"
	fi
      fi
    done

    # find the latest version between min_version and max_version
    qt_prev_version=$qt_min_version
    for n in $qt_found_dirs ; do

    AC_MSG_NOTICE([Checking dir $n])
      FUN_QT_VERSION($n)	

      if expr $qt_current_version '>=' $qt_prev_version > /dev/null ; then
        if expr $qt_current_version '<=' $qt_max_version > /dev/null ; then
          qt_includes=$n
          qt_prev_version=$qt_current_version
          qt_found_version=$qt_current_version
        fi
      fi
    done
  fi

  AC_MSG_CHECKING([for Qt headers])

  if test "x$qt_includes" = "x" ; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([cannot find correct Qt headers!])
  else
    # Ensure we have a version...
    if test "x$qt_found_version" = "x" ; then
      FUN_QT_VERSION($qt_includes)
      qt_found_version=$qt_current_version
    fi
    if test "x$qt_found_version" = "x" ; then
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([cannot find version of Qt!])
    fi

    AC_MSG_RESULT([$qt_includes])

    AC_MSG_CHECKING([for Qt4 headers])
    if test -r "$qt_includes/Qt/qglobal.h"; then
	  qt_v4_include="y"
      AC_MSG_RESULT([yes])
	else
      AC_MSG_RESULT([no])
	fi

    dnl TODO need to strip out white space
    QTVERSION=$qt_found_version;
    AC_MSG_CHECKING([include])
    if test "x$qt_v4_include" = "xy" ; then
      have_qt4="yes"
    else
      MOCDEFS=""
      AC_SUBST(MOCDEFS)
    fi
  fi
])#FUN_QT_HEADERS

# FUN_QT_VERSION(qt_include_dir)
# helper function for FUN_QT_HEADERS
# get version from qglobal.h

AC_DEFUN([FUN_QT_VERSION],
[
  dnl the 2x versions need the extra sed 
  if test -r "$1/qglobal.h"; then
    qt_current_version=`grep -w '#define QT_VERSION' "$1"/qglobal.h | grep -v '#define QT_VERSION_STR' |
    sed s/'#define QT_VERSION 0x'// | sed s/'#define QT_VERSION'//`
  else
    if test -r "$1/Qt/qglobal.h"; then
# if test "x$qt_current_version" = "x" ; then
      qt_current_version=`grep -w '#define QT_VERSION' "$1"/Qt/qglobal.h | grep -v '#define QT_VERSION_STR' |
      sed s/'#define QT_VERSION 0x'// | sed s/'#define QT_VERSION'//`
    fi
  fi
])#FUN_QT_VERSION

# FUN_QT_LIBRARIES
# helper function for FUN_CHECK_QT
# check for qt libs in standard locations

AC_DEFUN([FUN_QT_LIBRARIES],
[
  AC_REQUIRE([FUN_QT_HEADERS])

  AC_MSG_CHECKING([for Qt libraries])

  qt_libs="-lQtGui -lQtCore -lQtAssistantClient -lQtNetwork"

  # Return...
  if test "x$qt_libraries" = "x" ; then
    AC_MSG_RESULT([no (try specifying with --with-qt-libraries or try a different value)])
    AC_MSG_ERROR([cannot find Qt libraries!])
  else
    AC_MSG_RESULT([yes ($qt_libraries)])
    AC_CHECK_FILE([$qt_includes/QtGui/QWizard], [have_qtwizard=yes], [have_qtwizard=no])
    AC_CHECK_FILE([$qt_includes/QtGui/QMdiArea], [have_qtmdiarea=yes], [have_qtmdiarea=no])
  fi
])#FUN_QT_LIBRARIES

# FUN_QT_PROGRAMS
# helper function for FUN_CHECK_QT
# searches for moc and uic

AC_DEFUN([FUN_QT_PROGRAMS],
[
  AC_REQUIRE([FUN_QT_LIBRARIES])

  AC_MSG_CHECKING([for Qt utilities])

  if test "x$q_programs" = "x" ; then
    # see if it is relative to the libraries
    qt_tree="$qt_libraries"
    while test "x$qt_tree" != "x" ; do
      # first go around will fail
      if ls $qt_tree/bin/moc* > /dev/null 2> /dev/null ; then
        qt_programs=$qt_tree/bin
        break
      else
        # lop off tail of path
        dnl not as portable as it should be...
        qt_tree="`dirname $qt_tree`"
      fi
    done
    # if we haven't found the progs, there's not much more we can do
  fi  

  if test "x$qt_programs" = "x" ; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([cannot find Qt utilities!])
  else
    AC_MSG_RESULT([$qt_programs])
    # find the right moc
    if test -z "$MOC" ; then
      AC_CHECK_PROG(MOC, moc, moc )
      if test "x$MOC" = "x" ; then
        # could be renamed to avoid clashes
        if ls $qt_programs/moc > /dev/null 2> /dev/null ; then
          MOC="$qt_programs/moc"
        else
          if expr "$QTVERSION" '>=' "200" > /dev/null ; then
            if ls $qt_programs/moc2 > /dev/null 2> /dev/null ; then
              MOC="$qt_programs/moc2"
            fi
          else
            if expr "$QTVERSION" '>=' "300" > /dev/null ; then
              if $qt_programs/moc3 > /dev/null 2> /dev/null ; then
                MOC="$qt_programs/moc3"
              fi
            fi
          fi
        fi
      fi
      if test "x$MOC" = "x" ; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([cannot find Qt meta object compiler!])
      fi
    fi

    # find the right uic
    if expr "$QTVERSION" '>=' "220" > /dev/null ; then
      if test -z "$UIC" ; then
        AC_CHECK_PROG(UIC, uic, uic )
        if test "x$UIC" = "x" ; then
          # could be renamed to avoid clashes
          if ls $qt_programs/uic > /dev/null 2> /dev/null ; then
            UIC="$qt_programs/uic"
          else
            if expr "$QTVERSION" '>=' "300" > /dev/null ; then
              if ls $qt_programs/uic3 > /dev/null 2> /dev/null ; then
                UIC="$qt_programs/uic3"
              fi
            fi
          fi
        fi
      fi
    else
      # if uic is important to the build, change this
      UIC=""
    fi
  fi
])#FUN_QT_PROGRAMS

# FUN_QT_COMPILE
# helper function for FUN_CHECK_QT
# compile a simple qt program

AC_DEFUN([FUN_QT_COMPILE],
[
  AC_REQUIRE([FUN_QT_HEADERS])
  AC_REQUIRE([FUN_QT_LIBRARIES])
  AC_REQUIRE([FUN_QT_PROGRAMS])


  AC_LANG_PUSH(C++)

AC_MSG_NOTICE([qt_libraries=$qt_libraries])
AC_MSG_NOTICE([LDFLAGS=$LDFLAGS])
AC_MSG_NOTICE([X_LIBS=$X_LIBS])

  AC_MSG_CHECKING([whether a simple Qt program compiles])

  ac_cxxflags_save="$CXXFLAGS"
  ac_ldflags_save="$LDFLAGS"
  ac_libs_save="$LIBS"
  if expr "$QTVERSION" '>=' "040000" > /dev/null ; then
    CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS -I$qt_includes -I$qt_includes/QtCore -I$qt_includes/QtGui $X_CFLAGS $all_includes"
  else
    CXXFLAGS="$CXXFLAGS $PTHREAD_CFLAGS -I$qt_includes $X_CFLAGS $all_includes"
  fi
  LDFLAGS="$LDFLAGS -L$qt_libraries $X_LIBS $X_LDFLAGS"
  LIBS="$LIBS $PTHREAD_LIBS $qt_libs $X_EXTRA_LIBS -lXext -lX11 $X_PRE_LIBS $EXTRA_XLIBS"

  if expr "$QTVERSION" '>=' "040000" > /dev/null ; then
    AC_TRY_LINK([
      #include <QMessageBox>
      #include <QString>],
      [QString s = "hello world";
      QMessageBox::information(0, s, "no he is not");
      return 0;],
    qt_compile=yes, qt_compile=no)
  else
    AC_TRY_LINK([
      #include <qmessagebox.h>
      #include <qstring.h>],
      [QString s = "hello world";
      QMessageBox::information(0, s, "no he is not");
      return 0;],
    qt_compile=yes, qt_compile=no)
  fi

  CXXFLAGS="$ac_cxxflags_save"
  LDFLAGS="$ac_ldflags_save"
  LIBS="$ac_libs_save"

  AC_LANG_POP(C++)

  if test "x$qt_compile" = "xyes" ; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([cannot compile a Qt program!])
  fi
])#FUN_QT_COMPILE
