#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

// serialize primitive vector to output stream
// plaintext
template <class T>
void write_vector(std::ostream &s, const std::vector<T> &data) {
  std::ostream_iterator<T> osi{s, " "};
  std::copy(data.begin(), data.end(), osi);
}

// read primitive vector from input stream
// plaintext
template <class T> void read_vector(std::ifstream &s, std::vector<T> &data) {
  std::istream_iterator<T> iter(s);
  std::copy(iter, std::istream_iterator<T>{}, std::back_inserter(data));
}

template <class T> void print_vec(const std::vector<T> &vec) {
  std::cout << "------------------------------\n";
  for (const auto &e : vec)
    std::cout << std::to_string(e) << "\n";

  const auto cap = vec.size() * sizeof(T);
  std::cout << "sizeof(T): " << std::to_string(sizeof(T)) << "\n";
  std::cout << "sizeof(vec): " << std::to_string(cap) << "\n";
}

void timeit(std::function<void()> const &fn, const size_t num) {
  const auto start = std::chrono::system_clock::now();
  fn();
  const auto end = std::chrono::system_clock::now();
  const auto elapsed =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  const float ops = (float)num / elapsed.count();
  std::cout << elapsed.count() << "us " << ops << " op/s\n";
}

void test() {
  const auto num = 1'000;
  std::vector<uint64_t> vec0(num);
  std::iota(vec0.begin(), vec0.end(), 0);

  std::vector<uint64_t> vec1{};

  const std::string path{"vec.out"};

  std::cout << "n: " << num << '\n';
  std::cout << "bytes: " << num * sizeof(uint64_t) << '\n';

  // write vector
  std::ofstream ofs{path, std::ios::binary | std::ios::out};

  // write partial
  const auto do_write = [&]() -> void { write_vector(ofs, vec0); };

  std::cout << "write: ";
  timeit(do_write, num);
  ofs.close();

  // read vector
  std::ifstream ifs{path, std::ios::binary | std::ios::in};

  // read partial
  const auto do_read = [&]() -> void { read_vector(ifs, vec1); };

  std::cout << "read: ";
  timeit(do_read, num);

  assert(vec0 == vec1);
}

int main() {
  test();
  return 0;
}
