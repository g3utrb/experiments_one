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

    if (! np) {
      std::string s = "Warning: composite::bind supplied null dom::node ptr.";
      err.attach(support::error_code(-1, s));
      return false;
    }
    /// we need the underlying xerces node to iterate
    xercesc::DOMNode* xnode = np->xerces_node();
    if (! xnode) {
      std::string s = "Warning: composite::bind supplied null xerces::node ptr.";
      err.attach(support::error_code(-1, s));
      return false;
    }
    /// start at the first child of the supplied node
    xercesc::DOMNode* link = xnode->getFirstChild();
    if ( !link) {
      std::string s = "Warning: composite::bind encountered null first child.";
      err.attach(support::error_code(-1, s));
      return false;
    }
    /// iterate through all links as siblings
    bool result = true;
    link = xnode->getFirstChild();
    for ( ; link != 0; link = link->getNextSibling() ) {

      /// safe than sorry
      if (! link) {
        std::string s = "Warning: composite::bind a sibling link node is null.";
        err.attach(support::error_code(-1, s));
        result = false;
        continue;
      }
      char* s = xercesc::XMLString::transcode(link->getNodeName());
      std::string name = s;
      xercesc::XMLString::release(&s);
      // std::cout << "Processing link: " << name << std::endl;

      /// for instance "vMasterInstrument" -> an element node..
      mappings::iterator p = mappings_.find(name);
      if (p == mappings_.end()) {

        /// we're not interested, continue
        continue;
      }
      /// create the adapter dom node using the factory
      dom::node::ptr dnp = dom::node_factory::create(err, link);
      if (!dnp) {

        /// some unhandled node type...must be careful
        std::string s = "Warning: could not create adapter node from link.";
        err.attach(support::error_code(-1, s));
        result = false;
        continue;
      }
      /// ready to bind, pass in the current dom node ptr
      /// the binding node will adopt and it extract/convert the value
      binding::node_base* bnp = p->second;
      result &= bnp->bind(err, dnp);
    }
    /// this node may have child attributes
    result &= process_attributes(err, xnode);
    return result;
  }

  inline bool
  composite::
  process_attributes(support::error_code& err,
                     xercesc::DOMNode* xnode) {

    /// per xerces - only element nodes have attributes
    if (xnode->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) {
      return true;
    }
    /// get attributes of this node, could they be null..
    bool result = true;
    xercesc::DOMNamedNodeMap* attrs = xnode->getAttributes();
    if ( !attrs) {
      return true;
    }
    /// search for each attr in this nodes mappings
    for (XMLSize_t i = 0; i < attrs->getLength(); ++i) {

      xercesc::DOMNode* dap = attrs->item(i);
      if (! dap) {
        /// hmmm
        continue;
      }
      /// get the attribute name out of the item
      char* s = xercesc::XMLString::transcode(dap->getNodeName());
      std::string attr_name = s;
      // std::cout << "attr name: " << attr_name << std::endl;
      xercesc::XMLString::release(&s);

      /// skip if we're not interested in this attribute under this node
      mappings::iterator p = mappings_.find(attr_name);
      if (p == mappings_.end()) {
        continue;
      }
      binding::node_base* bnp = p->second;

      /// ask the factory to create the node
      dom::node::ptr anp = dom::node_factory::create(err, dap);
      if (! anp) {
        /// hmm
        continue;
      }
      /// and bind to it
      result &= bnp->bind(err, anp);
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
