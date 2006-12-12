#include <iostream>
#include <sstream>
using namespace std;

#include "otherutil.h"
#include "debug.h"


/*
	s_between ()
	return substr between the strings left and right
	not really needed, just for testing purposes
	*/

string
OtherUtil::s_between (string& line, const string left, const string right)
{
	unsigned int posleft, posright;
	
	if ((posleft = line.find (left)) != line.npos)
	{
		if ((posright = line.find (right)) != line.npos)
		{
			return line.substr(posleft + left.length(), posright - posleft - left.length());
		}
	}
	// no matching pair found
	return "";
}


/*
	s_trim ()
	strip spaces from both ends:
 	*/
 	
string
OtherUtil::s_trim(const string& s)
{
	if(s.length() == 0)
		return s;
	int b = s.find_first_not_of(" \t");
	int e = s.find_last_not_of(" \t\n");	
	if(b == -1) // No non-spaces
		return "";
	return string(s, b, e - b + 1);
}


/*
	s_split ()
	read line until delim[i] is found and return string between
	(delim[i-1] or benginl) and (delim[i] or endl).
	*/

vector<string>
OtherUtil::s_split (const string line, string *delim)
{
	int pos=0, newpos=0, len=0;
	vector<string> res;
	
	for (unsigned int i = 0; i < sizeof(delim); i++)
	{
		newpos = line.find (delim[i], pos + len);
		// append substr to result
		res.push_back (line.substr (pos + len, newpos - pos - len));
		
		pos = newpos;
		len = delim[i].length();
		
		//TRACE(("found " + delim[i] + ": ").c_str(), pos, 10, false);
		//TRACE(", " + res[res.size () - 1]);
	}
	res.push_back (line.substr (pos + len, line.length () - 1));

	//TRACE((res[res.size () - 1]));
	
	return res;
}

/*
	c_getdate ()
	converts string containing a specific date format into C time struc
	POSIX defines a suitable getdate(), but we don't use a POSIX layer
	C++ standard lib defines a suitable locale time_get facet with a method get_date, but what the hack ...

	mktime interprets values stored in tp as localtime (here they are MEZ in Hanover and Dresden)
	files downloaded are created with localtime
	timezones aren't handled properly, but this really makes no difference
	*/


int
OtherUtil::wildcmp(const char *wild, const char *str) {
  const char *cp = NULL, *mp = NULL;

  while ((*str) && (*wild != '*')) {
    if ((*wild != *str) && (*wild != '?')) {
      return 0;
    }
    wild++;
    str++;
  }

  while (*str) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = str+1;
    } else if ((*wild == *str) || (*wild == '?')) {
      wild++;
      str++;
    } else {
      wild = mp;
      str = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

//   stringSplit() can also be implemented using the <cstring> function strtok():
//   see: http://www.cplusplus.com/ref/cstring/strtok.html
//        http://www.codeguru.com/columns/DotNetTips/article.php/c9279/
//   char str[] ="This is a sample string,just testing.";
//   char * pch;
//   printf ("Splitting string \"%s\" in tokens:\n",str);
//   pch = strtok (str," ");
//   while (pch != NULL)
//   {
//     printf ("%s\n",pch);
//     pch = strtok (NULL, " ,.");
//   }

int
OtherUtil::stringSplit(const string& input, 
       const string& delimiter, vector<string>& results, 
       bool includeEmpties)
{
    int iPos = 0;
    int newPos = -1;
    int sizeS2 = (int)delimiter.size();
    int isize = (int)input.size();

    if( 
        ( isize == 0 )
        ||
        ( sizeS2 == 0 )
    )
    {
        return 0;
    }

    vector<int> positions;

    newPos = input.find (delimiter, 0);

    if( newPos < 0 )
    { 
        return 0; 
    }

    int numFound = 0;

    while( newPos >= iPos )
    {
        numFound++;
        positions.push_back(newPos);
        iPos = newPos;
        newPos = input.find (delimiter, iPos+sizeS2);
    }

    if( numFound == 0 )
    {
        return 0;
    }

    for( int i=0; i <= (int)positions.size(); ++i )
    {
        string s("");
        if( i == 0 ) 
        { 
            s = input.substr( i, positions[i] ); 
        }
        int offset = positions[i-1] + sizeS2;
        if( offset < isize )
        {
            if( i == positions.size() )
            {
                s = input.substr(offset);
            }
            else if( i > 0 )
            {
                s = input.substr( positions[i-1] + sizeS2, 
                      positions[i] - positions[i-1] - sizeS2 );
            }
        }
        if( includeEmpties || ( s.size() > 0 ) )
        {
            results.push_back(s);
        }
    }
    return numFound;
}


string
OtherUtil::s_join(vector<string> strings, const string& delim)
{
    string res;
    for(vector<string>::iterator i = strings.begin(); i != strings.end(); i++) {
        res.append((*i) + delim);
    }
    return res.substr(0, res.length() - delim.length());
}


string
OtherUtil::s_replace(const string& input, const char c, const char r)
{
    string res(input);
    int i;
    while ((i = input.find(c)) != string::npos) {
        res[i] = r;
    }
    return res;
}


// void
// OtherUtil::stringVectorConcat(vector<string>& result, const vector<string>& vec1, const vector<string>& vec2)
// {
//     result = vector<string>(vec1);
//     for(vector<string>::iterator i = vec2.begin(); i != vec2.end(); i++) {
//         result.push_back(*i);
//     }
// }


string
OtherUtil::printdate (time_t t)
{
	tm* res;
	char buf[18] = "";
	
	// FIX: is localtime() correct, or ctime(), gmtime() or other?
	res = localtime (&t);
	
	// FIX: maybe use one of the C-lib date formatting functions instead?
	if (res != NULL)
		sprintf (buf, "%02u.%02u.%4u %02u:%02u", 
                    res->tm_mday, res->tm_mon + 1, res->tm_year + 1900, res->tm_hour, res->tm_min);

	return buf;
}

void
OtherUtil::normalize_direntry (string& s)
{
	if (s.at (s.length () - 1) == '/')
		s.resize (s.length () - 1);
}


/*
	set_timestamp ()
	set modification and access time of file in path to values in date
	*/
	
void
OtherUtil::set_timestamp (string path, time_t date)
{
	//TRACE(("Setting time stamp to: ", date));
	struct utimbuf ut = {date, date};
	if (utime (path.c_str (), &ut) != 0)
	{
		//gp->error(string("set_timestamp, utime() failed.\n") + path + strerror (errno));
	}
}
