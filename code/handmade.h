
// TODO: Implement sine ourselves
#include <math.h>
#include <stdint.h>
#include <string.h>

#define internal        static
#define local_persist   static
#define global_variable static

#define true            1
#define false           0

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef i32      b32;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float    f32;
typedef double   f64;

#define PI   3.14159265359f
#define PI_2 6.28318530718f

#if HANDMADE_SLOW
#define Assert(Expression)                                                     \
if(!(Expression))                                                          \
{                                                                          \
__builtin_trap();                                                      \
}
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value)  ((u64)(Value) * 1024)
#define Megabytes(Value)  (Kilobytes(Value) * 1024)
#define Gigabytes(Value)  (Megabytes(Value) * 1024)
#define Terabytes(Value)  (Gigabytes(Value) * 1024)

#define ArrayCount(Array) ((i32)(sizeof(Array) / sizeof((Array)[0])))

internal u32
SafeTruncate_u64(u64 Value)
{
    Assert(Value <= 0xFFFFFFFF);
    u32 Result = (u32)Value;
    return Result;
}

typedef struct thread_context
{
    i32 Placeholder;
} thread_context;

#if HANDMADE_INTERNAL

typedef struct debug_read_file_result
{
    u32   ContentsSize;
    void *Contents;
} debug_read_file_result;

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(void *Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name)                                  \
debug_read_file_result name(char *Filename)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name)                                 \
b32 name(char *Filename, u32 MemorySize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);

#endif

typedef struct game_offscreen_buffer
{
    void *Memory;
    i32   Width;
    i32   Height;
    i32   Pitch;
    i32   BytesPerPixel;
} game_offscreen_buffer;

typedef struct game_sound_output_buffer
{
    i32  SampleFramesPerSecond;
    i32  SampleFramesToWrite;
    i16 *Memory;
} game_sound_output_buffer;

typedef struct game_controller_input
{
    b32 IsConnected;
    
    f32 L_StickAverageX;
    f32 L_StickAverageY;
    f32 R_StickAverageX;
    f32 R_StickAverageY;
    
    b32 ActionUp;
    b32 ActionDown;
    b32 ActionLeft;
    b32 ActionRight;
    
    b32 LeftShoulder;
    b32 RightShoulder;
    
    b32 Back;
    b32 Start;
    
    
} game_controller_input;

typedef struct game_input
{
    f32                   dtForFrame;
    
    game_controller_input Controllers[5];
} game_input;

internal game_controller_input *
GetController(game_input *Input, i32 ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(Input->Controllers));
    game_controller_input *Result = &Input->Controllers[ControllerIndex];
    return Result;
}

typedef struct game_memory
{
    b32                               IsInitialized;
    
    u64                               PermanentStorageSize;
    void                             *PermanentStorage;
    
    u64                               TransientStorageSize;
    void                             *TransientStorage;
    
    debug_platform_free_file_memory  *DEBUGPlatformFreeFileMemory;
    debug_platform_read_entire_file  *DEBUGPlatformReadEntireFile;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
} game_memory;

#define GAME_UPDATE_AND_RENDER(name)                                           \
void name(thread_context *Thread, game_memory *Memory, game_input *Input, game_sound_output_buffer *Sound)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

internal f32
Sqrt_f32(f32 Value)
{
    return sqrtf(Value);
}

typedef union vec3
{
    struct
    {
        f32 x, y, z;
    };
    f32 E[3];
} vec3;

internal vec3
_Vec3(f32 x, f32 y, f32 z)
{
    vec3 Result = {
        {x, y, z}
    };
    return Result;
}

internal vec3
Add_vec3(vec3 A, vec3 B)
{
    vec3 Result;
    
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    
    return Result;
}

internal vec3
Minus_vec3(vec3 A, vec3 B)
{
    vec3 Result;
    
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    
    return Result;
}

internal f32
Inner_vec3(vec3 A, vec3 B)
{
    f32 Result = A.x * B.x + A.y * B.y + A.z * B.z;
    
    return Result;
}

internal vec3
Cross_vec3(vec3 A, vec3 B)
{
    vec3 Result;
    
    Result.x = A.y * B.z - A.z * B.y;
    Result.y = A.z * B.x - A.x * B.z;
    Result.z = A.x * B.y - A.y * B.x;
    
    return Result;
}

internal vec3
Multiply_f32_vec3(f32 A, vec3 B)
{
    vec3 Result;
    Result.x = A * B.x;
    Result.y = A * B.y;
    Result.z = A * B.z;
    return Result;
}

internal f32
LengthSq_vec3(vec3 A)
{
    return Inner_vec3(A, A);
}

internal f32
Length_vec3(vec3 A)
{
    return Sqrt_f32(LengthSq_vec3(A));
}

internal vec3
Normalize_vec3(vec3 A)
{
    return Multiply_f32_vec3(1.0f / Length_vec3(A), A);
}

typedef union vec4
{
    struct
    {
        f32 x, y, z, w;
    };
    f32 E[4];
    
} vec4;

internal vec4
_Vec4(f32 x, f32 y, f32 z, f32 w)
{
    vec4 Result = {
        {x, y, z, w}
    };
    return Result;
}

typedef struct mat4
{
    // each vec4 is a column vector
    vec4 Column[4];
} mat4;

internal mat4
_Identity_mat4(void)
{
    return (mat4){_Vec4(1.0f, 0.0f, 0.0f, 0.0f), _Vec4(0.0f, 1.0f, 0.0f, 0.0f), _Vec4(0.0f, 0.0f, 1.0f, 0.0f), _Vec4(0.0f, 0.0f, 0.0f, 1.0f)};
}

internal f32
Inner_vec4(vec4 Vector, vec4 Other)
{
    return Vector.x * Other.x + Vector.y * Other.y + Vector.z * Other.z +
        Vector.w * Other.w;
}

internal vec4
Multiply_mat4_vec4(mat4 Matrix, vec4 Vector)
{
    vec4 Result;
    Result.x = Inner_vec4(Matrix.Column[0], Vector);
    Result.y = Inner_vec4(Matrix.Column[1], Vector);
    Result.z = Inner_vec4(Matrix.Column[2], Vector);
    Result.w = Inner_vec4(Matrix.Column[3], Vector);
    return Result;
}

internal void
Transpose_mat4(mat4 *Matrix)
{
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            f32 Temp               = Matrix->Column[i].E[j];
            Matrix->Column[i].E[j] = Matrix->Column[j].E[i];
            Matrix->Column[j].E[i] = Temp;
        }
    }
}

internal mat4
Multiply_mat4_mat4(mat4 Matrix, mat4 Other)
{
    mat4 Result;
    Transpose_mat4(&Other);
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            Result.Column[i].E[j] = Inner_vec4(Matrix.Column[i],
                                               Other.Column[j]);
        }
    }
    
    return Result;
}

typedef struct quaternion
{
    f32 x, y, z, w;
} quaternion;

internal quaternion
_Quaternion(f32 Radians, vec3 UnitAxis)
{
    quaternion Result;
    f32        HalfTheta    = Radians / 2.0f;
    f32        SinHalfTheta = sinf(HalfTheta);
    
    Result.w                = cosf(HalfTheta);
    Result.x                = UnitAxis.x * SinHalfTheta;
    Result.y                = UnitAxis.y * SinHalfTheta;
    Result.z                = UnitAxis.z * SinHalfTheta;
    
    return Result;
}

// IMPORTANT: conjugate function assumes normalized quaternion
// NOTE: If axis of rotation that quaternion is constructed from 
//       is a unit vector, then the quaternion will be a unit quaternion
internal quaternion
Conjugate_quaternion(quaternion Quaternion)
{
    return (quaternion){-Quaternion.x, -Quaternion.y, -Quaternion.z,
        Quaternion.w};
}

internal quaternion
Multiply_quaternion_vec3(quaternion q, vec3 v)
{
    f32 w = -(q.x * v.x) - (q.y * v.y) - (q.z * v.z);
    f32 x = (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
    f32 y = (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
    f32 z = (q.w * v.z) + (q.x * v.y) - (q.y * v.x);
    
    return (quaternion){x, y, z, w};
}

internal quaternion
Multiply_quaternion_quaternion(quaternion q1, quaternion q2)
{
    f32 w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
    f32 x = (q1.x * q2.w) + (q1.w * q2.x) + (q1.y * q2.z) - (q1.z * q2.y);
    f32 y = (q1.y * q2.w) + (q1.w * q2.y) + (q1.z * q2.x) - (q1.x * q2.z);
    f32 z = (q1.z * q2.w) + (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x);
    
    return (quaternion){x, y, z, w};
}

internal vec3
Rotate(quaternion q, vec3 v)
{
    quaternion Result = Multiply_quaternion_quaternion(
                                                       Multiply_quaternion_vec3(q, v), Conjugate_quaternion(q));
    
    return _Vec3(Result.x, Result.y, Result.z);
}

typedef struct memory_arena
{
    u8 *BaseAddress;
    u64 RemainingSpace;
} memory_arena;

typedef struct matrices
{
    mat4 Scale;
    mat4 Rotation;
    mat4 Translation;
} matrices;


typedef struct global_matrices
{
    mat4 View;
    mat4 Perspective;
} global_matrices;

typedef struct camera_basis
{
    // IMPORTANT: Cross products assume left handed system with +z going into the screen 
    vec3 WorldUp;
    vec3 LookAt;
    vec3 Up;
    vec3 Right;
    
};

typedef struct camera_orientation
{
    vec3 Position;
    f32 YAngle;
    f32 UAngle;
} camera_orientation;

typedef struct camera
{
    camera_basis Basis;
    camera_orientation Orientation;
    vec3 PlayerOffset;
    f32 Far;
    f32 Near;
} camera;


typedef struct cube
{
    vec3 Position;
    matrices Matrices;
    
} cube;


typedef struct player
{
    camera Camera;
    cube Cube;
};

typedef struct game_state
{
    memory_arena *Arena;
    player Player;
    global_matrices GlobalMatrices;
    
    
    
    
    u8 *GrassTexture;
    
} game_state;


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wconversion"
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wextra"
#pragma clang diagnostic ignored "-Wpedantic"
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma clang diagnostic pop

