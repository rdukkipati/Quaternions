#include "handmade.h"

internal void
GameOutputSound(game_state *GameState, game_sound_output_buffer *Sound,
                i32 ToneHz)
{
    i16  ToneVolume = 3000;
    i32  WavePeriod = Sound->SampleFramesPerSecond / ToneHz;
    i16 *Memory     = Sound->Memory;
    
    for(i32 SampleFrame = 0; SampleFrame < Sound->SampleFramesToWrite;
        ++SampleFrame)
    {
        
        i16 SampleValue = 0;
        
        *Memory++       = SampleValue;
        *Memory++       = SampleValue;
    }
}

internal void *
PushArena(arena *Arena, u64 Size)
{
    Assert(Arena->RemainingSpace >= Size);
    void *Result = Arena->BaseAddress;
    Arena->BaseAddress += Size;
    return Result;
}

extern GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    (void)Thread;
    
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    matrices   *Matrices  = &GameState->Matrices;
    if(!Memory->IsInitialized)
    {
        
        *GameState             = (game_state){0};
        
        GameState->Arena = (arena){0};
        GameState->Arena.BaseAddress = (u8 *)Memory->PermanentStorage + sizeof(game_state);
        GameState->Arena.RemainingSpace = Memory->PermanentStorageSize - sizeof(game_state);
        
        GameState->Player.Cube.Position = _Vec3(0.0f, 0.0f, 3.0f);
        GameState->Player.Camera.Orientation.Position = Add_vec3(GameState->Player.Cube.Position, _Vec3(0.0f, 2.0f, -3.0f));
        GameState->Player.Camera.PlayerOffset = Minus_vec3(GameState->Player.Camera.Orientation.Position, GameState->Player.Cube.Position);
        
        
        GameState->Player.Camera.Basis.WorldUp = _Vec3(0.0f, 1.0f, 0.0f);
        GameState->Player.Camera.Basis.LookAt = Normalize_vec3(Minus_vec3(GameState->Player.Cube.Position, GameState->Camera.Orientation.Position));
        
        GameState->Camera.Basis.Right = Normalize_vec3(
                                                       Cross_vec3(GameState->WorldUp, GameState->LookAt));
        GameState->Camera.Basis.Up = Normalize_vec3(
                                                    Cross_vec3(GameState->LookAt, GameState->CameraRight));
        
        // Right joystic rotates camera around y axis at origin, camera is offset from its origin
        // Camera's target is always object
        // Camera's world position starts slightly above and behind object in -z
        // Moving player forward and right moves the player according to the xz direction of camera's forward and right vectors
        // Grab camera's forward and right vectors after rotation is calculated, isolate the xz portion and normalize it
        // Apply movement * each vector to player
        // Also, rotate player so it's xz orientation is same as camera, but only if they move
        // Don't rotate the player if only the camera moved, only initiate rotation if player moves
        //
        
        //Keep track of default offset of player to camera
        //After grabbing input, rotate that offset vector according to calculated quaternion from angle
        //Calculate new position of camera
        //Calculate new basis vectors with target being player
        
        
        i32 ImageWidth, ImageHeight, ComponentsPerPixel;
        u8 *ImageData = stbi_load("Ground_01.png", &ImageWidth, &ImageHeight, &ComponentsPerPixel, 4);
        Assert(ImageData);
        GameState->GrassTexture = (u8 *)PushArena(&GameState->Arena, (u64)(ImageWidth*ImageHeight*4));
        
        memcpy(GameState->GrassTexture, ImageData, ImageWidth*ImageHeight*4);
        
        stbi_image_free(ImageData);
        
        Memory->IsInitialized = true;
    }
    
    
    GameState->Camera.Far = 20.0f;
    GameState->Camera.Near = 1.0f;
    
    
    GameState->GlobalMatrices.Perspective = (mat4)
    {
        _Vec4((1080.0f * GameState->Camera.Near) / 1920.0f, 0.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 0.0f, GameState->Camera.Far / (GameState->Camera.Far - GameState->Camera.Near), 1.0f),
        _Vec4(0.0f, 0.0f, -(GameState->Camera.Far * GameState->Camera.Near) / (GameState->Camera.Far - GameState->Camera.Near), 0.0f)
    };
    
    
    GameState->Cubes[0].Position = _Vec3(0.0f, 0.0f, 3.0f);
    GameState->Cubes[0].Matrices.Scale = _Identity_mat4();
    GameState->Cubes[0].Matrices.Rotation = 
        
        Matrices->Translation = (mat4)
    {
        _Vec4(1.0f, 0.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 0.0f, 1.0f, 0.0f),
        _Vec4(0.0f, 0.0f, 3.0f, 1.0f)
    };
    
    
    
    
    
    
    
    // speed in meters per second
#define SPEED 1.5f
    f32 MovementDelta = 0.0f;
    f32 Movement      = SPEED * Input->dtForFrame;
    
#define ROTATIONSPEED 0.1f
    
    for(i32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers); ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input,
                                                          ControllerIndex);
        
        if(Controller->IsConnected)
        {
            
            if(Controller->MoveUp)
            {
                ForwardDelta = DistancePerFrame;
            }
            if(Controller->MoveDown)
            {
                ForwardDelta = -DistancePerFrame;
            }
            if(Controller->MoveLeft)
            {
                RightDelta = -DistancePerFrame;
            }
            if(Controller->MoveRight)
            {
                RightDelta = DistancePerFrame;
            }
            if(Controller->ActionRight)
            {
                GameState->CameraYAngle += ROTATIONSPEED;
            }
            if(Controller->ActionLeft)
            {
                GameState->CameraYAngle += -ROTATIONSPEED;
            }
            if(Controller->ActionUp)
            {
                GameState->CameraUAngle += -ROTATIONSPEED;
            }
            if(Controller->ActionDown)
            {
                GameState->CameraUAngle += ROTATIONSPEED;
            }
            
        }
    }
    
    if(GameState->CameraUAngle > 1.5708f)
    {
        GameState->CameraUAngle = 1.5708f;
    }
    if(GameState->CameraUAngle < -1.5708f)
    {
        GameState->CameraUAngle = -1.5708f;
    }
    if(GameState->CameraYAngle > PI_2)
    {
        GameState->CameraYAngle -= PI_2;
    }
    if(GameState->CameraYAngle < -PI_2)
    {
        GameState->CameraYAngle += PI_2;
    }
    
    quaternion Q2 = _Quaternion(GameState->CameraYAngle, GameState->WorldUp);
    vec3       CameraRight = Normalize_vec3(Rotate(Q2, GameState->CameraRight));
    quaternion Q1          = _Quaternion(GameState->CameraUAngle, CameraRight);
    
    quaternion Q3          = Multiply_quaternion_quaternion(Q1, Q2);
    
    vec3       LookAt      = Normalize_vec3(Rotate(Q3, GameState->LookAt));
    vec3       CameraUp    = Normalize_vec3(Rotate(Q3, GameState->CameraUp));
    CameraRight            = Normalize_vec3(Rotate(Q3, GameState->CameraRight));
    
    GameState->CameraPosition = Add_vec3(
                                         GameState->CameraPosition,
                                         Multiply_f32_vec3(MovementDelta, CameraRight));
    
    GameState->GlobalMatrices.View = (mat4)
    {
        _Vec4(CameraRight.x, CameraUp.x, LookAt.x, 0.0f),
        _Vec4(CameraRight.y, CameraUp.y, LookAt.y, 0.0f),
        _Vec4(CameraRight.z, CameraUp.z, LookAt.z, 0.0f),
        _Vec4(-Inner_vec3(CameraRight, GameState->CameraPosition),
              -Inner_vec3(CameraUp, GameState->CameraPosition),
              -Inner_vec3(LookAt, GameState->CameraPosition), 1.0f)
    };
    
#define DELTA 0.0349066f
    
    GameState->Angle += DELTA;
    if(GameState->Angle >= PI_2)
    {
        GameState->Angle -= PI_2;
    }
    
    GameState->Objects[0].Matrices.Rotation = (mat4)
    {
        _Vec4(cosf(GameState->Angle), 0.0f, -sinf(GameState->Angle), 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(sinf(GameState->Angle), 0.0f, cosf(GameState->Angle), 0.0f),
        _Vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };
    
    GameOutputSound(GameState, Sound, 400);
}
