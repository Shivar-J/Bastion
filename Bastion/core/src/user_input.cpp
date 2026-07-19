//
// Created by sjadoona on 2026-07-14.
//

#include "../inc/user_input.h"

#include <iostream>

namespace Bastion
{
  void Input::add(const UserInput& input)
  {
    inputs.push(input);
  }

  void Input::process(Camera& camera)
  {
    const auto currentTime = std::chrono::high_resolution_clock::now();
    const float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    while (!inputs.empty())
    {
      UserInput& input = inputs.front();
      inputs.pop();

      auto keyCode = static_cast<KeyCode>(input.keyCode);
      auto mouseCode = static_cast<MouseCode>(input.mouseCode);

      switch (input.type)
      {
      case InputType::None:
        break;
      case InputType::KeyDown:
        if (input.keyCode != KeyCode::None)
        {
          keyState[keyCode] = std::make_pair(true, std::chrono::high_resolution_clock::now());
        }
        break;
      case InputType::KeyUp:
        if (input.keyCode != KeyCode::None)
        {
          keyState[keyCode] = std::make_pair(false, std::chrono::high_resolution_clock::now());
        }
        break;
      case InputType::MouseMove:
        camera.processOrientation(input.mouseX, input.mouseY);
        break;
      case InputType::MouseWheel:
        camera.processZoom(input.wheelX, input.wheelY);
        break;
      case InputType::MouseButton:
        if (input.mouseCode == MouseCode::None)
        {
          break;
        }
        if (mouseCode == MouseCode::LeftMouseDown ||
            mouseCode == MouseCode::RightMouseDown ||
            mouseCode == MouseCode::MiddleMouseDown)
        {
          mouseState[mouseCode] = std::make_pair(true, std::chrono::high_resolution_clock::now());
        }
        if (mouseCode == MouseCode::LeftMouseUp ||
            mouseCode == MouseCode::RightMouseUp ||
            mouseCode == MouseCode::MiddleMouseUp)
        {
          mouseState[mouseCode] = std::make_pair(false, std::chrono::high_resolution_clock::now());
        }
        break;
      default:
        throw std::runtime_error("Invalid input type");
      }
    }

    if (keyState[KeyCode::W].first || keyState[KeyCode::Up].first)
    {
      camera.processMovement(CameraMovement::Forward, deltaTime);
    }
    if (keyState[KeyCode::S].first || keyState[KeyCode::Down].first)
    {
      camera.processMovement(CameraMovement::Backward, deltaTime);
    }
    if (keyState[KeyCode::A].first || keyState[KeyCode::Left].first)
    {
      camera.processMovement(CameraMovement::Left, deltaTime);
    }
    if (keyState[KeyCode::D].first || keyState[KeyCode::Right].first)
    {
      camera.processMovement(CameraMovement::Right, deltaTime);
    }
  }

  void Input::processMouse(Camera& camera, const UserInput& input)
  {
  }
}
