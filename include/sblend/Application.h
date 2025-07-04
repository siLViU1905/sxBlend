#ifndef __APPLICATION_H__
#define __APPLICATION_H__
#include "../GL/glad.h"
#include "../GLFW/glfw3.h"
#include "Camera.h"
#include "LightManager.h"
#include "Menu.h"
#include "MeshBoolean.h"
#include "MeshManager.h"
#include "ShaderManager.h"
#include "Terrain.h"
#include "graphics/Mesh.h"
#include "graphics/Model.h"
#include "graphics/Reflection.h"
#include "graphics/Shader.h"
#include "graphics/Shadow.h"
#include "graphics/Skybox.h"
#include "graphics/Vertex.h"
#include "physics/PhysicsWorld.h"
#include <fstream>


namespace sx {
typedef GLFWwindow *Window;

class Application {
  static int WINDOW_WIDTH, WINDOW_HEIGHT;
  Window window;

  static void keyCallback(Window window, int key, int scancode, int action,
                          int mods);

  static void resizeCallback(Window window, int width, int height);

  static void scrollCallback(Window window, double x, double y);

  static bool swapInterval;

  ImGuiIO *imGuiIo;

  MainMenu mainMenu;

  bool renderObjectMenu;

  bool renderLightMenu;

  GLFWimage appIcon;

  static void newMenuFrame();

  static void renderMenuFrame();

  static Application *application;

  MeshManager *meshes;

  std::vector<Model> models;

  ShaderManager *shaders;

  LightManager lights;

  Skybox *skybox;

  bool useSkybox = false;

  int lightCount = 0;

  int pbrLightCount = 0;

  void updateMenuState();

  static bool windowResized;

  static glm::mat4 projection;

  static float fov;

  Mesh *grid, *gridLines;

  Camera camera;

  bool enableShadows = false;

  Shadow *shadow;

  Reflection *reflection;

  Physics physicsWorld;

  Terrain *terrain;

  bool useTerrain = false;

  void updatePhysics();

  void renderWithNoShadows();

  void renderWithShadows();

  void handleRightClickedMouseEventForMeshes();

  void handleRightClickedMouseEventForModels();

  void handleLeftClickedMouseEvent();

  bool leftMouseButtonPressedThisFrame = false;
  bool leftMouseButtonPressedBeforeThisFrame = false;

  double mouseX, mouseY;

  Application();

public:
  Application(const Application &app) = delete;

  Application(Application &&app) = delete;

  Application &operator=(const Application &app) = delete;

  Application &operator=(Application &&app) = delete;

  void run();

  static Application *getApplication();

  static void terminateApplication(Application *&application);
};
} // namespace sx

#endif // __APPLICATION_H__
