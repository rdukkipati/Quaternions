#include "handmade.h"

internal void
GameOutputSound(game_state *GameState, game_sound_output_buffer *Sound, i32 ToneHz)
{
    i16               ToneVolume = 3000;
    i32               WavePeriod = Sound->SampleFramesPerSecond / ToneHz;
    i16              *Memory     = Sound->Memory;
    
    for(i32 SampleFrame = 0; SampleFrame < Sound->SampleFramesToWrite;
        ++SampleFrame)
    {
        
        i16 SampleValue = 0;
        
        *Memory++        = SampleValue;
        *Memory++        = SampleValue;
        
    }
}

extern GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    (void)Thread;
    Assert((&Input->Controllers[0].Terminator -
            &Input->Controllers[0].Buttons[0]) ==
           (ArrayCount(Input->Controllers[0].Buttons)));
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    matrices *Matrices = &GameState->Matrices;
    if(!Memory->IsInitialized)
    {
        *GameState = (game_state){0};
        GameState->WorldUp = _Vec3(0.0f, 1.0f, 0.0f);
        GameState->LookAt = _Vec3(0.0f, 0.0f, 1.0f);
        GameState->CameraRight = Normalize_vec3(Cross_vec3(GameState->WorldUp, GameState->LookAt));
        GameState->CameraUp = Normalize_vec3(Cross_vec3(GameState->LookAt, GameState->CameraRight));
        
        
#define FAR 20.0f
#define NEAR 1.0f
        
        Matrices->Translation = (mat4)
        {
            _Vec4(1.0f, 0.0f, 0.0f, 0.0f),
            _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
            _Vec4(0.0f, 0.0f, 1.0f, 0.0f),
            _Vec4(0.0f, 0.0f, 2.0f, 1.0f)
        };
        
        Matrices->Perspective = (mat4)
        {
            _Vec4(1080.0f / 1920.0f, 0.0f, 0.0f, 0.0f),
            _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
            _Vec4(0.0f, 0.0f, FAR / (FAR - NEAR), 1.0f),
            _Vec4(0.0f, 0.0f, -(FAR * NEAR) / (FAR - NEAR), 0.0f)
        };
        
        
        Memory->IsInitialized = true;
    }
    // speed in meters per second
#define SPEED 1.5f
    f32 MovementDelta = 0.0f;
    f32 Movement = SPEED * Input->dtForFrame;
    
#define ROTATIONSPEED 0.1f
    
    f32 CameraYAngle = 0.0f;
    f32 CameraUAngle = 0.0f;
    
    for(i32 ControllerIndex = 0;
        ControllerIndex < ArrayCount(Input->Controllers); ++ControllerIndex)
    {
        game_controller_input *Controller = GetController(Input,
                                                          ControllerIndex);
        
        if(Controller->IsConnected)
        {
            if(Controller->IsAnalog)
            {
                
                
            }
            else
            {
                
                if(Controller->MoveUp.EndedDown)
                {
                    
                }
                if(Controller->MoveDown.EndedDown)
                {
                    
                }
                if(Controller->MoveLeft.EndedDown)
                {
                    MovementDelta = -Movement;
                }
                if(Controller->MoveRight.EndedDown)
                {
                    MovementDelta = Movement;
                }
                if(Controller->ActionRight.EndedDown)
                {
                    GameState->CameraYAngle += ROTATIONSPEED;
                }
                if(Controller->ActionLeft.EndedDown)
                {
                    GameState->CameraYAngle += -ROTATIONSPEED;
                }
                if(Controller->ActionUp.EndedDown)
                {
                    GameState->CameraUAngle += -ROTATIONSPEED;
                }
                if(Controller->ActionDown.EndedDown)
                {
                    GameState->CameraUAngle += ROTATIONSPEED;
                }
                
                
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
    
    quaternion Q1 = _Quaternion(GameState->CameraUAngle, GameState->CameraRight);
    quaternion Q2 = _Quaternion(GameState->CameraYAngle, GameState->WorldUp);
    
    quaternion Q3 = Multiply_quaternion_quaternion(Q1, Q2);
    
    vec3 LookAt = Normalize_vec3(Rotate(Q3, GameState->LookAt));
    vec3 CameraUp = Normalize_vec3(Rotate(Q3, GameState->CameraUp));
    vec3 CameraRight = Normalize_vec3(Rotate(Q3, GameState->CameraRight));
    
    
    
    
    GameState->CameraPosition = Add_vec3(GameState->CameraPosition, Multiply_f32_vec3(MovementDelta, CameraRight));
#if 0
    Matrices->View = (mat4)
    {
        _Vec4(CameraRight.x, CameraUp.x, LookAt.x, 0.0f),
        _Vec4(CameraRight.y, CameraUp.y, LookAt.y, 0.0f),
        _Vec4(CameraRight.z, CameraUp.z, LookAt.z, 0.0f),
        _Vec4(GameState->CameraPosition.x, GameState->CameraPosition.y, GameState->CameraPosition.z, 1.0f)
    };
#else
    Matrices->View = (mat4)
    {
        _Vec4(CameraRight.x, CameraUp.x, LookAt.x, 0.0f),
        _Vec4(CameraRight.y, CameraUp.y, LookAt.y, 0.0f),
        _Vec4(CameraRight.z, CameraUp.z, LookAt.z, 0.0f),
        _Vec4(-Inner_vec3(CameraRight, GameState->CameraPosition), -Inner_vec3(CameraUp, GameState->CameraPosition), -Inner_vec3(LookAt, GameState->CameraPosition), 1.0f)
    };
#endif
    
    
#define DELTA 0.0349066f
    
    GameState->Angle += DELTA;
    if(GameState->Angle >= PI_2)
    {
        GameState->Angle -= PI_2;
    }
    
    Matrices->Rotation = (mat4)
    {
        _Vec4(cosf(GameState->Angle), 0.0f, -sinf(GameState->Angle), 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(sinf(GameState->Angle), 0.0f, cosf(GameState->Angle), 0.0f),
        _Vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };
    
    
    
    
}

extern GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    (void)Thread;
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    GameOutputSound(GameState, Sound, 400);
}



