#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <chrono>
#include <cmath>
#include <string>
#include <algorithm>
#include <imgui/imgui.h>
#include <javascript/javascript.h>
#include <extra/Statistics/Statistics.h>

void Statistics::open()
{
    this->isOpen = true;
}
void Statistics::close()
{
    this->isOpen = false;
}
void Statistics::init()
{
    this->isOpen = false;
}
void Statistics::render()
{
    if (this->isOpen)
    {
        ImGui::Begin("Statistics");
        ImGui::Text("Heap in use -> %.2f MB", (double) total_allocated / (double) ONE_MEGABYTE);
        ImGui::End();  
    }
}