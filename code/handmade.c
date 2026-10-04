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
        *Matrices = (matrices){0};
        
#define FAR 20.0f
#define NEAR 1.0f
        
        Matrices->Translation = (mat4)
        {
            Vec4(1.0f, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, 1.0f, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, 1.0f, 0.0f),
            Vec4(0.0f, 0.0f, 2.0f, 1.0f)
        };
        
        Matrices->Perspective = (mat4)
        {
            Vec4(1080.0f / 1920.0f, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, 1.0f, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, FAR / (FAR - NEAR), 1.0f),
            Vec4(0.0f, 0.0f, -(FAR * NEAR) / (FAR - NEAR), 0.0f)
        };
        
        
        Memory->IsInitialized = true;
    }
    
    vec3 CameraPosition = Vec3(0.0f, 0.0f, -2.0f);
    
    Matrices->View = (mat4)
    {
        Vec4(1.0f, 0.0f, 0.0f, 0.0f),
        Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        Vec4(0.0f, 0.0f, 1.0f, 0.0f),
        Vec4(-CameraPosition.x, -CameraPosition.y, -CameraPosition.z, 1.0f)
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
        Vec4(cosf(GameState->Angle), sinf(GameState->Angle), 0.0f, 0.0f),
        Vec4(-sinf(GameState->Angle), cosf(GameState->Angle), 0.0f, 0.0f),
        Vec4(0.0f, 0.0f, 1.0f, 0.0f),
        Vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };
#endif
    
    Matrices->Rotation = (mat4)
    {
        Vec4(cosf(GameState->Angle), 0.0f, -sinf(GameState->Angle), 0.0f),
        Vec4(0.0f, 1.0f, 0.0f, 0.0f),
        Vec4(sinf(GameState->Angle), 0.0f, cosf(GameState->Angle), 0.0f),
        Vec4(0.0f, 0.0f, 0.0f, 1.0f)
    };
    
    
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
                    
                }
                if(Controller->MoveRight.EndedDown)
                {
                }
                
                
            }
            
        }
        
    }
    
    
}

extern GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
    (void)Thread;
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    GameOutputSound(GameState, Sound, 400);
}



