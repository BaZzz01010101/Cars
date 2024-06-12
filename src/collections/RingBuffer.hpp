#pragma once

namespace game
{

  template <typename Type, int Size>
  class RingBuffer
  {
  public:
    int head = 0;

    RingBuffer()
    {
      assert(Size > 0);
    }

    RingBuffer(Type& value)
    {
      assert(Size > 0);

      for (int i = 0; i < Size; i++)
        push(value);
    }

    RingBuffer(Type&& value)
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