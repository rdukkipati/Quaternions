
// TODO: Implement sine ourselves
#include <math.h>
#include <stdint.h>

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

typedef struct game_button_state
{
    i32 HalfTransitionCount;
    b32 EndedDown;
} game_button_state;

typedef struct game_controller_input
{
    b32 IsConnected;
    b32 IsAnalog;
    f32 StickAverageX;
    f32 StickAverageY;
    
    union
    {
        game_button_state Buttons[12];
        struct
        {
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;
            
            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;
            
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
            
            game_button_state Back;
            game_button_state Start;
            
            // NOTE(casey): All buttons must be added above this line
            
            game_button_state Terminator;
        };
    };
} game_controller_input;

typedef struct game_input
{
    game_button_state     MouseButtons[5];
    i32                   MouseX;
    i32                   MouseY;
    i32                   MouseZ;
    
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
void name(thread_context *Thread, game_memory *Memory, game_input *Input)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define GAME_GET_SOUND_SAMPLES(name)                                           \
void name(thread_context *Thread, game_memory *Memory,                     \
game_sound_output_buffer *Sound)
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);

typedef union vec3
{
    struct
    {
        f32 x, y, z;
    };
    f32 E[3];
} vec3;

internal vec3
Vec3(f32 x, f32 y, f32 z)
{
    vec3 Result = {{x, y, z}};
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
    f32 Result = A.x*B.x + A.y*B.y + A.z*B.z;
    
    return Result;
}

internal vec3
Cross_vec3(vec3 A, vec3 B)
{
    vec3 Result;
    
    Result.x = A.y*B.z - A.z*B.y;
    Result.y = A.z*B.x - A.x*B.z;
    Result.z = A.x*B.y - A.y*B.x;
    
    return Result;
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
Vec4(f32 x, f32 y, f32 z, f32 w)
{
    vec4 Result = {{x, y, z, w}};
    return Result;
}

typedef struct mat4
{
    // each vec4 is a column vector
    vec4 Column[4];
} mat4;



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
            f32 Temp            = Matrix->Column[i].E[j];
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
            Result.Column[i].E[j] = Inner_vec4(Matrix.Column[i], Other.Column[j]);
        }
    }
    
    return Result;
}

typedef struct matrices
{
    mat4 Rotation;
    mat4 Translation;
    mat4 View;
    mat4 Perspective;
} matrices;

typedef struct game_state
{
    matrices Matrices;
    f32 Angle;
    
} game_state;

