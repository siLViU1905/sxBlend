#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "../GLFW/glfw3.h"
#include "../glm/glm.hpp"

namespace sx
{

    class Camera
    {
        glm::mat4 view;
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        float yaw, pitch;

        float startTime;

        double lastX, lastY;

        float speed;

    public:
        Camera(GLFWwindow *window, glm::vec3 pos, float speed);

        glm::vec3 &getPosition();

        const glm::vec3 &getPosition() const;

        glm::mat4 &getView();

        const glm::mat4 &getView() const;

        glm::vec3 &getFront();

        const glm::vec3 &getFront() const;

        const glm::vec3 &getUp() const;

        glm::vec3 getRight() const;

        float getYaw() const;

        float getPitch() const;

        void update(GLFWwindow *window);

        bool updateMouse;

        void setSpeed(float speed);

    
    };

}

#endif // __CAMERA_H__