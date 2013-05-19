#include "lockmap.h"

LockMap& LockMap::get_instance()
{
    static LockMap l_instance;
    return l_instance;
}

void LockMap::lockReadLock(string table_name) {
    LockMapType::iterator it = this->lockmap.find(table_name);
    if (it != lockmap.end()) {
        it->second.first->lock();
        it->second.second->lock();
    } else {
        boost::mutex* readlock = new boost::mutex;
        boost::mutex* writelock = new boost::mutex;
        MutexPairType lock_pair(readlock, writelock);
        this->lockmap.insert(pair<string, MutexPairType>(table_name, lock_pair));
        lock_pair.first->lock();
        lock_pair.second->lock();
    }
}

void LockMap::unlockReadLock(string table_name) {
    LockMapType::iterator it = this->lockmap.find(table_name);
    if (it != lockmap.end()) {
        (*it).second.first->unlock();
        (*it).second.second->unlock();
    }
}

void LockMap::lockWriteLock(string table_name){
    LockMapType::iterator it = this->lockmap.find(table_name);
    if (it != lockmap.end())
        (*it).second.second->lock();
    else {
        boost::mutex* readlock = new boost::mutex;
        boost::mutex* writelock = new boost::mutex;
        MutexPairType lock_pair(readlock, writelock);
        this->lockmap.insert(pair<string, MutexPairType>(table_name, lock_pair));
        lock_pair.second->lock();
    }
}


void LockMap::unlockWriteLock(string table_name) {
    LockMapType::iterator it = this->lockmap.find(table_name);
    if (it != lockmap.end())
        it->second.second->unlock();
}
