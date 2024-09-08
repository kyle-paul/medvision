#include "app.h"
#include "stb_image/stb_image.h"

App::App()
{
    setup_glfw();
    setup_opengl();
    setup_imgui();
    setup_systems();
}

void App::setup_glfw()
{
    if (!glfwInit())
    {
        std::cout << "Error initializing GLFW" << std::endl;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(2200, 1300, "Medical Vision 3D", NULL, NULL);
    if (!window)
    {
        std::cout << "Error creating GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error initializing Glad" << std::endl;
        glfwTerminate();
    }

    // Icon application
    int width, height, nrChannels;
    unsigned char *img = stbi_load("../resources/images/logo.png", &width, &height, &nrChannels, 4);
    GLFWimage icon = {width, height, img};
    glfwSetWindowIcon(window, 1, &icon);
    stbi_image_free(img);

    // Shader uniform
    shader = new Shader("../resources/glsl/vertex.vert", "../resources/glsl/fragment.frag");
    shader->Bind();
    glm::mat4 projection_matrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
    shader->SetUniformMatrix4fv("projection", projection_matrix);
}

void App::setup_opengl()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void App::setup_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    // Font
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("c:\\windows\\Fonts\\ARIAL.TTF", 16);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable muliViewport

    // Theme
    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
    colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.CellPadding = ImVec2(6.00f, 6.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
    style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
    style.IndentSpacing = 25;
    style.ScrollbarSize = 15;
    style.GrabMinSize = 10;
    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;
    style.WindowRounding = 7;
    style.ChildRounding = 4;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ScrollbarRounding = 9;
    style.GrabRounding = 3;
    style.LogSliderDeadzone = 4;
    style.TabRounding = 4;
}

void App::setup_systems()
{
    renderSystem = new RenderSystem(shader, window);
}

void App::run()
{
    // Factory initialization
    Factory factory(transform_components, physics_components, render_components);

    // Entity configuration
    std::string obj_name = "Cortical gray matter";
    std::string obj_path = "../resources/objs/brain/case1/1.obj";
    std::string brain_texture_path = "../resources/textures/brain1.jpg";
    objectInit object(&obj_name, &obj_path, &brain_texture_path);
    objectArrays[objectMade++] = object;

    std::string obj_name2 = "Basal ganglia";
    std::string obj_path2 = "../resources/objs/brain/case1/2.obj";
    std::string brain_texture_path2 = "../resources/textures/brain2.jpg";
    objectInit object2(&obj_name2, &obj_path2, &brain_texture_path2);
    objectArrays[objectMade++] = object2;

    std::string obj_name3 = "White matter";
    std::string obj_path3 = "../resources/objs/brain/case1/3.obj";
    std::string brain_texture_path3 = "../resources/textures/brain3.jpg";
    objectInit object3(&obj_name3, &obj_path3, &brain_texture_path3);
    objectArrays[objectMade++] = object3;

    std::string obj_name4 = "White matter lesions";
    std::string obj_path4 = "../resources/objs/brain/case1/4.obj";
    std::string brain_texture_path4 = "../resources/textures/brain4.jpg";
    objectInit object4(&obj_name4, &obj_path4, &brain_texture_path4);
    objectArrays[objectMade++] = object4;

    std::string obj_name5 = "Cerebrospinal fluid";
    std::string obj_path5 = "../resources/objs/brain/case1/5.obj";
    std::string brain_texture_path5 = "../resources/textures/brain5.jpg";
    objectInit object5(&obj_name5, &obj_path5, &brain_texture_path5);
    objectArrays[objectMade++] = object5;

    std::string obj_name6 = "Ventricles";
    std::string obj_path6 = "../resources/objs/brain/case1/6.obj";
    std::string brain_texture_path6 = "../resources/textures/brain6.jpg";
    objectInit object6(&obj_name6, &obj_path6, &brain_texture_path6);
    objectArrays[objectMade++] = object6;

    std::string obj_name7 = "Cerebellum";
    std::string obj_path7 = "../resources/objs/brain/case1/7.obj";
    std::string brain_texture_path7 = "../resources/textures/brain7.jpg";
    objectInit object7(&obj_name7, &obj_path7, &brain_texture_path7);
    objectArrays[objectMade++] = object7;

    std::string obj_name8 = "Brainstem";
    std::string obj_path8 = "../resources/objs/brain/case1/8.obj";
    std::string brain_texture_path8 = "../resources/textures/brain8.jpg";
    objectInit object8(&obj_name8, &obj_path8, &brain_texture_path8);
    objectArrays[objectMade++] = object8;

    // ImGui config
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show_imgui = true;
    bool show_another_window = true;
    bool enable_transformation = false;
    bool mesh_created = false;

    // Clear all
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        // Clear Screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader->Bind();

        // Start rendering
        if (mesh_created)
        {
            renderSystem->update(transform_components, render_components);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a full-screen docking space
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground);
        ImGui::PopStyleVar(3);
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        // Window 1
        if (show_imgui)
            ImGui::ShowDemoWindow(&show_imgui);

        // Window 2
        if (show_another_window)
        {
            ImGui::Begin("Management Panel", &show_another_window);
            ImGui::Text("Manage display for window and panel tools!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::Checkbox("Enable transformation", &enable_transformation);
            ImGui::End();
        }

        // Window 3
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin(objectArrays[0].obj_name->c_str());
            ImGui::Text("3D reconstruction for better analysis");
            ImGui::Checkbox("Controlling Panel", &show_imgui);
            ImGui::Checkbox("Management Panel", &show_another_window);

            if (enable_transformation)
            {
                ImGui::SliderFloat3("Translation", &(*objectArrays[0].position).x, -4.0f, 4.0f);
                ImGui::SliderFloat3("Rotation", &(*objectArrays[0].eulers).x, 0.0f, 360.f);
                ImGui::SliderFloat("Zoom factor", objectArrays[0].zoom_factor, 1.0f, 4.0f);
                ImGui::ColorEdit3("clear color", (float *)&clear_color);
                ImGui::Checkbox("Turn on texture", objectArrays[0].onTexCoords);
            }

            if (ImGui::Button("Start reconstruction"))
            {
                factory.create_mesh(objectArrays[0]);
                factory.create_mesh(objectArrays[1]);
                factory.create_mesh(objectArrays[2]);
                factory.create_mesh(objectArrays[3]);
                factory.create_mesh(objectArrays[4]);
                factory.create_mesh(objectArrays[5]);
                factory.create_mesh(objectArrays[6]);
                factory.create_mesh(objectArrays[7]);
                mesh_created = true;
            }

            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::End();
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    factory.~Factory();
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}