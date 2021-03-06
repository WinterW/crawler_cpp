/**
 * Autogenerated by Thrift Compiler (0.9.1)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef DedupService_H
#define DedupService_H

#include <thrift/TDispatchProcessor.h>
#include "conf_crawler_types.h"



class DedupServiceIf {
 public:
  virtual ~DedupServiceIf() {}
  virtual void is_exists(DedupExistItem& _return, const std::string& url) = 0;
  virtual bool insert(const std::string& url) = 0;
  virtual bool test_exists_and_insert(const std::string& url) = 0;
  virtual bool remove(const std::string& url) = 0;
  virtual void info(std::string& _return) = 0;
  virtual int32_t set_bucket_count(const int32_t bucket_count) = 0;
  virtual int32_t batch_remove(const std::string& url_pattern) = 0;
};

class DedupServiceIfFactory {
 public:
  typedef DedupServiceIf Handler;

  virtual ~DedupServiceIfFactory() {}

  virtual DedupServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(DedupServiceIf* /* handler */) = 0;
};

class DedupServiceIfSingletonFactory : virtual public DedupServiceIfFactory {
 public:
  DedupServiceIfSingletonFactory(const boost::shared_ptr<DedupServiceIf>& iface) : iface_(iface) {}
  virtual ~DedupServiceIfSingletonFactory() {}

  virtual DedupServiceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(DedupServiceIf* /* handler */) {}

 protected:
  boost::shared_ptr<DedupServiceIf> iface_;
};

class DedupServiceNull : virtual public DedupServiceIf {
 public:
  virtual ~DedupServiceNull() {}
  void is_exists(DedupExistItem& /* _return */, const std::string& /* url */) {
    return;
  }
  bool insert(const std::string& /* url */) {
    bool _return = false;
    return _return;
  }
  bool test_exists_and_insert(const std::string& /* url */) {
    bool _return = false;
    return _return;
  }
  bool remove(const std::string& /* url */) {
    bool _return = false;
    return _return;
  }
  void info(std::string& /* _return */) {
    return;
  }
  int32_t set_bucket_count(const int32_t /* bucket_count */) {
    int32_t _return = 0;
    return _return;
  }
  int32_t batch_remove(const std::string& /* url_pattern */) {
    int32_t _return = 0;
    return _return;
  }
};

typedef struct _DedupService_is_exists_args__isset {
  _DedupService_is_exists_args__isset() : url(false) {}
  bool url;
} _DedupService_is_exists_args__isset;

class DedupService_is_exists_args {
 public:

  DedupService_is_exists_args() : url() {
  }

  virtual ~DedupService_is_exists_args() throw() {}

  std::string url;

  _DedupService_is_exists_args__isset __isset;

  void __set_url(const std::string& val) {
    url = val;
  }

  bool operator == (const DedupService_is_exists_args & rhs) const
  {
    if (!(url == rhs.url))
      return false;
    return true;
  }
  bool operator != (const DedupService_is_exists_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_is_exists_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_is_exists_pargs {
 public:


  virtual ~DedupService_is_exists_pargs() throw() {}

  const std::string* url;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_is_exists_result__isset {
  _DedupService_is_exists_result__isset() : success(false) {}
  bool success;
} _DedupService_is_exists_result__isset;

class DedupService_is_exists_result {
 public:

  DedupService_is_exists_result() {
  }

  virtual ~DedupService_is_exists_result() throw() {}

  DedupExistItem success;

  _DedupService_is_exists_result__isset __isset;

  void __set_success(const DedupExistItem& val) {
    success = val;
  }

  bool operator == (const DedupService_is_exists_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_is_exists_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_is_exists_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_is_exists_presult__isset {
  _DedupService_is_exists_presult__isset() : success(false) {}
  bool success;
} _DedupService_is_exists_presult__isset;

class DedupService_is_exists_presult {
 public:


  virtual ~DedupService_is_exists_presult() throw() {}

  DedupExistItem* success;

  _DedupService_is_exists_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _DedupService_insert_args__isset {
  _DedupService_insert_args__isset() : url(false) {}
  bool url;
} _DedupService_insert_args__isset;

class DedupService_insert_args {
 public:

  DedupService_insert_args() : url() {
  }

  virtual ~DedupService_insert_args() throw() {}

  std::string url;

  _DedupService_insert_args__isset __isset;

  void __set_url(const std::string& val) {
    url = val;
  }

  bool operator == (const DedupService_insert_args & rhs) const
  {
    if (!(url == rhs.url))
      return false;
    return true;
  }
  bool operator != (const DedupService_insert_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_insert_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_insert_pargs {
 public:


  virtual ~DedupService_insert_pargs() throw() {}

  const std::string* url;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_insert_result__isset {
  _DedupService_insert_result__isset() : success(false) {}
  bool success;
} _DedupService_insert_result__isset;

class DedupService_insert_result {
 public:

  DedupService_insert_result() : success(0) {
  }

  virtual ~DedupService_insert_result() throw() {}

  bool success;

  _DedupService_insert_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const DedupService_insert_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_insert_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_insert_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_insert_presult__isset {
  _DedupService_insert_presult__isset() : success(false) {}
  bool success;
} _DedupService_insert_presult__isset;

class DedupService_insert_presult {
 public:


  virtual ~DedupService_insert_presult() throw() {}

  bool* success;

  _DedupService_insert_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _DedupService_test_exists_and_insert_args__isset {
  _DedupService_test_exists_and_insert_args__isset() : url(false) {}
  bool url;
} _DedupService_test_exists_and_insert_args__isset;

class DedupService_test_exists_and_insert_args {
 public:

  DedupService_test_exists_and_insert_args() : url() {
  }

  virtual ~DedupService_test_exists_and_insert_args() throw() {}

  std::string url;

  _DedupService_test_exists_and_insert_args__isset __isset;

  void __set_url(const std::string& val) {
    url = val;
  }

  bool operator == (const DedupService_test_exists_and_insert_args & rhs) const
  {
    if (!(url == rhs.url))
      return false;
    return true;
  }
  bool operator != (const DedupService_test_exists_and_insert_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_test_exists_and_insert_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_test_exists_and_insert_pargs {
 public:


  virtual ~DedupService_test_exists_and_insert_pargs() throw() {}

  const std::string* url;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_test_exists_and_insert_result__isset {
  _DedupService_test_exists_and_insert_result__isset() : success(false) {}
  bool success;
} _DedupService_test_exists_and_insert_result__isset;

class DedupService_test_exists_and_insert_result {
 public:

  DedupService_test_exists_and_insert_result() : success(0) {
  }

  virtual ~DedupService_test_exists_and_insert_result() throw() {}

  bool success;

  _DedupService_test_exists_and_insert_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const DedupService_test_exists_and_insert_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_test_exists_and_insert_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_test_exists_and_insert_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_test_exists_and_insert_presult__isset {
  _DedupService_test_exists_and_insert_presult__isset() : success(false) {}
  bool success;
} _DedupService_test_exists_and_insert_presult__isset;

class DedupService_test_exists_and_insert_presult {
 public:


  virtual ~DedupService_test_exists_and_insert_presult() throw() {}

  bool* success;

  _DedupService_test_exists_and_insert_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _DedupService_remove_args__isset {
  _DedupService_remove_args__isset() : url(false) {}
  bool url;
} _DedupService_remove_args__isset;

class DedupService_remove_args {
 public:

  DedupService_remove_args() : url() {
  }

  virtual ~DedupService_remove_args() throw() {}

  std::string url;

  _DedupService_remove_args__isset __isset;

  void __set_url(const std::string& val) {
    url = val;
  }

  bool operator == (const DedupService_remove_args & rhs) const
  {
    if (!(url == rhs.url))
      return false;
    return true;
  }
  bool operator != (const DedupService_remove_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_remove_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_remove_pargs {
 public:


  virtual ~DedupService_remove_pargs() throw() {}

  const std::string* url;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_remove_result__isset {
  _DedupService_remove_result__isset() : success(false) {}
  bool success;
} _DedupService_remove_result__isset;

class DedupService_remove_result {
 public:

  DedupService_remove_result() : success(0) {
  }

  virtual ~DedupService_remove_result() throw() {}

  bool success;

  _DedupService_remove_result__isset __isset;

  void __set_success(const bool val) {
    success = val;
  }

  bool operator == (const DedupService_remove_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_remove_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_remove_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_remove_presult__isset {
  _DedupService_remove_presult__isset() : success(false) {}
  bool success;
} _DedupService_remove_presult__isset;

class DedupService_remove_presult {
 public:


  virtual ~DedupService_remove_presult() throw() {}

  bool* success;

  _DedupService_remove_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};


class DedupService_info_args {
 public:

  DedupService_info_args() {
  }

  virtual ~DedupService_info_args() throw() {}


  bool operator == (const DedupService_info_args & /* rhs */) const
  {
    return true;
  }
  bool operator != (const DedupService_info_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_info_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_info_pargs {
 public:


  virtual ~DedupService_info_pargs() throw() {}


  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_info_result__isset {
  _DedupService_info_result__isset() : success(false) {}
  bool success;
} _DedupService_info_result__isset;

class DedupService_info_result {
 public:

  DedupService_info_result() : success() {
  }

  virtual ~DedupService_info_result() throw() {}

  std::string success;

  _DedupService_info_result__isset __isset;

  void __set_success(const std::string& val) {
    success = val;
  }

  bool operator == (const DedupService_info_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_info_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_info_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_info_presult__isset {
  _DedupService_info_presult__isset() : success(false) {}
  bool success;
} _DedupService_info_presult__isset;

class DedupService_info_presult {
 public:


  virtual ~DedupService_info_presult() throw() {}

  std::string* success;

  _DedupService_info_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _DedupService_set_bucket_count_args__isset {
  _DedupService_set_bucket_count_args__isset() : bucket_count(false) {}
  bool bucket_count;
} _DedupService_set_bucket_count_args__isset;

class DedupService_set_bucket_count_args {
 public:

  DedupService_set_bucket_count_args() : bucket_count(0) {
  }

  virtual ~DedupService_set_bucket_count_args() throw() {}

  int32_t bucket_count;

  _DedupService_set_bucket_count_args__isset __isset;

  void __set_bucket_count(const int32_t val) {
    bucket_count = val;
  }

  bool operator == (const DedupService_set_bucket_count_args & rhs) const
  {
    if (!(bucket_count == rhs.bucket_count))
      return false;
    return true;
  }
  bool operator != (const DedupService_set_bucket_count_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_set_bucket_count_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_set_bucket_count_pargs {
 public:


  virtual ~DedupService_set_bucket_count_pargs() throw() {}

  const int32_t* bucket_count;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_set_bucket_count_result__isset {
  _DedupService_set_bucket_count_result__isset() : success(false) {}
  bool success;
} _DedupService_set_bucket_count_result__isset;

class DedupService_set_bucket_count_result {
 public:

  DedupService_set_bucket_count_result() : success(0) {
  }

  virtual ~DedupService_set_bucket_count_result() throw() {}

  int32_t success;

  _DedupService_set_bucket_count_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const DedupService_set_bucket_count_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_set_bucket_count_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_set_bucket_count_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_set_bucket_count_presult__isset {
  _DedupService_set_bucket_count_presult__isset() : success(false) {}
  bool success;
} _DedupService_set_bucket_count_presult__isset;

class DedupService_set_bucket_count_presult {
 public:


  virtual ~DedupService_set_bucket_count_presult() throw() {}

  int32_t* success;

  _DedupService_set_bucket_count_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

typedef struct _DedupService_batch_remove_args__isset {
  _DedupService_batch_remove_args__isset() : url_pattern(false) {}
  bool url_pattern;
} _DedupService_batch_remove_args__isset;

class DedupService_batch_remove_args {
 public:

  DedupService_batch_remove_args() : url_pattern() {
  }

  virtual ~DedupService_batch_remove_args() throw() {}

  std::string url_pattern;

  _DedupService_batch_remove_args__isset __isset;

  void __set_url_pattern(const std::string& val) {
    url_pattern = val;
  }

  bool operator == (const DedupService_batch_remove_args & rhs) const
  {
    if (!(url_pattern == rhs.url_pattern))
      return false;
    return true;
  }
  bool operator != (const DedupService_batch_remove_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_batch_remove_args & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};


class DedupService_batch_remove_pargs {
 public:


  virtual ~DedupService_batch_remove_pargs() throw() {}

  const std::string* url_pattern;

  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_batch_remove_result__isset {
  _DedupService_batch_remove_result__isset() : success(false) {}
  bool success;
} _DedupService_batch_remove_result__isset;

class DedupService_batch_remove_result {
 public:

  DedupService_batch_remove_result() : success(0) {
  }

  virtual ~DedupService_batch_remove_result() throw() {}

  int32_t success;

  _DedupService_batch_remove_result__isset __isset;

  void __set_success(const int32_t val) {
    success = val;
  }

  bool operator == (const DedupService_batch_remove_result & rhs) const
  {
    if (!(success == rhs.success))
      return false;
    return true;
  }
  bool operator != (const DedupService_batch_remove_result &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const DedupService_batch_remove_result & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

};

typedef struct _DedupService_batch_remove_presult__isset {
  _DedupService_batch_remove_presult__isset() : success(false) {}
  bool success;
} _DedupService_batch_remove_presult__isset;

class DedupService_batch_remove_presult {
 public:


  virtual ~DedupService_batch_remove_presult() throw() {}

  int32_t* success;

  _DedupService_batch_remove_presult__isset __isset;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);

};

class DedupServiceClient : virtual public DedupServiceIf {
 public:
  DedupServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> prot) :
    piprot_(prot),
    poprot_(prot) {
    iprot_ = prot.get();
    oprot_ = prot.get();
  }
  DedupServiceClient(boost::shared_ptr< ::apache::thrift::protocol::TProtocol> iprot, boost::shared_ptr< ::apache::thrift::protocol::TProtocol> oprot) :
    piprot_(iprot),
    poprot_(oprot) {
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return piprot_;
  }
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return poprot_;
  }
  void is_exists(DedupExistItem& _return, const std::string& url);
  void send_is_exists(const std::string& url);
  void recv_is_exists(DedupExistItem& _return);
  bool insert(const std::string& url);
  void send_insert(const std::string& url);
  bool recv_insert();
  bool test_exists_and_insert(const std::string& url);
  void send_test_exists_and_insert(const std::string& url);
  bool recv_test_exists_and_insert();
  bool remove(const std::string& url);
  void send_remove(const std::string& url);
  bool recv_remove();
  void info(std::string& _return);
  void send_info();
  void recv_info(std::string& _return);
  int32_t set_bucket_count(const int32_t bucket_count);
  void send_set_bucket_count(const int32_t bucket_count);
  int32_t recv_set_bucket_count();
  int32_t batch_remove(const std::string& url_pattern);
  void send_batch_remove(const std::string& url_pattern);
  int32_t recv_batch_remove();
 protected:
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> piprot_;
  boost::shared_ptr< ::apache::thrift::protocol::TProtocol> poprot_;
  ::apache::thrift::protocol::TProtocol* iprot_;
  ::apache::thrift::protocol::TProtocol* oprot_;
};

class DedupServiceProcessor : public ::apache::thrift::TDispatchProcessor {
 protected:
  boost::shared_ptr<DedupServiceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (DedupServiceProcessor::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef std::map<std::string, ProcessFunction> ProcessMap;
  ProcessMap processMap_;
  void process_is_exists(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_insert(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_test_exists_and_insert(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_remove(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_info(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_set_bucket_count(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_batch_remove(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
 public:
  DedupServiceProcessor(boost::shared_ptr<DedupServiceIf> iface) :
    iface_(iface) {
    processMap_["is_exists"] = &DedupServiceProcessor::process_is_exists;
    processMap_["insert"] = &DedupServiceProcessor::process_insert;
    processMap_["test_exists_and_insert"] = &DedupServiceProcessor::process_test_exists_and_insert;
    processMap_["remove"] = &DedupServiceProcessor::process_remove;
    processMap_["info"] = &DedupServiceProcessor::process_info;
    processMap_["set_bucket_count"] = &DedupServiceProcessor::process_set_bucket_count;
    processMap_["batch_remove"] = &DedupServiceProcessor::process_batch_remove;
  }

  virtual ~DedupServiceProcessor() {}
};

class DedupServiceProcessorFactory : public ::apache::thrift::TProcessorFactory {
 public:
  DedupServiceProcessorFactory(const ::boost::shared_ptr< DedupServiceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::boost::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::boost::shared_ptr< DedupServiceIfFactory > handlerFactory_;
};

class DedupServiceMultiface : virtual public DedupServiceIf {
 public:
  DedupServiceMultiface(std::vector<boost::shared_ptr<DedupServiceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~DedupServiceMultiface() {}
 protected:
  std::vector<boost::shared_ptr<DedupServiceIf> > ifaces_;
  DedupServiceMultiface() {}
  void add(boost::shared_ptr<DedupServiceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void is_exists(DedupExistItem& _return, const std::string& url) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->is_exists(_return, url);
    }
    ifaces_[i]->is_exists(_return, url);
    return;
  }

  bool insert(const std::string& url) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->insert(url);
    }
    return ifaces_[i]->insert(url);
  }

  bool test_exists_and_insert(const std::string& url) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->test_exists_and_insert(url);
    }
    return ifaces_[i]->test_exists_and_insert(url);
  }

  bool remove(const std::string& url) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->remove(url);
    }
    return ifaces_[i]->remove(url);
  }

  void info(std::string& _return) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->info(_return);
    }
    ifaces_[i]->info(_return);
    return;
  }

  int32_t set_bucket_count(const int32_t bucket_count) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->set_bucket_count(bucket_count);
    }
    return ifaces_[i]->set_bucket_count(bucket_count);
  }

  int32_t batch_remove(const std::string& url_pattern) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->batch_remove(url_pattern);
    }
    return ifaces_[i]->batch_remove(url_pattern);
  }

};



#endif
