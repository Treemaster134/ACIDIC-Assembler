//standard c++ headers
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <stdfloat>
//raylib headers
#include "raylib.h"
#include "libs/raymath.h"
#include "sound.hpp"

#define MAX_SAMPLES_PER_UPDATE   4096

const int notesPerSecond = 20;
const int maxTrackLengthInSeconds = 30;
const int maxTrackLengthInNotes = maxTrackLengthInSeconds * notesPerSecond;

int currentTrackLength = 6;

RenderTexture2D trackDisplay;
RenderTexture2D trackTextDisplay;
Texture2D waveTextures[3];

struct trackData
{
    int16_t freqData = 0;
    char waveType = 0;

    trackData(int16_t _f, char _w)
    {
        freqData = _f;
        waveType = _w;
    }

    trackData()
    {
        freqData = 0;
        waveType = 0;
    }
};


trackData track1[maxTrackLengthInNotes];
trackData track2[maxTrackLengthInNotes];
trackData track3[maxTrackLengthInNotes];

enum programState{
    IN_EDITOR = 0,
    CHOOSE_FREQUENCY = 1,
    CHOOSE_WAVE = 2
};

struct{
    
    Channel voice0;
    Channel voice1;
    Channel voice2;

    bool isPlaying = false;
    int progressFrameCount = 0;
    int trackProgress = 0;

    void SwitchPlayStatus()
    {
        isPlaying = !isPlaying;
        progressFrameCount = 0;
        trackProgress = 0;
    }

    void UpdateSounds()
    {
        if(isPlaying == false)
        {
            return;
        }
        if(trackProgress < maxTrackLengthInNotes)
        {
            VOICE0Wave.frequency = (int)track1[trackProgress].freqData;
            VOICE0Wave.waveType = (int)track1[trackProgress].waveType;
        
    
            VOICE1Wave.frequency = (int)track2[trackProgress].freqData;
            VOICE1Wave.waveType = (int)track2[trackProgress].waveType;
            
            
            VOICE2Wave.frequency = (int)track3[trackProgress].freqData;
            VOICE2Wave.waveType = (int)track3[trackProgress].waveType;
            
            trackProgress += 1;
        }
        else
        {
            isPlaying = false;
        }

    }

} SoundChip;

int selectedFrequency = 440;
int selectedWavetype = SQUAREWAVE;

void exportMusic()
{
    int writeLength = 0;
    for (int i = maxTrackLengthInNotes - 1; i >= 0; i--)
    {
        if(
            track1[i].freqData != 0 ||
            track2[i].freqData != 0 ||
            track3[i].freqData != 0
        )
        {
            writeLength = i + 1;
            break;
        }
    }

    if(writeLength < 1)
    {
        std::cout << "No music data\n";
        return;
    }

    std::ofstream outputStream;

    outputStream.open("track0.amus", std::ofstream::out);
    if(outputStream.is_open())
    {
        for(int i = 0; i < writeLength; i++)
        {   

            int16_t dataToWrite = 0;

            dataToWrite = (track1[i].waveType & 3) << 14;
            dataToWrite = dataToWrite | (track1[i].freqData & 16383);

            char highBits = (dataToWrite >> 8) & 0xFF;
            char lowBits = dataToWrite & 0xFF;


            std::cout << (int)dataToWrite << "\n";

            for (int j = 0; j < (int)(60/notesPerSecond); j++)
            {
                outputStream<<highBits;
                outputStream<<lowBits;
            }
            
            
        }
        
        outputStream.close();
        std::cout << "Finished writing to file: \"" << "track0.amus" << "\"";
    }
    else{std::cout<<"Failed to write to file: \""<< "track0.amus" <<"\"";}
    
    
    outputStream.open("track1.amus", std::ofstream::out);
    if(outputStream.is_open())
    {
        for(int i = 0; i < writeLength; i++)
        {   

            int16_t dataToWrite = 0;

            dataToWrite = (track2[i].waveType & 3) << 14;
            dataToWrite = dataToWrite | (track2[i].freqData & 16383);

            char highBits = (dataToWrite >> 8) & 0xFF;
            char lowBits = dataToWrite & 0xFF;


            

            for (int j = 0; j < (int)(60/notesPerSecond); j++)
            {
                outputStream<<highBits;
                outputStream<<lowBits;
            }
            
            
        }
        
        outputStream.close();
        std::cout << "Finished writing to file: \"" << "track1.amus" << "\"";
    }
    else{std::cout<<"Failed to write to file: \""<< "track1.amus" <<"\"";}
    
    
    outputStream.open("track2.amus", std::ofstream::out);
    if(outputStream.is_open())
    {
        for(int i = 0; i < writeLength; i++)
        {   

            int16_t dataToWrite = 0;

            dataToWrite = (track3[i].waveType & 3) << 14;
            dataToWrite = dataToWrite | (track3[i].freqData & 16383);

            char highBits = (dataToWrite >> 8) & 0xFF;
            char lowBits = dataToWrite & 0xFF;


            

            for (int j = 0; j < (int)(60/notesPerSecond); j++)
            {
                outputStream<<highBits;
                outputStream<<lowBits;
            }
            
            
        }
        
        outputStream.close();
        std::cout << "Finished writing to file: \"" << "track2.amus" << "\"";
    }
    else{std::cout<<"Failed to write to file: \""<< "track2.amus" <<"\"";}
    
    
}

int main()
{
    //screen width and height
    const int screenWidth = 1600;
    const int screenHeight = 800;
    
    //initialization
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Amus");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    SetTargetFPS(60);

    VOICE0Wave.stream = LoadAudioStream(11025, 16, 1);
    SetAudioStreamCallback(VOICE0Wave.stream, VOICE0InputCallback);
    PlayAudioStream(VOICE0Wave.stream);

    VOICE1Wave.stream = LoadAudioStream(11025, 16, 1);
    SetAudioStreamCallback(VOICE1Wave.stream, VOICE1InputCallback);
    PlayAudioStream(VOICE1Wave.stream);

    VOICE2Wave.stream = LoadAudioStream(11025, 16, 1);
    SetAudioStreamCallback(VOICE2Wave.stream, VOICE2InputCallback);
    PlayAudioStream(VOICE2Wave.stream);
    int resultWidth =  maxTrackLengthInNotes * (screenWidth/(maxTrackLengthInNotes/6));
    trackDisplay = LoadRenderTexture(resultWidth, screenHeight);
    trackTextDisplay = LoadRenderTexture(resultWidth, screenHeight);

    waveTextures[SQUAREWAVE] = LoadTexture("assets/squarewave.png");
    waveTextures[TRIANGLEWAVE] = LoadTexture("assets/triangle.png");
    waveTextures[SAWTOOTHWAVE] = LoadTexture("assets/sawtooth.png");

    int displayOffset = 0;
    int status = IN_EDITOR;

    int tempFreqChoice = 0;

    while (!WindowShouldClose())
    {
        switch(status)
        {
            case IN_EDITOR:
            {
                if(IsKeyPressed(KEY_F)){status = CHOOSE_FREQUENCY; SoundChip.isPlaying = false; break;}
                if(IsKeyPressed(KEY_W)){status = CHOOSE_WAVE; SoundChip.isPlaying = false; break;}
                if(IsKeyPressed(KEY_S)){exportMusic();}

                if(IsKeyPressed(KEY_SPACE)){SoundChip.SwitchPlayStatus();}

                if(SoundChip.isPlaying == true)
                {
                    if((SoundChip.progressFrameCount % (int)(60/notesPerSecond)) == 0){SoundChip.UpdateSounds();}
                    SoundChip.progressFrameCount += 1;
                }
                else
                {
                    VOICE0Wave.frequency = 0;VOICE1Wave.frequency = 0;VOICE2Wave.frequency = 0;
                }

                

                if(IsKeyDown(KEY_LEFT))
                {
                    displayOffset += 10;
                    if(displayOffset > abs(resultWidth - screenWidth))
                    {
                        displayOffset = abs(resultWidth - screenWidth);
                    }
                }
                if(IsKeyDown(KEY_RIGHT))
                {
                    displayOffset -= 10;
                    if(displayOffset < 0){displayOffset = 0;}
                }

                int recHeight = screenHeight/3;
                int recWidth = screenWidth/(maxTrackLengthInNotes/notesPerSecond);

                float waveTextureWidth = recWidth * (4.0f/6.0f);
                float waveTextureOffset = recWidth/6.0f;

                Vector2 mousePos = GetMousePosition();
                mousePos.x += displayOffset;
                mousePos.x = floor(mousePos.x / recWidth);
                mousePos.y = floor(mousePos.y / recHeight);

                BeginDrawing();
                ClearBackground(GetColor(0xC3C3C3FF));

                BeginTextureMode(trackTextDisplay);
                    ClearBackground(BLANK);

                    for(int t = 0; t < 3; t++)
                    {
                        for (int i = 0; i < maxTrackLengthInNotes; i++)
                        {
                            
                            switch (t)
                            {
                            case 2:
                            {
                                
                                DrawText(TextFormat("%i", track3[i].freqData), (float)((i + 0.5f) * recWidth) - MeasureText(TextFormat("%i", track3[i].freqData), 25)/2, (float)((t + 1) * recHeight) - 30, 25, BLACK);
                                break;
                            }
                            case 1:
                            {
                                DrawText(TextFormat("%i", track2[i].freqData), (float)((i + 0.5f) * recWidth) - MeasureText(TextFormat("%i", track2[i].freqData), 25)/2, (float)((t + 1) * recHeight) - 30, 25, BLACK);
                                break;
                            }
                            case 0:
                            {
                                DrawText(TextFormat("%i", track1[i].freqData), (float)((i + 0.5f) * recWidth) - MeasureText(TextFormat("%i", track1[i].freqData), 25)/2, (float)((t + 1) * recHeight) - 30, 25, BLACK);
                                break;
                            }
                            default:
                                break;
                            }

                            //DrawRectangleLines(i * recWidth, t * recHeight, recWidth, recHeight, BLACK);
                        }
                    }

                EndTextureMode();

                Rectangle source = { 0, 0, (float)trackDisplay.texture.width, (float)-trackDisplay.texture.height };

                BeginTextureMode(trackDisplay);
                    ClearBackground(GetColor(0xC3C3C3FF));
                
                    

                    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                    {
                        switch ((int)mousePos.y)
                        {
                        case 0:
                            track1[(int)mousePos.x].freqData = selectedFrequency;
                            track1[(int)mousePos.x].waveType = selectedWavetype;
                            break;
                        case 1:
                            track2[(int)mousePos.x].freqData = selectedFrequency;
                            track2[(int)mousePos.x].waveType = selectedWavetype;
                            break;
                        case 2:
                            track3[(int)mousePos.x].freqData = selectedFrequency;
                            track3[(int)mousePos.x].waveType = selectedWavetype;
                            break;
                        
                        default:
                            break;
                        }
                    }
                    if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                    {
                        switch ((int)mousePos.y)
                        {
                        case 0:
                            track1[(int)mousePos.x].freqData = 0;
                            break;
                        case 1:
                            track2[(int)mousePos.x].freqData = 0;
                            break;
                        case 2:
                            track3[(int)mousePos.x].freqData = 0;
                            break;
                        
                        default:
                            break;
                        }
                    }

                    for(int t = 0; t < 3; t++)
                    {
                        for (int i = 0; i < maxTrackLengthInNotes; i++)
                        {
                            switch (t)
                            {
                            case 2:
                            {
                                float freqHeight = recHeight * (track3[i].freqData/1000.0f);
                                DrawRectangle(i * recWidth, (t + 1) * recHeight - freqHeight, recWidth, freqHeight, GetColor(0x0000FF7F));
                                DrawTexturePro(waveTextures[track3[i].waveType], {0,0,6,27}, {(float)(i * recWidth) + waveTextureOffset, (float)(t * recHeight), (float)waveTextureWidth, (float)recHeight}, {0.0f,0.0f},0.0f,WHITE);
                                break;
                            }
                            case 1:
                            {
                                float freqHeight = recHeight * (track2[i].freqData/1000.0f);
                                DrawRectangle(i * recWidth, (t + 1) * recHeight - freqHeight, recWidth, freqHeight, GetColor(0x0000FF7F));
                                DrawTexturePro(waveTextures[track2[i].waveType], {0,0,6,27}, {(float)(i * recWidth) + waveTextureOffset, (float)(t * recHeight), (float)waveTextureWidth, (float)recHeight}, {0.0f,0.0f},0.0f,WHITE);
                                break;
                            }
                            case 0:
                            {
                                float freqHeight = recHeight * (track1[i].freqData/1000.0f);
                                DrawRectangle(i * recWidth, (t + 1) * recHeight - freqHeight, recWidth, freqHeight, GetColor(0x0000FF7F));
                                DrawTexturePro(waveTextures[track1[i].waveType], {0,0,6,27}, {(float)(i * recWidth) + waveTextureOffset, (float)(t * recHeight), (float)waveTextureWidth, (float)recHeight}, {0.0f,0.0f},0.0f,WHITE);
                                break;
                            }
                            default:
                                break;
                            }

                            DrawRectangleLines(i * recWidth, t * recHeight, recWidth, recHeight, BLACK);
                            if(i == SoundChip.trackProgress)
                            {
                                DrawRectangleLinesEx({(float)i * recWidth, 0, (float)recWidth,(float)screenHeight},4.0f,DARKGREEN);
                            }
                        }
                    }
                    DrawRectangleLines(mousePos.x * recWidth, mousePos.y * recHeight, recWidth,recHeight, DARKGREEN);
                    DrawRectangleLinesEx({mousePos.x * recWidth, mousePos.y * recHeight, (float)recWidth,(float)recHeight},3.0f,MAGENTA);
                    DrawTextureRec(trackTextDisplay.texture, source, (Vector2){ 0, 0 }, WHITE);
                EndTextureMode();
                
                source = { (float)displayOffset, 0.0f, (float)trackDisplay.texture.width, (float)-trackDisplay.texture.height };
                DrawTextureRec(trackDisplay.texture, source, (Vector2){ 0, 0 }, WHITE);

                DrawFPS(10,10);
                EndDrawing();
                break;
            }
            case CHOOSE_FREQUENCY:
            {
                BeginDrawing();
                ClearBackground(GetColor(0xC3C3C3FF));

                DrawText(TextFormat("%i Hz", tempFreqChoice), screenWidth/2, screenHeight/2, 64, BLACK);

                switch (GetKeyPressed())
                {
                    case KEY_NINE:
                        tempFreqChoice = (tempFreqChoice * 10) + 9;
                        break;
                    case KEY_EIGHT:
                        tempFreqChoice = (tempFreqChoice * 10) + 8;
                        break;
                    case KEY_SEVEN:
                        tempFreqChoice = (tempFreqChoice * 10) + 7;
                        break;
                    case KEY_SIX:
                        tempFreqChoice = (tempFreqChoice * 10) + 6;
                        break;
                    case KEY_FIVE:
                        tempFreqChoice = (tempFreqChoice * 10) + 5;
                        break;
                    case KEY_FOUR:
                        tempFreqChoice = (tempFreqChoice * 10) + 4;
                        break;
                    case KEY_THREE:
                        tempFreqChoice = (tempFreqChoice * 10) + 3;
                        break;
                    case KEY_TWO:
                        tempFreqChoice = (tempFreqChoice * 10) + 2;
                        break;
                    case KEY_ONE:
                        tempFreqChoice = (tempFreqChoice * 10) + 1;
                        break;
                    case KEY_ZERO:
                        tempFreqChoice = tempFreqChoice * 10;
                        break;
                    case KEY_BACKSPACE:
                    {
                        int amountToDecrease = tempFreqChoice % 10;
                        tempFreqChoice = (tempFreqChoice - amountToDecrease) / 10;
                        break;
                    }
                    case KEY_ENTER:
                        status = IN_EDITOR;
                        selectedFrequency = tempFreqChoice;
                        tempFreqChoice = 0;
                        break;
                    
                    default:
                        break;
                }

                EndDrawing();
                break;
            }
            case CHOOSE_WAVE:
            {
                BeginDrawing();
                ClearBackground(GetColor(0xC3C3C3FF));

                DrawText("Press:\n 0: Square Wave\n 1: Triangle Wave\n 2: Sawtooth Wave", screenWidth/2, screenHeight/2, 64, BLACK);

                switch (GetKeyPressed())
                {
                    case KEY_ZERO:
                        selectedWavetype = SQUAREWAVE;
                        status = IN_EDITOR;
                        break;
                    case KEY_ONE:
                        selectedWavetype = TRIANGLEWAVE;
                        status = IN_EDITOR;
                        break;
                    case KEY_TWO:
                        selectedWavetype = SAWTOOTHWAVE;
                        status = IN_EDITOR;
                        break;
                    default:
                        break;
                }

                EndDrawing();
                break;
            }
            default:
                status = IN_EDITOR;
                break;
        }
        
    }


    UnloadAudioStream(VOICE0Wave.stream);
    UnloadAudioStream(VOICE1Wave.stream);
    UnloadAudioStream(VOICE2Wave.stream);

    UnloadTexture(waveTextures[0]);
    UnloadTexture(waveTextures[1]);
    UnloadTexture(waveTextures[2]);
    UnloadRenderTexture(trackDisplay);
    UnloadRenderTexture(trackTextDisplay);


    CloseWindow();
    
}

