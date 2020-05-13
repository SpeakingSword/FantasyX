#ifndef TYPES_H
#define TYPES_H

#include <list>

#include "bitree.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glad\glad.h>

namespace fx {

    typedef glm::vec2 Vector2;
    typedef glm::vec3 Vector3;
    typedef glm::vec4 Vector4;
    typedef glm::mat4 Matrix4x4;

    const Vector3 WORLD_FRONT(0.0f, 0.0f, 1.0f);
    const Vector3 WORLD_RIGHT(1.0f, 0.0f, 0.0f);
    const Vector3 WORLD_UP(0.0f, 1.0f, 0.0f);
    
    typedef enum {
        COM_NONE = 0,
        COM_TRANSFORM,
        COM_LIGHT,
        COM_BOUNDINGBOX,
        COM_BOUNDINGSPHERE,
        COM_MESHFILTER,
        COM_RENDER,
        COM_CAMERA,
        COM_SCRIPT
    }ComponentType;
    
    typedef enum{
        WORLD,
        SELF
    }Space;

    typedef enum {
        GT_NONE,
        GT_OPAQUE,
        GT_TRANSPARENT,
        GT_LIGHT,
        GT_CAMERA
    }GameObjectTagType;

    // 三个常数会控制光的衰减距离
    // 大于衰减距离光照就会不起作用
    typedef struct {
        GLfloat constant;
        GLfloat linear;
        GLfloat quadratic;
    }Attenuation;

    // 囊括相机所有的移动方式
    typedef enum {
        CAM_FORWARD,
        CAM_BACKWARD,
        CAM_RIGHT,
        CAM_LEFT,
        CAM_YAW,
        CAM_PITCH,
        CAM_SURROUND_Y,
        CAM_SURROUND_X,
        CAM_ZOOM_IN,
        CAM_ZOOM_OUT
    }CameraMovement;

    typedef enum {
        CAM_FIRST_PERSON,
        CAM_ORBIT
    }CameraOperationMode;

    typedef enum {
        CAM_PERSPECTIVE,
        CAM_ORTHOGRAPHIC
    }CameraProjection;

    typedef struct {
        Vector3 position;
        Vector2 texcoord;
        Vector3 normal;
        Vector3 tangent;
        Vector3 bitangent;
    }Vertex;


    typedef enum {
        POST_NONE,
        POST_INVERSE,
        POST_MONOCHROME
    }PostProcessing;

    typedef enum {
        DRAW_WITH_DOTS,
        DRAW_WITH_LINES,
        DRAW_WITH_FACES
    }DrawMode;

    typedef enum {
        DISPLAY_ALL,
        DISPLAY_ALL_NO_POST_EFFECT,
        DISPLAY_ONLY_ALBEDO_MAP,
        DISPLAY_ONLY_NORMAL_MAP,
        DISPLAY_ONLY_METALLIC_MAP,
        DISPLAY_ONLY_ROUGHNESS_MAP,
        DISPLAY_ONLY_AO_MAP,
        DISPLAY_ONLY_POS_MAP,
        DISPLAY_ONLY_DEPTH_MAP
    }FinalDispalayType;

    /*
    typedef struct {
        GLuint buffer_id;
        GLuint pos_depth_map;
        GLuint albedo_metallic_map;
        GLuint normal_roughness_map;
        GLuint ao_map;
    }Gbuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint buffer_map;
    }DirLightDepthBuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint buffer_map;
    }DirectLightCalculateBuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint hdr_cube_map;
        GLuint hdr_cube_map_width;
        GLuint irradiance_map;
        GLuint irradiance_map_width;
        GLuint prefilter_map;
        GLuint prefilter_map_width;
        GLuint brdf_map;
        GLuint brdf_map_width;
    }IndirectLightCalculateBuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint buffer_map;
    }CombineMapBuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint buffer_map;
    }HightLightBuffer;

    typedef struct {
        GLuint buffer_id[2];
        GLuint buffer_map[2];
    }PingPongBuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint buffer_map;
    }FxaaEffectBuffer;

    typedef struct {
        GLuint buffer_id;
        GLuint buffer_map;
    }PostProcessingBuffer;
    */
}

#endif // !TYPES_H


