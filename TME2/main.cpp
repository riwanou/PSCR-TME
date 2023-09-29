#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>

bool is_new_word(const std::vector<std::string>& word_vec, const std::string& word) {
	for (const auto& w : word_vec) {
		if (w == word) return false;
	}
	return true;
}

void count_occurence(std::vector<std::pair<std::string, int>>& word_occ, const std::string& word) {
  for (auto& p : word_occ) {
    if (p.first == word) {
      p.second += 1;
      return;
    }
  }
  word_occ.emplace_back(word, 0);
}

int main () {
	using namespace std;
	using namespace std::chrono;

	ifstream input = ifstream("../WarAndPeace.txt");

	auto start = steady_clock::now();
	cout << "Parsing War and Peace" << endl;

	vector<string> word_bank;
	size_t unique_words = 0;

  vector<pair<string, int>> word_occurence;

	size_t nombre_lu = 0;
	// prochain mot lu
	string word;
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	regex re( R"([^a-zA-Z])");
	while (input >> word) {
		// élimine la ponctuation et les caractères spéciaux
		word = regex_replace ( word, re, "");
		// passe en lowercase
		transform(word.begin(),word.end(),word.begin(),::tolower);

		word_bank.push_back(word);
		if (is_new_word(word_bank, word)) unique_words++;

		// word est maintenant "tout propre"
		if (nombre_lu % 100 == 0)
			// on affiche un mot "propre" sur 100
			cout << nombre_lu << ": "<< word << "\n";
		nombre_lu++;
	}
	input.close();

	cout << "Finished Parsing War and Peace" << endl;

	auto end = steady_clock::now();
    cout << "Parsing took "
              << duration_cast<milliseconds>(end - start).count()
              << "ms.\n";

    cout << "Found a total of " << nombre_lu << " words." << endl;

	cout << "Number of unique words found with vector: " << unique_words << endl;

    return 0;
}


