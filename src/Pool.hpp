#pragma once

namespace game
{
  template <int Capacity>
  class bool_array
  {
  private:
    static int constexpr BUF_SIZE = (Capacity + 31) / 32;
    uint32_t buf[BUF_SIZE] = { 0 };

  public:
    bool_array(bool initValue)
    {
      std::fill_n(buf, BUF_SIZE, initValue * 0xFFFFFFFF);
    }

    struct reference
    {
      uint32_t& ref;
      int index = 0;

      reference& operator=(bool val)
      {
        const uint32_t mask = 1 << index;
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
      return reference { buf[index / 32], index % 32 };
    }

    const reference operator[](int index) const
    {
      return reference { const_cast<uint32_t&>(buf[index / 32]), index % 32 };
    }
  };

  template <class Type, int Capacity>
  class Pool
  {
    static constexpr int BUF_SIZE = sizeof(Type) * Capacity;

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

    void remove(int index)
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(alive[index]);

      object(index).~Type();
      alive[index] = false;
      aliveCount--;
    }

    Type& get(int index)
    {
      _ASSERT(index >= 0 && index < Capacity);
      _ASSERT(alive[index]);

      return object(index);
    }

    const Type& get(int index) const
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
    char buf[sizeof(Type) * Capacity] { 0 };
    bool_array<Capacity> alive { false };
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
