#include <cassert>
#include <chrono>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <unordered_map>

int main() {
  using namespace std;
  using namespace std::chrono;

  ifstream input = ifstream("../WarAndPeace.txt");
  auto start = steady_clock::now();
  cout << "Parsing War and Peace" << endl;

  unordered_map<string, int> words_map;

  size_t nombre_lu = 0;
  // prochain mot lu
  string word;
  // une regex qui reconnait les caractères anormaux (négation des lettres)
  regex re(R"([^a-zA-Z])");
  while (input >> word) {
    // élimine la ponctuation et les caractères spéciaux
    word = regex_replace(word, re, "");
    // passe en lowercase
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    auto it = words_map.insert({word, 1});
    // did not insert, return element
    if (!it.second) {
      it.first->second += 1;
    }

    // word est maintenant "tout propre"
    if (nombre_lu % 100 == 0)
      // on affiche un mot "propre" sur 100
      cout << nombre_lu << ": " << word << "\n";
    nombre_lu++;
  }
  input.close();

  cout << "Finished Parsing War and Peace" << endl;

  auto end = steady_clock::now();
  cout << "Parsing took " << duration_cast<milliseconds>(end - start).count()
       << "ms.\n";

  cout << "Found a total of " << nombre_lu << " words." << endl;

  cout << "Number of unique words found with hashmap: " << words_map.size()
       << endl;

  vector<pair<string, int>> word_vec(words_map.begin(), words_map.end());

  sort(word_vec.begin(), word_vec.end(),
       [](const auto &a, const auto &b) { return a.second > b.second; });

  cout << "\ntop 10 words:" << endl;
  for (int i = 0; i < 10; i++) {
    auto entry = word_vec[i];
    cout << entry.first << ": " << entry.second << endl;
  }

  return 0;
}
