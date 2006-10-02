/****************************************************************************
** JAMStreamPlayer meta object code from reading C++ file 'jamstreamplayer.h'
**
** Created: Sun Oct 1 20:47:57 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "jamstreamplayer.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *JAMStreamPlayer::className() const
{
    return "JAMStreamPlayer";
}

QMetaObject *JAMStreamPlayer::metaObj = 0;
static QMetaObjectCleanUp cleanUp_JAMStreamPlayer( "JAMStreamPlayer", &JAMStreamPlayer::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString JAMStreamPlayer::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "JAMStreamPlayer", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString JAMStreamPlayer::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "JAMStreamPlayer", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* JAMStreamPlayer::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "mrl", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"play", 1, param_slot_0 };
    static const QUMethod slot_1 = {"stop", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ "text", &static_QUType_QString, 0, QUParameter::In },
	{ "duration", &static_QUType_varptr, "\x11", QUParameter::In }
    };
    static const QUMethod slot_2 = {"showOSD", 2, param_slot_2 };
    static const QUMethod slot_3 = {"hideOSD", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "play(QString)", &slot_0, QMetaData::Public },
	{ "stop()", &slot_1, QMetaData::Public },
	{ "showOSD(QString,uint)", &slot_2, QMetaData::Public },
	{ "hideOSD()", &slot_3, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"JAMStreamPlayer", parentObject,
	slot_tbl, 4,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_JAMStreamPlayer.setMetaObject( metaObj );
    return metaObj;
}

void* JAMStreamPlayer::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "JAMStreamPlayer" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool JAMStreamPlayer::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: play((QString)static_QUType_QString.get(_o+1)); break;
    case 1: stop(); break;
    case 2: showOSD((QString)static_QUType_QString.get(_o+1),(uint)(*((uint*)static_QUType_ptr.get(_o+2)))); break;
    case 3: hideOSD(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool JAMStreamPlayer::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool JAMStreamPlayer::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool JAMStreamPlayer::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
