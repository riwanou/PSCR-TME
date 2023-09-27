#include <iostream>

int main() {
  std::cout << "Hello World!" << std::endl;

  int tab[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  
  for (int i = 0; i < 9; i++) {
    std::cout << tab[i] << ", ";
  }
  std::cout << tab[9] << std::endl;

  // Q6: i vaut 18446744073709551615
  // contenu des cellules du tableau non modifies
  for (int i = 9; i >= 0; i--) {
    if (tab[i] - tab[i - 1] != 1) {
      std::cout << "probleme !" << std::endl;
    }
  }

  return 0;
}
