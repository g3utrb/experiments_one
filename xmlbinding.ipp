namespace xml {
namespace binding {

  inline
  node_base::
  ~node_base()
  {}

  template <class T>
  inline const std::string&
  node<T>::
  name() const {
    return converter_.name();
  }

  template <class T>
  inline const std::string&
  node<T>::
  value() const {
    return converter_.value();
  }

  template <class T>
  inline std::string&
  node<T>::
  value() {
    return converter_.value();
  }

  template <class T>
  inline bool
  node<T>::
  bind(support::error_code& err,
       dom::node::ptr np) {
    return this->converter_.bind(err, np);
  }

  inline
  composite::
  composite() {}

  inline void
  composite::
  insert(const std::string& name,
         node_base* np) {
    mappings_[name] = np;
  }

  inline void
  composite::
  insert(const std::string& name,
         node_base& n) {
    mappings_[name] = &n;
  }

  inline bool
  composite::
  bind(support::error_code& err,
       dom::node::ptr np) {

    /// start at the first child of the supplied node
    xercesc::DOMNode* xnode = np->xerces_node();
    if (! xnode) {
      return false;
    }
    xercesc::DOMNode* link = xnode->getFirstChild();
    if ( !link) {
      return false;
    }
    bool result = true;

    /// iterate through all links as siblings
    for ( ; link != NULL; link = link->getNextSibling() ) {

      /// create the adapter dom node using the factory
      dom::node::ptr dnp = dom::node_factory::create(link);
      if (!dnp) {
        result = false;
        continue;
      }
      /// look in mappings_, does the name exist
      const std::string& name = dnp->name();
      mappings::iterator p = mappings_.find(name);
      if (p != mappings_.end()) {

        /// yes it does, call bind on the binding node ptr
        /// pass in this child node in the chain, the binding node
        /// will know what to do with it
        binding::node_base* bnp = p->second;
        result &= bnp->bind(err, dnp);
      }
    }
    return result;
  }

  template <class T>
  inline bool
  nodelist<T>::
  bind(support::error_code& err,
       dom::node::ptr dnp) {

    /// construct an instance of 'T' - default constructor??
    T item;

    /// now bind it to the supplied node ptr
    bool result = item.bind(err, dnp);

    /// could the binding fail, possibly
    if (result) {

      /// binding was ok, add to the list items
      chain_.push_back(item);
    }
    return result;
  }

  template <class T>
  inline const typename nodelist<T>::chain_t&
  nodelist<T>::
  chain() const {
    return chain_;
  }

  template <class T>
  inline typename nodelist<T>::chain_t&
  nodelist<T>::
  chain() {
    return chain_;
  }

  template <class T>
  inline size_t
  nodelist<T>::
  size() const {
    return chain_.size();
  }

  template <class T>
  inline bool
  nodelist<T>::
  empty() const {
    return chain_.empty();
  }

  template <class T>
  inline const T&
  nodelist<T>::
  operator[](size_t i) const {
    return chain_[i];
  }

  template <class T>
  inline T&
  nodelist<T>::
  operator[](size_t i) {
    return chain_[i];
  }

  typedef element<int_converter>       element_int;
  typedef element<short_converter>     element_short;
  typedef element<long_converter>      element_long;
  typedef element<float_converter>     element_float;
  typedef element<double_converter>    element_double;
  typedef element<string_converter>    element_string;

  typedef attribute<int_converter>     attribute_int;
  typedef attribute<short_converter>   attribute_short;
  typedef attribute<long_converter>    attribute_long;
  typedef attribute<float_converter>   attribute_float;
  typedef attribute<double_converter>  attribute_double;
  typedef attribute<string_converter>  attribute_string;

}}
