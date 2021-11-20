//#define _GLIBCXX_USE_CXX11_ABI 0

#include "symspell.h"
#include "timeinterval.h"
#define DICTIONARY_FILE "dict.txt"

int main(int argc, char const *argv[]) {

  std::vector<std::string> wordList;
  std::ifstream dictionary(DICTIONARY_FILE);

  std::string word;
  while (std::getline(dictionary, word)) {
    word.erase(std::remove(word.begin(), word.end(), ' '), word.end());
    wordList.push_back(word);
  }
  dictionary.close();



  Symspell s = Symspell(wordList);
  TimeInterval timer = TimeInterval();

  std::cout << "Word: " << argv[1] << std::endl;
  timer.start();
  std::pair<std::vector<std::string>, std::vector<int>> bestWords = s.BestWord(argv[1]);
  timer.stop();


  if (bestWords.first.size() > 0) {
    std::cout << "\nAll Sugestions\n";
    for (int i = 0; i < bestWords.first.size(); i++) {
      std::cout << bestWords.first[i] << "\t\t[distance: " << bestWords.second[i] << "]" << std::endl;
    }

    // Best word
    std::cout << "\nDid you mean " << bestWords.first[0] << "?\n\n";
  } else {
    std::cout << "No words found.";
  }

  std::cout << "Time: " << timer.GetInterval() << " micro seconds" << std::endl;

  return 0;
}
