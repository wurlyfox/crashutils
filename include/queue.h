#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

template<class U> class queue
{
  private:
  
  U *data;
  int size;
  int head;
  int tail;
   
  public:
  
  queue() {};
  queue(int initsize);
  ~queue();

  void allocate(int initsize);  
  void free();
  
  void reset(int newsize);
  void inflate(int newsize);

  void add(U src);
  U remove();
  bool empty();
};

template <class U>
queue<U>::queue(int initsize)
{
  allocate(initsize);
}

template <class U>
queue<U>::~queue()
{
  free();
}

template <class U>
void queue<U>::allocate(int initsize)
{
  size = initsize;
  data = (U*)malloc(sizeof(U)*size);
  
  head = 0;
  tail = 0;
}

template <class U>
void queue<U>::free()
{
  ::free(data);
}

template <class U>
void queue<U>::reset(int newsize)
{
  free();
  allocate(newsize);
}

template <class U>
void queue<U>::add(U src)
{
  data[tail++] = src;
  if (tail == size) { tail = 0; }
  if (head == tail) { inflate(size*2); }
}

template <class U>
U queue<U>::remove()
{
  if (head != tail) 
  {
    
  U elem = data[head++];
  if (head == size) { head = 0; }
  return elem;
  
  }
}

template <class U>
void queue<U>::inflate(int newsize)
{
  int i = head;
  int j = 0;
  U *q = (U*)malloc(sizeof(U)*newsize);

  if (tail < i)
  {  
    for (; i < size; i++)
      q[j++] = data[i];
    i = 0;
  }
  for (; i <= tail; i++)
    q[j++] = data[i];
    
  free();
  data = q;
  
  head = 0;
  tail = j;
}

template <class U>
bool queue<U>::empty()
{
  return (head == tail);
}

#endif