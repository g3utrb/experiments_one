#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <zlib_adapter.hpp>

namespace test {

  class parallel_mangler {
  public:

    parallel_mangler();
    void exec();

  private:

    typedef std::vector<std::thread>  threads_t;

    std::string  contents;
    threads_t    threads;

  };

  parallel_mangler::
  parallel_mangler() {

    /// load binary file
    std::ifstream ifs("./libstuff");
    std::ostringstream oss;
    oss << ifs.rdbuf();
    ifs.close();
    contents = oss.str();

  }

  void
  parallel_mangler::
  exec() {

    using namespace mangle;

    /// launch 10 threads - each compress, uncompress and compare
    for (int i = 0; i < 10; ++i) {
      std::thread t([this]() {

        support::error_code err;
        mangle::bytes output;
        std::string uncomp;

        if (! zlib_adapter::compress(err, output, contents)) {
          std::cout << "Failed to compress: "
                    << err
                    << std::endl;
        }
        else if (! zlib_adapter::uncompress(err, uncomp, output)) {
          std::cout << "Failed to uncompress: "
                    << err
                    << std::endl;
        }
        else if (contents != uncomp) {
          std::cout << "Results don't match. Contents: "
                    << contents
                    << ". Output: "
                    << uncomp
                    << std::endl;
        }
        else {
          std::cout << "really..." << std::endl;
        }
      });
      threads.push_back(std::move(t));
    }
    /// wait for them
    for (auto& t : threads)
      t.join();
  }
}

int main() {
  test::parallel_mangler pm;
  pm.exec();
}
