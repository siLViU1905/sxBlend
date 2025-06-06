#ifndef __MENU_H__
#define __MENU_H__
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../imgui/imfilebrowser.h"
#include <unordered_map>
#include "graphics/Light.h"
#include "graphics/Mesh.h"

namespace sx
{
    class Menu
    {
    protected:
        virtual void render() = 0;

    public:
        friend class Application;
    };

    class ObjectMenu : protected Menu
    {
    protected:
        void render() override;

    private:
        float *position;
        float *velocity;
        float *rotation;
        float *scale;
        float *color;
        float *metallic;
        float *roughness;
        float *ao;

        ImGuiWindowFlags windowFlags;

        ImVec2 menuPos, menuSize;

    public:
        ObjectMenu();

        friend class Application;
        friend class MainMenu;
    };

    class LightMenu : protected Menu
    {
        std::vector<unsigned char> selectedLightIndicator;
        bool deleteLight;

        ImVec2 menuPos, menuSize;

        ImGuiWindowFlags windowFlags;

        float *position;
        float *color;
        float *diffuse;
        float *specular;
        float *ambient;
        float *constant;
        float *linear;
        float *quadratic;

    protected:
        void render() override;

    public:
        LightMenu();

        friend class Application;
        friend class MainMenu;
    };

    class MainMenu : protected Menu
    {
        bool meshBtnPressed = false;
        bool lightBtnPressed = false;
        MeshType selectedMesh = MeshType::NaN;
        LightType selectedLight = LightType::NaN;

        bool useLightShader = false;

        bool usePbrLightShader = false;

        bool castShadows = false;

        bool renderSaveMenu = false;

        bool saveProperties = false;

        bool renderLoadMenu = false;

        void getProperties(std::ostringstream &stream);

        bool loadProperties = false;

        static constexpr int fNBufferSize = 32;

        char fileNameBuffer[fNBufferSize];

        std::vector<std::string> existentMeshes;

        std::vector<std::string> existentLights;

        ImGuiWindowFlags windowFlags;

        ImVec2 menuPos, menuSize;

        int selectedMeshIndex;

        int selectedLightIndex;

        bool deleteObject;

        bool deleteLight;

        bool newSphere, newCircle, newTorus;

        int slices, stacks;

        int segments;

        bool generate = false;

        std::vector<unsigned char> selectedMeshIndicator;

        std::unordered_map<int, int> meshTypeCounter;

        std::vector<unsigned char> selectedLightIndicator;

        std::unordered_map<int, int> lightTypeCounter;

        ObjectMenu objectMenu;

        LightMenu lightMenu;

        bool simulationState = false;

        bool resetSimulation = false;

        bool savePositionAndVelocity = false;

        bool renderTexMenu = false;

        bool setTexture = false;

        char texturePath[65];

    protected:
        void render() override;

    public:
        MainMenu();

        friend class Application;
    };

    class FileManagerMenu : protected Menu
    {
    protected:
        void render() override;

    public:
        friend class Application;
    };

}

#endif // __MENU_H__