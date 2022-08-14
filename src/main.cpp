#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "particle_system.h"
#include "euler_ode_solver.h"
#include "force/gravity.h"
#include "force/stokes_drag.h"
#include "force/drag.h"
#include "force/spring.h"

#include <cassert>
#include <vector>
#include <array>
#include <memory>
#include <random>
#include <chrono>
#include <iostream>

const float kFrameRate = 60.0;
const float kSimRate = 60000000000.0;

// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(
            "Dear ImGui SDL2+OpenGL3 example",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280, 720,
            window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(0); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    const size_t array_len = 10000;
    size_t array_index = 0;
    float out[array_len] = {0};

    ParticleSystem sys = ParticleSystem{};
    //ParticleState initial_state = { {0, 0, 0}, {0, 0, 10} };
    //ParticleInfo initial_info = { 5, {0, 0, 0} };
    //sys.addParticle(initial_state, initial_info);

    ParticleState initial_state = { {0, 0, 0}, {0, 0, 10} };
    ParticleInfo initial_info = { 5, {0, 0, 0} };
    sys.addParticle(initial_state, initial_info);

    sys.addParticle({{5,0,3}, {0,0,0}}, {10, {0,0,0}});

    std::random_device rd;
    auto gen = std::mt19937(rd());
    auto dis = std::uniform_real_distribution<float>(0, 100);
    for (uint c = 0; c < 10000; c++) {
        sys.addParticle(
                { Eigen::Vector3f::Random() * 10, Eigen::Vector3f::Random() * 50 },
                { dis(gen), {0, 0, 0} });
    }

    //sys.addParticle(initial_state, initial_info);

    Gravity g = Gravity();
    sys.addForce(&g);

    //StokesDrag sd = StokesDrag(-1, 18.5e-6, 0.01);
    //sys.addForce(&sd);

    Drag d = Drag(-1, 1.184, 0.47, M_PI*0.01*0.01);
    sys.addForce(&d);

    Spring spring(0, 1, 1, 20, 1);
    sys.addForce(&spring);

    EulerOdeSolver ode = EulerOdeSolver{};

    int active_particle = 0;
    uint sum = 0;
    const uint sum_len = 1000;
    uint sum_count = 0;

    // Main loop
    bool done = false;
    uint prev_render_time = 0;
    uint prev_sim_time = 0;
    while (!done)
    {

        if (prev_sim_time <= (SDL_GetTicks() - 1000.0/kSimRate)) {
            const auto t0 = std::chrono::high_resolution_clock::now();
            ode.step(sys, 0.001);
            const auto t1 = std::chrono::high_resolution_clock::now();

            //out[array_index] = sys.getState(active_particle).x[2];
            out[array_index] = sys.getState(active_particle).v[2];
            //out[array_index] = sys.getInfo(active_particle).f[2];
            array_index = (array_index + 1) % array_len;

            prev_sim_time = SDL_GetTicks();

            const auto duration = duration_cast<std::chrono::microseconds>(t1 - t0);
            //std::cout << "Sim took this many microseconds:" << duration.count() << std::endl;

            if (sum_count++ < sum_len)
                sum += duration.count();
            else
                std::cout << "Average duration of: " << sum / sum_len << "us" << std::endl;
        }

        // Only render the required number of times for the set framerate
        if (prev_render_time > ((double)SDL_GetTicks() - 1000.0/kFrameRate))
            continue;

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);


        ImGui::Begin("Plotting window");

        auto win_size = ImGui::GetContentRegionAvail();
        ImGui::PlotLines("Frame Times", out, array_len, 0, NULL, FLT_MAX, FLT_MAX, win_size);
        ImGui::End(); // Plotting Window

        ImGui::Begin("Particles");

        ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable | ImGuiTableFlags_Resizable;
        ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 8);

        if (ImGui::BeginTable("particle_table", 4, flags, outer_size)) {
            ImGui::TableSetupColumn("Particle #", ImGuiTableColumnFlags_DefaultSort, 0.0f);
            ImGui::TableSetupColumn("Mass");
            ImGui::TableSetupColumn("Position");
            ImGui::TableSetupColumn("Velocity");
            ImGui::TableHeadersRow();

            const uint particle_count = sys.count();
            ImGuiListClipper clipper;
            clipper.Begin(particle_count);
            while (clipper.Step()) {
                for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; index++) {
                    auto state = sys.getState(index);
                    auto info = sys.getInfo(index);

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGuiSelectableFlags sel_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;
                    char label[32];
                    sprintf(label, "%d", index);
                    if (ImGui::Selectable(label, active_particle == index, sel_flags)) {
                        active_particle = index;
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%f", info.m);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%f, %f, %f", state.x[0], state.x[1], state.x[2]);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%f, %f, %f", state.v[0], state.v[1], state.v[2]);
                }
            }
        }
        ImGui::EndTable();

        ImGui::End(); // Particles


        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        prev_render_time = SDL_GetTicks();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
