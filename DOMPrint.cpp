#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include "DOMTreeErrorReporter.hpp"
#include "DOMPrintFilter.hpp"
#include "DOMPrintErrorHandler.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>
#include <string.h>
#include <stdlib.h>
#include <memory>
#include <time.h>
#include "xmlbinding.hpp"

struct vmaster_header : xml::binding::composite {

  vmaster_header() {
    insert("vMasterInstrument",     instrument);
    insert("vMasterTradeStatus",    trade_status);
    insert("vMasterTradeDate",      trade_date);
    insert("vMasterStartDate",      start_date);
    insert("RTLCReferenceCode",     rtlc_reference_code);
    insert("vMasterEndDate",        end_date);
    insert("vMasterTradeOrigin",    trade_origin);
    insert("vMasterTradeOriginID",  trade_origin_id);
    insert("vMasterTrader",         trader);
    insert("vMasterCoverage",       coverage);
    insert("vMasterLocation",       location);
    insert("vMasterBook",           book);
    insert("vMasterUserLogin",      user_login);
    insert("vMasterBookLocation",   book_location);
    insert("vMasterBookDomicile",   book_domicile);
    insert("vMasterEntity",         entity);
    insert("vMasterEntityCoperID",  entity_coper_id);
    insert("vMasterMLDPGuarantee",  mldp_guarantee);
    insert("vMasterSwapclearFlag",  swap_clear_flag);
    insert("vMasterCreditCode",     credit_code);
    insert("vMasterDesk",           desk);
  }

  xml::binding::element_string  instrument;
  xml::binding::element_string  trade_status;
  xml::binding::element_string  trade_date;
  xml::binding::element_string  start_date;
  xml::binding::element_string  rtlc_reference_code;
  xml::binding::element_string  end_date;
  xml::binding::element_string  trade_origin;
  xml::binding::element_string  trade_origin_id;
  xml::binding::element_string  trader;
  xml::binding::element_string  coverage;
  xml::binding::element_string  location;
  xml::binding::element_string  book;
  xml::binding::element_string  user_login;
  xml::binding::element_string  book_location;
  xml::binding::element_string  book_domicile;
  xml::binding::element_string  entity;
  xml::binding::element_int     entity_coper_id;
  xml::binding::element_string  mldp_guarantee;
  xml::binding::element_string  swap_clear_flag;
  xml::binding::element_string  credit_code;
  xml::binding::element_string  desk;
};

struct vmaster_message : xml::binding::composite {

  vmaster_message() {
    insert("vMasterHeader", vm_header);
  }

  vmaster_header vm_header;
};

class trade;
typedef std::shared_ptr<trade> trade_ptr;
class trade_composer {
public:
  trade_ptr compose(support::error_code& err, vmaster_message& vm);
};

int main(int argC, char* argV[]) {

  try {
    XMLPlatformUtils::Initialize();
  }
  catch(const XMLException &toCatch) {
    XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
      << "  Exception message:"
      << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
    return 1;
  }

  std::string s;
  s  = "<vMasterMessage>\n";
  s += "  <vMasterHeader>\n";
  s += "    <vMasterInstrument>SWAP</vMasterInstrument>\n";
  s += "    <vMasterTradeStatus>PENDING_UNAPPROVED</vMasterTradeStatus>\n";
  s += "    <vMasterTradeDate>2009-12-04</vMasterTradeDate>\n";
  s += "    <vMasterStartDate>2009-12-04</vMasterStartDate>\n";
  s += "    <RTLCReferenceCode>something</RTLCReferenceCode>\n";
  s += "    <vMasterEndDate>2012-12-08</vMasterEndDate>\n";
  s += "    <vMasterTradeOrigin>test</vMasterTradeOrigin>\n";
  s += "    <vMasterTradeOriginID>test</vMasterTradeOriginID>\n";
  s += "    <vMasterTrader>Trader</vMasterTrader>\n";
  s += "    <vMasterCoverage>NONE, NONE</vMasterCoverage>\n";
  s += "    <vMasterLocation>LONDON</vMasterLocation>\n";
  s += "    <vMasterBook>LEMU</vMasterBook>\n";
  s += "    <vMasterUserLogin>nbdgh1e</vMasterUserLogin>\n";
  s += "    <vMasterBookLocation>LON</vMasterBookLocation>\n";
  s += "    <vMasterBookDomicile>LON</vMasterBookDomicile>\n";
  s += "    <vMasterEntity>MLC1</vMasterEntity>\n";
  s += "    <vMasterEntityCoperID>95280</vMasterEntityCoperID>\n";
  s += "    <vMasterMLDPGuarantee>No Guarantee</vMasterMLDPGuarantee>\n";
  s += "    <vMasterSwapClearFlag>Not Cleared</vMasterSwapClearFlag>\n";
  s += "    <vMasterCreditCode>NOT REQUIRED</vMasterCreditCode>\n";
  s += "    <vMasterDesk>SWAPSLON</vMasterDesk>\n";
  s += "  </vMasterHeader>\n";
  s += "</vMasterMessage>\n";
  std::cout << s << std::endl;

  xml::dom::parser par;
  support::error_code err;
  bool result = par.parse(err, s);
  xml::dom::node::ptr np = par.root();

  struct timespec start, stop;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

  vmaster_message vm;
  result = vm.bind(err, np);

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
  double elapsed = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e3;
  std::cout << "time in microseconds: " << elapsed << std::endl;

  const vmaster_header& vmh = vm.vm_header;

  std::cout << vmh.instrument() << std::endl;
  std::cout << vmh.entity_coper_id() << std::endl;
  std::cout << vmh.trade_origin_id() << std::endl;

  bool has_trader = vmh.trader;
  std::cout << "Has trader " << std::boolalpha << has_trader << std::endl;
  std::cout << "desk " << vmh.desk() << std::endl;

  XMLPlatformUtils::Terminate();
}
