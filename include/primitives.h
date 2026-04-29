//
// Created by Nico Russo on 4/27/26.
//

#ifndef LATTICE_PRIMITIVES_H
#define LATTICE_PRIMITIVES_H

#include <graphite.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Lattice {

    /**
     * @brief Constant world-space cardinal directions.
     *
     * These never transform — they are fixed reference axes in world space.
     * Use these as inputs to localDirection(), yawDirection(), etc. rather
     * than hardcoding vec3 literals at call sites.
     *
     * Convention: Y is up, Z is forward.
     */
    namespace Dir3D {
        inline constexpr glm::vec3 UP       {0,  1,  0};
        inline constexpr glm::vec3 DOWN     {0, -1,  0};
        inline constexpr glm::vec3 RIGHT    {1,  0,  0};
        inline constexpr glm::vec3 LEFT     {-1, 0,  0};
        inline constexpr glm::vec3 FORWARD  {0,  0,  1};
        inline constexpr glm::vec3 BACKWARD {0,  0,  -1};
    }

    /**
     * @brief A composable TRS (Translation-Rotation-Scale) transform.
     *
     * Rotation is stored as a unit quaternion, avoiding gimbal lock and
     * enabling smooth interpolation via lerp(). Transforms can be composed
     * with operator* (parent * child) and inverted with inverse().
     *
     * ### Coordinate convention
     * Y is up, Z is forward, X is right.
     *
     * ### Typical usage
     * @code
     * Lattice::Transform t;
     * t.position = {0, 1, 0};
     * t.rotateAroundAxis(WorldDir3D::UP, glm::radians(45.f));
     * glm::vec3 world = t.transformPoint({1, 0, 0});
     * @endcode
     */
    struct Transform {
        glm::vec3 position {0.f, 0.f, 0.f}; ///< World-space translation.
        glm::quat rotation {glm::identity<glm::quat>()}; ///< Orientation as a unit quaternion.
        glm::vec3 scale    {1.f, 1.f, 1.f}; ///< Per-axis scale factor.

        /**
         * @brief Build the 4x4 TRS matrix for this transform.
         *
         * Equivalent to T * R * S where T is the translation matrix,
         * R is the rotation matrix, and S is the scale matrix.
         * Suitable for passing directly to a shader as a model matrix.
         *
         * @return Combined TRS matrix.
         */
        [[nodiscard]] glm::mat4 toMatrix() const;

        /**
         * @brief Compose this transform with a child transform.
         *
         * Equivalent to applying `child` in the local space of `this`.
         * The child's position is scaled and rotated by the parent before
         * adding the parent's position.
         *
         * @param child The child transform to compose.
         * @return The composed world-space transform.
         */
        [[nodiscard]] Transform operator*(const Transform& child) const;

        /**
         * @brief Compute the inverse of this transform.
         *
         * The inverse undoes the full TRS — applying a transform and then
         * its inverse returns the original point. Handles non-uniform scale.
         *
         * @return The inverse transform.
         */
        [[nodiscard]] Transform inverse() const;

        /**
         * @brief Apply the full TRS to a point.
         *
         * Applies scale, then rotation, then translation. Use this for
         * transforming vertex positions from local to world space.
         *
         * @param point Local-space point to transform.
         * @return World-space point.
         */
        [[nodiscard]] glm::vec3 transformPoint(const glm::vec3& point) const;

        /**
         * @brief Apply rotation and scale to a direction vector, normalized.
         *
         * Translation is ignored (directions have no position). The result
         * is normalized, so scale affects only the direction, not magnitude.
         * For a direction that should preserve magnitude, use localDirection().
         *
         * @param direction Local-space direction to transform.
         * @return World-space direction (normalized).
         */
        [[nodiscard]] glm::vec3 transformDirection(const glm::vec3& direction) const;

        /**
         * @brief Rotate a direction into this transform's local space.
         *
         * Applies only the rotation quaternion — no translation, no scale.
         * The magnitude of the input vector is preserved.
         *
         * @code
         * // Where is "forward" for this object in world space?
         * glm::vec3 fwd = transform.localDirection(WorldDir3D::FORWARD);
         * @endcode
         *
         * @param direction Direction to rotate (does not need to be normalized).
         * @return Rotated direction in world space.
         */
        [[nodiscard]] glm::vec3 localDirection(const glm::vec3& direction) const;

        /**
         * @brief Apply the inverse rotation to a direction.
         *
         * Converts a world-space direction back into this transform's local space.
         * Equivalent to localDirection() with the conjugate quaternion.
         *
         * @param direction World-space direction to inverse-rotate.
         * @return Direction in local space.
         */
        [[nodiscard]] glm::vec3 localDirectionInverse(const glm::vec3& direction) const;

        /**
         * @brief Rotate a direction using only the yaw component of this transform.
         *
         * Extracts the yaw angle and constructs a rotation around WorldDir3D::UP,
         * ignoring pitch and roll. Keeps the result flat on the XZ plane.
         * Primarily useful for FPS-style camera movement.
         *
         * @code
         * // Flat forward regardless of camera pitch
         * glm::vec3 moveDir = transform.yawDirection(WorldDir3D::FORWARD);
         * @endcode
         *
         * @param direction Direction to rotate (typically a WorldDir3D constant).
         * @return Yaw-rotated direction.
         */
        [[nodiscard]] glm::vec3 yawDirection(const glm::vec3& direction) const;

        /**
         * @brief Rotate this transform about an arbitrary world-space axis.
         *
         * The rotation is applied in world space (pre-multiplied), so the axis
         * is interpreted in world coordinates. To rotate around a local axis,
         * pass localDirection(axis) as the axis argument.
         *
         * @param axis          World-space axis to rotate around (should be normalized).
         * @param angleRadians  Angle to rotate by, in radians.
         */
        void rotateAroundAxis(const glm::vec3& axis, float angleRadians);

        /**
         * @brief Rotate this transform so its local forward faces a world-space point.
         *
         * Uses WorldDir3D::UP as the reference up vector, so roll is unaffected
         * and the horizon stays level. If the target is directly above or below
         * this transform (degenerate case), the rotation is left unchanged.
         *
         * @param target World-space position to look at.
         */
        void lookAt(const glm::vec3& target);

        /**
         * @brief Extract the rotation as Euler angles in radians.
         *
         * Returns `{pitch, yaw, roll}` in ZYX order, consistent with fromEuler().
         * Prefer storing and manipulating the quaternion directly where possible —
         * Euler extraction is lossy and mainly useful for serialization or display.
         *
         * @return Euler angles `{pitch, yaw, roll}` in radians.
         */
        [[nodiscard]] glm::vec3 toEuler() const;

        /**
         * @brief Interpolate between two transforms.
         *
         * Position and scale are linearly interpolated; rotation is smooth lerp-d,
         * giving the shortest-path quaternion interpolation. Safe to use for
         * animation blending and smooth camera movement.
         *
         * @param a First transform (returned when t = 0).
         * @param b Second transform (returned when t = 1).
         * @param t Interpolation factor in [0, 1].
         * @return Interpolated transform.
         */
        [[nodiscard]] static Transform lerp(const Transform& a, const Transform& b, float t);

        /**
         * @brief Compute the distance between two transforms' positions.
         * @param other The other transform.
         * @return Euclidean distance between positions.
         */
        [[nodiscard]] float distanceTo(const Transform& other) const;

        /**
         * @brief Compute the normalized direction from this transform to another.
         * @param other The target transform.
         * @return Normalized world-space direction vector toward `other`.
         */
        [[nodiscard]] glm::vec3 directionTo(const Transform& other) const;

        /**
         * @brief Construct a Transform from Euler angles.
         *
         * Converts `{pitch, yaw, roll}` in ZYX order to a quaternion internally.
         * Prefer constructing quaternions directly where possible to avoid
         * gimbal lock and ambiguity in angle representation.
         *
         * @param eulerRadians Euler angles `{pitch, yaw, roll}` in radians.
         * @param position     World-space translation (default: origin).
         * @param scale        Per-axis scale (default: {1, 1, 1}).
         * @return Constructed Transform.
         */
        static Transform fromEuler(
            glm::vec3 eulerRadians,
            glm::vec3 position = {},
            glm::vec3 scale    = {1, 1, 1});
    };

    /**
     * @brief Rotate a point by a quaternion.
     * @param p Point to rotate.
     * @param q Unit quaternion representing the rotation.
     * @return Rotated point.
     */
    glm::vec3 rotate(const glm::vec3& p, const glm::quat& q);

    /**
     * @brief Apply the inverse rotation of a quaternion to a point.
     *
     * Equivalent to rotating by the conjugate quaternion. Assumes `q` is
     * a unit quaternion — conjugate is used instead of inverse for efficiency.
     *
     * @param p Point to inverse-rotate.
     * @param q Unit quaternion whose inverse rotation is applied.
     * @return Inverse-rotated point.
     */
    glm::vec3 rotateInverse(const glm::vec3& p, const glm::quat& q);

    /**
     * @brief Scale a point about the origin.
     * @param p     Point to scale.
     * @param scale Per-axis scale factors.
     * @return Scaled point.
     */
    glm::vec3 scalePoint(const glm::vec3& p, const glm::vec3& scale);

    /**
     * @brief Apply a full TRS transform to a point.
     *
     * Convenience wrapper around Transform::transformPoint().
     *
     * @param p         Local-space point.
     * @param transform Transform to apply.
     * @return World-space point.
     */
    glm::vec3 applyTransform(const glm::vec3& p, const Transform& transform);

    /**
     * @brief A 3D mesh asset storing geometry data.
     *
     * Pure data — no transform or spatial information. The owning entity
     * is responsible for providing a Transform component. Rendered via
     * a Camera3D using the draw methods in the rendering backend.
     */
    struct Model3D {
        /**
         * @brief Index references into a Model3D's attribute arrays for one polygon vertex.
         *
         * All indices are 0-based and index into the parallel arrays of the
         * owning Model3D. `faceColor` is a per-face color assigned at load time,
         * used by vertex-color rendering paths.
         */
        struct FaceIndex {
            glm::i32 vertexIdx{};           ///< Index into Model3D::vertices.
            glm::i32 texCoordIdx{};         ///< Index into Model3D::uvs.
            glm::i32 normalIdx{};           ///< Index into Model3D::normals.
            Graphite::Color faceColor = Graphite::Colors::Purple;
        };

        std::vector<glm::vec3>              vertices; ///< Vertex positions in object space.
        std::vector<glm::vec3>              normals;  ///< Per-vertex or per-face normals.
        std::vector<glm::vec2>              uvs;      ///< UV texture coordinates.
        std::vector<std::vector<FaceIndex>> faces;    ///< Polygon face list; each face is a list of FaceIndex.

        Graphite::Canvas* texture = nullptr;
    };

    /**
     * @brief Perspective camera component.
     *
     * Owns a Transform for position and orientation. Generates view and
     * projection matrices for rendering, and provides yaw-locked movement
     * directions for FPS-style camera control.
     *
     * ### Typical usage
     * @code
     * Camera3D cam;
     * cam.transform.position = {0, 1, -5};
     * glm::mat4 vp = cam.viewProjectionMatrix(16.f / 9.f);
     * @endcode
     */
    struct Camera3D {
        Transform transform;                    ///< Camera position and orientation.
        float fov      = glm::radians(60.f);    ///< Vertical field of view in radians.
        float nearPlane = 0.1f;                 ///< Near clip plane distance.
        float farPlane  = 100.f;                ///< Far clip plane distance.

        /**
         * @brief Compute the view matrix for this camera.
         *
         * The view matrix transforms world-space points into camera space.
         * Built as inverse-rotation * inverse-translation, without a full
         * matrix inversion — valid as long as camera scale is {1, 1, 1}.
         *
         * @return View matrix.
         */
        [[nodiscard]] glm::mat4 viewMatrix() const;

        /**
         * @brief Compute the perspective projection matrix.
         *
         * Uses `fov`, `nearPlane`, and `farPlane` with the given aspect ratio.
         *
         * @param aspectRatio Viewport width divided by height.
         * @return Projection matrix.
         */
        [[nodiscard]] glm::mat4 projectionMatrix(float aspectRatio) const;

        /**
         * @brief Compute the combined view-projection matrix.
         *
         * Equivalent to projectionMatrix(aspectRatio) * viewMatrix().
         * Pass this directly to a shader as the VP or MVP matrix (multiply
         * by model matrix on the CPU or GPU side).
         *
         * @param aspectRatio Viewport width divided by height.
         * @return View-projection matrix.
         */
        [[nodiscard]] glm::mat4 viewProjectionMatrix(float aspectRatio) const;

        /**
         * @brief Rotate a direction using only the yaw of this camera's transform.
         *
         * Ignores pitch and roll — keeps the result flat on the XZ plane.
         * Use for FPS-style movement where the camera can look up/down but
         * should only move horizontally.
         *
         * @param direction Direction to rotate (typically a WorldDir3D constant).
         * @return Yaw-rotated direction.
         */
        [[nodiscard]] glm::vec3 yawDirection(const glm::vec3& direction) const;
    };

    /**
     * @brief An infinite ray defined by an origin and a normalized direction.
     *
     * Used for mouse picking, line-of-sight checks, and collision queries.
     * The direction must be normalized for intersection results to be correct.
     */
    struct Ray3D {
        glm::vec3 origin;    ///< Ray origin in world space.
        glm::vec3 direction; ///< Ray direction (must be normalized).

        /**
         * @brief Evaluate the ray at parameter `t`.
         *
         * Returns the point `origin + direction * t`. Negative `t` values
         * produce points behind the ray origin.
         *
         * @param t Ray parameter.
         * @return World-space point on the ray.
         */
        [[nodiscard]] glm::vec3 at(float t) const;

        /**
         * @brief Compute the shortest distance from the ray to a point.
         *
         * Projects the point onto the ray and measures perpendicular distance.
         * The projection is clamped to the ray origin (t >= 0), so points
         * behind the origin return distance to the origin.
         *
         * @param point World-space point.
         * @return Shortest distance from the ray to the point.
         */
        [[nodiscard]] float distanceTo(const glm::vec3& point) const;

        /**
         * @brief Test intersection with a sphere.
         *
         * Solves the quadratic |origin + t*dir - center|^2 = radius^2.
         * On a hit, `t` is the ray parameter of the nearest intersection in front
         * of the origin. Retrieve the world-space hit position with at(t).
         * Since direction is normalized, t also equals the distance to the hit.
         *
         * @code
         * float t;
         * if (ray.intersectsSphere(center, radius, t)) {
         *     glm::vec3 hitPoint = ray.at(t);
         *     float distance = t;
         * }
         * @endcode
         *
         * @param center Sphere center in world space.
         * @param radius Sphere radius.
         * @param t      [out] Ray parameter at the nearest intersection. Only valid if true is returned.
         * @return `true` if the ray intersects the sphere.
         */
        [[nodiscard]] bool intersectsSphere(const glm::vec3& center, float radius, float& t) const;

        /**
         * @brief Test intersection with an infinite plane.
         *
         * The plane is defined as `dot(normal, p) = d`. Returns false if
         * the ray is parallel to the plane (within a small epsilon).
         *
         * @param normal Plane normal (should be normalized).
         * @param d      Plane offset along the normal.
         * @param t      [out] Ray parameter at the intersection point.
         * @return `true` if the ray intersects the plane.
         */
        [[nodiscard]] bool intersectsPlane(const glm::vec3& normal, float d, float& t) const;

        /**
         * @brief Test intersection with an axis-aligned bounding box.
         *
         * Uses the slab method — intersects the ray against each pair of
         * axis-aligned planes and tracks the entry/exit interval. Fast and
         * branchless, suitable for many ray-box tests per frame.
         *
         * @param min World-space AABB minimum corner.
         * @param max World-space AABB maximum corner.
         * @param t   [out] Ray parameter at the entry intersection point.
         * @return `true` if the ray intersects the AABB.
         */
        [[nodiscard]] bool intersectsAABB(const glm::vec3& min, const glm::vec3& max, float& t) const;

        /**
         * @brief Construct a world-space ray from a camera and NDC coordinates.
         *
         * Un-projects the NDC point through the inverse view-projection matrix,
         * producing a ray suitable for mouse picking. NDC coordinates should
         * be in [-1, 1] on both axes.
         *
         * @param cam         Source camera.
         * @param ndc         Normalized device coordinates of the screen point.
         * @param aspectRatio Viewport width divided by height.
         * @return World-space ray through the given screen point.
         */
        static Ray3D fromCamera(const Camera3D& cam, glm::vec2 ndc, float aspectRatio);
    };

    /**
     * @brief An axis-aligned bounding box defined by minimum and maximum corners.
     *
     * Used for broad-phase collision detection and view-frustum culling.
     * The box is always axis-aligned in whatever space its corners are expressed —
     * use transformed() to re-fit after applying a Transform.
     */
    struct AABB {
        glm::vec3 min {0.f}; ///< Minimum corner (smallest x, y, z).
        glm::vec3 max {0.f}; ///< Maximum corner (largest x, y, z).

        /**
         * @brief Compute the center of the bounding box.
         * @return Midpoint between min and max.
         */
        [[nodiscard]] glm::vec3 center() const;

        /**
         * @brief Compute the size of the bounding box.
         * @return Per-axis extents (max - min).
         */
        [[nodiscard]] glm::vec3 size() const;

        /**
         * @brief Test whether a point lies inside the bounding box (inclusive).
         * @param point World-space point to test.
         * @return `true` if the point is inside or on the boundary.
         */
        [[nodiscard]] bool contains(const glm::vec3& point) const;

        /**
         * @brief Test whether this bounding box overlaps another.
         *
         * Returns true for touching boxes (shared face or edge).
         *
         * @param other The other AABB to test against.
         * @return `true` if the two boxes overlap or touch.
         */
        [[nodiscard]] bool intersects(const AABB& other) const;

        /**
         * @brief Re-fit this AABB after applying a Transform.
         *
         * Transforms all 8 corners individually and computes a new axis-aligned
         * box that contains them. Necessary because rotation and non-uniform scale
         * mean the original corners no longer describe the extents.
         *
         * @param t Transform to apply.
         * @return New AABB enclosing the transformed box.
         */
        [[nodiscard]] AABB transformed(const Transform& t) const;

        /**
         * @brief Compute the tightest AABB enclosing a Model3D's vertices.
         *
         * Iterates all vertices in object space. Apply transformed() afterward
         * to get a world-space AABB for a model with a non-identity transform.
         *
         * @param model Source model.
         * @return AABB in object space.
         */
        static AABB fromModel(const Model3D& model);
    };

    /**
     * @brief Factory functions for primitive Model3D meshes.
     *
     * All primitives are centered at the origin in object space.
     * Use the owning entity's Transform to position, rotate, and scale them.
     */
    namespace Primitives {

        /**
         * @brief Generate a unit cube (side length 1, centered at origin).
         *
         * 6 faces, 12 triangles. Normals are per-face (flat shading).
         * UVs are per-face, each face mapped to the full [0,1] range.
         *
         * @return Model3D containing the cube mesh.
         */
        [[nodiscard]] Model3D cube();

        /**
         * @brief Generate a UV sphere.
         *
         * Tessellated by latitude/longitude rings. More segments = smoother
         * sphere but higher triangle count. Normals are smooth (per-vertex).
         *
         * @param radius    Sphere radius.
         * @param sectors   Longitudinal subdivisions (minimum 3).
         * @param stacks    Latitudinal subdivisions (minimum 2).
         * @return Model3D containing the sphere mesh.
         */
        [[nodiscard]] Model3D sphere(float radius = 1.f,
                                     glm::u32 sectors  = 32,
                                     glm::u32 stacks   = 16);

        /**
         * @brief Generate a flat grid on the XZ plane.
         *
         * Centered at origin. Each cell is subdivided into 2 triangles.
         * Useful as a ground plane or for terrain prototyping.
         *
         * @param width      Total width along X.
         * @param depth      Total depth along Z.
         * @param cellsX     Number of cells along X.
         * @param cellsZ     Number of cells along Z.
         * @return Model3D containing the grid mesh.
         */
        [[nodiscard]] Model3D grid(float width  = 10.f,
                                   float depth  = 10.f,
                                   glm::u32 cellsX   = 10,
                                   glm::u32 cellsZ   = 10);

        /**
         * @brief Generate a cylinder aligned along the Y axis.
         *
         * Caps are included. Normals are smooth on the barrel, flat on caps.
         *
         * @param radius    Cylinder radius.
         * @param height    Cylinder height.
         * @param sectors   Radial subdivisions (minimum 3).
         * @return Model3D containing the cylinder mesh.
         */
        [[nodiscard]] Model3D cylinder(float radius = 1.f,
                                       float height = 2.f,
                                       glm::u32 sectors  = 32);

        /**
         * @brief Generate a cone aligned along the Y axis, apex at +Y.
         *
         * Base cap is included. Normals are smooth on the surface, flat on cap.
         *
         * @param radius    Base radius.
         * @param height    Cone height.
         * @param sectors   Radial subdivisions (minimum 3).
         * @return Model3D containing the cone mesh.
         */
        [[nodiscard]] Model3D cone(float radius = 1.f,
                                   float height = 2.f,
                                   glm::u32 sectors  = 32);

        /**
         * @brief Generate a flat quad on the XY plane.
         *
         * 2 triangles, centered at origin. Useful for billboards, decals,
         * or UI elements rendered in world space.
         *
         * @param width  Width along X.
         * @param height Height along Y.
         * @return Model3D containing the quad mesh.
         */
        [[nodiscard]] Model3D quad(float width = 1.f, float height = 1.f);
    }
}





















#ifndef LATTICE_NO_IMPLEMENTATION
namespace Lattice {

    // ==================== TRANSFORM ====================

    inline glm::mat4 Transform::toMatrix() const {
        const glm::mat4 T = glm::translate(glm::mat4{1.f}, position);
        const glm::mat4 R = glm::toMat4(rotation);
        const glm::mat4 S = glm::scale(glm::mat4{1.f}, scale);
        return T * R * S;
    }

    inline Transform Transform::operator*(const Transform &child) const {
        Transform out;
        out.position = position + rotation * (scale * child.position);
        out.rotation = rotation * child.rotation;
        out.scale = scale * child.scale;
        return out;
    }

    inline Transform Transform::inverse() const {
        Transform out;
        const glm::quat invRot   = glm::inverse(rotation);
        const glm::vec3 invScale = 1.f / scale;
        out.rotation = invRot;
        out.scale    = invScale;
        out.position = invRot * (invScale * -position);
        return out;
    }

    inline glm::vec3 Transform::transformPoint(const glm::vec3& point) const {
        return position + rotation * (scale * point);
    }

    inline glm::vec3 Transform::transformDirection(const glm::vec3& direction) const {
        // Direction ignores translation; scale affects magnitude, so we normalize
        return glm::normalize(rotation * (scale * direction));
    }

    inline glm::vec3 Transform::localDirection(const glm::vec3& direction) const {
        return rotation * direction;
    }

    inline glm::vec3 Transform::localDirectionInverse(const glm::vec3& direction) const {
        return glm::inverse(rotation) * direction;
    }

    inline glm::vec3 Transform::yawDirection(const glm::vec3& direction) const {
        // Get the fully-rotated local direction
        const glm::vec3 local = rotation * direction;

        // Flatten onto XZ plane (kill Y component) and renormalize
        const glm::vec3 flat = glm::vec3{local.x, 0.f, local.z};

        // Guard against looking straight up/down
        if (glm::length(flat) < 1e-6f) return local;

        return glm::normalize(flat);
    }

    inline void Transform::rotateAroundAxis(const glm::vec3 &axis, const float angleRadians) {
        rotation = glm::angleAxis(angleRadians, axis) * rotation;
    }

    inline void Transform::lookAt(const glm::vec3 &target) {
        const glm::vec3 dir = glm::normalize(position - target);

        // Degenerate case — target is directly above or below, up axis is ambiguous.
        if (glm::abs(glm::dot(dir, Dir3D::UP)) > 0.9999f) return;

        rotation = glm::quatLookAt(dir, Dir3D::UP);
    }

    inline glm::vec3 Transform::toEuler() const {
        return glm::eulerAngles(rotation);
    }

    inline Transform Transform::fromEuler(const glm::vec3 eulerRadians,
                                          const glm::vec3 position,
                                          const glm::vec3 scale) {
        return Transform {
            position,
            glm::quat{eulerRadians},
            scale
        };
    }

    inline Transform Transform::lerp(const Transform& a, const Transform& b, const float t) {
        return Transform {
            glm::mix(a.position, b.position, t),
            glm::slerp(a.rotation, b.rotation, t),
            glm::mix(a.scale, b.scale, t)
        };
    }

    inline float Transform::distanceTo(const Transform& other) const {
        return glm::distance(position, other.position);
    }

    inline glm::vec3 Transform::directionTo(const Transform& other) const {
        return glm::normalize(other.position - position);
    }


    // ==================== GENERIC FUNCTIONS ====================

    inline glm::vec3 rotate(const glm::vec3& p, const glm::quat& q) {
        return q * p;
    }

    inline glm::vec3 rotateInverse(const glm::vec3& p, const glm::quat& q) {
        return glm::conjugate(q) * p;
    }

    inline glm::vec3 scalePoint(const glm::vec3& p, const glm::vec3& scale) {
        return p * scale;
    }

    inline glm::vec3 applyTransform(const glm::vec3& p, const Transform& transform) {
        return transform.transformPoint(p);
    }

    // ==================== CAMERA3D ====================

    inline glm::mat4 Camera3D::viewMatrix() const {
        // View matrix is the inverse of the camera's world transform.
        // glm::lookAt is equivalent but building from the transform directly
        // avoids redundant decomposition.
        const glm::mat4 R = glm::toMat4(glm::conjugate(transform.rotation));
        const glm::mat4 T = glm::translate(glm::mat4{1.f}, -transform.position);
        return R * T;
    }

    inline glm::mat4 Camera3D::projectionMatrix(const float aspectRatio) const {
        //return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
        return glm::perspectiveLH_ZO(fov, aspectRatio, nearPlane, farPlane);
    }

    inline glm::mat4 Camera3D::viewProjectionMatrix(const float aspectRatio) const {
        return projectionMatrix(aspectRatio) * viewMatrix();
    }

    inline glm::vec3 Camera3D::yawDirection(const glm::vec3& direction) const {
        const float yaw = glm::yaw(transform.rotation);
        return glm::angleAxis(yaw, Dir3D::UP) * direction;
    }

    // ==================== RAY3D ====================

    inline glm::vec3 Ray3D::at(float t) const {
        return origin + direction * t;
    }

    inline float Ray3D::distanceTo(const glm::vec3& point) const {
        // Project point onto ray, then measure perpendicular distance.
        const glm::vec3 toPoint = point - origin;
        const float proj = glm::dot(toPoint, direction);
        const glm::vec3 closest = at(glm::max(proj, 0.f));
        return glm::distance(closest, point);
    }

    inline bool Ray3D::intersectsSphere(const glm::vec3 &center, float radius, float &t) const {
        // Solve quadratic: |origin + t*dir - center|^2 = radius^2
        const glm::vec3 oc = origin - center;
        const float a = glm::dot(direction, direction); // 1 if normalized
        const float b = 2.f * glm::dot(oc, direction);
        const float c = glm::dot(oc, oc) - radius * radius;
        const float discriminant = b * b - 4.f * a * c;

        if (discriminant < 0.f) return false;

        const float sqrtDisc = glm::sqrt(discriminant);
        const float t0 = (-b - sqrtDisc) / (2.f * a);
        const float t1 = (-b + sqrtDisc) / (2.f * a);

        // Take nearest positive t (in front of ray origin)
        if (t0 >= 0.f)      { t = t0; return true; }
        else if (t1 >= 0.f) { t = t1; return true; }
        return false;
    }

    inline bool Ray3D::intersectsPlane(const glm::vec3& normal,
                                   const float d, float& t) const {
        // Plane defined as dot(normal, p) = d.
        const float denom = glm::dot(normal, direction);

        // Ray is parallel to plane (or nearly so).
        if (glm::abs(denom) < 1e-6f) return false;

        t = (d - glm::dot(normal, origin)) / denom;
        return t >= 0.f;
    }

    inline bool Ray3D::intersectsAABB(const glm::vec3& min,
                                      const glm::vec3& max, float& t) const {
        // Slab method — intersect ray with each pair of axis-aligned planes.
        const glm::vec3 invDir = 1.f / direction;
        const glm::vec3 t0     = (min - origin) * invDir;
        const glm::vec3 t1     = (max - origin) * invDir;

        const glm::vec3 tMin = glm::min(t0, t1);
        const glm::vec3 tMax = glm::max(t0, t1);

        const float tEnter = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
        const float tExit  = glm::min(glm::min(tMax.x, tMax.y), tMax.z);

        if (tExit < 0.f || tEnter > tExit) return false;

        t = tEnter >= 0.f ? tEnter : tExit;
        return true;
    }

    inline Ray3D Ray3D::fromCamera(const Camera3D& cam,
                                   const glm::vec2 ndc,
                                   const float aspectRatio) {
        // Un-project NDC through the inverse view-projection matrix.
        const glm::mat4 invVP = glm::inverse(cam.viewProjectionMatrix(aspectRatio));

        // Two points on the ray in clip space (near and far plane).
        const glm::vec4 nearClip = invVP * glm::vec4{ndc.x, ndc.y, -1.f, 1.f};
        const glm::vec4 farClip  = invVP * glm::vec4{ndc.x, ndc.y,  1.f, 1.f};

        const glm::vec3 nearWorld = glm::vec3{nearClip} / nearClip.w;
        const glm::vec3 farWorld  = glm::vec3{farClip}  / farClip.w;

        return Ray3D {
            nearWorld,
            glm::normalize(farWorld - nearWorld)
        };
    }

    // ==================== AABB ====================

    inline glm::vec3 AABB::center() const {
        return (min + max) * 0.5f;
    }

    inline glm::vec3 AABB::size() const {
        return max - min;
    }

    inline bool AABB::contains(const glm::vec3& point) const {
        return glm::all(glm::greaterThanEqual(point, min)) &&
               glm::all(glm::lessThanEqual(point, max));
    }

    inline bool AABB::intersects(const AABB& other) const {
        return glm::all(glm::lessThanEqual(min, other.max)) &&
               glm::all(glm::greaterThanEqual(max, other.min));
    }

    inline AABB AABB::transformed(const Transform& t) const {
        // Transform all 8 corners and re-fit — necessary because rotation
        // and non-uniform scale mean the AABB axes no longer align.
        const glm::vec3 corners[8] = {
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {min.x, max.y, min.z}, {max.x, max.y, min.z},
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z},
        };

        glm::vec3 newMin = t.transformPoint(corners[0]);
        glm::vec3 newMax = newMin;

        for (int i = 1; i < 8; i++) {
            const glm::vec3 p = t.transformPoint(corners[i]);
            newMin = glm::min(newMin, p);
            newMax = glm::max(newMax, p);
        }

        return AABB{newMin, newMax};
    }

    inline AABB AABB::fromModel(const Model3D& model) {
        if (model.vertices.empty()) return AABB{};

        glm::vec3 newMin = model.vertices[0];
        glm::vec3 newMax = model.vertices[0];

        for (const auto& v : model.vertices) {
            newMin = glm::min(newMin, v);
            newMax = glm::max(newMax, v);
        }

        return AABB{newMin, newMax};
    }

    namespace Primitives {

        inline std::vector<Model3D::FaceIndex> tri(
            glm::i32 a, glm::i32 b, glm::i32 c,           // vertex indices
            glm::i32 ta, glm::i32 tb, glm::i32 tc,        // uv indices
            glm::i32 na, glm::i32 nb, glm::i32 nc)        // normal indices
        {
                return { {a, ta, na}, {b, tb, nb}, {c, tc, nc} };
        }

        // Overload for when vertex, uv, and normal indices are all the same
        inline std::vector<Model3D::FaceIndex> tri(glm::i32 a, glm::i32 b, glm::i32 c) {
            return tri(a, b, c, a, b, c, a, b, c);
        }

        inline Model3D cube() {
            Model3D m;

            // 8 corners
            m.vertices = {
                {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f},
                { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
                {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f},
                { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
            };

            // 6 face normals
            m.normals = {
                { 0,  0, -1}, { 0,  0,  1},
                {-1,  0,  0}, { 1,  0,  0},
                { 0, -1,  0}, { 0,  1,  0},
            };

            // UVs (each face maps to full [0,1])
            m.uvs = {
                {0, 0}, {1, 0}, {1, 1}, {0, 1}
            };

            // Each face: 2 triangles (CCW winding), all verts share the face normal
            // {vertexIdx, texCoordIdx, normalIdx}
            auto face = [&](int a, int b, int c, int d, int n) {
                m.faces.push_back({ {a,0,n},{b,1,n},{c,2,n} });
                m.faces.push_back({ {a,0,n},{c,2,n},{d,3,n} });
            };

            face(0, 1, 2, 3, 0); // back
            face(5, 4, 7, 6, 1); // front
            face(4, 0, 3, 7, 2); // left
            face(1, 5, 6, 2, 3); // right
            face(4, 5, 1, 0, 4); // bottom
            face(3, 2, 6, 7, 5); // top

            return m;
        }

        inline Model3D sphere(const float radius, const glm::u32 sectors, const glm::u32 stacks) {
            Model3D m;

            for (glm::u32 i = 0; i <= stacks; i++) {
                const float phi    = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(stacks); // 0..pi
                const float sinPhi = glm::sin(phi);
                const float cosPhi = glm::cos(phi);

                for (glm::u32 j = 0; j <= sectors; j++) {
                    const float theta    = 2.f * glm::pi<float>() * static_cast<float>(j) / static_cast<float>(sectors);
                    const float sinTheta = glm::sin(theta);
                    const float cosTheta = glm::cos(theta);

                    const glm::vec3 normal { sinPhi * cosTheta, cosPhi, sinPhi * sinTheta };
                    m.vertices.push_back(normal * radius);
                    m.normals.push_back(normal);
                    m.uvs.emplace_back( static_cast<float>(j) / static_cast<float>(sectors),
                                      static_cast<float>(i) / static_cast<float>(stacks) );
                }
            }

            // Build faces
            for (glm::u32 i = 0; i < stacks; i++) {
                for (glm::u32 j = 0; j < sectors; j++) {
                    const glm::u32 a = i       * (sectors + 1) + j;
                    const glm::u32 b = a       + 1;
                    const glm::u32 c = (i + 1) * (sectors + 1) + j;
                    const glm::u32 d = c       + 1;

                    if (i != 0)          // skip degenerate top cap
                        m.faces.push_back(tri(a, d, b));
                    if (i != stacks - 1) // skip degenerate bottom cap
                        m.faces.push_back(tri(a, c, d));
                }
            }

            return m;
        }

        inline Model3D grid(const float width, const float depth,
                            const glm::u32 cellsX, const glm::u32 cellsZ) {
            Model3D m;

            const float dx = width / static_cast<float>(cellsX);
            const float dz = depth / static_cast<float>(cellsZ);

            for (glm::u32 z = 0; z <= cellsZ; z++) {
                for (glm::u32 x = 0; x <= cellsX; x++) {
                    m.vertices.emplace_back( -width * 0.5f + static_cast<float>(x) * dx,
                                            0.f,
                                           -depth * 0.5f + static_cast<float>(z) * dz );
                    m.normals.push_back(Dir3D::UP);
                    m.uvs.emplace_back( static_cast<float>(x) / static_cast<float>(cellsX),
                                       static_cast<float>(z) / static_cast<float>(cellsZ) );
                }
            }

            for (glm::u32 z = 0; z < cellsZ; z++) {
                for (glm::u32 x = 0; x < cellsX; x++) {
                    const glm::u32 a = z       * (cellsX + 1) + x;
                    const glm::u32 b = a       + 1;
                    const glm::u32 c = (z + 1) * (cellsX + 1) + x;
                    const glm::u32 d = c       + 1;

                    m.faces.push_back(tri(a,b,d));
                    m.faces.push_back(tri(a,d,c));
                }
            }

            return m;
        }

        inline Model3D cylinder(const float radius, const float height, const glm::u32 sectors) {
            Model3D m;

            const float half = height * 0.5f;
            const float step = 2.f * glm::pi<float>() / static_cast<float>(sectors);

            // Barrel vertices — two rings (bottom and top)
            for (glm::u32 i = 0; i < 2; i++) {
                const float y = i == 0 ? -half : half;
                for (glm::u32 j = 0; j <= sectors; j++) {
                    const float theta = static_cast<float>(j) * step;
                    const glm::vec3 normal { glm::cos(theta), 0.f, glm::sin(theta) };
                    m.vertices.emplace_back( normal.x * radius, y, normal.z * radius );
                    m.normals.push_back(normal);
                    m.uvs.emplace_back( static_cast<float>(j) / static_cast<float>(sectors),
                                       static_cast<float>(i) );
                }
            }

            // Barrel faces
            for (glm::u32 j = 0; j < sectors; j++) {
                const glm::u32 a = j;
                const glm::u32 b = j + 1;
                const glm::u32 c = j + sectors + 1;
                const glm::u32 d = c + 1;
                m.faces.push_back(tri(a, d, c));
                m.faces.push_back(tri(a, b, d));
            }

            // Caps
            auto addCap = [&](const float y, const glm::vec3& normal) {
                const auto centerIdx = static_cast<glm::u32>(m.vertices.size());
                m.vertices.emplace_back(0.f, y, 0.f);
                m.normals.push_back(normal);
                m.uvs.emplace_back(0.5f, 0.5f);

                const auto ringStart = static_cast<glm::u32>(m.vertices.size());
                for (glm::u32 j = 0; j <= sectors; j++) {
                    const float theta = static_cast<float>(j) * step;
                    m.vertices.emplace_back( glm::cos(theta) * radius, y,
                                            glm::sin(theta) * radius );
                    m.normals.push_back(normal);
                    m.uvs.emplace_back( glm::cos(theta) * 0.5f + 0.5f,
                                       glm::sin(theta) * 0.5f + 0.5f );
                }

                for (glm::u32 j = 0; j < sectors; j++) {
                    const glm::u32 a = ringStart + j;
                    const glm::u32 b = ringStart + j + 1;
                    if (normal.y > 0)
                        m.faces.push_back({{ {static_cast<glm::i32>(centerIdx),0,static_cast<glm::i32>(centerIdx)},
                                              {static_cast<glm::i32>(a),        0,static_cast<glm::i32>(a)},
                                              {static_cast<glm::i32>(b),        0,static_cast<glm::i32>(b)} }});
                    else
                        m.faces.push_back({{ {static_cast<glm::i32>(centerIdx),0,static_cast<glm::i32>(centerIdx)},
                                              {static_cast<glm::i32>(b),        0,static_cast<glm::i32>(b)},
                                              {static_cast<glm::i32>(a),        0,static_cast<glm::i32>(a)} }});
                }
            };

            addCap(-half, Dir3D::DOWN);
            addCap( half, Dir3D::UP);

            return m;
        }

        inline Model3D cone(const float radius, const float height, const glm::u32 sectors) {
            Model3D m;

            const float half = height * 0.5f;
            const float step = 2.f * glm::pi<float>() / static_cast<float>(sectors);

            // Apex
            m.vertices.emplace_back(0.f, half, 0.f);
            m.normals.push_back(Dir3D::UP);
            m.uvs.emplace_back(0.5f, 1.f);

            // Base ring
            for (glm::u32 j = 0; j <= sectors; j++) {
                const auto jf = static_cast<float>(j);
                const float theta  = jf * step;
                const float cx     = glm::cos(theta);
                const float cz     = glm::sin(theta);
                const glm::vec3 n  = glm::normalize(glm::vec3{cx, radius / height, cz});
                m.vertices.emplace_back( cx * radius, -half, cz * radius );
                m.normals.push_back(n);
                m.uvs.emplace_back( jf / static_cast<float>(sectors), 0.f );
            }

            // Surface faces
            for (glm::u32 j = 0; j < sectors; j++) {
                constexpr glm::u32 ringStart = 1;
                constexpr glm::u32 apexIdx = 0;
                const glm::u32 a = ringStart + j;
                const glm::u32 b = ringStart + j + 1;
                m.faces.push_back({{ {static_cast<glm::i32>(apexIdx), 0, static_cast<glm::i32>(apexIdx)},
                                       {static_cast<glm::i32>(a),       0, static_cast<glm::i32>(a)},
                                       {static_cast<glm::i32>(b),       0, static_cast<glm::i32>(b)} }});
            }

            // Base cap
            const auto capCenter = static_cast<glm::u32>(m.vertices.size());
            m.vertices.emplace_back(0.f, -half, 0.f);
            m.normals.push_back(Dir3D::DOWN);
            m.uvs.emplace_back(0.5f, 0.5f);

            const auto capRing = static_cast<glm::u32>(m.vertices.size());
            for (glm::u32 j = 0; j <= sectors; j++) {
                const float theta = static_cast<float>(j) * step;
                m.vertices.emplace_back( glm::cos(theta) * radius, -half,
                                        glm::sin(theta) * radius );
                m.normals.push_back(Dir3D::DOWN);
                m.uvs.emplace_back( glm::cos(theta) * 0.5f + 0.5f,
                                   glm::sin(theta) * 0.5f + 0.5f );
            }

            for (glm::u32 j = 0; j < sectors; j++) {
                const glm::u32 a = capRing + j;
                const glm::u32 b = capRing + j + 1;
                m.faces.push_back({{ {static_cast<glm::i32>(capCenter), 0, static_cast<glm::i32>(capCenter)},
                                       {static_cast<glm::i32>(b),         0, static_cast<glm::i32>(b)},
                                       {static_cast<glm::i32>(a),         0, static_cast<glm::i32>(a)} }});
            }

            return m;
        }

        inline Model3D quad(const float width, const float height) {
            Model3D m;

            const float hw = width  * 0.5f;
            const float hh = height * 0.5f;

            m.vertices = { {-hw, -hh, 0}, { hw, -hh, 0},
                            { hw,  hh, 0}, {-hw,  hh, 0} };
            m.normals  = { Dir3D::FORWARD };
            m.uvs      = { {0,0}, {1,0}, {1,1}, {0,1} };

            m.faces.push_back({{ {0,0,0},{1,1,0},{2,2,0} }});
            m.faces.push_back({{ {0,0,0},{2,2,0},{3,3,0} }});

            return m;
        }

    } // namespace Primitives

}
#endif

#endif //LATTICE_PRIMITIVES_H
