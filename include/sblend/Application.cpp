#include "Application.h"
#include <stdexcept>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

namespace sx
{

  int Application::WINDOW_WIDTH = 600;
  int Application::WINDOW_HEIGHT = 600;

  glm::mat4 Application::projection;

  std::ostringstream saveProperties;

  float Application::fov = 45.f;

  bool Application::windowResized = false;

  bool Application::swapInterval = true;

  Application *Application::application = nullptr;

  void Application::keyCallback(Window window, int key, int scancode, int action,
                                int mods)
  {
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

  void Application::resizeCallback(Window window, int width, int height)
  {
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

  void Application::scrollCallback(Window window, double x, double y)
  {
    if (fov < 15.f)
      fov = 15.f;
    else if (fov > 120.f)
      fov = 120.f;
    fov += -(float)y * 2.5f;
    projection =
        glm::perspective(glm::radians(fov),
                         (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);
  }

  void Application::newMenuFrame()
  {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
  }

  void Application::renderMenuFrame()
  {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

  void Application::updateMenuState()
  {
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
        mainMenu.selectedMesh = MeshType::NaN;
        break;

      case MeshType::CIRCLE:
        if (mainMenu.generate)
        {
          MeshManager::generateCircleMesh(vertices, indices, mainMenu.segments);
          meshes->meshes.emplace_back(vertices, indices);
          meshes->meshes.back().type = mainMenu.selectedMesh;
          meshes->meshes.back().slices = mainMenu.segments;
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
        mainMenu.selectedMesh = MeshType::NaN;
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
        break;

      case LightType::PBR:
        lights.lights.emplace_back(LightType::PBR);
        mainMenu.selectedLight = LightType::NaN;
        shaders->pbrLightningShader.setInt("lightCount", ++pbrLightCount);
        shaders->modelPbrLightningShader.setInt("lightCount", ++pbrLightCount);
        break;
      }
    }

    if (mainMenu.loadModel)
    {
      models.emplace_back(mainMenu.modelPath);
      mainMenu.loadModel = false;
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
      renderObjectMenu = true;

      if (mainMenu.setTexture)
      {
        meshes->meshes[mainMenu.selectedMeshIndex].applyTexture(
            ("../../textures/" + std::string(mainMenu.texturePath)).c_str());
        mainMenu.setTexture = false;
      }

      if (mainMenu.deleteObject)
      {
        auto &delMesh = meshes->meshes[mainMenu.selectedMeshIndex];
        glDeleteBuffers(1, &delMesh.vbo);
        glDeleteBuffers(1, &delMesh.ebo);
        glDeleteVertexArrays(1, &delMesh.vao);
        glDeleteTextures(1, &delMesh.texture);
        meshes->meshes.erase(meshes->meshes.begin() + mainMenu.selectedMeshIndex);
        meshes->initialMeshesPositionsAndVelocities.erase(
            meshes->initialMeshesPositionsAndVelocities.begin() +
            mainMenu.selectedMeshIndex);
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
      renderObjectMenu = true;

      if (mainMenu.deleteModel)
      {
        auto &delModel = models[mainMenu.selectedModelIndex];
        for (auto &delMesh : delModel.meshes)
        {
          glDeleteBuffers(1, &delMesh.vbo);
          glDeleteBuffers(1, &delMesh.ebo);
          glDeleteVertexArrays(1, &delMesh.vao);
          for (auto &tex : delMesh.textures)
            glDeleteTextures(1, &tex.id);
        }
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
      mainMenu.lightMenu.color = glm::value_ptr(light.color);
      mainMenu.lightMenu.diffuse = glm::value_ptr(light.diffuse);
      mainMenu.lightMenu.specular = glm::value_ptr(light.specular);
      mainMenu.lightMenu.ambient = glm::value_ptr(light.ambient);
      mainMenu.lightMenu.constant = &light.constant;
      mainMenu.lightMenu.linear = &light.linear;
      mainMenu.lightMenu.quadratic = &light.quadratic;
      renderLightMenu = true;

      if (mainMenu.deleteLight)
      {
        if ((lights.lights.begin() + mainMenu.selectedLightIndex)->type ==
            LightType::POINT)
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
      outputFile << "Scene\n";
      mainMenu.getProperties(saveProperties);
      outputFile << saveProperties.str() << '\n';
      outputFile.close();
      mainMenu.saveProperties = false;
    }

    if (mainMenu.loadProperties)
    {
      std::string fileName = mainMenu.fileNameBuffer;
      fileName += ".sblend";
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
          meshes->meshes.back().slices = slices;
          meshes->meshes.back().stacks = stacks;
        }
        while (inputFile >> ty && ty != "Scene")
        {
          LightType lightty;
          lightty = Light::LightStringMap[ty];
          lights.lights.emplace_back(lightty);
          if (lightty == LightType::POINT)
            shaders->lightningShader.setInt("lightCount", ++lightCount);
          else
            shaders->pbrLightningShader.setInt("lightCount", ++pbrLightCount);
          inputFile >> lights.lights.back().position.x >>
              lights.lights.back().position.y >> lights.lights.back().position.z;
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
          if (lightty == LightType::POINT)
            mainMenu.existentLights.emplace_back(
                "Point Light " +
                std::to_string(mainMenu.lightTypeCounter[(int)lightty]));
          else
            mainMenu.existentLights.emplace_back(
                "PBR Light " +
                std::to_string(mainMenu.lightTypeCounter[(int)lightty]));
          mainMenu.selectedLightIndicator.emplace_back(0);
          mainMenu.lightTypeCounter[(int)lightty]++;
        }
        inputFile >> mainMenu.useLightShader;
        inputFile >> mainMenu.usePbrLightShader;
        inputFile >> mainMenu.castShadows;
        inputFile.close();
      }
      mainMenu.loadProperties = false;
    }
  }

  void Application::updatePhysics()
  {
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

  void Application::renderWithNoShadows()
  {
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
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    grid->render(shaders->basicShader);
    gridLines->render(shaders->basicShader);

    if (mainMenu.useLightShader)
    {
      shaders->lightningShader.setMat4("projection", projection);
      shaders->lightningShader.setMat4("camera.view", camera.getView());
      shaders->lightningShader.setVec3("camera.position", camera.getPosition());

      shaders->modelLightningShader.setMat4("projection", projection);
      shaders->modelLightningShader.setMat4("camera.view", camera.getView());
      shaders->modelLightningShader.setVec3("camera.position", camera.getPosition());

      for (int i = 0; i < lights.lights.size(); ++i)
      {
        lights.lights[i].apply_to_shader(shaders->lightningShader, i);
        lights.lights[i].apply_to_shader(shaders->modelLightningShader, i);
      }

      for (auto &m : meshes->meshes)
        m.render(shaders->lightningShader);
      for (auto &m : models)
        m.render(shaders->modelLightningShader);
    }
    else if (mainMenu.usePbrLightShader)
    {
      shaders->pbrLightningShader.setMat4("projection", projection);
      shaders->pbrLightningShader.setMat4("camera.view", camera.getView());
      shaders->pbrLightningShader.setVec3("camera.position",
                                          camera.getPosition());

      shaders->modelPbrLightningShader.setMat4("projection", projection);
      shaders->modelPbrLightningShader.setMat4("camera.view", camera.getView());
      shaders->modelPbrLightningShader.setVec3("camera.position",
                                               camera.getPosition());

      for (int i = 0; i < lights.lights.size(); ++i)
      {
        lights.lights[i].apply_to_shader(shaders->pbrLightningShader, i);
        lights.lights[i].apply_to_shader(shaders->modelPbrLightningShader, i);
      }

      for (auto &m : meshes->meshes)
        m.render(shaders->pbrLightningShader);
      for (auto &m : models)
        m.render(shaders->modelPbrLightningShader);
    }
    else
    {
      for (auto &m : meshes->meshes)
        m.render(shaders->basicShader);
      for (auto &m : models)
        m.render(shaders->modelBasicShader);
    }

    newMenuFrame();

    mainMenu.render();

    if (renderObjectMenu)
      mainMenu.objectMenu.render();

    if (renderLightMenu)
      mainMenu.lightMenu.render();

    renderMenuFrame();

    glfwSwapBuffers(window);
  }

  void Application::renderWithShadows()
  {
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
    }

    shaders->basicShader.setInt("useShadows", 1);

    shaders->lightningShader.setInt("useShadows", 1);
    shaders->modelLightningShader.setInt("useShadows", 1);

    shadow->lightView = glm::lookAt(lights.lights.front().position,
                                    glm::vec3(0.f), glm::vec3(0.0f, 1.0f, 0.0f));

    shadow->shadowMode(true, WINDOW_WIDTH, WINDOW_HEIGHT);

    for (Mesh &mesh : meshes->meshes)
      shadow->apply_to_mesh(mesh);

    for (Model &model : models)
      shadow->apply_to_model(model);

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

    shadow->apply_shadow_texture(shaders->modelLightningShader);

    grid->render(shaders->basicShader);
    gridLines->render(shaders->basicShader);

    if (mainMenu.useLightShader)
    {
      shaders->lightningShader.setMat4("projection", projection);
      shaders->lightningShader.setMat4("camera.view", camera.getView());
      shaders->lightningShader.setVec3("camera.position", camera.getPosition());

      shaders->modelLightningShader.setMat4("projection", projection);
      shaders->modelLightningShader.setMat4("camera.view", camera.getView());
      shaders->modelLightningShader.setVec3("camera.position",
                                            camera.getPosition());

      for (int i = 0; i < lights.lights.size(); ++i)
      {
        lights.lights[i].apply_to_shader(shaders->lightningShader, i);
        lights.lights[i].apply_to_shader(shaders->modelLightningShader, i);
      }

      for (auto &m : meshes->meshes)
        m.render(shaders->lightningShader);
      for (auto &m : models)
        m.render(shaders->modelLightningShader);
    }
    else if (mainMenu.usePbrLightShader)
    {
      shaders->pbrLightningShader.setMat4("projection", projection);
      shaders->pbrLightningShader.setMat4("camera.view", camera.getView());
      shaders->pbrLightningShader.setVec3("camera.position",
                                          camera.getPosition());

      shaders->modelPbrLightningShader.setMat4("projection", projection);
      shaders->modelPbrLightningShader.setMat4("camera.view", camera.getView());
      shaders->modelPbrLightningShader.setVec3("camera.position",
                                               camera.getPosition());

      for (int i = 0; i < lights.lights.size(); ++i)
      {
        lights.lights[i].apply_to_shader(shaders->pbrLightningShader, i);
        lights.lights[i].apply_to_shader(shaders->modelPbrLightningShader, i);
      }

      for (auto &m : meshes->meshes)
        m.render(shaders->pbrLightningShader);
      for (auto &m : models)
        m.render(shaders->modelPbrLightningShader);
    }

    newMenuFrame();

    mainMenu.render();

    if (renderObjectMenu)
      mainMenu.objectMenu.render();

    if (renderLightMenu)
      mainMenu.lightMenu.render();

    renderMenuFrame();

    glfwSwapBuffers(window);
  }

  Application::Application() : camera(window, glm::vec3(0.f, 2.f, 3.f), 2.f)
  {

    if (!glfwInit())
      throw std::runtime_error("GLFW init failed");

    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 6);

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

    appIcon.pixels = stbi_load("../../src/appIcon.png", &appIcon.width,
                               &appIcon.height, nullptr, 0);

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

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    MeshManager::generatePlaneMesh(vertices, indices);

    grid = new Mesh(vertices, indices);
    grid->scale = glm::vec3(20.f, 0.f, 20.f);
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

    Mesh::MeshTypesMap.emplace(MeshType::PLANE, "PLANE");
    Mesh::MeshTypesMap.emplace(MeshType::CIRCLE, "CIRCLE");
    Mesh::MeshTypesMap.emplace(MeshType::CUBE, "CUBE");
    Mesh::MeshTypesMap.emplace(MeshType::SPHERE, "SPHERE");
    Mesh::MeshTypesMap.emplace(MeshType::TORUS, "TORUS");

    Mesh::MeshStringMap.emplace("PLANE", MeshType::PLANE);
    Mesh::MeshStringMap.emplace("CIRCLE", MeshType::CIRCLE);
    Mesh::MeshStringMap.emplace("CUBE", MeshType::CUBE);
    Mesh::MeshStringMap.emplace("SPHERE", MeshType::SPHERE);
    Mesh::MeshStringMap.emplace("TORUS", MeshType::TORUS);
  }

  void Application::run()
  {
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
    }
  }

  Application *Application::getApplication()
  {
    if (application == nullptr)
      application = new Application();

    return application;
  }

  void Application::terminateApplication(Application *&application)
  {
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
      for (auto &mesh : m.meshes)
      {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
        glDeleteVertexArrays(1, &mesh.vao);
        for (auto &tex : mesh.textures)
          glDeleteTextures(1, &tex.id);
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