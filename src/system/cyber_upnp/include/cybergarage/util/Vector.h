/******************************************************************
*
*	CyberUtil for C++
*
*	Copyright (C) Satoshi Konno 2002-2003
*
*	File: Vector.h
*
*	Revision;
*
*	04/17/03
*		- first revision
*	08/04/04
*		- Changed indexOf() and get() not to use at().
*
******************************************************************/

#ifndef _CUTIL_VECTOR_H_
#define _CUTIL_VECTOR_H_

#include <vector>

namespace CyberUtil {

class Vector : public std::vector<void *>
{
public:

	Vector()
	{
	}

	bool add(void *obj)
	{
		if (0 <= indexOf(obj))
			return false;
		push_back(obj);
		return true;
	}

	bool remove(void *obj)
	{
		int idx = indexOf(obj);
		if (idx < 0)
			return false;
		erase(begin() + idx);
		return true;
	}

	int indexOf(void *obj)
	{
		int cnt = size();
		for (int n=0; n<cnt; n++) {
			//if (obj == ((void *)at(n)))
			if (obj == (void *)((*this)[n]))
				return n;
		}
		return -1;
	}

	void *get(int index) 
	{
		if (index < 0)
			return NULL;
		if ((int)size() < (index+1))
			return NULL;
		return (void *)((*this)[index]);
		//return (void *)at(index);
	}

	bool insertAt(void *obj, int index)
	{
		if (0 <= indexOf(obj))
			return false;
		insert(begin() + index, obj);
		return true;
	}
};

}

#endif
