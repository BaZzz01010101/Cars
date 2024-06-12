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
      if (existsCount < Capacity)
        for (int i = 0; i < Capacity; i++)
          if (!existence[i])
          {
            new (&object(i)) Type(std::forward<Args>(args)...);
            existence[i] = true;
            existsCount++;

            return i;
          }

      return -1;
    }

    template <typename... Args>
    int tryAdd(int index, Args&&... args)
    {
      if (index < Capacity && !existence[index])
      {
        new (&object(index)) Type(std::forward<Args>(args)...);
        existence[index] = true;
        existsCount++;

        return index;
      }

      return -1;
    }

    void remove(int index)
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(existence[index]);

      object(index).~Type();
      existence[index] = false;
      existsCount--;
    }

    Type& operator[](int index)
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(existence[index]);

      return object(index);
    }

    const Type& operator[](int index) const
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(existence[index]);

      return const_cast<const Type&>(object(index));
    }

    bool exists(int index) const
    {
      _ASSERT(index >= 0 && index < Capacity);

      return existence[index];
    }

    int capacity() const
    {
      return Capacity;
    }

    int count() const
    {
      return existsCount;
    }

    void clear()
    {
      for (int i = 0; i < Capacity; i++)
        if (existence[i])
        {
          object(i).~Type();
          existence[i] = false;
        }

      existsCount = 0;
    }

  private:
    static constexpr int BUF_SIZE = sizeof(Type) * Capacity;

    char buf[BUF_SIZE] { 0 };

#ifdef _DEBUG
    // This is for debugging puposes only and allows to view types objects in the debugger.
    Type(&objects)[BUF_SIZE / sizeof(Type)] = reinterpret_cast<Type(&)[BUF_SIZE / sizeof(Type)]>(buf);
#endif

    BoolArray<Capacity> existence { false };
    int existsCount = 0;

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
