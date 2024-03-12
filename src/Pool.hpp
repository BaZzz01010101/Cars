#pragma once

namespace game
{
  template <class T>
  class Pool
  {
  public:
    Pool(int capacity) :
      //objects(capacity),
      alive(capacity, false),
      capacity(capacity),
      count(0)
    {
      objects = static_cast<T*>(malloc(sizeof(T) * capacity));
    }

    ~Pool()
    {
      clear();
      free(objects);
    }

    template <typename... Args>
    int tryAdd(Args&&... args)
    {
      _ASSERT(count < capacity);

      if (count < capacity)
        for (int i = 0; i < capacity; i++)
          if (!alive[i])
          {
            new (&objects[i]) T(std::forward<Args>(args)...);
            alive[i] = true;
            count++;

            return i;
          }

      _ASSERT(false);

      return -1;
    }

    void remove(int index)
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(alive[index]);

      objects[index].~T();
      alive[index] = false;
      count--;
    }

    T& get(int index)
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(alive[index]);

      return objects[index];
    }

    const T& get(int index) const
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(alive[index]);

      return objects[index];
    }

    bool isAlive(int index) const
    {
      _ASSERT(index >= 0 && index < capacity);

      return alive[index];
    }

    int size() const
    {
      return capacity;
    }

    int aliveCount() const
    {
      return count;
    }

    void clear()
    {
      for (int i = 0; i < capacity; i++)
        if (alive[i])
        {
          objects[i].~T();
          alive[i] = false;
        }

      count = 0;
    }

  private:
    //std::vector<T> objects;
    T* objects;
    std::vector<bool> alive;
    int count{};
    int capacity{};
  };

}
