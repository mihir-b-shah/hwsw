
#ifndef CODE_INFORMER_H
#define CODE_INFORMER_H

/*  This class manages all the different code_info's we want to use. */

#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <tuple>

#include "code_info.h"
#include "loop_info.h"
#include "call_stack.h"
#include "llvm_info.h"


/*  To avoid duplicating the list of code_info's we are using everywhere,
    we use this combination of a tuple and this macro to stamp out uses
    of all the code_info's. So V just needs to be something that takes
    a single template type parameter. (typelists are better but more effort)
    Hence, when adding a new code_info, add it to this list. */
#define TYPE_EXPAND(V) std::tuple<        \
  V<call_stack>,                          \
  V<loop_info>                            \
>

/*  So for example, we want vectors for each code_info, to give back their
    results, and we want to instantiate each of the code_info's */
typedef TYPE_EXPAND(std::vector) results_t;
typedef TYPE_EXPAND(code_info_impl) info_impls_t;

/*  Useful in user code, to take the results type, and get the vector
    for the info type that you want (a C++14 feature) */
#define GET_RESULT(results, type) std::get<std::vector<type>>(results)

/*  Queries want to request information about certain instruction ids,
    so the input comes as an initializer_list (no allocations),
    but to store it we need a vector, since it will be used later. */
typedef std::vector<uint64_t> instr_ids_t;
typedef std::initializer_list<uint64_t> init_iid_t;
  
/*  Base class to use, since we can't pass around a templated type
    everywhere (for the lambda).
    The key point- this is necessary because queries have to execute
    asnychronously, with respect to the user code submitting them. */
struct query {
  uint64_t trigger;
  instr_ids_t iids; 

  query(uint64_t trigger, init_iid_t iids) : trigger(trigger), iids(iids) {}
  virtual ~query(){}
  virtual void run_cb(const results_t& results) = 0;
};

/*  Derived class implementation. Passing in any lambda is nice
    since we can capture variables in the user code context 
    arbitrarily, so this feels almost like synactic sugar
    for an arbitrary class */
template<typename query_cb>
struct query_impl : query {
  query_cb cb;

  query_impl(uint64_t trigger, init_iid_t iid_list, query_cb cb) 
    : query(trigger, iid_list), cb(cb) {}
  ~query_impl(){}
  void run_cb(const results_t& results) override { cb(results); }
};

/*  Other entities' interface to manipulating code info- the user's,
    for accepting their query, and the ooo_cpu, for calling the
    retirement logic. */
class code_informer {
public:
  code_informer() {}

  template <typename query_cb>
  void accept_query(uint64_t trigger, init_iid_t iid_list, query_cb cb){
    todo[trigger] = new query_impl<query_cb>(trigger, iid_list, cb);
  }

  void handle_retire(ooo_model_instr* instr);
  
  static code_informer* get_instance(){
    static code_informer instance;
    return &instance;
  }

private:
  results_t results;
  info_impls_t infos;

  /*  todo has the invariant that there can be only one query per instruction-
      if you add another one, your first one will probably be ignored. */
  std::unordered_map<uint64_t, query*> todo;
};

#endif
