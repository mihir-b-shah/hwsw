
#ifndef INFO_MANAGER_H
#define INFO_MANAGER_H_

class info_manager {
public:
  info_manager(){}
  
  static info_manager* get_instance(){
    static info_manager instance;
    return &instance;
  }

  void update_all(ooo_model_instr* instr);
};

#endif
