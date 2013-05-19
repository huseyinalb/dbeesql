
#ifndef __dbeesql__lockmap__
#define __dbeesql__lockmap__

#include <iostream>
#include <map>
#include <boost/thread/mutex.hpp>
#include <list>

using namespace std;

typedef pair< boost::mutex*, boost::mutex* > MutexPairType;

typedef map< string, MutexPairType > LockMapType;

class LockMap
{
public:
    static LockMap& get_instance();
    // pair : read, write
    LockMapType lockmap;
    void lockReadLock(string table_name);
    void lockWriteLock(string table_name);
    void unlockReadLock(string table_name);
    void unlockWriteLock(string table_name);
};


#endif /* defined(__dbeesql__lockmap__) */
