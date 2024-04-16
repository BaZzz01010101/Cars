#pragma once

namespace game
{

  template <typename Type, int Size>
  class Queue
  {
  public:
    int head = 0;
    int tail = 0;

    bool isFull() const
    {
      return (head + 1) % Size == tail;
    }

    bool isEmpty() const
    {
      return head == tail;
    }

    bool tryPush(const Type& value)
    {
      int nextHead = (head + 1) % Size;

      if (nextHead == tail)
        return false;

      buf[tail] = value;
      tail = nextHead;

      return true;
    }

    bool tryPop(Type* outValue = nullptr)
    {
      if (tail == head)
        return false;

      if (outValue != nullptr)
        *outValue = buf[tail];

      tail = (tail + 1) % Size;

      return true;
    }

  private:
    std::array<Type, Size> buf {};
  };

}