#pragma once

namespace game
{

  template <int Capacity>
  class BoolArray
  {
  public:
    BoolArray(bool initValue)
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

  private:
    static int constexpr BUF_SIZE = (Capacity + 31) / 32;

    uint32_t buf[BUF_SIZE] = { 0 };
  };

}
