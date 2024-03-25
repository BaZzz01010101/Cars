#pragma once

template <typename T, int N>
class SemiVector
{
  public:
    int add(T value)
    {
      if(count < N)
        arr[count] = value;
      else
        vec.push_back(value);

      return count++;
    }

    void clear()
    {
      vec.clear();
      count = 0;
    }

    void remove(int index)
    {
      if (index < N)
      {
        for (int i = index + 1; i < N; i++)
          arr[i - 1] = arr[i];

        if (count > N)
          arr[N - 1] = vec[0];
      }

      for (int i = std::max(index - N, 0) + 1; i < vec.size(); i++)
        vec[i - 1] = vec[i];

      vec.resize(vec.size() - 1);
      --count;
    }

    T& operator[](int index)
    {
      return index < N ? arr[index] : vec[index - N];
    }

    const T& operator[](int index) const
    {
      return index < N ? arr[index] : vec[index - N];
    }

    int size() const
    {
      return count;
    }

private:
  int count = 0;
  std::array<T, N> arr;
  std::vector<T> vec;
};

