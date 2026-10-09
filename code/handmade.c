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
        
        GameState->Arena.BaseAddress = (u8 *)Memory->PermanentStorage + sizeof(game_state);
        GameState->Arena.RemainingSpace = Memory->PermanentStorageSize - sizeof(game_state);
        
        GameState->Player.Cube.Position = _Vec3(0.0f, 0.0f, 3.0f);
        GameState->Player.Camera.Orientation.Position = Add_vec3(GameState->Player.Cube.Position, _Vec3(0.0f, 2.0f, -3.0f));
        GameState->Player.ToCamera = Minus_vec3(GameState->Player.Camera.Orientation.Position, GameState->Player.Cube.Position);
        
        
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
    
    
    
    
    
    
    
    
    f32 L_StickAverageX = 0.0f;
    f32 L_StickAverageY = 0.0f;
    
    for(i32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers); ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input,
                                                          ControllerIndex);
        
        if(Controller->IsConnected)
        {
            
            if(Controller->L_StickAverageX > 0.0f)
            {
                L_StickAverageX = Controller->L_StickAverageX;
            }
            
            if(Controller->L_StickAverageY > 0.0f)
            {
                L_StickAverageY = Controller->L_StickAverageY;
            }
            
            if(Controller->R_StickAverageX)
            {
                GameState->Player.Camera.Orientation.YAngle += -Controller->R_StickAverageX * 0.1f;
            }
            
            if(Controller->R_StickAverageY)
            {
                GameState->Player.Camera.Orientation.UAngle += Controller->R_StickAverageY * 0.1f;
            }
            
            
            
            if(Controller->ActionRight)
            {
            }
            if(Controller->ActionLeft)
            {
            }
            if(Controller->ActionUp)
            {
            }
            
#define JUMP_VELOCITY 5.0f
            
            if(Controller->ActionDown)
            {
                GameState->Player.OldVelocity.y = JUMP_VELOCITY;
            }
            
#undef JUMP_VELOCITY
            
        }
    }
    
    if(GameState->Player.Camera.Orientation.UAngle > 1.5708f)
    {
        GameState->Player.Camera.Orientation.UAngle = 1.5708f;
    }
    if(GameState->Player.Camera.Orientation.UAngle < -1.5708f)
    {
        GameState->Player.Camera.Orientation.UAngle = -1.5708f;
    }
    if(GameState->Player.Camera.Orientation.YAngle > PI_2)
    {
        GameState->Player.Camera.Orientation.YAngle -= PI_2;
    }
    if(GameState->Player.Camera.Orientation.YAngle < -PI_2)
    {
        GameState->Player.Camera.Orientation.YAngle += PI_2;
    }
    
    quaternion Q2 = _Quaternion(GameState->CameraYAngle, GameState->WorldUp);
    vec3       CameraRight = Normalize_vec3(Rotate(Q2, GameState->CameraRight));
    quaternion Q1          = _Quaternion(GameState->CameraUAngle, CameraRight);
    
    quaternion Q3          = Multiply_quaternion_quaternion(Q1, Q2);
    
    vec3 NewToCamera = Rotate(Q3, GameState->Player.ToCamera);
    GameState->Player.Camera.Orientation.Position = Add_vec3(GameState->Player.Cube.Position, NewToCamera);
    
    vec3 LookAt = Normalize_vec3(Minus_vec3(GameState->Player.Cube.Position, GameState->Player.Camera.Orientation.Position));
    vec3 CameraRight = Normalize_vec3(Cross_vec3(GameState->Player.Camera.Basis.WorldUp, LookAt));
    vec3 CameraUp = Normalize_vec3(Cross_vec3(LookAt, CameraRight));
    
    // NOTE: We rotate camera around player's initial position
    // get our view matrix
    // then we move player
    
    // NOTE: we could instead move player based on camera's initial orientation and then determine offset of camera after that movement
    GameState->GlobalMatrices.View = (mat4)
    {
        _Vec4(CameraRight.x, CameraUp.x, LookAt.x, 0.0f),
        _Vec4(CameraRight.y, CameraUp.y, LookAt.y, 0.0f),
        _Vec4(CameraRight.z, CameraUp.z, LookAt.z, 0.0f),
        _Vec4(-Inner_vec3(CameraRight, GameState->Player.Camera.Orientation.Position),
              -Inner_vec3(CameraUp, GameState->Player.Camera.Orientation.Position),
              -Inner_vec3(LookAt, GameState->Player.Camera.Orientation.Position), 1.0f)
    };
    
    
    
    vec3 PlayerLookAt = Normalize_vec3(_Vec3(LookAt.x, 0.0f, LookAt.z));
    vec3 PlayerRight = Normalize_vec3(Cross_vec3(GameState->Player.Camera.Basis.WorldUp, PlayerLookAt));
    
    vec3 NewPlayerLookAt = Normalize_vec3(Add_vec3(Multiply_f32_vec3(L_StickAverageY, PlayerLookAt), Multiply_f32_vec3(L_StickAverageX, PlayerRight)));
    vec3 NewPlayerRight = Normalize_vec3(Cross_vec3(GameState->Player.Camera.Basis.WorldUp, NewPlayerLookAt));
    
    f32 StickMagnitude = Sqrt_f32(L_StickAverageX * L_StickAverageX + L_StickAverageY * L_StickAverageY);
    StickMagnitude = StickMagnitude > 1.0f ? 1.0f : StickMagnitude;
    
    // NOTE: Multiply this magnitude by acceleration
    // NOTE: Construct rotation matrix for cube from new basis vectors
    
#define PLAYER_ACCELERATION 0.2f
#define GRAVITY -9.8f
    
    // displacement = OldVelocity * Time + 0.5f * Magnitude * Acceleration * TimeSquared
    // NewVelocity = OldVelocity + Accleration * Time
    // 
    f32 a = PLAYER_ACCELERATION * StickMagnitude;
    vec3 Acceleration = _Vec3(NewPlayerLookAt.x * a, GRAVITY, NewPlayerLookAt.z * a);
    
    
#undef PLAYER_ACCELERATION
#undef GRAVITY
    
    vec3 NewVelocity = Add_vec3(GameState->Player.OldVelocity,  Multiply_f32_vec3(Input->dtForFrame, Acceleration));
    vec3 PlayerMovement = Add_vec3(Multiply_f32_vec3(Input->dtForFrame, GameState->Player.OldVelocity), Multiply_f32_vec3(0.5f * Input->dtForFrame * Input->dtForFrame, Acceleration));
    
    GameState->Player.OldVelocity = NewVelocity;
    
    GameState->Player.Cube.Position += PlayerMovement;
    if(GameState->Player.Cube.Position.y < 0.0f)
    {
        GameState->Player.Cube.Position.y = 0.0f;
    }
    
    GameState->Player.Cube.Matrices.Scale = _Identity_mat4();
    GameState->Player.Cube.Matrices.Rotation = (mat4)
    {
        _Vec4(NewPlayerRight.x, NewPlayerRight.y, NewPlayerRight.z, 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(NewPlayerLookAt.x, NewPlayerLookAt.y, NewPlayerLookAt.z, 0.0f),
        _Vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };
    GameState->Player.Cube.Matrices.Translation = (mat4)
    {
        _Vec4(1.0f, 0.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 0.0f, 1.0f, 0.0f),
        _Vec4(GameState->Player.Cube.Position.x, GameState->Player.Cube.Position.y, GameState->Player.Cube.Position.z, 1.0f)
    };
    
    
    
    GameOutputSound(GameState, Sound, 400);
}
