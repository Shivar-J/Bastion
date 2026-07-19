//
// Created by sjadoona on 2026-07-04.
//

#ifndef BASTION_USER_INPUT_H
#define BASTION_USER_INPUT_H

#include <chrono>
#include <cstdint>
#include <queue>
#include <map>

#include "camera.h"
#include "input_types.h"

#pragma pack(push, 1)
typedef struct UserInput
{
  InputType type;
  int32_t keyCode;
  int32_t mouseCode;
  float mouseX;
  float mouseY;
  float wheelX;
  float wheelY;

} UserInput;
#pragma pack(pop)

template<typename T>
using InputState = std::map<T, std::pair<bool, std::chrono::time_point<std::chrono::high_resolution_clock>>>;

namespace Bastion
{
  class Input
  {
  private:
    std::queue<UserInput> inputs;
    InputState<KeyCode> keyState;
    InputState<MouseCode> mouseState;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime = std::chrono::high_resolution_clock::now();

  public:
    void add(const UserInput& input);
    void process(Camera& camera);

  private:
    void processMouse(Camera& camera, const UserInput& input);
  };
}

#endif //BASTION_USER_INPUT_H
