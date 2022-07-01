#pragma once

#include <string>

namespace support {

  class error_code {
  public:

    error_code();
    error_code(int code, const std::string& text);
    void attach(const support::error_code& ec);
  
    int code() const;
    const std::string& text() const;

    typedef std::vector<error_code>  error_codes;
    const error_codes& chain() const;

  private:

    int          code_;
    std::string  text_;
    error_codes  chain_;

    template <class T>
    friend T& operator<<(T& out, const error_code& ec);
  };

  inline
  error_code::
  error_code() : code_(0), text_("")
  {}

  inline
  error_code::
  error_code(int code,
             const std::string& text) :
    code_(code),
    text_(text)
  {}

  inline void
  error_code::
  attach(const support::error_code& err) {
    chain_.push_back(err);
  }

  inline int
  error_code::
  code() const {
    return code_;
  }

  inline const std::string&
  error_code::
  text() const {
    return text_;
  }

  inline const error_code::error_codes&
  error_code::
  chain() const {
    return chain_;
  }

  template <class T>
  inline T&
  operator<<(T& os,
             const error_code& ec) {
    os << "Error code: "
       << ec.code_
       << ". Error text: "
       << ec.text_
       << std::endl;
    os << "Underlying error codes: " << std::endl;
    for (size_t i = 0; i < ec.chain_.size(); ++i) {
      os << ec.chain_[i] << std::endl;
    }
    return os;
  }

}  /// namespace support
