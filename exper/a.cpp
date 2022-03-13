#include <iostream>
#include "b.h"

using namespace std;

int main() {
  AA *aa = new A();
  ((A*)aa)->test();
  delete aa;
  
  cout << two(10) << endl;
}
