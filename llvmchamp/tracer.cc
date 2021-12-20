
#include <llvm/ADT/SmallVector.h>

int main(){
  llvm::SmallVector<int,10> sv;
  sv.push_back(1);
  sv.push_back(2);
  sv.push_back(3);
}
