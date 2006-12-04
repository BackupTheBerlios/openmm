/****************************************************************************
** TvRecPlayer meta object code from reading C++ file 'tvrecplayer.h'
**
** Created: Tue Dec 5 17:33:48 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tvrecplayer.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *TvRecPlayer::className() const
{
    return "TvRecPlayer";
}

QMetaObject *TvRecPlayer::metaObj = 0;
static QMetaObjectCleanUp cleanUp_TvRecPlayer( "TvRecPlayer", &TvRecPlayer::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString TvRecPlayer::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvRecPlayer", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString TvRecPlayer::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "TvRecPlayer", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* TvRecPlayer::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = MediaPlayer::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"TvRecPlayer", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_TvRecPlayer.setMetaObject( metaObj );
    return metaObj;
}

void* TvRecPlayer::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "TvRecPlayer" ) )
	return this;
    return MediaPlayer::qt_cast( clname );
}

bool TvRecPlayer::qt_invoke( int _id, QUObject* _o )
{
    return MediaPlayer::qt_invoke(_id,_o);
}

bool TvRecPlayer::qt_emit( int _id, QUObject* _o )
{
    return MediaPlayer::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool TvRecPlayer::qt_property( int id, int f, QVariant* v)
{
    return MediaPlayer::qt_property( id, f, v);
}

bool TvRecPlayer::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
