#include "main_menu.h"

#include "imgui_inc.h"

using namespace game;

void main_menu::update()
{
  IMGUI_CALL(
  ImGui::Text("Main Menu"));
  IMGUI_CALL(
  _play_clicked = ImGui::Button("Play"));
}