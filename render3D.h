//
// Created by Nico Russo on 4/27/26.
//

#ifndef LATTICE_RENDER3D_H
#define LATTICE_RENDER3D_H
#include "graphite.h"
#include "primitives.h"

namespace Lattice::Render3D {

    // ==================== INTERNAL HELPER ====================

    struct ProjectedVertex {
        glm::vec2 screen;
        float     clipW;
        float     invZ;  // 1/z in camera space, for z-buffering
        bool      valid;
    };

    inline ProjectedVertex projectVertex(const glm::vec3& vertex,
                                         const glm::mat4& mvp,
                                         const float canvasW,
                                         const float canvasH) {
        const glm::vec4 clip = mvp * glm::vec4{vertex, 1.f};
        if (clip.w <= 0.f) return {{}, clip.w, 0.f, false};

        const glm::vec2 ndc = glm::vec2{clip} / clip.w;
        const float x = (ndc.x * 0.5f + 0.5f) * canvasW;
        const float y = (1.f - (ndc.y * 0.5f + 0.5f)) * canvasH;
        return {{x, y}, clip.w, 1.f / clip.w, true};
    }

    inline glm::mat4 buildMVP(const Transform& t, const Camera3D& camera, float aspectRatio) {
        return camera.viewProjectionMatrix(aspectRatio) * t.toMatrix();
    }

    // ==================== RENDER FUNCTIONS ====================

    // Points — one point per vertex
    inline void renderPoints(const Graphite::Canvas& canvas,
                             const Transform& t,
                             const Model3D& model,
                             const Camera3D& camera,
                             const int pointRadius,
                             const Graphite::Color& color) {
        const float w   = canvas.getWidth();
        const float h   = canvas.getHeight();
        const auto  mvp = buildMVP(t, camera, w / h);

        for (const glm::vec3& vertex : model.vertices) {
            const auto pv = projectVertex(vertex, mvp, w, h);
            if (!pv.valid) continue;
            canvas.drawPoint(pv.screen.x, pv.screen.y, pointRadius, color);
        }
    }

    // Wireframe — draws an edge between each pair of vertices in a face
    inline void renderWireframe(const Graphite::Canvas& canvas,
                                const Transform& t,
                                const Model3D& model,
                                const Camera3D& camera,
                                const float lineThickness,
                                const Graphite::Color& color) {
        const float w   = canvas.getWidth();
        const float h   = canvas.getHeight();
        const auto  mvp = buildMVP(t, camera, w / h);

        for (const auto& face : model.faces) {
            const int n = static_cast<int>(face.size());
            for (int i = 0; i < n; i++) {
                const auto& a = model.vertices[face[i].vertexIdx];
                const auto& b = model.vertices[face[(i + 1) % n].vertexIdx];

                const auto pa = projectVertex(a, mvp, w, h);
                const auto pb = projectVertex(b, mvp, w, h);
                if (!pa.valid || !pb.valid) continue;

                canvas.drawLine(pa.screen.x, pa.screen.y,
                                pb.screen.x, pb.screen.y,
                                color, lineThickness);
            }
        }
    }

    inline void renderFlat(const Graphite::Canvas& canvas,
                           const Transform& t,
                           const Model3D& model,
                           const Camera3D& camera,
                           const Graphite::Color& color,
                           bool diffuse = false,
                           const glm::vec3& lightSource = Lattice::Dir3D::UP,
                           const Graphite::Canvas* zBuffer = nullptr) {
        const float w   = static_cast<float>(canvas.getWidth());
        const float h   = static_cast<float>(canvas.getHeight());
        const auto  mvp = buildMVP(t, camera, w / h);
        const glm::vec3 normalizedLight = glm::normalize(lightSource);
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(t.toMatrix())));

        for (const auto& face : model.faces) {
            if (face.size() < 3) continue;

            std::vector<ProjectedVertex> pverts;
            pverts.reserve(face.size());
            bool anyValid = false;
            for (const auto& fi : face) {
                const auto pv = projectVertex(model.vertices[fi.vertexIdx], mvp, w, h);
                pverts.push_back(pv);
                if (pv.valid) anyValid = true;
            }
            if (!anyValid) continue;

            // Back-face cull
            const float cross = (pverts[1].screen.x - pverts[0].screen.x) * (pverts[2].screen.y - pverts[0].screen.y)
                              - (pverts[1].screen.y - pverts[0].screen.y) * (pverts[2].screen.x - pverts[0].screen.x);
            if (cross >= 0.f) continue;

            // Diffuse
            Graphite::Color shaded = color;
            if (diffuse) {
                const glm::vec3& v0 = model.vertices[face[0].vertexIdx];
                const glm::vec3& v1 = model.vertices[face[1].vertexIdx];
                const glm::vec3& v2 = model.vertices[face[2].vertexIdx];
                const glm::vec3 faceNormal = glm::normalize(
                    normalMatrix * glm::cross(v1 - v0, v2 - v0));
                const float lambert = (glm::dot(faceNormal, normalizedLight) + 1.f) / 2.f;
                shaded = lerpColors(color, Graphite::Colors::Black, lambert);
            }

            // Fan triangulate
            const auto& p0 = pverts[0];
            for (int i = 1; i + 1 < (int)pverts.size(); i++) {
                const auto& p1 = pverts[i];
                const auto& p2 = pverts[i + 1];
                if (!p0.valid || !p1.valid || !p2.valid) continue;

                canvas.fillTriangle(
                    p0.screen, p1.screen, p2.screen,
                    shaded,
                    p0.invZ, p1.invZ, p2.invZ,
                    zBuffer
                );
            }
        }
    }

    inline void renderFaceColors(const Graphite::Canvas& canvas,
                                 const Transform& t,
                                 const Model3D& model,
                                 const Camera3D& camera,
                                 bool diffuse = false,
                                 const glm::vec3& lightSource = Lattice::Dir3D::UP,
                                 const Graphite::Canvas* zBuffer = nullptr) {
        const float w   = static_cast<float>(canvas.getWidth());
        const float h   = static_cast<float>(canvas.getHeight());
        const auto  mvp = buildMVP(t, camera, w / h);
        const glm::vec3 normalizedLight = glm::normalize(lightSource);
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(t.toMatrix())));

        for (const auto& face : model.faces) {
            if (face.size() < 3) continue;

            std::vector<ProjectedVertex> pverts;
            pverts.reserve(face.size());
            bool anyValid = false;
            for (const auto& fi : face) {
                const auto pv = projectVertex(model.vertices[fi.vertexIdx], mvp, w, h);
                pverts.push_back(pv);
                if (pv.valid) anyValid = true;
            }
            if (!anyValid) continue;

            const float cross = (pverts[1].screen.x - pverts[0].screen.x) * (pverts[2].screen.y - pverts[0].screen.y)
                              - (pverts[1].screen.y - pverts[0].screen.y) * (pverts[2].screen.x - pverts[0].screen.x);
            if (cross >= 0.f) continue;

            Graphite::Color faceColor = face[0].faceColor;
            if (diffuse) {
                const glm::vec3& v0 = model.vertices[face[0].vertexIdx];
                const glm::vec3& v1 = model.vertices[face[1].vertexIdx];
                const glm::vec3& v2 = model.vertices[face[2].vertexIdx];
                const glm::vec3 faceNormal = glm::normalize(
                    normalMatrix * glm::cross(v1 - v0, v2 - v0));
                const float lambert = (glm::dot(faceNormal, normalizedLight) + 1.f) / 2.f;
                faceColor = lerpColors(faceColor, Graphite::Colors::Black, lambert);
            }

            const auto& p0 = pverts[0];
            for (int i = 1; i + 1 < (int)pverts.size(); i++) {
                const auto& p1 = pverts[i];
                const auto& p2 = pverts[i + 1];
                if (!p0.valid || !p1.valid || !p2.valid) continue;

                canvas.fillTriangle(
                    p0.screen, p1.screen, p2.screen,
                    faceColor,
                    p0.invZ, p1.invZ, p2.invZ,
                    zBuffer
                );
            }
        }
    }


}

#endif //LATTICE_RENDER3D_H
