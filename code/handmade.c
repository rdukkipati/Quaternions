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
        GameState->CameraUp = Normalize_vec3(Cross_vec3(GameState->CameraRight, GameState->LookAt));
        
        
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
                
                
            }
            
        }
        
    }
    
    GameState->CameraPosition = Add_vec3(GameState->CameraPosition, Multiply_f32_vec3(MovementDelta, GameState->CameraRight));
    
    Matrices->View = (mat4)
    {
        _Vec4(1.0f, 0.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        _Vec4(0.0f, 0.0f, 1.0f, 0.0f),
        _Vec4(-GameState->CameraPosition.x, -GameState->CameraPosition.y, -GameState->CameraPosition.z, 1.0f)
    };
    
    
#define DELTA 0.0349066f
    
    GameState->Angle += DELTA;
    if(GameState->Angle >= PI_2)
    {
        GameState->Angle -= PI_2;
    }
    
#if 0
    Matrices->Rotation = (mat4)
    {
        _Vec4(cosf(GameState->Angle), sinf(GameState->Angle), 0.0f, 0.0f),
        _Vec4(-sinf(GameState->Angle), cosf(GameState->Angle), 0.0f, 0.0f),
        _Vec4(0.0f, 0.0f, 1.0f, 0.0f),
        _Vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };
#endif
    
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



