#include <cassert>
#include <chrono>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>

#include "hashmap.h"
#include "iterator.h"

bool is_new_word(std::vector<std::string> &word_vec, const std::string &word) {
  for (const auto &w : word_vec) {
    if (w == word)
      return false;
  }
  word_vec.push_back(word);
  return true;
}

void count_occurence(std::vector<std::pair<std::string, uint>> &word_occ,
                     const std::string &word) {
  for (auto &p : word_occ) {
    if (p.first == word) {
      p.second += 1;
      return;
    }
  }
  word_occ.emplace_back(word, 1);
}

int main() {
  using namespace std;
  using namespace std::chrono;

  ifstream input = ifstream("../WarAndPeace.txt");
  auto start = steady_clock::now();
  cout << "Parsing War and Peace" << endl;

  HashMap<string, int> map(10);

  // vector<string> word_bank;
  // size_t unique_words = 0;
  // vector<pair<string, uint>> word_occurence;

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

    // if (is_new_word(word_bank, word)) unique_words++;
    // count_occurence(word_occurence, word);
    int *nb = map.get(word);
    if (nb) {
      map.put(word, *nb + 1);
    } else {
      map.put(word, 1);
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

  // cout << "Number of unique words found with vector: " << unique_words <<
  // endl;

  // for (const auto& p : word_occurence) {
  //   if (p.first == "war" || p.first == "peace" || p.first == "toto") {
  //     cout << p.first << ": " << p.second << endl;
  //   }
  // }

  // cout << *map.get("war") << endl;
  // cout << *map.get("peace") << endl;
  cout << (map.get("toto") == nullptr) << endl;

  cout << "Number of unique words found with hashmap: " << map.size() << endl;

  vector<pair<string, int>> word_vec(map.size());
  for (auto it = map.begin(); it != map.end(); ++it) {
    word_vec.push_back(*it);
  }

  sort(word_vec.begin(), word_vec.end(),
       [](const auto &a, const auto &b) { return a.second > b.second; });

  cout << "\ntop 10 words:" << endl;
  for (int i = 0; i < 10; i++) {
    auto entry = word_vec[i];
    cout << entry.first << ": " << entry.second << endl;
  }

  return 0;
}
