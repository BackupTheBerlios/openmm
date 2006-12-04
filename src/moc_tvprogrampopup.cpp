/****************************************************************************
** TvProgramPopup meta object code from reading C++ file 'tvprogrampopup.h'
**
** Created: Tue Dec 5 17:33:55 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tvprogrampopup.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *TvProgramPopup::className() const
{
    return "TvProgramPopup";
}

QMetaObject *TvProgramPopup::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TvProgramPopup( "TvProgramPopup", &TvProgramPopup::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TvProgramPopup::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvProgramPopup", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TvProgramPopup::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvProgramPopup", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TvProgramPopup::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = PopupMenu::staticMetaObject();
    static const QUMethod slot_0 = {"addTimer", 0, 0 };
    static const QUMethod slot_1 = {"switchChannel", 0, 0 };
    static const QUMethod slot_2 = {"showChannel", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "addTimer()", &slot_0, QMetaData::Private },
	{ "switchChannel()", &slot_1, QMetaData::Private },
	{ "showChannel()", &slot_2, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"TvProgramPopup", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TvProgramPopup.setMetaObject( metaObj );
    return metaObj;
}

void* TvProgramPopup::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TvProgramPopup" ) )
	return this;
    return PopupMenu::qt_cast( clname );
}

bool TvProgramPopup::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: addTimer(); break;
    case 1: switchChannel(); break;
    case 2: showChannel(); break;
    default:
	return PopupMenu::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool TvProgramPopup::qt_emit( int _id, QUObject* _o )
{
    return PopupMenu::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool TvProgramPopup::qt_property( int id, int f, QVariant* v)
{
    return PopupMenu::qt_property( id, f, v);
}

bool TvProgramPopup::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
