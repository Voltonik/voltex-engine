#include <renderer/utilities/vk_types.h>
#include <SDL_events.h>

class Camera {
public:
    glm::vec3 velocity{ 0, 0, 0 };
    glm::vec3 position{ 0, 0, 0 };
    // vertical rotation
    float pitch{ 0.f };
    // horizontal rotation
    float yaw{ 0.f };

    glm::mat4 getViewMatrix();
    glm::mat4 getRotationMatrix();

    void processSDLEvent(SDL_Event& e);

    void update();
};