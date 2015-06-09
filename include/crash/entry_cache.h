#ifndef ENTRY_CACHE_H
#define ENTRY_CACHE_H

#include "entry.h"

template <typename T>
struct entryCacheEntry
{
  entry *key;
  T data;
};

template <typename T>
struct itemCacheEntry
{
  entry *key;
  T data[32];
};

template <class T>
class entryCache
{
  private:
  
  int keyCount;
  entryCacheEntry<T> cache[0x100];  
  
  public:
	
  void reset();
  bool get(entry *key, T*&data);
  bool add(entry *key, T data);
};

template <typename T>
class itemCache
{
  private:
  
  int keyCount;
  itemCacheEntry<T> cache[0x100];

  public:
  void reset();
  bool get(entry *key, int item, T*&data);
  bool add(entry *key, int item, T data);
};

// - resets all cache pairs
template <class T>
void entryCache<T>::reset()
{
  keyCount = 0;
  
  for (int lp = 0; lp < 0x100; lp++)
  {
    cache[lp].key  = 0;
    cache[lp].data = 0;
  }
}

// - returns true if a new entry is added
// - returns false if the entry already exists in the cache
//   or there is no space left in cache for the entry
template <class T>
bool entryCache<T>::add(entry *key, T data=0)
{
  unsigned long EID        = key->EID;
  unsigned long cacheIndex = (EID >> 15) & 0xFF;
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
    if (cache[newIndex].key == 0)
    {
      if (keyCount < 0x100)
      {
        cache[newIndex].key  = key;
        cache[newIndex].data = data;
        keyCount++;
      }
      
      return true;
    }
    else if (cache[newIndex].key == key)
      break;
  }
  
  return false;
}

// - returns true if the entry key/data pair is found successfully
// - returns false if the entry key/data pair is not initially found, 
//   in which case a new pair must be created to associate the entry

// in either case, the passed by reference 'data' value is pointed to 
// the location of the associated data from the found or newly created 
// pair

template <class T>
bool entryCache<T>::get(entry *key, T *&data)
{
  unsigned long EID        = key->EID;
  unsigned long cacheIndex = (EID >> 15) & 0xFF;
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
    
    if (cache[newIndex].key == 0)
    {
      cache[newIndex].key = key;
      data = &cache[newIndex].data;
      break;
    }
    else if (cache[newIndex].key == key)
    {
      data = &cache[newIndex].data;
      return true;
    }
  }
  
  return false;
}

// - resets all items of each cache pair 
template <class T> 
void itemCache<T>::reset()
{
  keyCount = 0;
  
  for (int lp = 0; lp < 0x100; lp++)
  {
    cache[lp].key = 0;
    for (int item = 0; item < 32; item++)
      cache[lp].object[item] = 0;
  }
}
    
// - returns true if a new item is added
// - returns false if the item already exists in the cache
//   or there is no space left in cache for the item
template <class T>
bool itemCache<T>::add(entry *key, int item, T data=0)
{
  unsigned long EID        = key->EID;
  unsigned long cacheIndex = (EID >> 15) & 0xFF;
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
    if (cache[newIndex].key == 0)
    {
      if (keyCount < 0x100)
      {
        cache[newIndex].key        = key;
        cache[newIndex].data[item] = data;
        keyCount++;
      }
 
      return true;
    }
    else if (cache[newIndex].key == key)
    {
      if (cache[newIndex].object[item] == 0)
      {
        cache[newIndex].object[item] = data;
        return true;
      }        
      else
        break;
    }      
  }
  
  return false;
}
    
// - returns true if the entry key/data pair is found successfully
//   additionally the passed by reference 'data' value is pointed to the
//   location of the associated data in the pair for the requested item
// - returns false if the entry is not found or the requested item does 
//   not exist for the entry
template <class T>
bool itemCache<T>::get(entry *key, int item, T *&data)
{
  unsigned long EID        = key->EID;
  unsigned long cacheIndex = (EID >> 15) & 0xFF;
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
    if (cache[newIndex].key == 0)
    {
      data = &cache[newIndex].object[item];
      break;
    }
    else if (cache[newIndex].key == key)
    {
      data = &cache[newIndex].object[item];
      if (cache[newIndex].object[item] == 0)
        break;
        
      return true;
    }
  }
  
  return false;
}

#endif