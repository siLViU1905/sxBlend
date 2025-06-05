#include "Camera.h"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <iostream>

namespace sx
{

    Camera::Camera(GLFWwindow *window, glm::vec3 pos, float speed) : speed(speed)
    {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        position = pos;

        up = glm::vec3(0.0f, 1.0f, 0.0f);

        front = glm::vec3(0.0f, 0.0f, -1.0f);

        yaw = -90.f;
        pitch = 0.f;

        glfwGetCursorPos(window, &lastX, &lastY);

        startTime = (float)glfwGetTime();

        view = glm::lookAt(position,
                           position + front,
                           up);

        updateMouse = true;
    }

    glm::vec3 &Camera::getPosition()
    {
        return position;
    }

    const glm::vec3 &Camera::getPosition() const
    {
        return position;
    }

    glm::mat4 &Camera::getView()
    {
        return view;
    }

    const glm::mat4 &Camera::getView() const
    {
        return view;
    }

    glm::vec3 &Camera::getFront()
    {
        return front;
    }

    const glm::vec3 &Camera::getFront() const
    {
        return front;
    }

    const glm::vec3 &Camera::getUp() const
    {
        return up;
    }

    glm::vec3 Camera::getRight() const
    {
        return glm::normalize(glm::cross(front, up));
    }

    float Camera::getYaw() const
    {
        return yaw;
    }

    float Camera::getPitch() const
    {
        return pitch;
    }

    void Camera::update(GLFWwindow *window)
    {
        if (updateMouse)
        {
            int width, height;
            glfwGetWindowSize(window, &width, &height);

            double newX, newY;
            glfwGetCursorPos(window, &newX, &newY);

            static double lastX = width / 2.0, lastY = height / 2.0; 

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
            {
              
                float xoffset = (newX - lastX) * 0.1f;
                float yoffset = (lastY - newY) * 0.1f; 

                yaw += xoffset;
                pitch += yoffset;
            }

           
            lastX = newX;
            lastY = newY;
    

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        glm::vec3 direction;
        direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        direction.y = std::sin(glm::radians(pitch));
        direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        front = glm::normalize(direction);

        float currentTime = (float)glfwGetTime();
        float deltaTime = (currentTime - startTime);
        startTime = currentTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * deltaTime * speed;
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * deltaTime * speed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(front, up)) * deltaTime * speed;
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(front, up)) * deltaTime * speed;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            position += glm::vec3(0.f, deltaTime, 0.f) * speed;
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            position += glm::vec3(0.f, -deltaTime, 0.f) * speed;

        view = glm::lookAt(position,
                           position + front,
                           up);
    }

    void Camera::setSpeed(float speed)
    {
        this->speed = speed;
    }
}