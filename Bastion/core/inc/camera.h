//
// Created by sjadoona on 2026-07-14.
//

#ifndef BASTION_CAMERA_H
#define BASTION_CAMERA_H

#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Bastion
{
  enum class CameraMovement
  {
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down,
    Unknown
  };

  class Camera
  {
  private:
    float speed = 0.1f;
    float sensitivity = 0.01f;
    float fov = 60.0f;
    float near = 0.001f;
    float far = 100000.0f;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    // 0 = right, 1 = up
    glm::vec2 angle = glm::vec2(0.0f, 0.0f);

  public:
    explicit Camera(std::optional<glm::vec3> _position = std::nullopt,
                    const std::optional<glm::quat>& _orientation = std::nullopt,
                    std::optional<float> _speed = std::nullopt,
                    std::optional<float> _sensitivity = std::nullopt);

    [[nodiscard]] glm::mat4 getViewMatrix() const;
    [[nodiscard]] glm::mat4 getProjectionMatrix(float width, float height) const;

    void setPosition(glm::vec3 _position);
    void setOrientation(glm::quat _orientation);
    void setAngle(glm::vec2 _angle);

    [[nodiscard]] glm::vec3 getPosition() const;
    [[nodiscard]] glm::quat getOrientation() const;
    [[nodiscard]] glm::vec2 getAngle() const;

    void processMovement(CameraMovement direction, float deltaTime);
    void processOrientation(float mouseX, float mouseY);
    void processZoom(float wheelX, float wheelY);
    void update();
  };
} // Bastion

#endif //BASTION_CAMERA_H
