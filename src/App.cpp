#include "App.hpp"

#include "compute_units/Average.hpp"
#include "compute_units/ComputeUnit.hpp"
#include "imgui.h"
#include "portable_file_dialogs/portable_file_dialogs.h"
#include <algorithm>
#include <boost/array.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <format>
#include <memory>
#include <tuple>
#include <unistd.h>
#include <utility>
#include <variant>
#include <vector>

App::App() {
  auto avg = new Average(this);
  compute_units.push_back(avg);
  avg = new Average(this);
  compute_units.push_back(avg);
}
void App::print() { std::cout << "Hello from MyClass!" << std::endl; }
void App::render_docking() {
  static bool opt_fullscreen = true;
  static bool opt_padding = false;
  static bool *p_open;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window
  // not dockable into, because it would be confusing to have two docking
  // targets within each others.
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen) {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
  } else {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render
  // our background and handle the pass-thru hole, so we ask Begin() to not
  // render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window
  // is collapsed). This is because we want to keep our DockSpace() active. If a
  // DockSpace() is inactive, all active windows docked into it will lose their
  // parent and become undocked. We cannot preserve the docking relationship
  // between an active window and an inactive docking, otherwise any change of
  // dockspace/settings would lead to windows being stuck in limbo and never
  // being visible.
  if (!opt_padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", p_open, window_flags);
  if (!opt_padding)
    ImGui::PopStyleVar();

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  // Submit the DockSpace
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  } else {
    ShowDockingDisabledMessage();
  }

  if (ImGui::BeginMenuBar()) {
    render_menu();
    if (ImGui::BeginMenu("Options")) {
      // Disabling fullscreen would allow the window to be moved to the front of
      // other windows, which we can't undo at the moment without finer window
      // depth/z control.
      ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
      ImGui::MenuItem("Padding", NULL, &opt_padding);
      ImGui::Separator();

      if (ImGui::MenuItem("Flag: NoSplit", "",
                          (dockspace_flags & ImGuiDockNodeFlags_NoSplit) !=
                              0)) {
        dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
      }
      if (ImGui::MenuItem("Flag: NoResize", "",
                          (dockspace_flags & ImGuiDockNodeFlags_NoResize) !=
                              0)) {
        dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
      }
      if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
                          (dockspace_flags &
                           ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) {
        dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
      }
      if (ImGui::MenuItem(
              "Flag: AutoHideTabBar", "",
              (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
        dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
      }
      if (ImGui::MenuItem(
              "Flag: PassthruCentralNode", "",
              (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
              opt_fullscreen)) {
        dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
      }
      ImGui::Separator();

      if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
        *p_open = false;
      ImGui::EndMenu();
    }
    HelpMarker(
        "When docking is enabled, you can ALWAYS dock MOST window into "
        "another! Try it now!"
        "\n"
        "- Drag from window title bar or their tab to dock/undock."
        "\n"
        "- Drag from window menu button (upper-left button) to undock an "
        "entire node (all windows)."
        "\n"
        "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == "
        "false, default)"
        "\n"
        "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)"
        "\n"
        "This demo app has nothing to do with enabling docking!"
        "\n\n"
        "This demo app only demonstrate the use of ImGui::DockSpace() which "
        "allows you to manually create a docking node _within_ another window."
        "\n\n"
        "Read comments in ShowExampleAppDockSpace() for more details.");

    ImGui::EndMenuBar();
  }

  ImGui::End();
}

void App::HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered(ImGuiHoveredFlags_None)) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void App::ShowDockingDisabledMessage() {
  ImGuiIO &io = ImGui::GetIO();
  ImGui::Text("ERROR: Docking is not enabled! See Demo > Configuration.");
  ImGui::Text(
      "Set io.ConfigFlags |= ImGuiConfigFlags_DockingEnable in your code, or ");
  ImGui::SameLine(0.0f, 0.0f);
  if (ImGui::SmallButton("click here"))
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
void App::on_start() {
  std::cout << "Starting Application  ....." << std::endl;
}
void App::on_stop() { std::cout << "closing Application   ....." << std::endl; }
void App::render_logger() {
  ImGui::Begin("logger");
  ImGui::Text(logs.c_str());
  ImGui::End();
}

// void App::log(std::string &str) { logs.append("\n").append(str); }
void App::log(std::string str) {
  std::cout << str << std::endl;
  logs.append("\n").append(str);
}
void App::render_menu() {
  command_packet *pkt;
  // if (ImGui::BeginMenuBar())
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("Add hdf5")) {
      pkt = new command_packet();
      pkt->cmd = OPEN_H5;
      pkt->status = NEW;
      enqueue_command(pkt);
    }

    if (ImGui::MenuItem("Open numpy"))
      user_want_to_load_numpy = true;

    if (ImGui::MenuItem("Create H5 file")) {
      pkt = new command_packet();
      pkt->cmd = CREATE_H5;
      pkt->status = NEW;
      enqueue_command(pkt);
    }

    ImGui::EndMenu();
  }
}
std::string App::open_file_dialog() {
  std::string path =
      "/home/archiver/storage/githubs/ASCAD/ATMEGA_AES_v1/ATM_AES_v1_fixed_key/"
      "ASCAD_data/ASCAD_databases/ASCAD_desync50.h5";
  auto selection = pfd::open_file("Select a file").result();
  if (!selection.empty())
    path = selection[0];
  return path;
}

void App::tree_recursive_rendere(H5Group *g, ImGuiTreeNodeFlags flags,
                                 std::string current_path, int index) {
  // bool is_open = ImGui::TreeNodeEx((void *)(intptr_t)current_level, flags,
  // g->get_name().c_str());
  if (ImGui::TreeNode(g->get_name().c_str())) {
    if (!g->get_groups().empty())
      for (auto inner_g : g->get_groups())
        tree_recursive_rendere(inner_g, flags,
                               current_path + "/" + inner_g->get_name(), index);

    for (auto inner_leaf : g->get_leafs())
      if (ImGui::Button(inner_leaf.c_str())) {
        selected_dataset = current_path + "/" + inner_leaf;
        selected_file_index = index;
      }
    ImGui::TreePop();
  }
}
void App::render_files() {
  static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                         ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                         ImGuiTreeNodeFlags_SpanAvailWidth;
  static bool align_label_with_current_x_position = false;
  static bool test_drag_and_drop = false;
  static int selection_mask = (1 << 2);
  int node_clicked = -1;

  ImGuiTreeNodeFlags node_flags = base_flags;
  node_flags |=
      ImGuiTreeNodeFlags_Leaf |
      ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

  ImGui::Begin("File Browser");

  for (size_t i = 0; i < h5_files.size(); i++) {

    selected_file = h5_files.at(i)->get_filename();
    if (ImGui::TreeNode(selected_file.c_str())) {
      tree_recursive_rendere(h5_files.at(i)->get_groups(), node_flags, "/", i);
      if (ImGui::Button("+")) {
        command_packet *pkt = new command_packet();
        pkt->cmd = INSERT_DATASET_INTO_H5;
        pkt->int_arg0 = i;
        pkt->status = NEW;
        enqueue_command(pkt);
      }

      ImGui::TreePop();
    }
  }
  ImGui::End();
}
static void PushStyleCompact() {
  ImGuiStyle &style = ImGui::GetStyle();
  ImGui::PushStyleVar(
      ImGuiStyleVar_FramePadding,
      ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
  ImGui::PushStyleVar(
      ImGuiStyleVar_ItemSpacing,
      ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact() { ImGui::PopStyleVar(2); }

H5 *App::get_selected_file_descriptor() {
  return h5_files.at(selected_file_index);
}

void App::render_table() {
  if (selected_dataset.empty() || selected_file.empty())
    return;

  auto file = get_selected_file_descriptor();
  log(std::format("table rendering file name {} dataset {}",file->get_filename(),selected_dataset));
  file->select_dataset(selected_dataset);
  auto shape = file->get_shape();
  auto dim = file->get_dim();
  size_t nrows = shape.at(0);
  size_t ncols = 1;
  size_t cols_span = 1;

  if (dim > 1) {
    ncols = shape.at(1);

    cols_span = 30;
  }

  size_t display_cols = 0;
  auto datatype = file->get_datatype();
  std::string cell_format = "";
  std::vector<std::vector<int>> please_help_int;
  std::vector<std::vector<float>> please_help_flt;
  std::vector<std::vector<std::string>> please_help_str;
  std::vector<int> please_help_int1d;
  std::vector<float> please_help_flt1d;
  std::vector<std::string> please_help_str1d;

  static ImGuiTableFlags flags =
      ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
      ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable;
  PushStyleCompact();
  ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags,
                       ImGuiTableFlags_ScrollY);
  PopStyleCompact();
  const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

  ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
  ImGui::Begin("table view");
  ImGui::DragInt("horizontal scroll", &horizontal_cursor, 1.0f, 0,
                 ncols - cols_span);

  if (ImGui::BeginTable("table_scrolly", cols_span + 1, flags, outer_size)) {
    ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
    display_cols = horizontal_cursor + cols_span;
    for (int i = horizontal_cursor; i < display_cols; i++)
      ImGui::TableSetupColumn(std::format("{}", i).c_str(),
                              ImGuiTableColumnFlags_None);

    ImGui::TableHeadersRow();

    // Demonstrate using clipper for large vertical lists
    ImGuiListClipper clipper;
    clipper.Begin(nrows);
    while (clipper.Step()) {
      switch (datatype) {
      case H5::TYPES::Integer:
        if (dim > 1)
          file->read_selection<std::vector<int>>(
              clipper.DisplayStart, horizontal_cursor,
              clipper.DisplayEnd - clipper.DisplayStart, cols_span,
              please_help_int);
        else
          file->read_selection<int>(clipper.DisplayStart, ncols,
                                    clipper.DisplayEnd - clipper.DisplayStart,
                                    ncols, please_help_int1d);

        break;
      case H5::TYPES::Float:
        if (dim > 1)
          file->read_selection<std::vector<float>>(
              clipper.DisplayStart, horizontal_cursor,
              clipper.DisplayEnd - clipper.DisplayStart, cols_span,
              please_help_flt);
        else
          file->read_selection<float>(clipper.DisplayStart, ncols,
                                      clipper.DisplayEnd - clipper.DisplayStart,
                                      ncols, please_help_flt1d);

        break;

      case H5::TYPES::String:
        if (dim > 1)
          file->read_selection<std::vector<std::string>>(
              clipper.DisplayStart, horizontal_cursor,
              clipper.DisplayEnd - clipper.DisplayStart, cols_span,
              please_help_str);
        else
          file->read_selection<std::string>(clipper.DisplayStart, ncols,
                                            clipper.DisplayEnd -
                                                clipper.DisplayStart,
                                            ncols, please_help_str1d);

        break;
      }
      for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", row);
        for (int column = 0; column < cols_span; column++) {
          ImGui::TableSetColumnIndex(column + 1);
          int c = column + horizontal_cursor;

          switch (datatype) {
          case H5::TYPES::Integer:
            if (dim > 1)
              ImGui::Text(
                  "%d",
                  please_help_int.at(row - clipper.DisplayStart).at(column));
            else
              ImGui::Text("%d",
                          please_help_int1d.at(row - clipper.DisplayStart));

            break;
          case H5::TYPES::Float:
            if (dim > 1)
              ImGui::Text(
                  "%f",
                  please_help_flt.at(row - clipper.DisplayStart).at(column));
            else
              ImGui::Text("%f",
                          please_help_flt1d.at(row - clipper.DisplayStart));
            break;
          case H5::TYPES::String:
            if (dim > 1)
              ImGui::Text("%s", please_help_str.at(row - clipper.DisplayStart)
                                    .at(column)
                                    .c_str());
            else
              ImGui::Text(
                  "%s",
                  please_help_str1d.at(row - clipper.DisplayStart).c_str());

            break;
          default:
            ImGui::Text("nani");
          }
        }
        // if (ImGui::Selectable("selected", true,
        //                       ImGuiSelectableFlags_SpanAllColumns,
        //                       ImVec2(0, 1.f)))
        //   log("im selected");
      }
    }
    ImGui::EndTable();
  }

  ImGui::End();
}
void App::render() {

  static bool u = false;
  render_docking();
  render_menu();
  render_logger();
  render_files();
  render_table();
  render_compute();
  // ImGui::PushID(rand());
  handle_queue();
  // ImGui::PopID();
   ImGui::ShowIDStackToolWindow(&u);
}

std::string App::ask_string(std::string title, std::string content,
                            std::string hint, bool *blocking,int id) {
  const int str_size = 100;
  static char str_out[str_size] = "not ready";
  bool used = true;

  ImGui::OpenPopup(title.c_str());
  if (ImGui::BeginPopupModal(title.c_str())) {
    ImGui::PushID(id);
    ImGui::Text(content.c_str());

    ImGui::InputTextWithHint("  ", hint.c_str(), str_out, str_size);
    if (ImGui::Button("Confirm")) {
      ImGui::CloseCurrentPopup();
      *blocking = true;
    }
    ImGui::PopID();
    ImGui::EndPopup();
  }
  return str_out;
}

std::tuple<std::string, int, int, int> App::ask_dim(std::string title,
                                                    std::string content,
                                                    std::string hint,
                                                    bool *blocking) {
  const int str_size = 100;
  static char str_out[str_size] = "not ready";
  static char str_out2[str_size] = "not ready";
  static int dim[2] = {0, 0};
  static int int_0 = 0;
  auto types = H5::get_type_names();
  ImGui::OpenPopup(title.c_str());
  if (ImGui::BeginPopupModal(title.c_str(), NULL, ImGuiWindowFlags_MenuBar)) {

    ImGui::InputTextWithHint("  ", hint.c_str(), str_out, str_size);
    ImGui::InputInt2(content.c_str(), dim);
    ImGui::Combo("Select Dataset Type", &int_0, types.data(), types.size());
    if (ImGui::Button("Confirm")) {
      ImGui::CloseCurrentPopup();
      *blocking = true;
    }
    ImGui::EndPopup();
  }
  return {str_out, dim[0], dim[1], 1 << (int_0 + 1)};
}
void App::run() {

  HelloImGui::RunnerParams runnerParams;
  runnerParams.callbacks.ShowGui = [&] { render(); };
  runnerParams.callbacks.PostInit = [&] { on_start(); };
  runnerParams.callbacks.BeforeExit = [&] { on_stop(); };
  runnerParams.appWindowParams.windowTitle = "imgui node editor";
  runnerParams.appWindowParams.windowGeometry.size = {1200, 800};

  ImmApp::Run(runnerParams);
}

void App::render_compute() {
  ImGui::Begin("Compute Operations");

  std::vector<char *> items;
  ComputeUnit::parameter_list empty_pl{"", 0};
  for (auto c : compute_units)
    items.push_back(
        const_cast<char *>(c->get_unit_name().c_str())); // performace hit
  // Custom size: use all width, 5 items tall
  ImGui::Text("Available Compute Units");
  if (ImGui::BeginListBox(
          "##listbox 2",
          ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
    for (int n = 0; n < items.size(); n++) {
      const bool is_selected = (selected_compute_unit == n);
      if (ImGui::Selectable(items[n], is_selected)) {
        if (selected_compute_unit != n)
          current_parameter_list.clear();

        selected_compute_unit = n;
      }

      // Set the initial focus when opening the combo (scrolling + keyboard
      // navigation focus)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndListBox();
  }

  auto sel_compute = compute_units[selected_compute_unit];

  for (auto p : sel_compute->get_parameters_descriptions()) {
    if (current_parameter_list.count(p.first) == 0)
      current_parameter_list[p.first] = {"", 0};

    ImGui::Text(p.first.c_str());
    if (p.second.first == ComputeUnit::DATASET) {
      ImGui::Text("Available Files");
      if (ImGui::BeginListBox(
              p.first.c_str(),
              ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
        for (int n = 0; n < h5_files.size(); n++) {
          const bool is_selected =
              (std::get<int>(current_parameter_list.at(p.first).arg0) == n);

          if (ImGui::Selectable(h5_files[n]->get_filename().c_str(),
                                is_selected))
            current_parameter_list.at(p.first).arg0 = n;

          // Set the initial focus when opening the combo (scrolling + keyboard
          // navigation focus)
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }

      int i = std::get<int>(current_parameter_list[p.first].arg0);
      if (!h5_files.empty() &&
          ImGui::BeginListBox(
              (p.first + " datasets").c_str(),
              ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing()))) {
        auto &datasets = h5_files.at(i)->get_leafs();

        for (int n = 0; n < datasets.size(); n++) {
          const bool is_selected =
              (current_parameter_list.at(p.first).dataset_name == datasets[n]);
          if (ImGui::Selectable(datasets[n].c_str(), is_selected))
            current_parameter_list[p.first].dataset_name = datasets[n];
          // Set the initial focus when opening the combo (scrolling + keyboard
          // navigation focus)
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
      }
    } else if (p.second.first == ComputeUnit::INT) {
    }
    ///// add selected dataset
  }
  ImGui::End();

  // std::vector<std::vector<float>> y{{88,88},
  //                                   {92,92},
  //                                   {95,95},
  //                                   {82,82},
  //                                   {74,74},
  //                                   {78,78},
  //                                   {96,96}};

  // std::vector<std::vector<float>> x{{98, 98},
  //                                   {87, 87},
  //                                   {91, 91},
  //                                   {75, 75},
  //                                   {81, 81},
  //                                   {68, 68},
  //                                   {93, 93}};
  // std::vector<float> o(1000);
  // //gpu_correlation(y, x, o);

  // for(auto f: o)
  //   std::cout << f ;
  // std::cout << std::endl;
}

void App::render_h5() {}

static int counter = 0;
void App::enqueue_command(command_packet *pkt) {
  log("creating packet ");
  pkt->id = ++counter;
  command_queue.push_back(std::unique_ptr<command_packet>(pkt));
}

void App::handle_queue() {
     
  if (command_queue.empty())
    return;

  for (auto &c : command_queue) {
    command_packet * pkt = c.get();
    if (c->status == NEW || c->status == WAITING) 
      process_cmd(c.get());
    
    
  }
  command_queue.erase(std::remove_if(command_queue.begin(), command_queue.end(), [](auto & x){return x->status== DONE;}),command_queue.end());
}

void App::process_cmd(command_packet *pkt) {
  


  switch (pkt->cmd) {
  case CREATE_H5:
    try {
      bool blocking = false; 
      auto filename = ask_string("Create H5", "Create new H5 file",
                                "enter filename", &blocking,pkt->id);
      if (blocking) {
        std::cout << "creating : " << filename << std::endl;
        HighFive::File file(filename, HighFive::File::Create);
        H5 *h5 = new H5(filename);
        h5_files.push_back(h5);
        pkt->status = DONE;
      }


          } catch (std::exception &e) {
      pkt->status = ERROR;
      log(e.what());
    }
    break;
  case OPEN_H5:
    try {
      // if()

      std::string path = open_file_dialog();
      log(path);
      H5 *h5_file = new H5(path);
      h5_files.push_back(h5_file);
      pkt->status = DONE;

    } catch (std::exception &e) {
      pkt->status = ERROR;
      log(e.what());
    }
    break;
  case DELETE_H5:
    break;
  case INSERT_DATASET_INTO_H5:
    try {

      bool blocking = false;
      auto [dataset_name, y_dim, x_dim, type] = ask_dim(
          "Dataset Informations", "enter the dimenstions :", "", &blocking);
      if (blocking) {
        H5 *h5 = h5_files.at(pkt->int_arg0);

        switch (type) {
        case H5::TYPES::Float:
          create_float_dataset(pkt->int_arg0, dataset_name, y_dim, x_dim);
          log("user want enter to create float dataset");
          break;
        case H5::TYPES::Integer:

          create_int_dataset(pkt->int_arg0, dataset_name, y_dim, x_dim);
          log("user want enter to create int dataset");
          break;
        default:
          log("Unsupported DataSet Type");
          break;
        }

        pkt->status = DONE;
      }
    } catch (std::exception &e) {
      pkt->status = ERROR;
      log(e.what());
    }

    break;
  case DELETE_DATASET_FROM_H5:
    break;
  case RUN_COMPUTE_UNIT:
    break;
  case KILL_COMPUTE_UNIT:
    break;
  default:
    break;
  }
}


void App::create_int_dataset(int h5_index,std::string dataset_name,int w, int h){

  auto h5 = h5_files.at(h5_index);
  log(std::format("creating dataset {} in {}",dataset_name,h5->get_filename()));
  h5->create_dataset<int>(h, w, dataset_name);   

};

void App::create_float_dataset(int h5_index,std::string dataset_name, int h, int w){

  auto h5 = h5_files.at(h5_index);
  log(std::format("creating dataset {} in {}",dataset_name,h5->get_filename()));
  h5->create_dataset<float>(h, w, dataset_name);   
  log(std::format("done populating"));
};
