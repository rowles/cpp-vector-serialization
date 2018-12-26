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

// serialize primative vector to plaintext
namespace plaintext {
// serialize primitive vector to output stream
template <class T>
void write_vector(std::ostream &s, const std::vector<T> &data) {
  std::ostream_iterator<T> osi{s, " "};
  std::copy(data.begin(), data.end(), osi);
}

// read primitive vector from input stream
template <class T> void read_vector(std::ifstream &s, std::vector<T> &data) {
  std::istream_iterator<T> iter(s);
  std::copy(iter, std::istream_iterator<T>{}, std::back_inserter(data));
}
} // namespace plaintext

// serialize primative vector
namespace binary {
template <class T>
void write_vector(std::ostream &s, const std::vector<T> &data) {
  const auto len = data.size();

  s.write(reinterpret_cast<const char *>(&len), sizeof(len));
  s.write(reinterpret_cast<const char *>(&data[0]), len * sizeof(T));
}

template <class T> void read_vector(std::ifstream &s, std::vector<T> &data) {
  size_t len = 0;
  s.read(reinterpret_cast<char *>(&len), sizeof(size_t));
  data.resize(len);
  s.read(reinterpret_cast<char *>(&data[0]), sizeof(T) * len);
}
} // namespace binary

// serialize string vector
namespace binstr {

void write_vector(std::ostream &s, const std::vector<std::string> &data) {
  const auto veclen = data.size();

  s.write(reinterpret_cast<const char *>(&veclen), sizeof(veclen));

  for (const auto &d : data) {
    const auto len = d.size();
    s.write(reinterpret_cast<const char *>(&len), sizeof(len));
    s.write(&d[0], sizeof(char) * len);
  }
}

void read_vector(std::ifstream &s, std::vector<std::string> &data) {
  auto veclen = std::size_t{0};
  s.read(reinterpret_cast<char *>(&veclen), sizeof(std::size_t));

  data.resize(veclen);

  for (auto i = 0; i < veclen; i++) {
    auto len = std::size_t{0};
    s.read(reinterpret_cast<char *>(&len), sizeof(std::size_t));

    std::string st;
    st.resize(len);
    s.read(reinterpret_cast<char *>(&st[0]), sizeof(char) * len);

    data[i] = st;
  }
}

} // namespace binstr

// debug print vector
template <class T> void print_vec(const std::vector<T> &vec) {
  std::cout << "------------------------------\n";
  for (const auto &e : vec)
    std::cout << e << "\n";

  const auto cap = vec.size() * sizeof(T);
  std::cout << "sizeof(T): " << std::to_string(sizeof(T)) << "\n";
  std::cout << "sizeof(vec): " << std::to_string(cap) << "\n";
}

// time operation helper
void timeit(std::function<void()> const &fn, const size_t num) {
  const auto start = std::chrono::system_clock::now();
  fn();
  const auto end = std::chrono::system_clock::now();
  const auto elapsed =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  const float ops = (float) num / elapsed.count();
  std::cout << elapsed.count() << "us " << ops << " op/us\n";
}

// test primative vector serialization
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

  std::cout << "write: ";
  timeit([&]() -> void { binary::write_vector(ofs, vec0); }, num);
  ofs.close();

  // read vector
  std::ifstream ifs{path, std::ios::binary | std::ios::in};

  std::cout << "read: ";
  timeit([&]() -> void { binary::read_vector(ifs, vec1); }, num);

  assert(vec0 == vec1);
}

// test string vector serialization
void test_str() {
  const auto num = 5;
  std::vector<std::string> vec0 = {"abc", "xyz012", "0123456789", "7654321"};
  std::vector<std::string> vec1{};

  const auto path = "vec.out";

  // write vector
  std::ofstream ofs{path, std::ios::binary | std::ios::out};

  std::cout << "write: ";
  timeit([&]() -> void { binstr::write_vector(ofs, vec0); }, num);
  ofs.close();

  // read vector
  std::ifstream ifs{path, std::ios::binary | std::ios::in};

  std::cout << "read: ";
  timeit([&]() -> void { binstr::read_vector(ifs, vec1); }, num);

  assert(vec0 == vec1);
  // print_vec(vec0);
  // print_vec(vec1);
}

int main()
{
  std::cout << "------ vec[uint64_t]\n";
  test();
  std::cout << "------ vec[std::string]\n";
  test_str();
  return 0;
}
