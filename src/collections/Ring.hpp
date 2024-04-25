#pragma once

namespace game
{

  template <typename Type, int Size>
  class Ring
  {
  public:
    int head = 0;

    Ring()
    {
      assert(Size > 0);
    }

    Ring(Type& value)
    {
      assert(Size > 0);

      for (int i = 0; i < Size; i++)
        push(value);
    }

    Ring(Type&& value)
    {
      assert(Size > 0);

      for (int i = 0; i < Size; i++)
        push(value);
    }

    void push(const Type& value)
    {
      buf[head] = value;

      head = (head + 1) % Size;
    }

    Type& operator[](int index)
    {
      assert(index >= 0);
      assert(index < Size);

      return buf[(head + Size - index - 1) % Size];
    }

  private:
    std::array<Type, Size> buf {};
  };

}