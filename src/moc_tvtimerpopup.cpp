/****************************************************************************
** TvTimerPopup meta object code from reading C++ file 'tvtimerpopup.h'
**
** Created: Tue Dec 5 17:33:53 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tvtimerpopup.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *TvTimerPopup::className() const
{
    return "TvTimerPopup";
}

QMetaObject *TvTimerPopup::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TvTimerPopup( "TvTimerPopup", &TvTimerPopup::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TvTimerPopup::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvTimerPopup", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TvTimerPopup::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvTimerPopup", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TvTimerPopup::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = PopupMenu::staticMetaObject();
    static const QUMethod slot_0 = {"timerEdit", 0, 0 };
    static const QUMethod slot_1 = {"timerDelete", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "timerEdit()", &slot_0, QMetaData::Private },
	{ "timerDelete()", &slot_1, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"TvTimerPopup", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TvTimerPopup.setMetaObject( metaObj );
    return metaObj;
}

void* TvTimerPopup::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TvTimerPopup" ) )
	return this;
    return PopupMenu::qt_cast( clname );
}

bool TvTimerPopup::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: timerEdit(); break;
    case 1: timerDelete(); break;
    default:
	return PopupMenu::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool TvTimerPopup::qt_emit( int _id, QUObject* _o )
{
    return PopupMenu::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool TvTimerPopup::qt_property( int id, int f, QVariant* v)
{
    return PopupMenu::qt_property( id, f, v);
}

bool TvTimerPopup::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
