#include "Application.h"
#include "Menu.h"
#include "Terrain.h"
#include "glm/gtc/type_ptr.hpp"
#include <stdexcept>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "../stb_image.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "../glm/gtx/intersect.hpp"

namespace sx {
int Application::WINDOW_WIDTH = 600;
int Application::WINDOW_HEIGHT = 600;

glm::mat4 Application::projection;

std::ostringstream saveProperties;

float Application::fov = 45.f;

bool Application::windowResized = false;

bool Application::swapInterval = true;

Application *Application::application = nullptr;

void Application::keyCallback(Window window, int key, int scancode, int action,
                              int mods) {
  if (action == GLFW_PRESS)
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, true);
      break;

    case GLFW_KEY_LEFT_ALT:
      swapInterval = !swapInterval;
      glfwSwapInterval(swapInterval);
      break;
    }
}

void Application::resizeCallback(Window window, int width, int height) {
  windowResized = true;
  if (height <= 0)
    height = 1;
  glViewport(0, 0, width, height);
  WINDOW_HEIGHT = height;
  WINDOW_WIDTH = width;
  projection =
      glm::perspective(glm::radians(fov),
                       (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);
}

void Application::scrollCallback(Window window, double x, double y) {
  if (fov < 15.f)
    fov = 15.f;
  else if (fov > 120.f)
    fov = 120.f;
  fov += -(float)y * 2.5f;
  projection =
      glm::perspective(glm::radians(fov),
                       (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);
}

void Application::newMenuFrame() {
  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
}

void Application::renderMenuFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::updateMenuState() {
  if (mainMenu.selectedMesh != MeshType::NaN)
  {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    switch (mainMenu.selectedMesh)
    {
    case MeshType::PLANE:
      MeshManager::generatePlaneMesh(vertices, indices);
      meshes->meshes.emplace_back(vertices, indices);
      meshes->meshes.back().type = mainMenu.selectedMesh;
      meshes->meshes.back().useFlatReflection = 1;
      mainMenu.selectedMesh = MeshType::NaN;
      break;

    case MeshType::CIRCLE:
      if (mainMenu.generate)
      {
        MeshManager::generateCircleMesh(vertices, indices, mainMenu.segments);
        meshes->meshes.emplace_back(vertices, indices);
        meshes->meshes.back().type = mainMenu.selectedMesh;
        meshes->meshes.back().slices = mainMenu.segments;
        meshes->meshes.back().useFlatReflection = 0;
        mainMenu.existentMeshes.emplace_back(
            "Circle " +
            std::to_string(
                mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
        mainMenu.selectedMeshIndicator.emplace_back(0);
        ++mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh];
        mainMenu.generate = false;
        mainMenu.newCircle = false;
        mainMenu.selectedMesh = MeshType::NaN;
        mainMenu.meshBtnPressed = false;
      }
      break;

    case MeshType::CUBE:
      MeshManager::generateCubeMesh(vertices, indices);
      meshes->meshes.emplace_back(vertices, indices);
      meshes->meshes.back().type = mainMenu.selectedMesh;
      meshes->meshes.back().useFlatReflection = 1;
      mainMenu.selectedMesh = MeshType::NaN;
      break;

    case MeshType::CONE:
      if (mainMenu.generate)
      {
        MeshManager::generateConeMesh(vertices, indices, mainMenu.segments);
        meshes->meshes.emplace_back(vertices, indices);
        meshes->meshes.back().type = mainMenu.selectedMesh;
        meshes->meshes.back().slices = mainMenu.segments;
        meshes->meshes.back().stacks = 0;
        meshes->meshes.back().useFlatReflection = 0;
        mainMenu.existentMeshes.emplace_back(
            "Cone " +
            std::to_string(
                mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
        mainMenu.selectedMeshIndicator.emplace_back(0);
        ++mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh];
        mainMenu.generate = false;
        mainMenu.newCone = false;
        mainMenu.selectedMesh = MeshType::NaN;
        mainMenu.meshBtnPressed = false;
      }
      break;

    case MeshType::CYLINDER:
      if (mainMenu.generate)
      {
        MeshManager::generateCylinderMesh(vertices, indices, mainMenu.segments);
        meshes->meshes.emplace_back(vertices, indices);
        meshes->meshes.back().type = mainMenu.selectedMesh;
        meshes->meshes.back().slices = mainMenu.segments;
        meshes->meshes.back().stacks = 0;
        meshes->meshes.back().useFlatReflection = 0;
        mainMenu.existentMeshes.emplace_back(
            "Cylinder " +
            std::to_string(
                mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
        mainMenu.selectedMeshIndicator.emplace_back(0);
        ++mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh];
        mainMenu.generate = false;
        mainMenu.newCylinder = false;
        mainMenu.selectedMesh = MeshType::NaN;
        mainMenu.meshBtnPressed = false;
      }
      break;

    case MeshType::SPHERE:
      if (mainMenu.generate)
      {
        MeshManager::generateSphereMesh(vertices, indices, mainMenu.slices,
                                        mainMenu.stacks);
        meshes->meshes.emplace_back(vertices, indices);
        meshes->meshes.back().type = mainMenu.selectedMesh;
        meshes->meshes.back().slices = mainMenu.slices;
        meshes->meshes.back().stacks = mainMenu.stacks;
        meshes->meshes.back().useFlatReflection = 0;
        mainMenu.existentMeshes.emplace_back(
            "Sphere " +
            std::to_string(
                mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
        mainMenu.selectedMeshIndicator.emplace_back(0);
        ++mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh];
        mainMenu.generate = false;
        mainMenu.newSphere = false;
        mainMenu.selectedMesh = MeshType::NaN;
        mainMenu.meshBtnPressed = false;
      }
      break;

    case MeshType::TORUS:
      if (mainMenu.generate)
      {
        MeshManager::generateTorusMesh(vertices, indices, mainMenu.slices,
                                       mainMenu.stacks);
        meshes->meshes.emplace_back(vertices, indices);
        meshes->meshes.back().type = mainMenu.selectedMesh;
        meshes->meshes.back().slices = mainMenu.slices;
        meshes->meshes.back().stacks = mainMenu.stacks;
        meshes->meshes.back().useFlatReflection = 0;
        mainMenu.existentMeshes.emplace_back(
            "Torus " +
            std::to_string(
                mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
        mainMenu.selectedMeshIndicator.emplace_back(0);
        ++mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh];
        mainMenu.generate = false;
        mainMenu.newTorus = false;
        mainMenu.selectedMesh = MeshType::NaN;
        mainMenu.meshBtnPressed = false;
      }
      break;
    }
  }

  if (mainMenu.selectedLight != LightType::NaN)
  {
    switch (mainMenu.selectedLight)
    {
    case LightType::POINT:
      lights.lights.emplace_back(LightType::POINT);
      mainMenu.selectedLight = LightType::NaN;
      shaders->lightningShader.setInt("lightCount", ++lightCount);
      shaders->modelLightningShader.setInt("lightCount", lightCount);
      shaders->terrainLightningShader.setInt("lightCount", lightCount);
      break;

    case LightType::DIRECTIONAL:
      lights.lights.emplace_back(LightType::DIRECTIONAL);
      mainMenu.selectedLight = LightType::NaN;
      shaders->lightningShader.setInt("lightCount", ++lightCount);
      shaders->modelLightningShader.setInt("lightCount", lightCount);
      shaders->terrainLightningShader.setInt("lightCount", lightCount);
      break;

    case LightType::SPOT:
      lights.lights.emplace_back(LightType::SPOT);
      mainMenu.selectedLight = LightType::NaN;
      shaders->lightningShader.setInt("lightCount", ++lightCount);
      shaders->modelLightningShader.setInt("lightCount", lightCount);
      shaders->terrainLightningShader.setInt("lightCount", lightCount);
      break;

    case LightType::PBR:
      lights.lights.emplace_back(LightType::PBR);
      mainMenu.selectedLight = LightType::NaN;
      shaders->pbrLightningShader.setInt("lightCount", ++pbrLightCount);
      shaders->modelPbrLightningShader.setInt("lightCount", pbrLightCount);
      shaders->terrainPbrLightningShader.setInt("lightCount", pbrLightCount);
      break;
    }
  }

  if (mainMenu.loadRequest.type == LoadRequestType::LOAD_MODEL &&
      !mainMenu.loadRequest.path.empty())
  {
    models.emplace_back(mainMenu.loadRequest.path.c_str());
    if (models.back().hasLoaded == false)
    {
      mainMenu.errorMenu.renderMenu = true;
      mainMenu.errorMenu.title = "Model loading failed";
      mainMenu.errorMenu.message = "File not found or unsupported format";
      models.pop_back();
    } else
    {
      mainMenu.existentModels.emplace_back(
          "Model " + std::to_string(mainMenu.modelCounter++));
      mainMenu.selectedModelIndicator.emplace_back(0);
    }
    mainMenu.loadRequest.type = LoadRequestType::NONE;
    mainMenu.loadRequest.path.clear();
  }

  renderObjectMenu = false;

  if (mainMenu.selectedMeshIndex != -1)
  {
    mainMenu.objectMenu.position =
        glm::value_ptr(meshes->meshes[mainMenu.selectedMeshIndex].position);
    mainMenu.objectMenu.velocity =
        glm::value_ptr(meshes->meshes[mainMenu.selectedMeshIndex].velocity);
    mainMenu.objectMenu.rotation =
        glm::value_ptr(meshes->meshes[mainMenu.selectedMeshIndex].angles);
    mainMenu.objectMenu.scale =
        glm::value_ptr(meshes->meshes[mainMenu.selectedMeshIndex].scale);
    mainMenu.objectMenu.color =
        glm::value_ptr(meshes->meshes[mainMenu.selectedMeshIndex].color);
    mainMenu.objectMenu.metallic =
        &meshes->meshes[mainMenu.selectedMeshIndex].metallic;
    mainMenu.objectMenu.roughness =
        &meshes->meshes[mainMenu.selectedMeshIndex].roughness;
    mainMenu.objectMenu.ao = &meshes->meshes[mainMenu.selectedMeshIndex].ao;
    mainMenu.objectMenu.reflective =
        &meshes->meshes[mainMenu.selectedMeshIndex].isReflective;
    mainMenu.objectMenu.flatReflection =
        &meshes->meshes[mainMenu.selectedMeshIndex].useFlatReflection;
    renderObjectMenu = true;

    handleRightClickedMouseEventForMeshes();

    if (mainMenu.loadRequest.type == LoadRequestType::LOAD_TEXTURE &&
        !mainMenu.loadRequest.path.empty())
    {
      meshes->meshes[mainMenu.selectedMeshIndex].applyTexture(
          mainMenu.loadRequest.path.c_str());
      if (meshes->meshes[mainMenu.selectedMeshIndex].hasTexture == false)
      {
        mainMenu.errorMenu.renderMenu = true;
        mainMenu.errorMenu.title = "Texture loading failed";
        mainMenu.errorMenu.message = "Couldnt find file or invalid image";
      }
      mainMenu.loadRequest.type = LoadRequestType::NONE;
      mainMenu.loadRequest.path.clear();
    }

    if (mainMenu.deleteObject)
    {
      auto &delMesh = meshes->meshes[mainMenu.selectedMeshIndex];
      glDeleteBuffers(1, &delMesh.vbo);
      glDeleteBuffers(1, &delMesh.ebo);
      glDeleteVertexArrays(1, &delMesh.vao);
      glDeleteTextures(1, &delMesh.texture);
      meshes->meshes.erase(meshes->meshes.begin() + mainMenu.selectedMeshIndex);
      mainMenu.existentMeshes.erase(mainMenu.existentMeshes.begin() +
                                    mainMenu.selectedMeshIndex);
      mainMenu.selectedMeshIndicator.erase(
          mainMenu.selectedMeshIndicator.begin() + mainMenu.selectedMeshIndex);
      renderObjectMenu = false;
    }
  }

  if (mainMenu.selectedModelIndex != -1)
  {
    mainMenu.objectMenu.position =
        glm::value_ptr(models[mainMenu.selectedModelIndex].position);
    mainMenu.objectMenu.velocity =
        glm::value_ptr(models[mainMenu.selectedModelIndex].velocity);
    mainMenu.objectMenu.rotation =
        glm::value_ptr(models[mainMenu.selectedModelIndex].angles);
    mainMenu.objectMenu.scale =
        glm::value_ptr(models[mainMenu.selectedModelIndex].scale);
    mainMenu.objectMenu.color =
        glm::value_ptr(models[mainMenu.selectedModelIndex].color);
    mainMenu.objectMenu.metallic =
        &models[mainMenu.selectedModelIndex].metallic;
    mainMenu.objectMenu.roughness =
        &models[mainMenu.selectedModelIndex].roughness;
    mainMenu.objectMenu.ao = &models[mainMenu.selectedModelIndex].ao;
    mainMenu.objectMenu.reflective =
        &models[mainMenu.selectedModelIndex].isReflective;
    mainMenu.objectMenu.flatReflection =
        &models[mainMenu.selectedModelIndex].useFlatReflection;
    renderObjectMenu = true;

    handleRightClickedMouseEventForModels();

    if (mainMenu.deleteModel)
    {
      auto &delModel = models[mainMenu.selectedModelIndex];
      glDeleteBuffers(1, &delModel.vbo);
      glDeleteBuffers(1, &delModel.ebo);
      glDeleteBuffers(1, &delModel.indirectBuffer);
      glDeleteBuffers(1, &delModel.materialUBO);
      glDeleteVertexArrays(1, &delModel.vao);
      glDeleteTextures(1, &delModel.diffuseTexArray);
      glDeleteTextures(1, &delModel.specularTexArray);
      glDeleteTextures(1, &delModel.normalTexArray);
      glDeleteTextures(1, &delModel.heightTexArray);
      glDeleteTextures(1, &delModel.emissiveTexArray);
      glDeleteTextures(1, &delModel.ambientOcclusionTexArray);
      glDeleteTextures(1, &delModel.metalnessTexArray);
      glDeleteTextures(1, &delModel.roughnessTexArray);
      glDeleteTextures(1, &delModel.opacityTexArray);
      glDeleteTextures(1, &delModel.shininessTexArray);
      models.erase(models.begin() + mainMenu.selectedModelIndex);
      mainMenu.existentModels.erase(mainMenu.existentModels.begin() +
                                    mainMenu.selectedModelIndex);
      mainMenu.selectedModelIndicator.erase(
          mainMenu.selectedModelIndicator.begin() +
          mainMenu.selectedModelIndex);
      mainMenu.modelCounter--;
      renderObjectMenu = false;
    }
  }

  mainMenu.deleteObject = false;

  renderLightMenu = false;

  if (mainMenu.selectedLightIndex != -1)
  {
    auto &light = lights.lights[mainMenu.selectedLightIndex];
    mainMenu.lightMenu.position = glm::value_ptr(light.position);
    mainMenu.lightMenu.direction = glm::value_ptr(light.direction);
    mainMenu.lightMenu.color = glm::value_ptr(light.color);
    mainMenu.lightMenu.diffuse = glm::value_ptr(light.diffuse);
    mainMenu.lightMenu.specular = glm::value_ptr(light.specular);
    mainMenu.lightMenu.ambient = glm::value_ptr(light.ambient);
    mainMenu.lightMenu.constant = &light.constant;
    mainMenu.lightMenu.linear = &light.linear;
    mainMenu.lightMenu.quadratic = &light.quadratic;
    mainMenu.lightMenu.cutOff = &light.cutOff;
    mainMenu.lightMenu.outerCutOff = &light.outerCutOff;
    mainMenu.lightMenu.intensity = &light.intensity;
    renderLightMenu = true;

    if (mainMenu.deleteLight)
    {
      if ((lights.lights.begin() + mainMenu.selectedLightIndex)->type ==
              LightType::POINT ||
          (lights.lights.begin() + mainMenu.selectedLightIndex)->type ==
              LightType::DIRECTIONAL ||
          (lights.lights.begin() + mainMenu.selectedLightIndex)->type ==
              LightType::SPOT)
        shaders->lightningShader.setInt("lightCount", --lightCount);
      else
        shaders->pbrLightningShader.setInt("lightCount", --pbrLightCount);

      lights.lights.erase(lights.lights.begin() + mainMenu.selectedLightIndex);

      mainMenu.existentLights.erase(mainMenu.existentLights.begin() +
                                    mainMenu.selectedLightIndex);
      mainMenu.selectedLightIndicator.erase(
          mainMenu.selectedLightIndicator.begin() +
          mainMenu.selectedLightIndex);
      renderLightMenu = false;
    }
  }

  mainMenu.objectMenu.isTerrainMenu = false;

  if (mainMenu.isTerrainSelected)
  {
    mainMenu.objectMenu.position = glm::value_ptr(terrain->mesh->position);
    mainMenu.objectMenu.velocity = glm::value_ptr(terrain->mesh->velocity);
    mainMenu.objectMenu.rotation = glm::value_ptr(terrain->mesh->angles);
    mainMenu.objectMenu.scale = glm::value_ptr(terrain->mesh->scale);
    mainMenu.objectMenu.color = glm::value_ptr(terrain->mesh->color);
    mainMenu.objectMenu.metallic = &terrain->mesh->metallic;
    mainMenu.objectMenu.roughness = &terrain->mesh->roughness;
    mainMenu.objectMenu.ao = &terrain->mesh->ao;
    mainMenu.objectMenu.reflective = &terrain->mesh->isReflective;
    mainMenu.objectMenu.flatReflection = &terrain->mesh->useFlatReflection;
    mainMenu.objectMenu.grassColor = glm::value_ptr(terrain->grassColor);
    mainMenu.objectMenu.rockColor = glm::value_ptr(terrain->rockColor);
    mainMenu.objectMenu.snowColor = glm::value_ptr(terrain->snowColor);
    renderObjectMenu = true;
    mainMenu.objectMenu.isTerrainMenu = true;
  }

  if (mainMenu.saveProperties)
  {
    std::string filename = mainMenu.fileNameBuffer;
    filename += ".sblend";
    std::ofstream outputFile(filename);
    for (Mesh &mesh : meshes->meshes)
    {
      mesh.getProperties(saveProperties);
      outputFile << saveProperties.str() << '\n';
    }
    outputFile << "LIGHTS\n";
    for (const Light &light : lights.lights)
    {
      light.getProperties(saveProperties);
      outputFile << saveProperties.str() << '\n';
    }
    outputFile << "MODELS\n";
    for (Model &model : models)
    {
      model.getProperties(saveProperties);
      outputFile << saveProperties.str() << '\n';
    }
    outputFile << "SKYBOX\n";
    skybox->getProperties(saveProperties);
    outputFile << saveProperties.str() << '\n';

    outputFile << "TERRAIN\n";
    terrain->getProperties(saveProperties);
    outputFile << saveProperties.str() << '\n';

    outputFile << "Scene\n";
    mainMenu.getProperties(saveProperties);
    outputFile << saveProperties.str() << '\n';
    outputFile << useSkybox << '\n';
    outputFile.close();
    mainMenu.saveProperties = false;
  }

  if (mainMenu.loadRequest.type == LoadRequestType::LOAD_SCENE &&
      !mainMenu.loadRequest.path.empty())
  {
    std::string fileName = mainMenu.loadRequest.path;
    std::ifstream inputFile(fileName);
    if (inputFile.is_open())
    {
      std::string ty;
      while (inputFile >> ty && ty != "LIGHTS")
      {
        MeshType meshty;
        meshty = Mesh::MeshStringMap[ty];
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        int slices, stacks;
        inputFile >> slices;
        inputFile >> stacks;
        switch (meshty)
        {
        case MeshType::PLANE:
          MeshManager::generatePlaneMesh(vertices, indices);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Plane " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;

        case MeshType::CIRCLE:
          MeshManager::generateCircleMesh(vertices, indices, slices);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Circle " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;

        case MeshType::CUBE:
          MeshManager::generateCubeMesh(vertices, indices);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Cube " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;

        case MeshType::CONE:
          MeshManager::generateConeMesh(vertices, indices, slices);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Cone " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;

        case MeshType::CYLINDER:
          MeshManager::generateCylinderMesh(vertices, indices, slices);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Cylinder " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;

        case MeshType::SPHERE:

          MeshManager::generateSphereMesh(vertices, indices, slices, stacks);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Sphere " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;

        case MeshType::TORUS:
          MeshManager::generateTorusMesh(vertices, indices, slices, stacks);
          meshes->meshes.emplace_back(vertices, indices);
          mainMenu.existentMeshes.emplace_back(
              "Torus " +
              std::to_string(
                  mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]));
          mainMenu.selectedMeshIndicator.emplace_back(0);
          mainMenu.meshTypeCounter[(int)mainMenu.selectedMesh]++;
          break;
        }
        meshes->meshes.back().type = meshty;
        inputFile >> meshes->meshes.back().position.x >>
            meshes->meshes.back().position.y >>
            meshes->meshes.back().position.z;
        inputFile >> meshes->meshes.back().velocity.x >>
            meshes->meshes.back().velocity.y >>
            meshes->meshes.back().velocity.z;
        inputFile >> meshes->meshes.back().scale.x >>
            meshes->meshes.back().scale.y >> meshes->meshes.back().scale.z;
        inputFile >> meshes->meshes.back().angles.x >>
            meshes->meshes.back().angles.y >> meshes->meshes.back().angles.z;
        inputFile >> meshes->meshes.back().color.x >>
            meshes->meshes.back().color.y >> meshes->meshes.back().color.z;
        inputFile >> meshes->meshes.back().metallic;
        inputFile >> meshes->meshes.back().roughness;
        inputFile >> meshes->meshes.back().ao;
        inputFile >> meshes->meshes.back().isReflective;
        inputFile >> meshes->meshes.back().useFlatReflection;
        meshes->meshes.back().slices = slices;
        meshes->meshes.back().stacks = stacks;
      }
      while (inputFile >> ty && ty != "MODELS")
      {
        LightType lightty;
        lightty = Light::LightStringMap[ty];
        lights.lights.emplace_back(lightty);
        if (lightty == LightType::POINT || lightty == LightType::DIRECTIONAL ||
            lightty == LightType::SPOT)
          shaders->lightningShader.setInt("lightCount", ++lightCount);
        else
          shaders->pbrLightningShader.setInt("lightCount", ++pbrLightCount);
        inputFile >> lights.lights.back().position.x >>
            lights.lights.back().position.y >> lights.lights.back().position.z;
        inputFile >> lights.lights.back().direction.x >>
            lights.lights.back().direction.y >>
            lights.lights.back().direction.z;
        inputFile >> lights.lights.back().diffuse.x >>
            lights.lights.back().diffuse.y >> lights.lights.back().diffuse.z;
        inputFile >> lights.lights.back().specular.x >>
            lights.lights.back().specular.y >> lights.lights.back().specular.z;
        inputFile >> lights.lights.back().ambient.x >>
            lights.lights.back().ambient.y >> lights.lights.back().ambient.z;
        inputFile >> lights.lights.back().color.x >>
            lights.lights.back().color.y >> lights.lights.back().color.z;
        inputFile >> lights.lights.back().linear;
        inputFile >> lights.lights.back().constant;
        inputFile >> lights.lights.back().quadratic;
        inputFile >> lights.lights.back().cutOff;
        inputFile >> lights.lights.back().outerCutOff;
        inputFile >> lights.lights.back().intensity;
        if (lightty == LightType::POINT)
          mainMenu.existentLights.emplace_back(
              "Point " +
              std::to_string(mainMenu.lightTypeCounter[(int)lightty]));
        else if (lightty == LightType::DIRECTIONAL)
          mainMenu.existentLights.emplace_back(
              "Directional " +
              std::to_string(mainMenu.lightTypeCounter[(int)lightty]));
        else if (lightty == LightType::SPOT)
          mainMenu.existentLights.emplace_back(
              "Spot " +
              std::to_string(mainMenu.lightTypeCounter[(int)lightty]));
        else
          mainMenu.existentLights.emplace_back(
              "PBR " + std::to_string(mainMenu.lightTypeCounter[(int)lightty]));
        mainMenu.selectedLightIndicator.emplace_back(0);
        mainMenu.lightTypeCounter[(int)lightty]++;
      }
      while (inputFile >> ty && ty != "SKYBOX")
      {
        models.emplace_back(ty.c_str());
        inputFile >> models.back().position.x >> models.back().position.y >>
            models.back().position.z;
        inputFile >> models.back().velocity.x >> models.back().velocity.y >>
            models.back().velocity.z;
        inputFile >> models.back().scale.x >> models.back().scale.y >>
            models.back().scale.z;
        inputFile >> models.back().angles.x >> models.back().angles.y >>
            models.back().angles.z;
        inputFile >> models.back().color.x >> models.back().color.y >>
            models.back().color.z;
        inputFile >> models.back().metallic;
        inputFile >> models.back().roughness;
        inputFile >> models.back().ao;
        inputFile >> models.back().isReflective;
        inputFile >> models.back().useFlatReflection;

        mainMenu.selectedModelIndicator.emplace_back(0);
        mainMenu.existentModels.emplace_back(
            "Model " + std::to_string(mainMenu.modelCounter));
        mainMenu.modelCounter++;
      }

      std::string skyBoxPath;
      inputFile >> skyBoxPath;
      if (skyBoxPath != "!")
      {
        size_t extensionPos = skyBoxPath.find_last_of('.');
        std::string imagesType = skyBoxPath.substr(extensionPos);
        std::string directory =
            skyBoxPath.substr(0, skyBoxPath.find_last_of("/\\") + 1);
        std::vector<std::string> faces;
        faces.push_back(skyBoxPath);

        faces.push_back(directory + "left" + imagesType);
        faces.push_back(directory + "top" + imagesType);
        faces.push_back(directory + "bottom" + imagesType);
        faces.push_back(directory + "front" + imagesType);
        faces.push_back(directory + "back" + imagesType);
        skybox->loadTexture(faces);
      }

      std::string hmPath;
      inputFile >> hmPath;
       inputFile >> hmPath;

      if(terrain->loadHeightMap(hmPath.c_str()))
      {
        mainMenu.terrainIsExistent = true;
        useTerrain = true;
      }

      inputFile >> terrain->mesh->position.x >>
            terrain->mesh->position.y >>
            terrain->mesh->position.z;
        inputFile >> terrain->mesh->velocity.x >>
            terrain->mesh->velocity.y >>
            terrain->mesh->velocity.z;
        inputFile >> terrain->mesh->scale.x >>
            terrain->mesh->scale.y >> terrain->mesh->scale.z;
        inputFile >> terrain->mesh->angles.x >>
            terrain->mesh->angles.y >> terrain->mesh->angles.z;
        inputFile >> terrain->mesh->color.x >>
            terrain->mesh->color.y >> terrain->mesh->color.z;
        inputFile >> terrain->mesh->metallic;
        inputFile >> terrain->mesh->roughness;
        inputFile >> terrain->mesh->ao;

        inputFile >> terrain->maxHeight;
        inputFile >> terrain->scale;

      std::vector<std::string> terrainPaths(3);
      inputFile >> terrainPaths[0];

      if (terrainPaths[0] != "!")
      {
        inputFile >> terrainPaths[1];
        inputFile >> terrainPaths[2];

        terrain->loadTextures(terrainPaths);
      }

      inputFile >> mainMenu.useLightShader;
      inputFile >> mainMenu.usePbrLightShader;
      inputFile >> mainMenu.castShadows;
      inputFile.close();
      useSkybox = mainMenu.setSkybox;
      mainMenu.loadRequest.type = LoadRequestType::NONE;
      mainMenu.loadRequest.path.clear();
    } else
    {
      mainMenu.errorMenu.renderMenu = true;
      mainMenu.errorMenu.title = "Scene loading failed";
      mainMenu.errorMenu.message = "File not found";
    }
  }

  if (mainMenu.loadRequest.type == LoadRequestType::LOAD_SKYBOX &&
      !mainMenu.loadRequest.path.empty())
  {
    size_t extensionPos = mainMenu.loadRequest.path.find_last_of('.');
    if (extensionPos == std::string::npos)
    {
      mainMenu.errorMenu.renderMenu = true;
      mainMenu.errorMenu.title = "Skybox images loading failed";
      mainMenu.errorMenu.message = "File not found";
    } else
    {
      std::string imagesType = mainMenu.loadRequest.path.substr(extensionPos);
      std::string directory = mainMenu.loadRequest.path.substr(
          0, mainMenu.loadRequest.path.find_last_of("/\\") + 1);
      std::vector<std::string> faces;
      faces.push_back(mainMenu.loadRequest.path);

      faces.push_back(directory + "left" + imagesType);
      faces.push_back(directory + "top" + imagesType);
      faces.push_back(directory + "bottom" + imagesType);
      faces.push_back(directory + "front" + imagesType);
      faces.push_back(directory + "back" + imagesType);
      skybox->loadTexture(faces);
      useSkybox = mainMenu.setSkybox;
    }
    mainMenu.loadRequest.type = LoadRequestType::NONE;
    mainMenu.loadRequest.path.clear();
  }

  if (mainMenu.loadRequest.type == LoadRequestType::LOAD_TERRAIN &&
      !mainMenu.loadRequest.path.empty())
  {
    terrain->maxHeight = mainMenu.terrainMaxHeight;
    terrain->scale = mainMenu.terrainScale;

    if (!terrain->loadHeightMap(mainMenu.loadRequest.path.c_str()))
    {
      mainMenu.errorMenu.renderMenu = true;
      mainMenu.errorMenu.title = "Terrain loading failed";
      mainMenu.errorMenu.message = "File not found";
    } else
      mainMenu.terrainIsExistent = true;

    mainMenu.loadRequest.path = "";
  }

  if (mainMenu.loadRequest.type == LoadRequestType::LOAD_TERRAIN_TEXTURES &&
      !mainMenu.loadRequest.path.empty())
  {
    auto imageType = mainMenu.loadRequest.path.substr(
        mainMenu.loadRequest.path.find_last_of('.'));
    auto directory = mainMenu.loadRequest.path.substr(
        0, mainMenu.loadRequest.path.find_last_of("/\\") + 1);

    std::vector<std::string> texturesPaths;
    texturesPaths.push_back(mainMenu.loadRequest.path);
    texturesPaths.push_back(directory + "rock" + imageType);
    texturesPaths.push_back(directory + "snow" + imageType);

    if (!terrain->loadTextures(texturesPaths))
    {
      mainMenu.errorMenu.renderMenu = true;
      mainMenu.errorMenu.title = "Terrain textures loading failed";
      mainMenu.errorMenu.message = "File not found";
    }
    mainMenu.loadRequest.path = "";
  }

  if (mainMenu.chosenMeshForBoolean != -1)
  {
    std::vector<Vertex> newVertices;
    std::vector<uint32_t> newIndices;
    MeshBoolean::performOperation(meshes->meshes[mainMenu.selectedMeshIndex],
                                  meshes->meshes[mainMenu.chosenMeshForBoolean],
                                  mainMenu.objectMenu.booleanOperation,
                                  newVertices, newIndices);
    if (newVertices.size() > 0 && newIndices.size() > 0)
    {
      meshes->meshes[mainMenu.selectedMeshIndex].setVertices(newVertices);
      meshes->meshes[mainMenu.selectedMeshIndex].setIndices(newIndices);
    }
    mainMenu.objectMenu.renderMeshChoiceMenu = false;
  }
}

void Application::updatePhysics() {
  physicsWorld.deltaTime = (float)glfwGetTime() - physicsWorld.deltaTime;

  if (mainMenu.simulationState)
  {
    if (mainMenu.savePositionAndVelocity)
    {
      for (Mesh &mesh : meshes->meshes)
        meshes->initialMeshesPositionsAndVelocities.push_back(
            std::make_pair(mesh.position, mesh.velocity));
      mainMenu.savePositionAndVelocity = false;
    }
    for (Mesh &mesh1 : meshes->meshes)
    {
      for (Mesh &mesh2 : meshes->meshes)
        if (&mesh1 != &mesh2)
          physicsWorld.applyMeshCollision(mesh1, mesh2);
      physicsWorld.applyDrag(mesh1);
      physicsWorld.applyGravity(mesh1, *gridLines);
    }
  }

  physicsWorld.deltaTime = (float)glfwGetTime();

  if (mainMenu.resetSimulation)
  {
    mainMenu.resetSimulation = false;
    int i = 0;
    for (auto &mesh : meshes->meshes)
    {
      mesh.position = meshes->initialMeshesPositionsAndVelocities[i].first;
      mesh.velocity = meshes->initialMeshesPositionsAndVelocities[i].second;
      ++i;
    }
  }
}

void Application::renderWithNoShadows() {
  if (windowResized)
  {
    windowResized = false;

    mainMenu.menuPos = ImVec2((float)WINDOW_WIDTH - 100.f, 0.f);
    mainMenu.menuSize =
        ImVec2((float)WINDOW_WIDTH / 10.f, (float)WINDOW_HEIGHT);
    mainMenu.objectMenu.menuPos =
        ImVec2((float)WINDOW_WIDTH - (float)WINDOW_WIDTH / 4.f, 0);
    mainMenu.objectMenu.menuSize =
        ImVec2((float)WINDOW_WIDTH / 4.f, (float)WINDOW_HEIGHT / 2.f);
    mainMenu.lightMenu.menuPos =
        ImVec2((float)WINDOW_WIDTH - (float)WINDOW_WIDTH / 4.f,
               (float)WINDOW_HEIGHT / 2.f);
    mainMenu.lightMenu.menuSize =
        ImVec2((float)WINDOW_WIDTH / 4.f, (float)WINDOW_HEIGHT / 2.f);

    mainMenu.errorMenu.position = ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    reflection->updateFrameBufferSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (useSkybox)
  {
    shaders->skyboxShader.setMat4("projection", projection);
    shaders->skyboxShader.setMat4("view",
                                  glm::mat4(glm::mat3(camera.getView())));

    skybox->render(shaders->skyboxShader);
  }

  shaders->basicShader.setMat4("projection", projection);
  shaders->basicShader.setMat4("camera.view", camera.getView());
  shaders->basicShader.setVec3("camera.position", camera.getPosition());
  shaders->basicShader.setInt("useShadows", 0);

  shaders->lightningShader.setInt("useShadows", 0);
  shaders->modelBasicShader.setInt("useShadows", 0);
  shaders->modelLightningShader.setInt("useShadows", 0);

  shaders->modelBasicShader.setMat4("projection", projection);
  shaders->modelBasicShader.setMat4("camera.view", camera.getView());
  shaders->modelBasicShader.setVec3("camera.position", camera.getPosition());
  shaders->modelBasicShader.setInt("useShadows", 0);

  shaders->terrainBasicShader.setMat4("projection", projection);
  shaders->terrainBasicShader.setMat4("camera.view", camera.getView());
  shaders->terrainBasicShader.setVec3("camera.position", camera.getPosition());
  shaders->terrainBasicShader.setInt("useShadows", 0);

  shaders->terrainLightningShader.setMat4("projection", projection);
  shaders->terrainLightningShader.setMat4("camera.view", camera.getView());
  shaders->terrainLightningShader.setVec3("camera.position",
                                          camera.getPosition());
  shaders->terrainLightningShader.setInt("useShadows", 0);

  if (!useSkybox)
  {
    grid->render(shaders->basicShader);
    gridLines->render(shaders->basicShader);
  }

  if (mainMenu.useLightShader)
  {
    reflection->bind();

    shaders->lightningShader.setMat4("projection", projection);
    shaders->lightningShader.setVec3("camera.position", camera.getPosition());
    shaders->modelLightningShader.setMat4("projection", projection);
    shaders->modelLightningShader.setVec3("camera.position",
                                          camera.getPosition());

    for (auto &rm : meshes->meshes)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->calculateReflectionViewMatrix(camera, rm);
        shaders->lightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->lightningShader);

        shaders->modelLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainLightningShader.setMat4(
            "view", glm::mat4(glm::mat3(reflectionView)));
        terrain->render(shaders->terrainLightningShader);
      }

    for (auto &rm : models)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->model_calculateReflectionViewMatrix(camera, rm);
        shaders->lightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->lightningShader);

        shaders->modelLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainLightningShader.setMat4(
            "view", glm::mat4(glm::mat3(reflectionView)));
        terrain->render(shaders->terrainLightningShader);
      }

    reflection->unbind();

    shaders->lightningShader.setMat4("projection", projection);
    shaders->lightningShader.setMat4("camera.view", camera.getView());
    shaders->lightningShader.setVec3("camera.position", camera.getPosition());

    shaders->modelLightningShader.setMat4("projection", projection);
    shaders->modelLightningShader.setMat4("camera.view", camera.getView());
    shaders->modelLightningShader.setVec3("camera.position",
                                          camera.getPosition());

    shaders->terrainLightningShader.setMat4("projection", projection);
    shaders->terrainLightningShader.setMat4("camera.view", camera.getView());
    shaders->terrainLightningShader.setVec3("camera.position",
                                            camera.getPosition());

    for (int i = 0; i < lights.lights.size(); ++i)
    {
      lights.lights[i].apply_to_shader(shaders->lightningShader, i);
      lights.lights[i].apply_to_shader(shaders->modelLightningShader, i);
      lights.lights[i].apply_to_shader(shaders->terrainLightningShader, i);
    }

    for (auto &m : meshes->meshes)
      if (!m.isReflective)
        m.render(shaders->lightningShader);

    for (auto &m : models)
      if (!m.isReflective)
        m.render(shaders->modelLightningShader);

    terrain->render(shaders->terrainLightningShader);

    if (!useSkybox)
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);
    } else
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());
    }
  } else if (mainMenu.usePbrLightShader)
  {
    reflection->bind();

    shaders->pbrLightningShader.setMat4("projection", projection);
    shaders->pbrLightningShader.setVec3("camera.position",
                                        camera.getPosition());
    shaders->modelPbrLightningShader.setMat4("projection", projection);
    shaders->modelPbrLightningShader.setVec3("camera.position",
                                             camera.getPosition());

    for (auto &rm : meshes->meshes)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->calculateReflectionViewMatrix(camera, rm);
        shaders->pbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->pbrLightningShader);

        shaders->modelPbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelPbrLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainPbrLightningShader.setMat4(
            "view", glm::mat4(glm::mat3(reflectionView)));
        terrain->render(shaders->terrainPbrLightningShader);
      }

    for (auto &rm : models)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->model_calculateReflectionViewMatrix(camera, rm);
        shaders->pbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->pbrLightningShader);

        shaders->modelPbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelPbrLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainPbrLightningShader.setMat4(
            "view", glm::mat4(glm::mat3(reflectionView)));
        terrain->render(shaders->terrainPbrLightningShader);
      }

    reflection->unbind();

    shaders->pbrLightningShader.setMat4("projection", projection);
    shaders->pbrLightningShader.setMat4("camera.view", camera.getView());
    shaders->pbrLightningShader.setVec3("camera.position",
                                        camera.getPosition());

    shaders->modelPbrLightningShader.setMat4("projection", projection);
    shaders->modelPbrLightningShader.setMat4("camera.view", camera.getView());
    shaders->modelPbrLightningShader.setVec3("camera.position",
                                             camera.getPosition());

    shaders->terrainPbrLightningShader.setMat4("projection", projection);
    shaders->terrainPbrLightningShader.setMat4("camera.view", camera.getView());
    shaders->terrainPbrLightningShader.setVec3("camera.position",
                                               camera.getPosition());

    for (int i = 0; i < lights.lights.size(); ++i)
    {
      lights.lights[i].apply_to_shader(shaders->pbrLightningShader, i);
      lights.lights[i].apply_to_shader(shaders->modelPbrLightningShader, i);
      lights.lights[i].apply_to_shader(shaders->terrainPbrLightningShader, i);
    }

    for (auto &m : meshes->meshes)
      if (!m.isReflective)
        m.render(shaders->pbrLightningShader);
    for (auto &m : models)
      if (!m.isReflective)
        m.render(shaders->modelPbrLightningShader);

    terrain->render(shaders->terrainPbrLightningShader);

    if (!useSkybox)
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);
    } else
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());
    }
  } else
  {
    reflection->bind();

    shaders->basicShader.setMat4("projection", projection);
    shaders->basicShader.setVec3("camera.position", camera.getPosition());

    shaders->modelBasicShader.setMat4("projection", projection);
    shaders->modelBasicShader.setVec3("camera.position", camera.getPosition());

    shaders->terrainBasicShader.setMat4("projection", projection);
    shaders->terrainBasicShader.setVec3("camera.position",
                                        camera.getPosition());

    for (auto &rm : meshes->meshes)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->calculateReflectionViewMatrix(camera, rm);
        shaders->basicShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->basicShader);

        shaders->modelBasicShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          m.render(shaders->modelBasicShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainBasicShader.setMat4(
            "view", glm::mat4(glm::mat3(reflectionView)));
        terrain->render(shaders->terrainBasicShader);
      }

    for (auto &rm : models)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->model_calculateReflectionViewMatrix(camera, rm);
        shaders->basicShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->basicShader);

        shaders->modelBasicShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelBasicShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainBasicShader.setMat4(
            "view", glm::mat4(glm::mat3(reflectionView)));
        terrain->render(shaders->terrainBasicShader);
      }

    reflection->unbind();

    shaders->basicShader.use();
    shaders->basicShader.setMat4("projection", projection);
    shaders->basicShader.setMat4("camera.view", camera.getView());

    shaders->modelBasicShader.use();
    shaders->modelBasicShader.setMat4("projection", projection);
    shaders->modelBasicShader.setMat4("camera.view", camera.getView());

    shaders->terrainBasicShader.use();
    shaders->terrainBasicShader.setMat4("projection", projection);
    shaders->terrainBasicShader.setMat4("camera.view", camera.getView());

    for (auto &m : meshes->meshes)
      if (!m.isReflective)
        m.render(shaders->basicShader);

    for (auto &m : models)
      if (!m.isReflective)
        m.render(shaders->modelBasicShader);

    terrain->render(shaders->terrainBasicShader);

    if (!useSkybox)
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);
    } else
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());
    }
  }

  newMenuFrame();

  mainMenu.render();

  if (renderObjectMenu)
    mainMenu.objectMenu.render();

  if (renderLightMenu)
    mainMenu.lightMenu.render();

  if (mainMenu.loadRequest.type != LoadRequestType::NONE)
  {
    mainMenu.fileDialog.Display();

    if (mainMenu.fileDialog.HasSelected())
    {
      mainMenu.loadRequest.path = mainMenu.fileDialog.GetSelected().string();
      mainMenu.fileDialog.ClearSelected();
    }
  }

  renderMenuFrame();

  glfwSwapBuffers(window);
}

void Application::renderWithShadows() {
  if (lights.lights.empty())
  {
    renderWithNoShadows();
    return;
  }

  if (windowResized)
  {
    windowResized = false;

    mainMenu.menuPos = ImVec2((float)WINDOW_WIDTH - 100.f, 0.f);
    mainMenu.menuSize =
        ImVec2((float)WINDOW_WIDTH / 10.f, (float)WINDOW_HEIGHT);
    mainMenu.objectMenu.menuPos =
        ImVec2((float)WINDOW_WIDTH - (float)WINDOW_WIDTH / 4.f, 0);
    mainMenu.objectMenu.menuSize =
        ImVec2((float)WINDOW_WIDTH / 4.f, (float)WINDOW_HEIGHT / 2.f);
    mainMenu.lightMenu.menuPos =
        ImVec2((float)WINDOW_WIDTH - (float)WINDOW_WIDTH / 4.f,
               (float)WINDOW_HEIGHT / 2.f);
    mainMenu.lightMenu.menuSize =
        ImVec2((float)WINDOW_WIDTH / 4.f, (float)WINDOW_HEIGHT / 2.f);

    mainMenu.errorMenu.position = ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    reflection->updateFrameBufferSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  }

  if (useSkybox)
  {
    shaders->skyboxShader.setMat4("projection", projection);
    shaders->skyboxShader.setMat4("view",
                                  glm::mat4(glm::mat3(camera.getView())));

    skybox->render(shaders->skyboxShader);
  }

  shaders->basicShader.setInt("useShadows", 1);

  shaders->lightningShader.setInt("useShadows", 1);

  shaders->pbrLightningShader.setInt("useShadows", 1);

  shaders->modelLightningShader.setInt("useShadows", 1);

  shaders->modelPbrLightningShader.setInt("useShadows", 1);

  shaders->terrainLightningShader.setInt("useShadows", 1);

  shaders->terrainPbrLightningShader.setInt("useShadows", 1);

  shadow->lightView = glm::lookAt(lights.lights.front().position,
                                  glm::vec3(0.f), glm::vec3(0.0f, 1.0f, 0.0f));

  shadow->shadowMode(true, WINDOW_WIDTH, WINDOW_HEIGHT);

  for (Mesh &mesh : meshes->meshes)
    shadow->apply_to_mesh(mesh);

  for (Model &model : models)
    shadow->apply_to_model(model);

  if (useTerrain)
    shadow->apply_to_mesh(*terrain->mesh);

  shadow->shadowMode(false, WINDOW_WIDTH, WINDOW_HEIGHT);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaders->basicShader.setMat4("projection", projection);
  shaders->basicShader.setMat4("camera.view", camera.getView());
  shaders->basicShader.setVec3("camera.position", camera.getPosition());

  shaders->modelBasicShader.setMat4("projection", projection);
  shaders->modelBasicShader.setMat4("camera.view", camera.getView());
  shaders->modelBasicShader.setVec3("camera.position", camera.getPosition());

  shadow->apply_shadow_texture(shaders->basicShader);

  shadow->apply_shadow_texture(shaders->lightningShader);

  shadow->apply_shadow_texture(shaders->pbrLightningShader);

  shadow->apply_shadow_texture(shaders->modelLightningShader);

  shadow->apply_shadow_texture(shaders->modelPbrLightningShader);

  shadow->apply_shadow_texture(shaders->terrainLightningShader);

  shadow->apply_shadow_texture(shaders->terrainPbrLightningShader);

  grid->render(shaders->basicShader);
  gridLines->render(shaders->basicShader);

  if (mainMenu.useLightShader)
  {
    reflection->bind();

    shaders->lightningShader.setMat4("projection", projection);
    shaders->lightningShader.setVec3("camera.position", camera.getPosition());
    shaders->modelLightningShader.setMat4("projection", projection);
    shaders->modelLightningShader.setVec3("camera.position",
                                          camera.getPosition());

    shaders->terrainLightningShader.setMat4("projection", projection);
    shaders->terrainLightningShader.setVec3("camera.position",
                                            camera.getPosition());

    for (auto &rm : meshes->meshes)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->calculateReflectionViewMatrix(camera, rm);
        shaders->lightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->lightningShader);

        shaders->modelLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          m.render(shaders->modelLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainLightningShader.setMat4("camera.view", reflectionView);
        terrain->render(shaders->terrainLightningShader);
      }

    for (auto &rm : models)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->model_calculateReflectionViewMatrix(camera, rm);
        shaders->lightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->lightningShader);

        shaders->modelLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainLightningShader.setMat4("camera.view", reflectionView);
        terrain->render(shaders->terrainLightningShader);
      }

    reflection->unbind();

    shaders->lightningShader.setMat4("projection", projection);
    shaders->lightningShader.setMat4("camera.view", camera.getView());
    shaders->lightningShader.setVec3("camera.position", camera.getPosition());

    shaders->modelLightningShader.setMat4("projection", projection);
    shaders->modelLightningShader.setMat4("camera.view", camera.getView());
    shaders->modelLightningShader.setVec3("camera.position",
                                          camera.getPosition());

    shaders->terrainLightningShader.setMat4("projection", projection);
    shaders->terrainLightningShader.setMat4("camera.view", camera.getView());
    shaders->terrainLightningShader.setVec3("camera.position",
                                            camera.getPosition());

    for (int i = 0; i < lights.lights.size(); ++i)
    {
      lights.lights[i].apply_to_shader(shaders->lightningShader, i);
      lights.lights[i].apply_to_shader(shaders->modelLightningShader, i);
      lights.lights[i].apply_to_shader(shaders->terrainLightningShader, i);
    }

    for (auto &m : meshes->meshes)
      if (!m.isReflective)
        m.render(shaders->lightningShader);
    for (auto &m : models)
      if (!m.isReflective)
        m.render(shaders->modelLightningShader);

    terrain->render(shaders->terrainLightningShader);

    if (!useSkybox)
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);
    } else
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());
    }
  } else if (mainMenu.usePbrLightShader)
  {
    reflection->bind();

    shaders->pbrLightningShader.setMat4("projection", projection);
    shaders->pbrLightningShader.setVec3("camera.position",
                                        camera.getPosition());
    shaders->modelPbrLightningShader.setMat4("projection", projection);
    shaders->modelPbrLightningShader.setVec3("camera.position",
                                             camera.getPosition());

    shaders->terrainPbrLightningShader.setMat4("projection", projection);
    shaders->terrainPbrLightningShader.setVec3("camera.position",
                                               camera.getPosition());

    for (auto &rm : meshes->meshes)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->calculateReflectionViewMatrix(camera, rm);
        shaders->lightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->pbrLightningShader);

        shaders->modelPbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          m.render(shaders->modelPbrLightningShader);

        skybox->render(shaders->skyboxShader);

        shaders->terrainPbrLightningShader.setMat4("camera.view",
                                                   reflectionView);
        terrain->render(shaders->terrainPbrLightningShader);
      }

    for (auto &rm : models)
      if (rm.isReflective)
      {
        glm::mat4 reflectionView =
            reflection->model_calculateReflectionViewMatrix(camera, rm);
        shaders->pbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : meshes->meshes)
          if (!m.isReflective)
            m.render(shaders->pbrLightningShader);

        shaders->modelPbrLightningShader.setMat4("camera.view", reflectionView);
        for (auto &m : models)
          if (!m.isReflective)
            m.render(shaders->modelPbrLightningShader);

        shaders->skyboxShader.setMat4("view",
                                      glm::mat4(glm::mat3(reflectionView)));
        skybox->render(shaders->skyboxShader);

        shaders->terrainPbrLightningShader.setMat4("camera.view",
                                                   reflectionView);
        terrain->render(shaders->terrainPbrLightningShader);
      }

    reflection->unbind();

    shaders->pbrLightningShader.setMat4("projection", projection);
    shaders->pbrLightningShader.setMat4("camera.view", camera.getView());
    shaders->pbrLightningShader.setVec3("camera.position",
                                        camera.getPosition());

    shaders->modelPbrLightningShader.setMat4("projection", projection);
    shaders->modelPbrLightningShader.setMat4("camera.view", camera.getView());
    shaders->modelPbrLightningShader.setVec3("camera.position",
                                             camera.getPosition());

    shaders->terrainPbrLightningShader.setMat4("projection", projection);
    shaders->terrainPbrLightningShader.setMat4("camera.view", camera.getView());
    shaders->terrainPbrLightningShader.setVec3("camera.position",
                                               camera.getPosition());

    for (int i = 0; i < lights.lights.size(); ++i)
    {
      lights.lights[i].apply_to_shader(shaders->pbrLightningShader, i);
      lights.lights[i].apply_to_shader(shaders->modelPbrLightningShader, i);
      lights.lights[i].apply_to_shader(shaders->terrainPbrLightningShader, i);
    }

    for (auto &m : meshes->meshes)
      if (!m.isReflective)
        m.render(shaders->pbrLightningShader);
    for (auto &m : models)
      if (!m.isReflective)
        m.render(shaders->modelPbrLightningShader);

    terrain->render(shaders->terrainPbrLightningShader);

    if (!useSkybox)
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection);
    } else
    {
      for (auto &m : meshes->meshes)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());

      for (auto &m : models)
        if (m.isReflective)
          reflection->renderSurface(m, camera, projection,
                                    skybox->getTextureID());
    }
  }

  newMenuFrame();

  mainMenu.render();

  if (renderObjectMenu)
    mainMenu.objectMenu.render();

  if (renderLightMenu)
    mainMenu.lightMenu.render();

  if (mainMenu.loadRequest.type != LoadRequestType::NONE)
  {
    mainMenu.fileDialog.Display();

    if (mainMenu.fileDialog.HasSelected())
    {
      mainMenu.loadRequest.path = mainMenu.fileDialog.GetSelected().string();
      mainMenu.fileDialog.ClearSelected();
    }
  }

  renderMenuFrame();

  glfwSwapBuffers(window);
}

void Application::handleRightClickedMouseEventForMeshes() {
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
  {
    double newMouseX, newMousey;
    glfwGetCursorPos(window, &newMouseX, &newMousey);

    float xOffset = (float)newMouseX - (float)mouseX;
    float yOffset = (float)newMousey - (float)mouseY;

    xOffset *= 0.1f;
    yOffset *= 0.1f;

    const float TRANSLATION_SENSITIVITY = 0.07f;
    const float SCALE_SENSITIVITY = 0.05f;
    const float ROTATION_SENSITIVITY = 0.5f;

    if (mainMenu.objectMenu.translateObjectWithMouse)
    {
      meshes->meshes[mainMenu.selectedMeshIndex].position.y +=
          -yOffset * TRANSLATION_SENSITIVITY;

      if (camera.getPosition().z > 0.f)
        meshes->meshes[mainMenu.selectedMeshIndex].position.x +=
            xOffset * TRANSLATION_SENSITIVITY;
      else
        meshes->meshes[mainMenu.selectedMeshIndex].position.z +=
            xOffset * TRANSLATION_SENSITIVITY;
    } else if (mainMenu.objectMenu.scaleObjectWithMouse)
    {
      meshes->meshes[mainMenu.selectedMeshIndex].scale.y +=
          yOffset * SCALE_SENSITIVITY;

      if (camera.getPosition().z > 0.f)
        meshes->meshes[mainMenu.selectedMeshIndex].scale.x +=
            xOffset * SCALE_SENSITIVITY;
      else
        meshes->meshes[mainMenu.selectedMeshIndex].scale.z +=
            xOffset * SCALE_SENSITIVITY;
    } else if (mainMenu.objectMenu.rotateObjectWithMouse)
    {
      meshes->meshes[mainMenu.selectedMeshIndex].angles.y += xOffset * 10.f;

      if (camera.getPosition().z > 0.f)
        meshes->meshes[mainMenu.selectedMeshIndex].angles.x += yOffset * 10.f;
      else
        meshes->meshes[mainMenu.selectedMeshIndex].angles.z += yOffset * 10.f;
    }
  }
}

void Application::handleRightClickedMouseEventForModels() {
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
  {
    double newMouseX, newMousey;
    glfwGetCursorPos(window, &newMouseX, &newMousey);

    float xOffset = (float)newMouseX - (float)mouseX;
    float yOffset = (float)newMousey - (float)mouseY;

    xOffset *= 0.01f;
    yOffset *= 0.01f;

    if (mainMenu.objectMenu.translateObjectWithMouse)
    {
      models[mainMenu.selectedModelIndex].position.y += -yOffset;

      if (camera.getPosition().z > 0.f)
        models[mainMenu.selectedModelIndex].position.x += xOffset;
      else
        models[mainMenu.selectedModelIndex].position.z += xOffset;
    } else if (mainMenu.objectMenu.scaleObjectWithMouse)
    {
      models[mainMenu.selectedModelIndex].scale.y += -yOffset;

      if (camera.getPosition().z > 0.f)
        models[mainMenu.selectedModelIndex].scale.x += xOffset;
      else
        models[mainMenu.selectedModelIndex].scale.z += xOffset;
    } else if (mainMenu.objectMenu.rotateObjectWithMouse)
    {
      models[mainMenu.selectedModelIndex].angles.y += xOffset * 100.f;

      if (camera.getPosition().z > 0.f)
        models[mainMenu.selectedModelIndex].angles.x += yOffset * 100.f;
      else
        models[mainMenu.selectedModelIndex].angles.z += yOffset * 100.f;
    }
  }
}

void Application::handleLeftClickedMouseEvent() {
  leftMouseButtonPressedThisFrame =
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;

  if (leftMouseButtonPressedThisFrame &&
      !leftMouseButtonPressedBeforeThisFrame &&
      (!meshes->meshes.empty() || !models.empty()))
  {
    const auto &view = camera.getView();
    auto viewport = glm::vec4(0.f, 0.f, WINDOW_WIDTH, WINDOW_HEIGHT);

    auto winCoordNear =
        glm::vec3((float)mouseX, (float)WINDOW_HEIGHT - (float)mouseY, 0.f);
    auto winCoordFar =
        glm::vec3((float)mouseX, (float)WINDOW_HEIGHT - (float)mouseY, 1.f);

    auto worldCoordNear =
        glm::unProject(winCoordNear, view, projection, viewport);
    auto worldCoordFar =
        glm::unProject(winCoordFar, view, projection, viewport);

    auto rayDirection = glm::normalize(worldCoordFar - worldCoordNear);
    glm::vec3 rayOrigin = worldCoordNear;

    int i = 0;
    for (auto &m : meshes->meshes)
    {
      const glm::mat4 &model = m.getModel();

      if (m.type == MeshType::CONE || m.type == MeshType::CYLINDER ||
          m.type == MeshType::SPHERE || m.type == MeshType::TORUS)
      {
        auto sphereCenterWorld =
            glm::vec3(model * glm::vec4(m.boundingSphereCenter, 1.f));

        auto maxScale = glm::max(glm::max(m.scale.x, m.scale.y), m.scale.z);

        float sphereRadiusWorld = m.boundingSphereRadius * maxScale;

        float outDist;

        if (glm::intersectRaySphere(rayOrigin, rayDirection, sphereCenterWorld,
                                    sphereRadiusWorld, outDist))
        {
          mainMenu.selectedMeshIndicator[i] = 1;
          return;
        }
      } else
      {
        const auto &vertices = m.getVertices();
        const auto &indices = m.getIndices();

        for (size_t j = 0; j < indices.size(); j += 3)
        {
          glm::vec3 v0_local = vertices[indices[j]].position;
          glm::vec3 v1_local = vertices[indices[j + 1]].position;
          glm::vec3 v2_local = vertices[indices[j + 2]].position;

          glm::vec3 v0_world = glm::vec3(model * glm::vec4(v0_local, 1.0f));
          glm::vec3 v1_world = glm::vec3(model * glm::vec4(v1_local, 1.0f));
          glm::vec3 v2_world = glm::vec3(model * glm::vec4(v2_local, 1.0f));

          glm::vec2 barycentricCoords;
          float distance;

          if (glm::intersectRayTriangle(rayOrigin, rayDirection, v0_world,
                                        v1_world, v2_world, barycentricCoords,
                                        distance))
          {
            mainMenu.selectedMeshIndicator[i] = 1;
            return;
          }
        }
      }
      ++i;
    }

    i = 0;

    for (auto &m : models)
    {
      const auto &model = m.getModel();

      auto sphereCenterWorld =
          glm::vec3(model * glm::vec4(m.boundingSphereCenter, 1.f));

      auto maxScale = glm::max(glm::max(m.scale.x, m.scale.y), m.scale.z);

      float sphereRadiusWorld = m.boundingSphereRadius * maxScale;

      float outDist;

      if (glm::intersectRaySphere(rayOrigin, rayDirection, sphereCenterWorld,
                                  sphereRadiusWorld, outDist))
      {
        mainMenu.selectedModelIndicator[i] = 1;
        return;
      }
      ++i;
    }
  }
  leftMouseButtonPressedBeforeThisFrame = leftMouseButtonPressedThisFrame;
}

Application::Application() : camera(window, glm::vec3(0.f, 2.f, 3.f), 2.f) {
  if (!glfwInit())
    throw std::runtime_error("GLFW init failed");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "sBlend", nullptr, nullptr);

  if (!window)
    throw std::runtime_error("Window init failed");

  camera.update(window);

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("GLAD init failed");

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetScrollCallback(window, scrollCallback);

  appIcon.pixels =
      stbi_load("appIcon.png", &appIcon.width, &appIcon.height, nullptr, 0);

  if (appIcon.pixels == nullptr)
    throw std::runtime_error("Failed to load app icon");

  glfwSetWindowIcon(window, 1, &appIcon);

  ImGui::CreateContext();
  imGuiIo = &ImGui::GetIO();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  ImGui::StyleColorsDark();

  meshes = MeshManager::getMeshManager();

  shaders = ShaderManager::getShaderManager();

  shadow = new Shadow(shaders->shadowMapShader);

  reflection = new Reflection(shaders->reflectionShader);

  shadow->lightProjection = glm::ortho(-5.f, 5.f, -5.f, 5.f, 0.1f, 20.f);

  projection =
      glm::perspective(glm::radians(fov),
                       (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);

  mainMenu.menuPos = ImVec2((float)WINDOW_WIDTH - 100.f, 0.f);
  mainMenu.menuSize = ImVec2((float)WINDOW_WIDTH / 10.f, (float)WINDOW_HEIGHT);
  mainMenu.objectMenu.menuPos =
      ImVec2((float)WINDOW_WIDTH - (float)WINDOW_WIDTH / 4.f, 0);
  mainMenu.objectMenu.menuSize =
      ImVec2((float)WINDOW_WIDTH / 4.f, (float)WINDOW_HEIGHT / 2.f);
  mainMenu.lightMenu.menuPos =
      ImVec2((float)WINDOW_WIDTH - (float)WINDOW_WIDTH / 4.f,
             (float)WINDOW_HEIGHT / 2.f);
  mainMenu.lightMenu.menuSize =
      ImVec2((float)WINDOW_WIDTH / 4.f, (float)WINDOW_HEIGHT / 2.f);
  renderLightMenu = renderObjectMenu = false;

  mainMenu.errorMenu.position = ImVec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  MeshManager::generatePlaneMesh(vertices, indices);

  grid = new Mesh(vertices, indices);
  grid->scale = glm::vec3(100.f, 0.f, 100.f);
  grid->color = glm::vec3(0.2f);
  grid->position.y = -1.f;

  std::vector<Vertex>().swap(vertices);
  std::vector<uint32_t>().swap(indices);

  MeshManager::generateGridLines(vertices, indices);

  gridLines = new Mesh(vertices, indices, GL_LINES);
  gridLines->color = glm::vec3(0.f);
  gridLines->position = glm::vec3(0.f, -0.8f, 0.f);

  physicsWorld.gravity = glm::vec3(0.f, -9.81f, 0.f);
  physicsWorld.deltaTime = (float)glfwGetTime();
  physicsWorld.setGrid(*grid);

  std::vector<Vertex>().swap(vertices);
  std::vector<uint32_t>().swap(indices);

  MeshManager::generateCubeMesh(vertices, indices);

  skybox = new Skybox(vertices, indices);

  terrain = new Terrain();

  Mesh::MeshTypesMap.emplace(MeshType::PLANE, "PLANE");
  Mesh::MeshTypesMap.emplace(MeshType::CIRCLE, "CIRCLE");
  Mesh::MeshTypesMap.emplace(MeshType::CUBE, "CUBE");
  Mesh::MeshTypesMap.emplace(MeshType::CONE, "CONE");
  Mesh::MeshTypesMap.emplace(MeshType::CYLINDER, "CYLINDER");
  Mesh::MeshTypesMap.emplace(MeshType::SPHERE, "SPHERE");
  Mesh::MeshTypesMap.emplace(MeshType::TORUS, "TORUS");

  Mesh::MeshStringMap.emplace("PLANE", MeshType::PLANE);
  Mesh::MeshStringMap.emplace("CIRCLE", MeshType::CIRCLE);
  Mesh::MeshStringMap.emplace("CUBE", MeshType::CUBE);
  Mesh::MeshStringMap.emplace("CONE", MeshType::CONE);
  Mesh::MeshStringMap.emplace("CYLINDER", MeshType::CYLINDER);
  Mesh::MeshStringMap.emplace("SPHERE", MeshType::SPHERE);
  Mesh::MeshStringMap.emplace("TORUS", MeshType::TORUS);
}

void Application::run() {
  while (!glfwWindowShouldClose(window))
  {
    if (!mainMenu.castShadows)
      renderWithNoShadows();
    else
      renderWithShadows();

    updatePhysics();

    glfwPollEvents();

    camera.update(window);

    updateMenuState();

    glfwGetCursorPos(window, &mouseX, &mouseY);

    handleLeftClickedMouseEvent();
  }
}

Application *Application::getApplication() {
  if (application == nullptr)
    application = new Application();

  return application;
}

void Application::terminateApplication(Application *&application) {
  delete application->terrain;

  delete application->skybox;

  delete application->reflection;

  delete application->shadow;

  delete application->grid;
  delete application->gridLines;

  delete application->shaders;

  for (auto &m : application->meshes->meshes)
  {
    glDeleteBuffers(1, &m.vbo);
    glDeleteBuffers(1, &m.ebo);
    glDeleteVertexArrays(1, &m.vao);
    glDeleteTextures(1, &m.texture);
  }

  for (auto &m : application->models)
  {
    glDeleteBuffers(1, &m.vbo);
    glDeleteBuffers(1, &m.ebo);
    glDeleteBuffers(1, &m.materialUBO);
    glDeleteBuffers(1, &m.indirectBuffer);
    glDeleteVertexArrays(1, &m.vao);
    glDeleteTextures(1, &m.diffuseTexArray);
    glDeleteTextures(1, &m.specularTexArray);
    glDeleteTextures(1, &m.normalTexArray);
    glDeleteTextures(1, &m.heightTexArray);
    glDeleteTextures(1, &m.emissiveTexArray);
    glDeleteTextures(1, &m.ambientOcclusionTexArray);
    glDeleteTextures(1, &m.metalnessTexArray);
    glDeleteTextures(1, &m.roughnessTexArray);
    glDeleteTextures(1, &m.opacityTexArray);
    glDeleteTextures(1, &m.shininessTexArray);
  }

  delete application->meshes;

  stbi_image_free(application->appIcon.pixels);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(application->window);
  glfwTerminate();
  delete application;
  application = nullptr;
}
} // namespace sx
