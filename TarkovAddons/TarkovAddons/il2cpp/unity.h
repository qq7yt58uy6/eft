#pragma once
#include "libil2cpp/il2cpp-api-types.h"
#include <Windows.h>

#include "Il2CppObjectInstance.h"
#include "libil2cpp/il2cpp-object-internals.h"
#include "il2utils.h"

#define PI 3.14159265359f

namespace unity
{
    struct vector2
    {
        float x, y;

        vector2() { x = y = 0.f; }

        vector2(const float f1, const float f2)
        {
            x = f1;
            y = f2;
        }
    };

    struct vector3
    {
        float x, y, z;

        vector3() { x = y = z = 0.f; }

        vector3(const float f1, const float f2, const float f3)
        {
            x = f1;
            y = f2;
            z = f3;
        }

        [[nodiscard]] vector3 add(const vector3 b) const
        {
            return {x + b.x, y + b.y, z + b.z};
        }

        [[nodiscard]] float distance(const vector3 b) const
        {
            return static_cast<float>(sqrt(pow(x - b.x, 2) + pow(y - b.y, 2) + pow(z - b.z, 2)));
        }

        [[nodiscard]] float length() const
        {
            return x * x + y * y + z * z;
        }

        [[nodiscard]] float dot(const vector3 b) const
        {
            return x * b.x + y * b.y + z * b.z;
        }

        vector3 normalize()
        {
            if (const float len = length(); len > 0)
                return {x / len, y / len, z / len};
            return {x, y, z};
        }

        void to_vectors(vector3* m_p_forward, vector3* m_p_right, vector3* m_p_up) const
        {
            constexpr float m_f_deg2_rad = PI / 180.f;

            const float m_f_sin_x = sinf(x * m_f_deg2_rad);
            const float m_f_cos_x = cosf(x * m_f_deg2_rad);

            const float m_f_sin_y = sinf(y * m_f_deg2_rad);
            const float m_f_cos_y = cosf(y * m_f_deg2_rad);

            const float m_f_sin_z = sinf(z * m_f_deg2_rad);
            const float m_f_cos_z = cosf(z * m_f_deg2_rad);

            if (m_p_forward)
            {
                m_p_forward->x = m_f_cos_x * m_f_cos_y;
                m_p_forward->y = -m_f_sin_x;
                m_p_forward->z = m_f_cos_x * m_f_sin_y;
            }

            if (m_p_right)
            {
                m_p_right->x = -1.f * m_f_sin_z * m_f_sin_x * m_f_cos_y + -1.f * m_f_cos_z * -m_f_sin_y;
                m_p_right->y = -1.f * m_f_sin_z * m_f_cos_x;
                m_p_right->z = -1.f * m_f_sin_z * m_f_sin_x * m_f_sin_y + -1.f * m_f_cos_z * m_f_cos_y;
            }

            if (m_p_up)
            {
                m_p_up->x = m_f_cos_z * m_f_sin_x * m_f_cos_y + -m_f_sin_z * -m_f_sin_y;
                m_p_up->y = m_f_cos_z * m_f_cos_x;
                m_p_up->z = m_f_cos_z * m_f_sin_x * m_f_sin_y + -m_f_sin_z * m_f_cos_y;
            }
        }
    };

    struct vector4
    {
        float x, y, z, w;

        vector4() { x = y = z = w = 0.f; }

        vector4(const float f1, const float f2, const float f3, const float f4)
        {
            x = f1;
            y = f2;
            z = f3;
            w = f4;
        }
    };

    struct quaternion
    {
        float x, y, z, w;

        quaternion() { x = y = z = w = 0.f; }

        quaternion(const float f1, const float f2, const float f3, const float f4)
        {
            x = f1;
            y = f2;
            z = f3;
            w = f4;
        }

        quaternion euler(float m_f_x, float m_f_y, float m_f_z)
        {
            constexpr float m_f_deg2_rad = PI / 180.f;

            m_f_x = m_f_x * m_f_deg2_rad * 0.5f;
            m_f_y = m_f_y * m_f_deg2_rad * 0.5f;
            m_f_z = m_f_z * m_f_deg2_rad * 0.5f;

            const float m_f_sin_x = sinf(m_f_x);
            const float m_f_cos_x = cosf(m_f_x);

            const float m_f_sin_y = sinf(m_f_y);
            const float m_f_cos_y = cosf(m_f_y);

            const float m_f_sin_z = sinf(m_f_z);
            const float m_f_cos_z = cosf(m_f_z);

            x = m_f_cos_y * m_f_sin_x * m_f_cos_z + m_f_sin_y * m_f_cos_x * m_f_sin_z;
            y = m_f_sin_y * m_f_cos_x * m_f_cos_z - m_f_cos_y * m_f_sin_x * m_f_sin_z;
            z = m_f_cos_y * m_f_cos_x * m_f_sin_z - m_f_sin_y * m_f_sin_x * m_f_cos_z;
            w = m_f_cos_y * m_f_cos_x * m_f_cos_z + m_f_sin_y * m_f_sin_x * m_f_sin_z;

            return *this;
        }

        quaternion euler(const vector3 m_v_rot)
        {
            return euler(m_v_rot.x, m_v_rot.y, m_v_rot.z);
        }

        [[nodiscard]] vector3 to_euler() const
        {
            vector3 m_v_euler;

            const float m_f_dist = (x * x) + (y * y) + (z * z) + (w * w);

            const float m_f_test = x * w - y * z;
            if (m_f_test > 0.4995f * m_f_dist)
            {
                m_v_euler.x = PI * 0.5f;
                m_v_euler.y = 2.f * atan2f(y, x);
                m_v_euler.z = 0.f;
            }
            else if (m_f_test < -0.4995f * m_f_dist)
            {
                m_v_euler.x = PI * -0.5f;
                m_v_euler.y = -2.f * atan2f(y, x);
                m_v_euler.z = 0.f;
            }
            else
            {
                m_v_euler.x = asinf(2.f * (w * x - y * z));
                m_v_euler.y = atan2f(2.f * w * y + 2.f * z * x, 1.f - 2.f * (x * x + y * y));
                m_v_euler.z = atan2f(2.f * w * z + 2.f * x * y, 1.f - 2.f * (z * z + x * x));
            }

            constexpr float m_f_rad_2deg = 180.f / PI;
            m_v_euler.x *= m_f_rad_2deg;
            m_v_euler.y *= m_f_rad_2deg;
            m_v_euler.z *= m_f_rad_2deg;

            return m_v_euler;
        }
    };

    struct bounds
    {
        vector3 m_v_center;
        vector3 m_v_extents;
    };

    struct plane
    {
        vector3 m_v_normal;
        float f_distance;
    };

    struct ray
    {
        vector3 m_v_origin;
        vector3 m_v_direction;
    };

    struct rect
    {
        float f_x, f_y;
        float f_width, f_height;

        rect() { f_x = f_y = f_width = f_height = 0.f; }

        rect(const float f1, const float f2, const float f3, const float f4)
        {
            f_x = f1;
            f_y = f2;
            f_width = f3;
            f_height = f4;
        }
    };

    struct color
    {
        float r, g, b, a;

        color() { r = g = b = a = 0.f; }

        explicit color(const float f_red = 0.f, const float f_green = 0.f, const float f_blue = 0.f,
                       const float f_alpha = 1.f)
        {
            r = f_red;
            g = f_green;
            b = f_blue;
            a = f_alpha;
        }
    };

    struct matrix4_x4
    {
        float m[4][4] = {};

        float* operator[](int i) { return m[i]; }
    };

    template <typename TKey, typename TValue>
    struct dict : Il2CppObject
    {
        struct entry
        {
            int m_i_hash_code;
            int m_i_next;
            TKey m_t_key;
            TValue m_t_value;
        };

        Il2CppArray* m_p_buckets;
        Il2CppArray* m_p_entries;
        int m_i_count;
        int m_i_version;
        int m_i_free_list;
        int m_i_free_count;
        void* m_p_comparer;
        void* m_p_keys;
        void* m_p_values;

        entry* get_entry()
        {
            return reinterpret_cast<entry*>(&m_p_entries->data);
        }

        TKey get_key_by_index(int i_index)
        {
            TKey t_key = {0};

            entry* p_entry = get_entry();
            if (p_entry)
                t_key = p_entry[i_index].m_t_key;

            return t_key;
        }

        TValue get_value_by_index(int i_index)
        {
            TValue t_value = {0};

            entry* p_entry = get_entry();
            if (p_entry)
                t_value = p_entry[i_index].m_t_value;

            return t_value;
        }

        TValue get_value_by_key(TKey t_key)
        {
            TValue t_value = {0};
            for (int i = 0; i < m_i_count; i++)
            {
                if (get_entry()[i].m_t_key == t_key)
                    t_value = get_entry()[i].m_t_value;
            }
            return t_value;
        }
    };

    struct list : Il2CppObject
    {
        Il2CppArray* m_p_list_array;

        [[nodiscard]] Il2CppArray* to_array() const { return m_p_list_array; }
    };

    using find_objects_of_type_sig = Il2CppArray*(*)(PVOID);
    static find_objects_of_type_sig find_objects_of_type_internal = nullptr;
    using find_object_of_type_sig = Il2CppObject*(*)(PVOID);
    static find_object_of_type_sig find_object_of_type_internal = nullptr;
    using get_name_sig = Il2CppString*(*)(Il2CppObject*);
    static get_name_sig get_name_internal = nullptr;
    using get_component_sig = Il2CppObject*(*)(Il2CppObject*, Il2CppObject*);
    static get_component_sig get_component_internal = nullptr;
    using get_position_sig = vector3(*)(Il2CppObject*);
    static get_position_sig get_position_internal = nullptr;
    using component_get_game_object_sig = Il2CppObject*(*)(Il2CppObject*);
    static component_get_game_object_sig component_get_game_object_internal = nullptr;
    using world_to_screen_point_sig = vector3(*)(Il2CppObject*, vector3);
    static world_to_screen_point_sig world_to_screen_point_internal = nullptr;
    using camera_get_current_sig = Il2CppObject*(*)();
    static camera_get_current_sig camera_get_current_internal = nullptr;
    using camera_get_all_sig = Il2CppArray*(*)();
    static camera_get_all_sig camera_get_all_internal = nullptr;
    using enumerable_to_array_sig = Il2CppArray*(*)(Il2CppObject*);
    static enumerable_to_array_sig enumerable_to_array_internal = nullptr;

    static const Il2CppClass* camera;

    static Il2CppObject* transform_type;

    inline void init()
    {
        Il2CppImage* unity_engine = il2utils::resolve_image("UnityEngine.CoreModule.dll");
        Il2CppImage* system_core = il2utils::resolve_image("System.Core.dll");
        const Il2CppClass* object = il2utils::resolve_class(unity_engine, "UnityEngine", "Object");
        const Il2CppClass* game_object = il2utils::resolve_class(unity_engine, "UnityEngine", "GameObject");
        const Il2CppClass* transform = il2utils::resolve_class(unity_engine, "UnityEngine", "Transform");
        const Il2CppClass* component = il2utils::resolve_class(unity_engine, "UnityEngine", "Component");
        const Il2CppClass* enumerable = il2utils::resolve_class(system_core, "System.Linq", "Enumerable");
        camera = il2utils::resolve_class(unity_engine, "UnityEngine", "Camera");

        find_objects_of_type_internal = il2utils::get_method<find_objects_of_type_sig>(object, "FindObjectsOfType", 1);
        find_object_of_type_internal = il2utils::get_method<find_object_of_type_sig>(object, "FindObjectOfType", 1);
        get_name_internal = il2utils::get_method<get_name_sig>(object, "GetName", 1);
        get_component_internal = il2utils::get_method<get_component_sig>(game_object, "GetComponent", 1);
        get_position_internal = il2utils::get_method<get_position_sig>(transform, "get_position", 0);
        component_get_game_object_internal = il2utils::get_method<component_get_game_object_sig>(
            component, "get_gameObject", 0);
        world_to_screen_point_internal = il2utils::get_method<world_to_screen_point_sig>(
            camera, "WorldToScreenPoint", 1);
        camera_get_current_internal = il2utils::get_method<camera_get_current_sig>(camera, "get_current", 0);
        camera_get_all_internal = il2utils::get_method<camera_get_all_sig>(camera, "get_allCameras", 0);
        enumerable_to_array_internal = il2utils::get_method<enumerable_to_array_sig>(enumerable, "ToArray", 1);

        transform_type = il2utils::get_system_type(unity_engine, "UnityEngine", "Transform");
    }

    inline Il2CppObject** find_objects_of_type(Il2CppObject* type, _Out_ size_t* amount)
    {
        Il2CppArray* array = find_objects_of_type_internal(type);
        *amount = array->max_length;
        return reinterpret_cast<Il2CppObject**>(&array->data);
    }

    inline Il2CppObject* find_object_of_type(Il2CppObject* type)
    {
        return find_object_of_type_internal(type);
    }

    inline std::string get_name(Il2CppObject* object)
    {
        Il2CppString* name = get_name_internal(object);
        return il2utils::conv_string(name);
    }

    inline vector3 transform_get_pos(Il2CppObject* transform)
    {
        return get_position_internal(transform);
    }

    inline Il2CppObject* component_get_game_object(Il2CppObject* component)
    {
        return component_get_game_object_internal(component);
    }

    inline Il2CppObject* gameobject_get_transform(Il2CppObject* game_object)
    {
        return get_component_internal(game_object, transform_type);
    }

    inline Il2CppObject* gameobject_get_component(Il2CppObject* game_object, Il2CppObject* system_type)
    {
        return get_component_internal(game_object, system_type);
    }

    inline vector3 world_to_screen_point(Il2CppObject* camera_inst, vector3 pos)
    {
        return world_to_screen_point_internal(camera_inst, pos);
    }

    inline int camera_pixel_height(Il2CppObject* camera_inst)
    {
        Il2CppObjectInstance inst(camera_inst);
        return inst.get_method<int(*)(Il2CppObject*)>("get_pixelHeight", 0)(camera_inst);
    }

    inline int camera_pixel_width(Il2CppObject* camera_inst)
    {
        Il2CppObjectInstance inst(camera_inst);
        return inst.get_method<int(*)(Il2CppObject*)>("get_pixelWidth", 0)(camera_inst);
    }

    inline float camera_aspect(Il2CppObject* camera_inst)
    {
        Il2CppObjectInstance inst(camera_inst);
        return inst.get_method<float(*)(Il2CppObject*)>("get_aspect", 0)(camera_inst);
    }

    inline float camera_fov(Il2CppObject* camera_inst)
    {
        Il2CppObjectInstance inst(camera_inst);
        return inst.get_method<float(*)(Il2CppObject*)>("get_fieldOfView", 0)(camera_inst);
    }

    inline Il2CppObject* get_current_camera()
    {
        return camera_get_current_internal();
    }

    inline Il2CppObject* get_named_camera(const char* name)
    {
        Il2CppArray* all = camera_get_all_internal();
        for (size_t i = 0; i < all->max_length; ++i)
        {
            Il2CppObject* cam = reinterpret_cast<Il2CppObject**>(&all->data)[i];
            if (get_name(cam) == std::string(name)) return cam;
        }
        return nullptr;
    }

    inline Il2CppArray* enumerable_to_array(Il2CppObject* enumerable)
    {
        return enumerable_to_array_internal(enumerable);
    }

    inline int transform_get_child_count(Il2CppObject* transform)
    {
        Il2CppObjectInstance instance(transform);
        return instance.get_method<int(*)(Il2CppObject*)>("get_childCount", 0)(transform);
    }

    inline Il2CppObject* transform_get_child(Il2CppObject* transform, const int index)
    {
        Il2CppObjectInstance instance(transform);
        return instance.get_method<Il2CppObject*(*)(Il2CppObject*, int)>("GetChild", 1)(transform, index);
    }

    inline Il2CppObject* transform_get_game_object(Il2CppObject* transform)
    {
        Il2CppObjectInstance instance(transform);
        return instance.get_method<Il2CppObject*(*)(Il2CppObject*)>("get_gameObject", 0)(transform);
    }

    inline void transform_apply_to_all_children(Il2CppObject* transform, const std::function<void(Il2CppObject*)>& func)
    {
        for (int i = 0; i < transform_get_child_count(transform); ++i)
        {
            Il2CppObject* child = transform_get_child(transform, i);
            transform_apply_to_all_children(child, func);
            func(child);
        }
    }
}
