#include <iostream>

class AA {
public:
  virtual ~AA() = default;
};

class A : public AA {
public:
  void test() {
    std::cout << "aa" << std::endl;
  }
  ~A() {
    std::cout << "a" << std::endl;
  }
};


int two(int);
