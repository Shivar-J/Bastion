//
// Created by sjadoona on 2026-07-14.
//

#include "../inc/camera.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace Bastion
{
  Camera::Camera(const std::optional<glm::vec3> _position,
    const std::optional<glm::quat>& _orientation,
    const std::optional<float> _speed,
    const std::optional<float> _sensitivity)
  {
    if (_position)
    {
      position = *_position;
    }
    if (_orientation)
    {
      orientation = *_orientation;
    }
    if (_speed)
    {
      speed = *_speed;
    }
    if (_sensitivity)
    {
      sensitivity = *_sensitivity;
    }

    update();
  }

  glm::mat4 Camera::getViewMatrix() const
  {
    const glm::quat reverse = glm::conjugate(orientation);
    const glm::mat4 rotation = glm::mat4_cast(reverse);
    const glm::mat4 translation = glm::translate(glm::mat4(1.0f), -position);

    return rotation * translation;
  }

  glm::mat4 Camera::getProjectionMatrix(const float width, const float height) const
  {
    glm::mat4 projection = glm::perspectiveFov(glm::radians(fov), width, height, near, far);
    projection[1][1] *= -1.0f;
    return projection;
  }

  void Camera::setPosition(const glm::vec3 _position)
  {
    position = _position;
  }

  void Camera::setOrientation(const glm::quat _orientation)
  {
    orientation = _orientation;
  }

  void Camera::setAngle(const glm::vec2 _angle)
  {
    angle = _angle;
  }

  glm::vec3 Camera::getPosition() const
  {
    return position;
  }

  glm::quat Camera::getOrientation() const
  {
    return orientation;
  }

  glm::vec2 Camera::getAngle() const
  {
    return angle;
  }

  void Camera::processMovement(CameraMovement direction, const float deltaTime)
  {
    const float velocity = speed * deltaTime;

    const glm::quat qF = orientation * glm::quat(0, 0, 0, -1) * glm::conjugate(orientation);
    const glm::vec3 front = { qF.x, qF.y, qF.z };
    const glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    const glm::vec3 up = glm::normalize(glm::cross(right, front));

    switch (direction)
    {
    case CameraMovement::Forward:
      position += front * velocity;
      break;
    case CameraMovement::Backward:
      position -= front * velocity;
      break;
    case CameraMovement::Left:
      position -= right * velocity;
      break;
    case CameraMovement::Right:
      position += right * velocity;
      break;
    case CameraMovement::Up:
      position += up * velocity;
      break;
    case CameraMovement::Down:
      position -= up * velocity;
      break;
    default:
      break;
    }

    update();
  }

  void Camera::processOrientation(float mouseX, float mouseY)
  {
    angle[0] += mouseX * sensitivity;
    angle[1] += mouseY * sensitivity;

    angle[1] = std::clamp(angle[1], -89.0f, 89.0f);
    update();
  }

  void Camera::processZoom(float wheelX, float wheelY)
  {

  }

  void Camera::update()
  {
    const glm::quat yaw = glm::angleAxis(glm::radians(-angle[0]), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::quat pitch = glm::angleAxis(glm::radians(angle[1]), glm::vec3(1.0f, 0.0f, 0.0f));

    orientation = glm::normalize(yaw * pitch);
  }
} // Bastion