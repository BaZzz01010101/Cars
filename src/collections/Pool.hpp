#pragma once
#include "BoolArray.hpp"

namespace game
{

  template <class Type, int Capacity>
  class Pool
  {
  public:
    ~Pool()
    {
      clear();
    }

    template <typename... Args>
    int tryAdd(Args&&... args)
    {
      if (aliveCount < Capacity)
        for (int i = 0; i < Capacity; i++)
          if (!alive[i])
          {
            new (&object(i)) Type(std::forward<Args>(args)...);
            alive[i] = true;
            aliveCount++;

            return i;
          }

      return -1;
    }

    template <typename... Args>
    int tryAdd(int index, Args&&... args)
    {
      if (!alive[index])
      {
        new (&object(index)) Type(std::forward<Args>(args)...);
        alive[index] = true;
        aliveCount++;

        return index;
      }

      return -1;
    }

    void remove(int index)
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(alive[index]);

      object(index).~Type();
      alive[index] = false;
      aliveCount--;
    }

    Type& operator[](int index)
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(alive[index]);

      return object(index);
    }

    const Type& operator[](int index) const
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(alive[index]);

      return const_cast<const Type&>(object(index));
    }

    bool isAlive(int index) const
    {
      _ASSERT(index >= 0 && index < Capacity);

      return alive[index];
    }

    int capacity() const
    {
      return Capacity;
    }

    int count() const
    {
      return aliveCount;
    }

    void clear()
    {
      for (int i = 0; i < Capacity; i++)
        if (alive[i])
        {
          object(i).~Type();
          alive[i] = false;
        }

      aliveCount = 0;
    }

  private:
    static constexpr int BUF_SIZE = sizeof(Type) * Capacity;

    char buf[BUF_SIZE] { 0 };

#ifdef _DEBUG
    // This is for debugging puposes only and allows to view types objects in the debugger.
    Type(&objects)[BUF_SIZE / sizeof(Type)] = reinterpret_cast<Type(&)[BUF_SIZE / sizeof(Type)]>(buf);
#endif

    BoolArray<Capacity> alive { false };
    int aliveCount = 0;

    const Type& object(int index) const
    {
      return reinterpret_cast<const Type*>(buf)[index];
    }

    Type& object(int index)
    {
      return reinterpret_cast<Type*>(buf)[index];
    }
  };

}
