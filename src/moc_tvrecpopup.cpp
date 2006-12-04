/****************************************************************************
** TvRecPopup meta object code from reading C++ file 'tvrecpopup.h'
**
** Created: Tue Dec 5 17:33:54 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tvrecpopup.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *TvRecPopup::className() const
{
    return "TvRecPopup";
}

QMetaObject *TvRecPopup::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TvRecPopup( "TvRecPopup", &TvRecPopup::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TvRecPopup::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvRecPopup", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TvRecPopup::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvRecPopup", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TvRecPopup::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = PopupMenu::staticMetaObject();
    static const QUMethod slot_0 = {"play", 0, 0 };
    static const QUMethod slot_1 = {"del", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "play()", &slot_0, QMetaData::Private },
	{ "del()", &slot_1, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"TvRecPopup", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TvRecPopup.setMetaObject( metaObj );
    return metaObj;
}

void* TvRecPopup::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TvRecPopup" ) )
	return this;
    return PopupMenu::qt_cast( clname );
}

bool TvRecPopup::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: play(); break;
    case 1: del(); break;
    default:
	return PopupMenu::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool TvRecPopup::qt_emit( int _id, QUObject* _o )
{
    return PopupMenu::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool TvRecPopup::qt_property( int id, int f, QVariant* v)
{
    return PopupMenu::qt_property( id, f, v);
}

bool TvRecPopup::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
