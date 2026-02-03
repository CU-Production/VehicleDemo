#pragma once

#include "threepp/canvas/Canvas.hpp"
#include "threepp/canvas/Monitor.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <functional>

class ImguiContextCompat {
public:
    explicit ImguiContextCompat(void* window) {
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window), true);
#if EMSCRIPTEN
        ImGui_ImplOpenGL3_Init("#version 300 es");
#else
        ImGui_ImplOpenGL3_Init("#version 330 core");
#endif

        setFontScale(threepp::monitor::contentScale().first);
    }

    explicit ImguiContextCompat(const threepp::Canvas& canvas) : ImguiContextCompat(canvas.windowPtr()) {
        canvas.onMonitorChange([this](int monitor) {
            setFontScale(threepp::monitor::contentScale(monitor).first);
        });
    }

    ImguiContextCompat(ImguiContextCompat&&) = delete;
    ImguiContextCompat(const ImguiContextCompat&) = delete;
    ImguiContextCompat& operator=(const ImguiContextCompat&) = delete;

    void render() {
        if (!dpiAwareIsConfigured_) {
            ImGuiStyle& style = ImGui::GetStyle();
            style = ImGuiStyle();
            style.ScaleAllSizes(dpiScale_);
            dpiAwareIsConfigured_ = true;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        onRender();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    virtual ~ImguiContextCompat() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void setFontScale(float scale) {
        dpiAwareIsConfigured_ = false;
        dpiScale_ = scale;
    }

    [[nodiscard]] float dpiScale() const {
        return dpiScale_;
    }

protected:
    virtual void onRender() = 0;

private:
    bool dpiAwareIsConfigured_ = true;
    float dpiScale_ = 1.f;
};

class ImguiFunctionalContextCompat : public ImguiContextCompat {
public:
    explicit ImguiFunctionalContextCompat(void* window, std::function<void()> f)
        : ImguiContextCompat(window), f_(std::move(f)) {}

    explicit ImguiFunctionalContextCompat(const threepp::Canvas& canvas, std::function<void()> f)
        : ImguiContextCompat(canvas), f_(std::move(f)) {}

protected:
    void onRender() override {
        f_();
    }

private:
    std::function<void()> f_;
};
