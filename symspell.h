#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <bits/stdc++.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <iterator>
#include <utility>

bool lengthCompare(const std::string&, const std::string&);

bool lengthCompare(const std::string& a, const std::string& b) {
  return (a.length() > b.length());
}

class Symspell {
private:
  int maxDistance;
  std::map<std::string, std::pair<int, std::vector<std::string>>> dictionary;
  // std::map<std::string, std::map<int, std::vector<std::string>>> dictionary;
  int maxLength;

public:
  Symspell(std::vector<std::string>, int);
  ~Symspell();

  void Build(std::vector<std::string>);
  int Levenshtein(std::string, std::string);
  bool inDictionary(std::string);
  std::vector<std::string> GenerateDeletes(std::string, int);
  std::map<std::string, std::pair<int, int>> CorrectWords(std::string);
  std::pair<std::vector<std::string>, std::vector<int>> BestWord(std::string);
};

// Constructor
Symspell::Symspell(std::vector<std::string> wordList, int _maxDistance = 3) {
  this->maxDistance = _maxDistance;
  this->dictionary = {};
  this->maxLength = 0;
  this->Build(wordList);
}

// Destructor
Symspell::~Symspell() {
}

void Symspell::Build(std::vector<std::string> words) {
  // Get the size of the wordlist
  int listSize = words.size();

  // Loop through each word and create the deletes for each word
  for (size_t i = 0; i < listSize; i++) {
    // Grab the word at index
    std::string word = words[i];
    
    // Check if word is in the dictionary
    if (inDictionary(word)) {
      this->dictionary[word].first += 1; 
      this->dictionary[word].second = this->dictionary[word].second; 
      //this->dictionary[word].insert({this->dictionary[word].begin()->first + 1, this->dictionary[word].begin()->second});
    } else {
      this->dictionary[word].first = 1;
      this->dictionary[word].second = {};
      //this->dictionary.insert({word, {{1, {}}}});
      this->maxLength = std::max(this->maxLength, (int)word.length());
    }

    if (this->dictionary[word].first == 1) {
      std::vector<std::string> deletes = GenerateDeletes(word, this->maxDistance);

      for (auto &&d : deletes) {
        if (inDictionary(d)) {
          std::vector<std::string> newDeletes = this->dictionary[d].second;
          newDeletes.push_back(word);
          this->dictionary[d].second = newDeletes;
        } else {
          this->dictionary.insert({d, std::make_pair(0, std::vector<std::string>{word})});
        }
      }
    }
  }

}

// Check if a word is in the dictionary
bool Symspell::inDictionary(std::string word) {
  if (this->dictionary.find(word) == this->dictionary.end()) {
    // Word not in dictionary
    return false;
  } else {
    // Word in dictionary
    return true;
  }
}

// Generate deletes for a word
std::vector<std::string> Symspell::GenerateDeletes(std::string word, int maxDistance) {
  std::vector<std::string> deletes;
  std::vector<std::string> queue;

  // Add word to the queue
  queue.push_back(word);


  for (size_t i = 0; i < maxDistance; i++) {
    std::vector<std::string> tempQueue;

    for (auto &&queueItem : queue) {
    
      if (queueItem.length() > 1) {
        
        for (int c = 0; c < queueItem.length(); c++) {
          // Remove the character at the specified index
          std::string wordWithoutCharacter;
          for (int i = 0; i < queueItem.length(); i++) {
            if (i != c) {
              wordWithoutCharacter.push_back(queueItem[i]);
            }
          }
          
          // Check if word is not in dictionary
          if (std::find(deletes.begin(), deletes.end(), wordWithoutCharacter) == deletes.end()) {
            deletes.push_back(wordWithoutCharacter);
          }
          
          // Check if word is not in temporary queue
          if (std::find(tempQueue.begin(), tempQueue.end(), wordWithoutCharacter) == tempQueue.end()) {
            tempQueue.push_back(wordWithoutCharacter);
          }
        }
      }
    }
    queue = tempQueue;
  }
  return deletes;
}

// Calculate the Levenshtein distance between two strings
// https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
int Symspell::Levenshtein(std::string X, std::string Y) {   
    size_t xLen = X.length();
    size_t yLen = Y.length();

    int d[xLen + 1][yLen + 1];

    int cost;

    for (int i = 0; i <= xLen; i++) {
      d[i][0] = i;
    }
    for(int j = 0; j <= yLen; j++) {
      d[0][j] = j;
    }

    for (int i = 1; i <= xLen; i++) {
      for(int j = 1; j <= yLen; j++) {
        if( X[i] == Y[j] ) {
          cost = 0;
        } else {
          cost = 1;
        }
         
        d[i][j] = std::min(d[i-1][j] + 1,     // Delete
                  std::min(d[i][j-1] + 1,     // Insert
                           d[i-1][j-1] + cost // Substitution        
        ));

        if((i > 1) && (j > 1) && (X[i] == Y[j - 1]) && (X[i] == Y[j - 1])) {
          d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + 1); // Transpose
        }
      }
    }
    return d[xLen][yLen];
}

std::map<std::string, std::pair<int, int>> Symspell::CorrectWords(std::string givenWord) {

  if (((int)givenWord.length() - this->maxLength) > this->maxDistance) {
    return {};
  }

  std::map<std::string, std::pair<int, int>> correctionsDictionary = {};
  int minCorrectLength = INT16_MAX;
  
  std::vector<std::string> queue = { givenWord };
  std::vector<std::string> deletes = GenerateDeletes(givenWord, this->maxDistance);

  // Merge the two vectors together
  queue.insert(queue.end(), deletes.begin(), deletes.end());

  // Sort the vectors by size of word
  std::sort(queue.begin(), queue.end(), lengthCompare);

  while (queue.size() > 0) {
    // Pop
    std::string queueItem = queue.front();
    queue.erase(queue.begin());

    if ((correctionsDictionary.size() > 0) && 
        (givenWord.length() - queueItem.length() > minCorrectLength)) {
      break;
    }

    if ((inDictionary(queueItem)) && 
        (correctionsDictionary.find(queueItem) == correctionsDictionary.end())) {
      

      if (this->dictionary[queueItem].first > 0) {
        correctionsDictionary.insert(std::make_pair(queueItem, std::make_pair(0,0)));
        correctionsDictionary[queueItem].first = this->dictionary[queueItem].first;
        correctionsDictionary[queueItem].second = givenWord.length() - queueItem.length(); // TODO

        if (givenWord.length() == queueItem.length()) {
          break;
        } else if ((givenWord.length() - queueItem.length()) < minCorrectLength) {
          minCorrectLength = givenWord.length() - queueItem.length();
        }
        
      }
      
      for (std::string wordCorrection : this->dictionary[queueItem].second) {  

        // Check if word correction does not exist in the corrections dictionary
        if (correctionsDictionary.find(wordCorrection) == correctionsDictionary.end()) {

          int queueItemDistance;

          if (queueItem.length() == givenWord.length()) {
            queueItemDistance = wordCorrection.length() - queueItem.length();
          }

          wordCorrection.erase(std::remove_if(wordCorrection.begin(), wordCorrection.end(), std::not1(std::ptr_fun( (int(*)(int))std::isalnum))), wordCorrection.end());
          givenWord.erase(std::remove_if(givenWord.begin(), givenWord.end(), std::not1(std::ptr_fun( (int(*)(int))std::isalnum))), givenWord.end());

          queueItemDistance = Levenshtein(wordCorrection, givenWord);

          if (queueItemDistance > minCorrectLength) {
            ; // Pass
          } else if (queueItemDistance <= this->maxDistance) {
            correctionsDictionary.insert(std::make_pair(wordCorrection, std::make_pair(0, 0)));
            correctionsDictionary[wordCorrection].first = this->dictionary[wordCorrection].first;
            correctionsDictionary[wordCorrection].second = queueItemDistance;

            if (queueItemDistance < minCorrectLength) {
              minCorrectLength  = queueItemDistance;
            }
          }
        }
      }
    }
  }
  return correctionsDictionary;
}

std::pair<std::vector<std::string>, std::vector<int>> Symspell::BestWord(std::string givenWord) {

  std::map<std::string, std::pair<int, int>> closeWords = CorrectWords(givenWord);
  std::pair<std::vector<std::string>, std::vector<int>> bestWords;
  for(std::map<std::string, std::pair<int, int>>::iterator it = closeWords.begin(); it != closeWords.end(); ++it) {
    bestWords.first.push_back(it->first);
    bestWords.second.push_back(it->second.second);
  }

  return bestWords;

}


