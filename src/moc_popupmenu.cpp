/****************************************************************************
** PopupMenu meta object code from reading C++ file 'popupmenu.h'
**
** Created: Tue Dec 5 17:33:53 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "popupmenu.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *PopupMenu::className() const
{
    return "PopupMenu";
}

QMetaObject *PopupMenu::metaObj = 0;
static QMetaObjectCleanUp cleanUp_PopupMenu( "PopupMenu", &PopupMenu::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString PopupMenu::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "PopupMenu", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString PopupMenu::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "PopupMenu", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* PopupMenu::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"PopupMenu", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_PopupMenu.setMetaObject( metaObj );
    return metaObj;
}

void* PopupMenu::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "PopupMenu" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool PopupMenu::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool PopupMenu::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool PopupMenu::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool PopupMenu::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
