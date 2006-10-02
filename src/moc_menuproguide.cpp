/****************************************************************************
** MenuProGuide meta object code from reading C++ file 'menuproguide.h'
**
** Created: Sun Oct 1 22:44:03 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "menuproguide.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MenuProGuide::className() const
{
    return "MenuProGuide";
}

QMetaObject *MenuProGuide::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MenuProGuide( "MenuProGuide", &MenuProGuide::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MenuProGuide::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MenuProGuide", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MenuProGuide::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MenuProGuide", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MenuProGuide::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Menu::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "channel", &static_QUType_ptr, "QListViewItem", QUParameter::In }
    };
    static const QUMethod slot_0 = {"showChannelMenu", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "showChannelMenu(QListViewItem*)", &slot_0, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"MenuProGuide", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MenuProGuide.setMetaObject( metaObj );
    return metaObj;
}

void* MenuProGuide::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MenuProGuide" ) )
	return this;
    return Menu::qt_cast( clname );
}

bool MenuProGuide::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: showChannelMenu((QListViewItem*)static_QUType_ptr.get(_o+1)); break;
    default:
	return Menu::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MenuProGuide::qt_emit( int _id, QUObject* _o )
{
    return Menu::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MenuProGuide::qt_property( int id, int f, QVariant* v)
{
    return Menu::qt_property( id, f, v);
}

bool MenuProGuide::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *ProGuideEventFilter::className() const
{
    return "ProGuideEventFilter";
}

QMetaObject *ProGuideEventFilter::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ProGuideEventFilter( "ProGuideEventFilter", &ProGuideEventFilter::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ProGuideEventFilter::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ProGuideEventFilter", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ProGuideEventFilter::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ProGuideEventFilter", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ProGuideEventFilter::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"ProGuideEventFilter", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ProGuideEventFilter.setMetaObject( metaObj );
    return metaObj;
}

void* ProGuideEventFilter::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ProGuideEventFilter" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool ProGuideEventFilter::qt_invoke( int _id, QUObject* _o )
{
    return QObject::qt_invoke(_id,_o);
}

bool ProGuideEventFilter::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ProGuideEventFilter::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool ProGuideEventFilter::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
