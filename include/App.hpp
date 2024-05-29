// include/MyClass.h
#ifndef APP_H
#define APP_H

#include "H5.hpp"
#include "H5Group.hpp"
#include "compute_units/ComputeUnit.hpp"
#include "highfive/highfive.hpp"
#include "imgui.h"
#include "immapp/immapp.h"
#include "immapp/runner.h"
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
class App {
public:
  App();
  void run();
  void render();
  void print();
  void on_start();
  void on_stop();
  void render_docking();
  void render_logger();
  void render_menu();
  void render_files();
  void render_table();
  void render_compute();
  void render_h5();
  std::string ask_string(std::string title, std::string content,
                         std::string hint, bool *blocking,int id);
  std::tuple<std::string,int, int, int> ask_dim(std::string title, std::string content,
                              std::string hint, bool *blocking);
  // void log(std::string & str);
  void log(std::string str);
  std::string open_file_dialog();
  void HelpMarker(const char *desc);
  void ShowDockingDisabledMessage();
  H5 *get_selected_file_descriptor();
  void tree_recursive_rendere(H5Group *g, ImGuiTreeNodeFlags flags,
                              std::string current_path, int index);

  enum COMMANDS {
    CREATE_H5,
    OPEN_H5,
    DELETE_H5,
    INSERT_DATASET_INTO_H5,
    DELETE_DATASET_FROM_H5,
    RUN_COMPUTE_UNIT,
    KILL_COMPUTE_UNIT
  };
  enum COMMAND_STATUS {
    NEW,
    WAITING,
    RUNNIGN,
    DONE,
    ERROR
  };
    struct command_packet{
      COMMANDS cmd;
      COMMAND_STATUS status;
      int id;
      std::string str_arg0;
      int int_arg0;
      float float_arg0;
    };
    void enqueue_command(command_packet * pkt);
  void handle_queue();
  void create_int_dataset(int h5_index,std::string dataset_name,int w, int h);
  void create_float_dataset(int h5_index, std::string dataset_name,int w, int h);
  void process_cmd(command_packet * pkt);
private:
  bool user_want_to_load_hdf5 = false;
  bool user_want_to_load_numpy = false;
  bool user_want_to_create_h5 = false;
  bool user_want_to_create_dataset= false;
  std::string logs;
  std::vector<H5 *> h5_files;
  std::vector<HighFive::File *> nump_files;
  std::string selected_dataset;
  std::string selected_file;
  H5 *current_file;
  int horizontal_cursor = 0;
  int selected_file_index = 0;
  int selected_compute_unit = 0;
  std::vector<ComputeUnit *> compute_units;
  std::map<std::string, ComputeUnit::parameter_list> current_parameter_list;
  std::vector<std::unique_ptr<command_packet>> command_queue;

  // must add command queue and compute queue
};

#endif // MYCLASS_H
