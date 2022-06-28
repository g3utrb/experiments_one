#include <cstdlib>
#include "xmldom.hpp"

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

}}

#include "xmlconverter.ipp"
