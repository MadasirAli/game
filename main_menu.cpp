#include "main_menu.h"

#include "imgui.h"

using namespace game;

void main_menu::update()
{
  ImGui::Text("Main Menu");
  _play_clicked = ImGui::Button("Play");
}