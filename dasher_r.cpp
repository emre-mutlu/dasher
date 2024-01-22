#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data,float deltaTime, int maxFrame)
{
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0;

        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
        data.frame = 0;
        }
    }
    return data;
}

int main()
{
    int windowDim[2];
    windowDim[0] = 512;
    windowDim[1] = 380;

    SetTargetFPS(60);

    const int gravity{1000};
    int jumpVel{-600};
    int velocity{};
    int nebVelocity{-300};
    bool isInAir{};
    bool collision{};

    InitWindow(windowDim[0],windowDim[1],"Dasher");

    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    Texture2D scarfy = LoadTexture("textures/scarfy.png");

    AnimData scarfyData {
        {0,0,scarfy.width/6,scarfy.height},
        {windowDim[0]/2 - scarfyData.rec.width/2,windowDim[1]-scarfyData.rec.height},
        0,
        1.0/12.0,
        0
    };

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    float bgX{};
    float mgX{};
    float fgX{};

    const int nebulaeCount{4};

    AnimData nebulae[nebulaeCount]{};

    for(int i = 0; i < nebulaeCount; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.x = windowDim[0] + i * 600;
        nebulae[i].pos.y = windowDim[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].updateTime = 1.0/16.0;
        nebulae[i].runningTime = 0;
    }

    float finishLine{nebulae[nebulaeCount-1].pos.x+300};

    while (!WindowShouldClose())
    {
        float dT = GetFrameTime();

        BeginDrawing();
        ClearBackground(WHITE);

        bgX -= 20 * dT;

        if (bgX <= -background.width*2)
        {
            bgX = 0.0;
        }

        Vector2 bg1Pos{bgX,0.0};
        DrawTextureEx(background,bg1Pos,0.0,2.0,WHITE);
        Vector2 bg2Pos{bgX+background.width*2,0.0};
        DrawTextureEx(background,bg2Pos,0.0,2.0,WHITE);

        mgX -= 40 * dT;

        if(mgX <= -midground.width*2)
        {
            mgX = 0.0;
        }

        Vector2 mg1Pos{mgX,0.0};
        DrawTextureEx(midground,mg1Pos,0.0,2.0,WHITE);
        Vector2 mg2Pos{mgX+midground.width*2,0.0};
        DrawTextureEx(midground,mg2Pos,0.0,2.0,WHITE);

        
        if(fgX <= - foreground.width*2)
        {
            fgX = 0.0;
        }

        fgX -= 80 * dT;
        Vector2 fg1Pos{fgX,0.0};
        DrawTextureEx(foreground,fg1Pos,0.0,2.0,WHITE);
        Vector2 fg2Pos{fgX+foreground.width*2};
        DrawTextureEx(foreground,fg2Pos,0.0,2.0,WHITE);

        if (isOnGround(scarfyData,windowDim[1]))
        {
            velocity = 0;
            isInAir = false;
        }
        else
        {
            velocity += gravity * dT;
            isInAir = true;
        }

        if(IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;    
        }

        scarfyData.pos.y += velocity * dT;

        if(!isInAir){
            scarfyData = updateAnimData(scarfyData,dT,5);
        }

        for (int i = 0; i < nebulaeCount; i++)
        {
            nebulae[i].pos.x += nebVelocity * dT;
        }

        finishLine += nebVelocity * dT;

        for(int i = 0; i < nebulaeCount; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i],dT,7);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x+pad,
                nebula.pos.y+pad,
                nebula.rec.width-2*pad,
                nebula.rec.height-2*pad,
            };
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height,
            };
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }

        }

        if(!collision)
        {

            if(scarfyData.pos.x > finishLine)
            {
                DrawText("You Win!",windowDim[0]/2,windowDim[1]/2,20,GREEN);
            }
            else
            {
                DrawTextureRec(scarfy,scarfyData.rec,scarfyData.pos,WHITE);

                for(int i = 0; i < nebulaeCount; i++)
                {
                    if (i%2 == 0)
                    {
                        DrawTextureRec(nebula,nebulae[i].rec,nebulae[i].pos,WHITE);
                    }
                    else
                    {
                        DrawTextureRec(nebula,nebulae[i].rec,nebulae[i].pos,RED);
                    }
                }
            }

        }
        else
        {
            DrawText("Game Over",windowDim[0]/2,windowDim[1]/2,20,RED);
        }

        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}