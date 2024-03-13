#pragma once

namespace game
{
  template <int Capacity>
  class bool_array
  {
  private:
    char buf[Capacity / 8 + 1];

  public:
    bool_array() 
    {
      memset(buf, 0, sizeof(buf));
    }

    struct reference
    {
      char& ref;
      char index;

      reference& operator=(bool val)
      {
        const char mask = 1 << index;
        ref &= ~(1 << index);
        ref |= (val << index);

        return *this;
      }

      operator bool() const
      {
        return (ref >> index) & 1;
      }
    };

    reference operator[](int index)
    {
      return reference{ buf[index / 8], index % 8 };
    }

    const reference operator[](int index) const
    {
      return reference{ const_cast<char&>(buf[index / 8]), index % 8};
    }
  };

  template <class Type, int Capacity>
  class Pool
  {
  public:
    Pool() :
      capacity(Capacity),
      count(0)
    {}

    ~Pool()
    {
      clear();
    }

    template <typename... Args>
    int tryAdd(Args&&... args)
    {
      if (count < Capacity)
        for (int i = 0; i < capacity; i++)
          if (!alive[i])
          {
            new (&object(i)) Type(std::forward<Args>(args)...);
            alive[i] = true;
            count++;

            return i;
          }

      return -1;
    }

    void remove(int index)
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(alive[index]);

      object(index).~Type();
      alive[index] = false;
      count--;
    }

    Type& get(int index)
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(alive[index]);

      return object(index);
    }

    const Type& get(int index) const
    {
      _ASSERT(index >= 0 && index < capacity);
      _ASSERT(alive[index]);

      return const_cast<const Type&>(object(index));
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
          object(i).~Type();
          alive[i] = false;
        }

      count = 0;
    }

  private:
    char buf[sizeof(Type) * Capacity];
    bool_array<Capacity> alive;
    int count{};
    int capacity{};

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
