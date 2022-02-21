
#pragma once

#include <random>
#include <vector>

#include "utils_noncopyable.h"

/**
 * see /lib_python/lib_python/__init__.py
 */
class RandomInterface {
 public:
  virtual unsigned int randint(unsigned int a, unsigned int b) = 0;

  virtual unsigned int choice(std::vector<unsigned int> &vect) = 0;

  virtual ~RandomInterface() {}

  /**
   * Shuffle a vector, using Fisher-Yates algo
   * Modified in-place
   */
  // NOTE: NOT virtual
  template <typename Type>
  void shuffle(std::vector<Type> &vect) {
    auto currentIndexCounter = vect.size() - 1;

    for (auto iter = vect.rbegin(); iter != vect.rend();
         ++iter, --currentIndexCounter) {
      const int randomIndex = randint(0, currentIndexCounter);

      if (*iter != vect.at(randomIndex)) {
        std::swap(vect.at(randomIndex), *iter);
      }
    }
  }
};

// TODO
class Random : public RandomInterface, private classutils::noncopyable {
 public:
  Random() {
    std::random_device rd;
    mt19937_ = std::mt19937(rd());
  }

  unsigned int randint(unsigned int a, unsigned int b) override {
    std::uniform_int_distribution<> dis(a, b);
    return dis(mt19937_);
  }

  unsigned int choice(std::vector<unsigned int> &vect) override {
    std::uniform_int_distribution<> dis(0, vect.size() - 1);
    return vect[dis(mt19937_)];
  }

  ~Random(){};

 private:
  std::mt19937 mt19937_;
};

/**
 * TEST/DEV
 * Not random at all!
 * Useful for tests comparing the python outputs vs cpp ones.
 *
 * NOTE: IF we didn't need to compare python and cpp we could use Random with a
 * fixed/given seed
 *
 * see /lib_python/lib_python/__init__.py
 */
class FakeRandom : public RandomInterface, private classutils::noncopyable {
 public:
  unsigned int randint(unsigned int, unsigned int b) override { return b; }

  unsigned int choice(std::vector<unsigned int> &vect) override {
    return vect[0];
  }

  ~FakeRandom(){};
};