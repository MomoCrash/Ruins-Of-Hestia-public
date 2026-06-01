# Ruins of Hestia

Projet étudiant réalisé par **9 personnes en 6 semaines**. Jeu de gestion de village en 3D avec phases d'expédition, construit sur un moteur maison en C++/Vulkan.

---

## Prérequis

- **CLion** ou tout compilateur **MinGW**
- **Vulkan SDK** — installation minimale suffisante (pas besoin de GLM ou des extensions avancées)
- **vcpkg** — pour la gestion des dépendances CMake

---

## Installation

**1. Cloner le repo**
```bash
git clone https://github.com/votre-org/ruins-of-hestia.git
cd ruins-of-hestia
```

**2. Lancer le build**
```bat
BUILD.bat
```

**3. Configurer vcpkg dans les options CMake**

Ajouter l'option suivante dans les paramètres CMake (CLion : `Settings > Build > CMake > CMake options`) :
```
-DCMAKE_TOOLCHAIN_FILE=C:/Users/[USERNAME]/vcpkg/scripts/buildsystems/vcpkg.cmake
```
Remplacer `[USERNAME]` par votre nom d'utilisateur Windows.



---

## Architecture technique

Le projet tourne sur un moteur ECS (Entity-Component-System) maison. Les scènes héritent de `SceneDefault` et passent par trois phases : `OnAssetsLoad` → `OnCreate` → `OnStart`.

### Création d'entités

```cpp
// Entité 3D (monde)
EntityID id = CreateEntityAs3D();
Transform3D* tr = GetComponent<Transform3D>(id);

// Entité 2D (UI)
EntityID id = CreateEntityAs2D();
```

### Ajout de composants et behaviors

```cpp
// Composant natif (rendu, audio, lumière...)
auto* renderer = AddComponent<Render3DComponent>(id);
renderer->pGeometry = GPC_MESH("Sphere");

auto* audio = AddComponent<AudioSource>(id);
audio->Resource = GPC_AUDIO("MySound");

// Behavior (logique update par frame)
auto* behavior = AddBehavior<MyBehavior>(id);
```

### Chargement de ressources

```cpp
// Textures
GPC_ASSETS->AddTexture("MyTex", "Resources/UI/image.png");
GPC_ASSETS->AddTextureArray("Atlas", width, height, { GPC_TEXTURE("MyTex"), ... });

// Meshes FBX
mp_MeshData = GPC_FBX->Load("Silene", "Resources/Models/Silene/Idle.fbx");

// Audio
GPC_AUDIO("SoundName")   // référence un asset audio chargé
GPC_MESH("MeshName")     // référence un mesh chargé
```

### Système de scènes

Les scènes communiquent via un contexte typé :
```cpp
SceneChangeCtx ctx{};
ctx.pData    = myCtxPtr;
ctx.DataSize = sizeof(MyCtxType);
SwapToScene("NomDeLaScene", ctx);
```

Scènes principales : `SceneMainMenu` → `SceneVillageV1` ↔ `SceneExpeditionPrep` → `SceneExpedition`

### UI (UiBuilder)

```cpp
UiBuilder b(this);

UiCanvas* canvas = b
    .At(Anchors::TOP_LEFT, { 0.0f, 0.0f })
    .Size({ 1920.0f, 1080.0f })
    .FitToScreen(true)
    .BuildCanvas();

UiText* text = b.ChildOf(canvas->GetEntityID())
    .At(Anchors::MIDDLE_MIDDLE, { 0.0f, 0.0f })
    .Font(UiTheme::DefaultFont, 24)
    .Text("Hello")
    .BuildText();

// Bouton à partir d'un sprite existant
UiButton* btn = b.BuildButton(mySprite);
btn->OnClick.ConnectLambda([this]() { /* ... */ });
```

### Animations UI

```cpp
UiAnimSlot slot{};
slot.Delay    = 0.5f;
slot.Duration = 1.0f;
slot.Ease     = Tweening::EasingType::EaseInQuad;
slot.Setter   = [](float t) { /* interpolation */ };
slot.OnComplete = [this]() { /* callback fin */ };
UiAnim::PushAnim(*this, entityId, std::move(slot));
```

### Particules

```cpp
GPC_ASSETS->AddParticleProgram("My Particles", {
    .Looping         = true,
    .MaxParticle     = 128,
    .Mode            = ParticleEmissionMode::OVER_TIME,
    .Shape           = ParticleEmissionShape::SHPERE,
    .StartMinPosition = glm::vec3(-1.0f),
    .StartMaxPosition = glm::vec3(1.0f),
    .LifeTimeMin     = 5.0f,
    .LifeTimeMax     = 10.0f,
    // ...
}, GPC_SHADER("Compute Shader Particle"));

auto* emitter = AddComponent<ParticleEmitter>(entityId);
emitter->pComputeProgram = &GPC_COMPUTE_PROGRAM("My Particles")->Program;
emitter->AutoStart();
```

---

## Équipe

9 personnes — développement sur 6 semaines.

| Membre | Contributions technique |
|--------|--------------|
| **Tristan** *(Lead Tech)* | ECS, scènes, map, gameplay expédition, CMake |
| **Ethan** *(Lead Tech)* | Rendu Vulkan, compatibilité Linux, gameplay village, CMake |
| **Willem** | Binary files, UI |
| **Tiago** | Physique, système d'UI, UI/UX, tutoriel village |
| **Noa** | Audio, gameplay village, UI/UX |
| **Thomas** | Rendu Vulkan, dialogues, tutoriel village |
| **Killian** | Meta files, système d'UI, UI/UX, gameplay village |
| **Hugo** | Assets, UI, gameplay village |
| **Ilan** | Chargement FBX, animations, UI |