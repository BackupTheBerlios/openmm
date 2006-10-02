/****************************************************************************
** MenuMain meta object code from reading C++ file 'menumain.h'
**
** Created: Sun Oct 1 20:48:01 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "menumain.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MenuMain::className() const
{
    return "MenuMain";
}

QMetaObject *MenuMain::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MenuMain( "MenuMain", &MenuMain::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MenuMain::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MenuMain", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MenuMain::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MenuMain", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MenuMain::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Menu::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "i", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_0 = {"menuItemSelected", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "menuItemSelected(QListViewItem*)", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"MenuMain", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MenuMain.setMetaObject( metaObj );
    return metaObj;
}

void* MenuMain::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MenuMain" ) )
	return this;
    return Menu::qt_cast( clname );
}

bool MenuMain::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: menuItemSelected((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    default:
	return Menu::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MenuMain::qt_emit( int _id, QUObject* _o )
{
    return Menu::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MenuMain::qt_property( int id, int f, QVariant* v)
{
    return Menu::qt_property( id, f, v);
}

bool MenuMain::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
