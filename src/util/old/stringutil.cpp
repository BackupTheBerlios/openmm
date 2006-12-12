#include <iostream>
#include <sstream>
using namespace std;

#include "utils.h"
#include "debug.h"
#include "glue.h"

/*
	c_string()
	casts std::string to char*
	not really needed, using std::string.c_str() instead
	*/
	
char* 
c_string (string& s)
{
	char* buf = new char[s.length()+1];
	s.copy (buf, s.length());
	buf[s.length()] = '\0';  // C strings need null termination
	return buf;  // probably a memory leak
}

/*
	s_between ()
	return substr between the strings left and right
	not really needed, just for testing purposes
	*/

string
s_between (string& line, const string left, const string right)
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
s_trim(const string& s)
{
	if(s.length() == 0)
		return s;
	int b = s.find_first_not_of(" \t");
	int e = s.find_last_not_of(" \t");	
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
s_split (const string line, string *delim)
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
		
		TRACE((("found " + delim[i] + ": ").c_str(), pos, 10, false));
		TRACE((", " + res[res.size () - 1]));
	}
	res.push_back (line.substr (pos + len, line.length () - 1));

	TRACE((res[res.size () - 1]));
	
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

static string months = "JanFebMarAprMayJunJulAugSepOctNovDec";

time_t
getdate (string& s)
{
	istringstream sstr(s);
	char monthstr[3], c;
	tm res;
	time_t retval;  // time_t is long int, or similar
	
	sstr >> res.tm_mday >> c;
	sstr.get(monthstr, 4); // read one more char, is this '\0' ?
	sstr >> c >> res.tm_year >> res.tm_hour >> c >> res.tm_min;
	
	res.tm_year -= 1900;  // tm_year is years since 1900	
	res.tm_mon = months.find (monthstr) / 3;
	res.tm_sec = 1;
	res.tm_isdst = -1;	
	//TRACE(("getdate: " << res.tm_year << "-" << res.tm_mon << "-" << res.tm_mday << " " << res.tm_hour << ":" << res.tm_min))
	
	retval = mktime (&res);	
	TRACE(("getdate, mktime returns: ", retval));
	if (retval == -1)
	{
		gp->error(string("getdate, mktime() failed.\n") + strerror (errno));
	}
	
	return retval;	
}

string
printdate (time_t t)
{
	tm* res;
	char buf[18] = "";
	
	// FIX: is localtime() correct, or ctime(), gmtime() or other?
	res = localtime (&t);
	
	// FIX: maybe use one of the C-lib date formatting functions instead?
	if (res != NULL)
		sprintf (buf, "%02u.%02u.%4u %02u:%02u", res->tm_mday, res->tm_mon + 1, res->tm_year + 1900, res->tm_hour, res->tm_min);

	return buf;
}

void
normalize_direntry (string& s)
{
	if (s.at (s.length () - 1) == '/')
		s.resize (s.length () - 1);
}


/*
	set_timestamp ()
	set modification and access time of file in path to values in date
	*/
	
void
set_timestamp (string path, time_t date)
{
	TRACE(("Setting time stamp to: ", date));
	struct utimbuf ut = {date, date};
	if (utime (path.c_str (), &ut) != 0)
	{
		gp->error(string("set_timestamp, utime() failed.\n") + path + strerror (errno));
	}
}


/*
	change_directory ()
 	change into directory path, honoring a leading drive letter on windows platform
	(creating sub directories if necessary (FIX: this is not implemented, yet))
	*/
	
void
change_directory (string path)
{
#ifdef __WIN32__
	if (!SetCurrentDirectory (path.c_str ()))
		gp->error(string("change_directory, SetCurrentDirectory() failed.\n") + path);
#else
	if (chdir (destpath.c_str ()) != 0)
 		gp->error(string("change_directory, chdir() failed.\n") + path + ": " + strerror (errno));	
#endif // __WIN32__		
}    


string
current_directory ()
{
#ifdef __WIN32__
	char buffer[1024];
	if (!GetCurrentDirectory (1024, buffer))
		gp->error(string("current_directory, GetCurrentDirectory() failed.\n") + buffer);
	return buffer;
#else
	// FIX: not implemented yet
#endif // __WIN32__	
}


void
CryptPWD (string* pwd)
{
	TRACE(("CryptPWD"));
	for (unsigned int i = 0; i < pwd->length (); i++)
		(*pwd)[i] = ~((*pwd)[i]);
}
