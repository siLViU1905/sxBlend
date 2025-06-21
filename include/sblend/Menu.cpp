#include "Menu.h"

#include <iostream>

#include "../GLFW/glfw3.h"
#include <string>

namespace sx
{
    void MainMenu::render()
    {
        ImGui::SetNextWindowSize(menuSize, ImGuiCond_Always);

        ImGui::Begin("sBlend", NULL, windowFlags);

        if (ImGui::Button("Mesh"))
            meshBtnPressed = !meshBtnPressed;

        if (ImGui::Button("Model"))
        {
            loadRequest.type = LoadRequestType::LOAD_MODEL;
            fileDialog.Open();
        }

        if (ImGui::Button("Light"))
            lightBtnPressed = !lightBtnPressed;

        std::string lightText =
                useLightShader ? "Disable Lightning" : "Enable Lightning";

        if (ImGui::Button(lightText.c_str()))
            useLightShader = !useLightShader;

        std::string pbrLightText =
                usePbrLightShader ? "Disable PBR Lightning" : "Enable PBR Lightning";

        if (ImGui::Button(pbrLightText.c_str()))
            usePbrLightShader = !usePbrLightShader;

        std::string shadowText = castShadows ? "Disable Shadows" : "Enable Shadows";

        if (ImGui::Button(shadowText.c_str()))
            castShadows = !castShadows;

        if (ImGui::Button("Set Skybox"))
        {
            setSkybox = !setSkybox;
            loadRequest.type = LoadRequestType::LOAD_SKYBOX;
            fileDialog.Open();
        }

        if (ImGui::Button("Save"))
            renderSaveMenu = true;

        if (ImGui::Button("Load"))
        {
            // renderLoadMenu = !renderLoadMenu;
            loadRequest.type = LoadRequestType::LOAD_SCENE;
            fileDialog.Open();
        }

        std::string simulationText =
                simulationState ? "Stop Simulation" : "Start Simulation";

        if (ImGui::Button(simulationText.c_str()))
        {
            simulationState = !simulationState;
            savePositionAndVelocity = simulationState;
        }

        if (ImGui::Button("Reset Simulation"))
        {
            resetSimulation = true;
            simulationState = false;
        }

        ImGui::End();

        if (meshBtnPressed)
        {
            ImGui::SetNextWindowSize(ImVec2(100.f, 100.f), ImGuiCond_Always);

            ImGui::Begin("Meshes", 0, windowFlags);

            if (ImGui::Button("Plane"))
            {
                selectedMesh = MeshType::PLANE;
                meshBtnPressed = false;
                existentMeshes.emplace_back(
                    "Plane " + std::to_string(meshTypeCounter[(int) selectedMesh]));
                selectedMeshIndicator.emplace_back(0);
                ++meshTypeCounter[(int) selectedMesh];
            }

            if (ImGui::Button("Circle"))
            {
                selectedMesh = MeshType::CIRCLE;
                newCircle = true;
            }

            if (ImGui::Button("Cube"))
            {
                selectedMesh = MeshType::CUBE;
                meshBtnPressed = false;
                existentMeshes.emplace_back(
                    "Cube " + std::to_string(meshTypeCounter[(int) selectedMesh]));
                selectedMeshIndicator.emplace_back(0);
                ++meshTypeCounter[(int) selectedMesh];
            }

            if (ImGui::Button("Sphere"))
            {
                selectedMesh = MeshType::SPHERE;
                newSphere = true;
            }

            if (ImGui::Button("Torus"))
            {
                selectedMesh = MeshType::TORUS;
                newTorus = true;
            }

            ImGui::End();

            if (newSphere)
            {
                ImGui::Begin("New Sphere");

                ImGui::InputInt("Slices", &slices);
                ImGui::InputInt("Stacks", &stacks);

                generate = ImGui::Button("Ok");

                ImGui::End();
            }

            if (newCircle)
            {
                ImGui::Begin("New Circle");

                ImGui::InputInt("Segments", &segments);

                generate = ImGui::Button("Ok");

                ImGui::End();
            }

            if (newTorus)
            {
                ImGui::Begin("New Torus");

                ImGui::InputInt("Slices", &slices);
                ImGui::InputInt("Stacks", &stacks);

                generate = ImGui::Button("Ok");

                ImGui::End();
            }
        }

        if (lightBtnPressed)
        {
            ImGui::SetNextWindowSize(ImVec2(100.f, 100.f), ImGuiCond_Always);

            ImGui::Begin("Light Type");

            if (ImGui::Button("Point"))
            {
                selectedLight = LightType::POINT;
                lightBtnPressed = false;
                existentLights.emplace_back(
                    "Point " + std::to_string(lightTypeCounter[(int) selectedLight]));
                selectedLightIndicator.emplace_back(0);
                ++lightTypeCounter[(int) selectedLight];
            }

            if (ImGui::Button("PBR"))
            {
                selectedLight = LightType::PBR;
                lightBtnPressed = false;
                existentLights.emplace_back(
                    "PBR " + std::to_string(lightTypeCounter[(int) selectedLight]));
                selectedLightIndicator.emplace_back(0);
                ++lightTypeCounter[(int) selectedLight];
            }

            ImGui::End();
        }

        ImGui::SetNextWindowPos(objectMenu.menuPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(objectMenu.menuSize, ImGuiCond_Always);

        ImGui::Begin("Objects", 0, windowFlags);

        selectedMeshIndex = -1;

        for (int i = 0; i < existentMeshes.size(); ++i)
        {
            bool *b = reinterpret_cast<bool *>(&selectedMeshIndicator[i]);
            if (ImGui::Checkbox(existentMeshes[i].c_str(), b))
                for (int j = 0; j < selectedMeshIndicator.size(); ++j)
                    if (j != i)
                        selectedMeshIndicator[j] = 0;

            if (*b)
            {
                selectedMeshIndex = i;

                deleteObject = false;

                deleteObject =
                        glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DELETE) == GLFW_PRESS;

                if (ImGui::Button("Set texture"))
                {
                    loadRequest.type = LoadRequestType::LOAD_TEXTURE;
                    fileDialog.Open();
                }

                if (ImGui::Button("Delete"))
                    deleteObject = true;

                setReflective = false;

                if (ImGui::Button("Make reflective"))
                    setReflective = true;
            }
        }

        selectedModelIndex = -1;

        for (int i = 0; i < existentModels.size(); ++i)
        {
            bool *b = (bool *) &selectedModelIndicator[i];
            ImGui::Checkbox(existentModels[i].c_str(), b);
            if (*b)
            {
                selectedModelIndex = i;

                deleteModel = false;

                deleteModel =
                        glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DELETE) == GLFW_PRESS;

                if (ImGui::Button("Delete"))
                    deleteModel = true;
            }
        }

        ImGui::End();

        ImGui::SetNextWindowPos(lightMenu.menuPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(lightMenu.menuSize, ImGuiCond_Always);

        ImGui::Begin("Lights", 0, windowFlags);

        selectedLightIndex = -1;

        for (int i = 0; i < existentLights.size(); ++i)
        {
            bool *b = (bool *) &selectedLightIndicator[i];
            ImGui::Checkbox(existentLights[i].c_str(), b);
            if (*b)
            {
                selectedLightIndex = i;

                deleteLight = false;

                deleteLight =
                        glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_DELETE) == GLFW_PRESS;

                if (ImGui::Button("Delete"))
                    deleteLight = true;
            }
        }

        ImGui::End();

        if (renderSaveMenu)
        {
            ImGui::Begin("Save file");
            ImGui::InputText("Name", fileNameBuffer, fNBufferSize);
            if (ImGui::Button("Ok"))
            {
                saveProperties = true;
                renderSaveMenu = false;
            }

            ImGui::End();
        }

        errorMenu.render();
    }

    MainMenu::MainMenu()
    {
        windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_NoCollapse;
        selectedMeshIndex = -1;
        newTorus = newCircle = newSphere = false;
        slices = 32;
        stacks = 16;
        segments = 30;
        *fileNameBuffer = 0;
    }

    void ObjectMenu::render()
    {
        ImGui::SetNextWindowPos(ImVec2(menuPos.x - 300.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300.f, 220.f), ImGuiCond_Always);

        ImGui::Begin("Properties", NULL, windowFlags);

        ImGui::SetNextItemOpen(false, ImGuiCond_Once);

        if (ImGui::CollapsingHeader("Transformations", windowFlags))
        {
            ImGui::SliderFloat3("Position", position, -10.f, 10.f);
            ImGui::InputFloat3("Pos", position);
            ImGui::SliderFloat3("Velocity", velocity, -10.f, 10.f);
            ImGui::InputFloat3("Vel", velocity);
            ImGui::SliderFloat3("Rotation", rotation, 0.f, 360.f);
            ImGui::InputFloat3("Rot", rotation);
            ImGui::SliderFloat3("Scale", scale, 0.f, 3.f);
            ImGui::InputFloat3("Sca", scale);
        }

        ImGui::SetNextItemOpen(false, ImGuiCond_Once);

        if (ImGui::CollapsingHeader("Material", windowFlags))
        {
            ImGui::ColorPicker3("Color", color);
            ImGui::SliderFloat("Metallic", metallic, 0.f, 1.f);
            ImGui::InputFloat("Met", metallic);
            ImGui::SliderFloat("Roughness", roughness, 0.f, 1.f);
            ImGui::InputFloat("Rou", roughness);
            ImGui::SliderFloat("AO", ao, 0.f, 1.f);
            ImGui::InputFloat("Ao", ao);
        }

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(menuPos.x - 405.f, 0.f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(100.f, 100.f), ImGuiCond_Always);

        ImGui::Begin("Mouse control", NULL, windowFlags);

        if (ImGui::Button("Translate"))
        {
            translateObjectWithMouse = true;
            scaleObjectWithMouse = rotateObjectWithMouse = false;
        }
        if (ImGui::Button("Scale"))
        {
            scaleObjectWithMouse = true;
            translateObjectWithMouse = rotateObjectWithMouse = false;
        }
        if (ImGui::Button("Rotate"))
        {
            rotateObjectWithMouse = true;
            translateObjectWithMouse = scaleObjectWithMouse = false;
        }

        ImGui::End();
    }

    ObjectMenu::ObjectMenu()
    {
        windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_NoCollapse;
    }

    void LightMenu::render()
    {
        ImGui::SetNextWindowPos(ImVec2(menuPos.x - 300.f, menuPos.y),
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(300.f, 220.f), ImGuiCond_Always);

        ImGui::Begin("Transformations", 0, windowFlags);

        ImGui::SliderFloat3("Position", position, -10.f, 10.f);
        ImGui::InputFloat3("Pos", position);
        ImGui::ColorPicker3("Color", color);
        ImGui::ColorPicker3("Diffuse", diffuse);
        ImGui::ColorPicker3("Specular", specular);
        ImGui::ColorPicker3("Ambient", ambient);
        ImGui::SliderFloat("Constant", constant, 0.f, 1.f);
        ImGui::InputFloat("Con", constant);
        ImGui::SliderFloat("Linear", linear, 0.f, 1.f);
        ImGui::InputFloat("Lin", linear);
        ImGui::SliderFloat("Quadratic", quadratic, 0.f, 1.f);
        ImGui::InputFloat("Qua", quadratic);

        ImGui::End();
    }

    LightMenu::LightMenu()
    {
        windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                      ImGuiWindowFlags_NoCollapse;
    }

    void ErrorMenu::render()
    {
        if (!renderMenu)
            return;

        ImGui::SetNextWindowPos(position, ImGuiCond_Always);

        ImGui::Begin(title.c_str(), NULL,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        ImGui::TextColored(ImVec4(0.8f, 0.f, 0.f, 1.f), message.c_str());

        if (ImGui::Button("Ok"))
            renderMenu = false;

        ImGui::End();
    }

    void MainMenu::getProperties(std::ostringstream &stream)
    {
        stream.clear();
        stream.str("");

        stream << useLightShader << '\n';
        stream << usePbrLightShader << '\n';
        stream << castShadows << '\n';
    }
} // namespace sx
