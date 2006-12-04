/****************************************************************************
** StreamPlayer meta object code from reading C++ file 'streamplayer.h'
**
** Created: Tue Dec 5 17:33:43 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "streamplayer.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *StreamPlayer::className() const
{
    return "StreamPlayer";
}

QMetaObject *StreamPlayer::metaObj = 0;
static QMetaObjectCleanUp cleanUp_StreamPlayer( "StreamPlayer", &StreamPlayer::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString StreamPlayer::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "StreamPlayer", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString StreamPlayer::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "StreamPlayer", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* StreamPlayer::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Page::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "title", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod slot_0 = {"play", 1, param_slot_0 };
    static const QUMethod slot_1 = {"stop", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ "text", &static_QUType_QString, 0, QUParameter::In },
	{ "duration", &static_QUType_varptr, "\x11", QUParameter::In }
    };
    static const QUMethod slot_2 = {"showOsd", 2, param_slot_2 };
    static const QUMethod slot_3 = {"hideOsd", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "play(Title*)", &slot_0, QMetaData::Public },
	{ "stop()", &slot_1, QMetaData::Public },
	{ "showOsd(QString,uint)", &slot_2, QMetaData::Public },
	{ "hideOsd()", &slot_3, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"StreamPlayer", parentObject,
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_StreamPlayer.setMetaObject( metaObj );
    return metaObj;
}

void* StreamPlayer::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "StreamPlayer" ) )
	return this;
    return Page::qt_cast( clname );
}

bool StreamPlayer::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: play((Title*)static_QUType_ptr.get(_o+1)); break;
    case 1: stop(); break;
    case 2: showOsd((QString)static_QUType_QString.get(_o+1),(uint)(*((uint*)static_QUType_ptr.get(_o+2)))); break;
    case 3: hideOsd(); break;
    default:
	return Page::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool StreamPlayer::qt_emit( int _id, QUObject* _o )
{
    return Page::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool StreamPlayer::qt_property( int id, int f, QVariant* v)
{
    return Page::qt_property( id, f, v);
}

bool StreamPlayer::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
