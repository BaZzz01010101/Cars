#pragma once
#include <Object.h>

namespace game
{
  template <class T>
  class Pool
  {
  public:
    Pool(int capacity) :
      objects(capacity),
      flags(capacity),
      capacity(capacity),
      count(0)
    {}

    template <typename... Args>
    int tryAdd(Args&&... args)
    {
      _ASSERT(count < capacity);

      if (count < capacity)
        for (int i = 0; i < capacity; i++)
          if (!flags[i])
          {
            new (&objects[i]) T(std::forward<Args>(args)...);
            flags[i] = true;
            count++;

            return i;
          }

      _ASSERT(false);

      return -1;
    }

    void remove(int index)
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(flags[index]);

      objects[index].~T();
      flags[index] = false;
      count--;
    }

    T& get(int index)
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(flags[index]);

      return objects[index];
    }

    const T& get(int index) const
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(flags[index]);

      return objects[index];
    }

    int size()
    {
      return count;
    }

    void clear()
    {
      for (int i = 0; i < capacity; i++)
      {
        objects[i].~T();
        flags[i] = false;
      }

      count = 0;
    }

  private:
    std::vector<T> objects;
    std::vector<bool> flags;
    int count{};
    int capacity{};
  };

}
