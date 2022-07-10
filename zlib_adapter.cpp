#include <fstream>
#include <sstream>
#include <zlib_adapter.hpp>

int main() {
  std::ifstream ifs("./libstuff");
  std::ostringstream oss;
  oss << ifs.rdbuf();
  std::string s = oss.str();
  // std::cout << s << std::endl;
  support::error_code err;
  mangle::bytes out;
  if (! mangle::zlib_adapter::compress(err, out, s)) {
    std::cout << "compress failed: " << err << std::endl;
  }
  else {
    //std::cout << "compress ok" << std::endl;
  }
  std::string res;
  if (! mangle::zlib_adapter::uncompress(err, res, out)) {
    std::cout << "uncompress failed: " << err << std::endl;
  }
  else {
    //std::cout << "uncompress ok" << std::endl;
    std::cout << res;
  }
}
