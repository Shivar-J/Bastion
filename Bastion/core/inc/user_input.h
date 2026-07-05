//
// Created by sjadoona on 2026-07-04.
//

#ifndef BASTION_USER_INPUT_H
#define BASTION_USER_INPUT_H

#include <cstdint>

enum class InputType : int32_t
{
  None = 0,
  KeyDown = 1,
  KeyUp = 2,
  MouseMove = 3,
  MouseWheel = 4
};

#pragma pack(push, 1)
typedef struct UserInput
{
  InputType type;
  int32_t keyCode;
  float mouseX;
  float mouseY;
  float wheelX;
  float wheelY;
} UserInput;
#pragma pack(pop)

#endif //BASTION_USER_INPUT_H
