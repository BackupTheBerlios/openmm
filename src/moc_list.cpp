/****************************************************************************
** List meta object code from reading C++ file 'list.h'
**
** Created: Tue Dec 5 17:33:44 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "list.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *List::className() const
{
    return "List";
}

QMetaObject *List::metaObj = 0;
static QMetaObjectCleanUp cleanUp_List( "List", &List::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString List::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "List", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString List::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "List", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* List::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "entry", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod slot_0 = {"addTitle", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "entry", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod slot_1 = {"delTitle", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "entry", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod slot_2 = {"addTitleEntry", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "entry", &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod slot_3 = {"delTitleEntry", 1, param_slot_3 };
    static const QMetaData slot_tbl[] = {
	{ "addTitle(Title*)", &slot_0, QMetaData::Public },
	{ "delTitle(Title*)", &slot_1, QMetaData::Public },
	{ "addTitleEntry(Title*)", &slot_2, QMetaData::Public },
	{ "delTitleEntry(Title*)", &slot_3, QMetaData::Public }
    };
    static const QUMethod signal_0 = {"changed", 0, 0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod signal_1 = {"pushTitle", 1, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_ptr, "Title", QUParameter::In }
    };
    static const QUMethod signal_2 = {"popTitle", 1, param_signal_2 };
    static const QMetaData signal_tbl[] = {
	{ "changed()", &signal_0, QMetaData::Public },
	{ "pushTitle(Title*)", &signal_1, QMetaData::Public },
	{ "popTitle(Title*)", &signal_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"List", parentObject,
	slot_tbl, 4,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_List.setMetaObject( metaObj );
    return metaObj;
}

void* List::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "List" ) )
	return this;
    return QObject::qt_cast( clname );
}

// SIGNAL changed
void List::changed()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL pushTitle
void List::pushTitle( Title* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL popTitle
void List::popTitle( Title* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

bool List::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: addTitle((Title*)static_QUType_ptr.get(_o+1)); break;
    case 1: delTitle((Title*)static_QUType_ptr.get(_o+1)); break;
    case 2: addTitleEntry((Title*)static_QUType_ptr.get(_o+1)); break;
    case 3: delTitleEntry((Title*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool List::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: changed(); break;
    case 1: pushTitle((Title*)static_QUType_ptr.get(_o+1)); break;
    case 2: popTitle((Title*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QObject::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool List::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool List::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
