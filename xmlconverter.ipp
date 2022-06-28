
namespace xml {
namespace binding {

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
