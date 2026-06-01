#include "../../include/Samples/SceneGraphics.h"

#include "Assets.h"
#include "FreeCamera.h"
#include "CameraSystem.h"
#include "LightingSystem.h"
#include "Random.h"
#include "../../include/Expedition/PlayerController.h"
#include "Render3DSystem.h"
#include "Map Generation/TerrainGeneratorBehavior.h"

namespace GPC
{
    ErrorType SceneGraphics::OnAssetsLoad()
    {
        SceneDefault::OnAssetsLoad();

        FbxLoader loader;
        loader.Load("MODEL_TREE", "Resources/Models/Tree_4_A_Color1.fbx");

        LoadTextureArray("MODEL", 1024, 1024, {
            {"TREE", "Resources/Models/forest_texture.png"}
        });

        // Batching Program
        {
            GPC_ASSETS->AddShader("Batching Vertex", "batching.vert", Shader::VERTEX);
            GPC_ASSETS->AddShader("Batching Fragment", "batching.frag", Shader::FRAGMENT);

            GPC_ASSETS->AddGraphicProgram(
                "Batching Program",
                GPC_SHADER("Batching Vertex"),
                GPC_SHADER("Batching Fragment"),
                CullMode::FRONT,
                PolygonMode::MODE_FILL,
                {
                    {
                        .BindingDescription = GPC::Vertex::GetBindingDescription(),
                        .AttributeDescriptions = GPC::Vertex::GetAttributeDescriptions()
                    },
                    {
                        .BindingDescription = GPC::Batched3DObject::GetBindingDescription(),
                        .AttributeDescriptions = GPC::Batched3DObject::GetAttributeDescriptions()
                    }
                },
                {
                    GPC::DescriptorSetInformation::ForCamera(0),
                    GPC::DescriptorSetInformation::ForMaterials(1),
                    GPC::DescriptorSetInformation::ForTextures(2),
                },
                GRAPHICS_3D,
                mp_Window
            );

            auto& program = GPC_GRAPHIC_PROGRAM("Batching Program")->Program;
            program.InitializeWith({&GPC_TEXTURE_ARRAY("MODEL")->Array}, GPC_TEXTURE_ARRAY("MODEL")->TextureSampler);


        }

        return ErrorType::SUCCESS;
    }

    void SceneGraphics::OnAssetsDestroy() {

    }

    ErrorType SceneGraphics::OnCreate(SceneInformation &info) {
        SceneDefault::OnCreate(info);

        auto cLight = AddComponent<LightingComponent>(mp_MainCamera->GetEntityID());
        cLight->pLightData = GPC_POINT_LIGHT("PointBlue");

        auto behavior = AddBehavior<GPC::FreeCamera>(mp_MainCamera->GetEntityID());
        behavior->MovementSpeed = 50;
        // auto player = CreateEntityAs3D();
        // auto* pRenderer = AddComponent<GPC::Render3DComponent>(player);
        // auto* pTransform = GetComponent<GPC::Transform3D>(player);
        // auto* pLight = AddComponent<GPC::LightingComponent>(player);

        // pRenderer->pGeometry = GPC_MESH("Cube");
        // // pRenderer->pGraphicProgram = &GPC_GRAPHIC_PROGRAM("Default Program")->Program; // TODO New line <- Check it out
        // pRenderer->RenderMaterial.Diffuse = GPC_TEXTURE_ID("1024x1024", "Debug");
        // pTransform->LocalTransform.SetPosition(0, 1, 0);
        // pTransform->LocalTransform.SetScale(1, 1, 1);
        // pLight->pLightData = GPC_POINT_LIGHT("PointWhite");

        // mp_MainCameraTransform->LocalTransform.SetPosition(pTransform->LocalTransform.GetPosition() + glm::vec3{-3, 8, -3});

        CreateDebugOverlay();

        auto    batchedTrees = CreateEntityAs3D();
        auto*   pBatching = AddComponent<GPC::Batching3DComponent>(batchedTrees);


        float treesCountX = 300.0f;
        float treesCountZ = 300.0f;
        float total = treesCountX*treesCountZ;
        float x = 0;
        float z = 0;
        pBatching->Batched3DObjects.resize(total);

        for (int i = 0; i < total; i++) {

            glm::vec3 pos = glm::vec3(x * 3, 0.0f, z * 3);
            x += 1.0f;
            if (x >= treesCountX) {
                z += 1.0f;
                x = 0;
            }
            float size = Random::Float(1.0f, 2.0f);
            int r = Random::Integer(1, 255);
            int g = Random::Integer(1, 255);
            int b = Random::Integer(1, 255);
            pBatching->Batched3DObjects[i] = {
                glm::vec4(pos, 1.0f),
                Transform::Euler_To_Quaternion(0.0f, i * glm::pi<float>()/20.0f, 0.0f, XYZ),
                glm::vec4{size, size, size, 1.0f},
                Color(r, g, b, 255)
            };
        }

        pBatching->pGeometry = GPC_MESH("MODEL_TREE");
        pBatching->pGraphicProgram = &GPC_GRAPHIC_PROGRAM("Batching Program")->Program;
        pBatching->SetTexture("MODEL", "TREE");

        return ErrorType::SUCCESS;
    }

    void SceneGraphics::OnDestroy() {

    }

} // GPC