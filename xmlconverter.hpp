#include <cstdlib>
// #include "xmldom.hpp"

namespace xml {
namespace binding {

  template <class T, class U>
  struct converter_traits {
    typedef T converter_type;
    typedef U value_type;
  };

  template <class T>
  class converter {
  public:

    typedef typename T::converter_type converter_type;
    typedef typename T::value_type     value_type;

    bool bind(support::error_code& err, dom::node::ptr np);
    const value_type& access() const;
    value_type& access();

    const std::string& name() const;
    const std::string& value() const;
    std::string& value();

    typedef converter<T> this_type;
    typedef void (this_type::*bool_hack)() const;
    void bool_workaround() const
    {}

    operator bool_hack() const {
      bool result = node_.get() != nullptr;
      return result ? &this_type::bool_workaround : 0;
    }

    operator bool() const
    { return node_.get() != nullptr; }
  
  protected:

    converter(converter_type& crtp);
    bool bind_continued(support::error_code& err);

    converter_type&  crtp_;
    value_type       value_;
    dom::node::ptr   node_;
  };

  class string_converter;
  typedef converter_traits<string_converter, std::string> string_converter_traits;

  class string_converter : public converter<string_converter_traits> {
  public:

    string_converter();
    const std::string& access() const;
    std::string& access();

    bool bind_continued(support::error_code& err);
  };

  template <class T>
  class member_converter : public converter<T> {
  public:
    member_converter(typename T::converter_type& crtp);
    const typename T::value_type& access() const;
    typename T::value_type& access();
  };

  template <class T>
  class atoi_converter : public member_converter<T> {
  public:
    atoi_converter(typename T::converter_type& crtp);
    bool bind_continued(support::error_code& err);
  };

  class int_converter;
  typedef converter_traits<int_converter, int> int_converter_traits;

  class int_converter : public atoi_converter<int_converter_traits> {
  public:
    int_converter();
  };

  class short_converter;
  typedef converter_traits<short_converter, short> short_converter_traits;

  class short_converter : public atoi_converter<short_converter_traits> {
  public:
    short_converter();
  };

  class long_converter;
  typedef converter_traits<long_converter, long> long_converter_traits;

  class long_converter : public atoi_converter<long_converter_traits> {
  public:
    long_converter();
  };

  template <class T>
  class atof_converter : public member_converter<T> {
  public:
    atof_converter(typename T::converter_type& crtp);
    bool bind_continued(support::error_code& err);
  };

  class float_converter;
  typedef converter_traits<float_converter, float> float_converter_traits;

  class float_converter : public atof_converter<float_converter_traits> {
  public:
    float_converter();
  };

  class double_converter;
  typedef converter_traits<double_converter, double> double_converter_traits;

  class double_converter : public atof_converter<double_converter_traits> {
  public:
    double_converter();
  };

  template <class T>
  inline
  converter<T>::
  converter(converter_type& crtp) : crtp_(crtp) {}

  template <class T>
  inline bool
  converter<T>::
  bind(support::error_code& err,
       dom::node::ptr np) {
    node_ = np;
    return crtp_.bind_continued(err);
  }

  template <class T>
  inline const typename converter<T>::value_type&
  converter<T>::
  access() const {
    return crtp_.access();
  }

  template <class T>
  inline typename converter<T>::value_type&
  converter<T>::
  access() {
    return crtp_.access();
  }

  template <class T>
  inline const std::string&
  converter<T>::
  name() const {
    return node_->name();
  }

  template <class T>
  inline const std::string&
  converter<T>::
  value() const {
    return node_->value();
  }

  template <class T>
  inline std::string&
  converter<T>::
  value() {
    return node_->value();
  }

  inline 
  string_converter::
  string_converter() :
    converter<string_converter_traits>(*this)
  {}

  inline const std::string&
  string_converter::
  access() const {
    return node_->value();
  }

  inline std::string&
  string_converter::
  access() {
    return node_->value();
  }

  inline bool
  string_converter::
  bind_continued(support::error_code& err) {
    return true;
  }

  template <class T>
  inline
  member_converter<T>::
  member_converter(typename T::converter_type& crtp) :
    converter<T>(crtp)
  {}

  template <class T>
  inline const typename T::value_type&
  member_converter<T>::
  access() const {
    return this->value_;
  }

  template <class T>
  inline typename T::value_type&
  member_converter<T>::
  access() {
    return this->value_;
  }

  template <class T>
  inline
  atoi_converter<T>::
  atoi_converter(typename T::converter_type& crtp) :
    member_converter<T>(crtp)
  {}

  template <class T>
  inline bool
  atoi_converter<T>::
  bind_continued(support::error_code& err) {
    this->value_ = ::atoi(this->node_->value().c_str());
    return true;
  }

  inline
  int_converter::
  int_converter() : atoi_converter<int_converter_traits>(*this)
  {}

  inline
  short_converter::
  short_converter() : atoi_converter<short_converter_traits>(*this)
  {}

  inline
  long_converter::
  long_converter() : atoi_converter<long_converter_traits>(*this)
  {}

  template <class T>
  inline
  atof_converter<T>::
  atof_converter(typename T::converter_type& crtp) :
    member_converter<T>(crtp)
  {}

  template <class T>
  inline bool
  atof_converter<T>::
  bind_continued(support::error_code& err) {
    this->value_ = ::atof(this->node_->value().c_str());
    return true;
  }

  inline
  float_converter::
  float_converter() : atof_converter<float_converter_traits>(*this)
  {}

  inline
  double_converter::
  double_converter() : atof_converter<double_converter_traits>(*this)
  {}

}}
