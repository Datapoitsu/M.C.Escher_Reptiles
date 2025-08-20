//Windows libraries.
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <math.h>

//SDL2
#include <SDL2/SDL.h>

//My library.
#include <tools/input.h>

SDL_Window *Window;
SDL_Renderer *RenderInformation;
const int screenWidth = 1000;
const int screenHeigth = 1000;
int backgroundColour[3] = {103,141,89};
int color1[3] = {255,154,83};
int color2[3] = {103,141,89};

//Function declaration
void Start();
void Update();
bool endApp = false;

const double PI = 3.141592653589793;

struct Vector2
{
    float x;
    float y;
    bool draw = true;
    int color[3] = {color1[0],color1[1],color1[2]}; 
};

std::vector<Vector2> leftSide =
{
    {0.0,0.0},
    {0.3,0.2},
    {0.23,0.38},
    {0.34,0.50},
    {0.45,0.43},
    {0.45,0.36},
    {0.37,0.37},
    {0.44,0.24},
    {0.61,0.38},
    {0.5,0.5},
};

std::vector<Vector2> rightSide =
{
    {1.0,1.0},
    {1.10,0.65},
    {0.96,0.55},
    {0.90,0.32},
    {1.00,0.19},
    {1.09,0.24},
    {1.02,0.34},
    {1.12,0.41},
    {1.20,0.19},
    {1,0},
};

std::vector<Vector2> resultVec;

void MirrorLeftSide(std::vector<Vector2> *vec)
{
    int size = vec->size();
    for(int i = 0; i < size - 1; i++)
    {
        vec->push_back({1 - vec->at(size - 2 -i).x, 1 - vec->at(size - 2 -i).y});
    }
}

void MirrorRightSide(std::vector<Vector2> *vec)
{
    int size = vec->size();
    for(int i = 0; i < size - 1; i++)
    {
        vec->push_back({1 - vec->at(size - 2 -i).y, -(1 - vec->at(size - 2 - i).x)});
    }
}

void ChangeColor(std::vector<Vector2> *vec)
{
    int size = vec->size();
    for(int i = 0; i < size - 1; i++)
    {
        vec->at(i).color[0] = color2[0];
        vec->at(i).color[1] = color2[1];
        vec->at(i).color[2] = color2[2];
    }
}

void CombineVectors(std::vector<Vector2> *vec1, std::vector<Vector2> *vec2)
{
    for(int i = 0; i < vec2->size(); i++)
    {
        vec1->push_back(vec2->at(i));
    }
}

void CopyVec(std::vector<Vector2> *vec1, std::vector<Vector2> *vec2)
{
    for(int i = 0; i < vec1->size(); i++)
    {
        vec2->push_back({vec1->at(i).x, vec1->at(i).y, vec1->at(i).draw});
    }
}

void RotateVec(std::vector<Vector2> *vec, float additionalAngle = 0)
{

    for(int i = 0; i < vec->size(); i++)
    {
        float curAngle = atan2(vec->at(i).y, vec->at(i).x);
        float distance = sqrt(powf(vec->at(i).x,2) + powf(vec->at(i).y,2));
        if(i == 0)
        {
            vec->at(i).draw = false;
        }
        vec->at(i).x = cos(curAngle + additionalAngle) * distance;
        vec->at(i).y = sin(curAngle + additionalAngle) * distance;
    }
}

void MoveVec(std::vector<Vector2> *vec, float x, float y)
{
    for(int i = 0; i < vec->size(); i++)
    {
        vec->at(i).x += x;
        vec->at(i).y += y;
    }
}

bool createWindow()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    Window = SDL_CreateWindow("SDL Practice",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,screenWidth,screenHeigth,SDL_WINDOW_ALLOW_HIGHDPI);
    if (Window != NULL)
    {
        RenderInformation = SDL_CreateRenderer(Window, -1, 0);
        std::cout << "Did create a window!" << std::endl; 
    }
    else
    {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
    }
    return Window != NULL;
}

void closeWindow()
{
    SDL_DestroyWindow(Window);
    SDL_Quit();
}


int iterationCount = 4;
int lsysIndex = 0;
float offsetX = 400;
float offsetY = 400;
float zoom = 1;
float offsetSpeed = 50.0f;

void RenderFrame(std::vector<Vector2> *vec)
{
    // ----- Draw background ----- //
    SDL_SetRenderDrawColor(RenderInformation, backgroundColour[0], backgroundColour[1], backgroundColour[2], 255);
    SDL_RenderClear(RenderInformation); //Fills the screen with the background colour
    
    for(int i = 1; i < vec->size(); i++)
    {
        SDL_SetRenderDrawColor(RenderInformation,vec->at(i).color[0],vec->at(i).color[1],vec->at(i).color[2],255);
        if(vec->at(i).draw)
        {
            Vector2 v1 = vec->at(i);
            Vector2 v2 = vec->at(i-1);
            SDL_RenderDrawLine(RenderInformation,v1.x * screenWidth / 2 * zoom + offsetX, -v1.y * screenHeigth / 2 * zoom + offsetY, v2.x * screenWidth / 2 * zoom + offsetX, -v2.y * screenHeigth / 2 * zoom + offsetY);
        }
    }

    SDL_RenderPresent(RenderInformation);
}


// ----- Time ----- //
double deltaTime;
timeval t1, t2; //Time at start and end of the frame
double elapsedTime;
double sessionTime = 0; //Total time the session has been on.
int fpsLimiter = 60;

void Restart()
{
    offsetX = 0;
    offsetY = 0;
    zoom = 1;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    std::cout << "Starting" << std::endl;
    if(createWindow() == false)
    {
        return 1;
    }
    Start();

    //Begining of calculating time.
    mingw_gettimeofday(&t1, NULL); 
    
    std::cout << "Start completed succesfully" << std::endl;

    while (true)
    {
        SDL_Event Event;
        if (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_QUIT) //Ending application.
            {
                endApp = true;
            }
            if(Event.type == SDL_MOUSEMOTION)
            {
                if(Event.button.button == 1)
                {
                    if(Event.motion.xrel != 0 || Event.motion.yrel != 0)
                    {
                        offsetX += Event.motion.xrel / zoom;
                        offsetY += Event.motion.yrel / zoom;
                        RenderFrame(&resultVec);
                    }
                }                
            }
            if(Event.type == SDL_MOUSEWHEEL)
            {
                zoom += Event.wheel.y * 0.1;
                if(zoom <= 0)
                {
                    zoom = 0.1f;
                }
                RenderFrame(&resultVec);
            }
        }
        if(endApp)
        {
            break;
        }

        UpdateInputs(Event);

        Update();

        UpdatePreviousInputs(Event); //Updates previousinputs, used for keyUp and keyDown functions

        //fps limiter
        Sleep(std::max(0.0,(1000 / fpsLimiter) - (deltaTime * 1000)));

        //Calculating passing time.
        mingw_gettimeofday(&t2, NULL);
        elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
        elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
        deltaTime = elapsedTime / 1000;
        t1 = t2;
        sessionTime += deltaTime;
    }

    closeWindow();
    return 1;
}

void QuitApplication(){
    endApp = true;
}

void PrintVec(std::vector<Vector2> *v)
{
    for(int i = 0; i < v->size(); i++)
    {
        std::cout << i << ". {" << v->at(i).x << "," << v->at(i).y << "}" << std::endl;
    }
    std::cout << std::endl;
}

void Start()
{
    //Calculates a single reptile
    MirrorLeftSide(&leftSide);
    leftSide[0].draw = false;
    MirrorRightSide(&rightSide);
    CombineVectors(&resultVec, &leftSide);
    CombineVectors(&resultVec, &rightSide);

    //Flipped reptail on the left side
    
    std::vector<Vector2> copy;
    CopyVec(&resultVec, &copy);
    RotateVec(&copy, PI);
    MoveVec(&copy,1,1);
    CombineVectors(&resultVec, &copy);

    for(int i = 0; i < 3; i++)
    {
        std::vector<Vector2> copy;
        CopyVec(&resultVec, &copy);
        RotateVec(&copy,PI / 2 * (i+1));
        CombineVectors(&resultVec, &copy);
    }
    
    std::vector<Vector2> final;
    for(int i = 0; i < 3; i++)
    {
        for(int k = 0; k < 3; k++)
        {
            std::vector<Vector2> copy;
            CopyVec(&resultVec, &copy);
            MoveVec(&copy, i * 2, k * 2);
            CombineVectors(&final, &copy);
        }
    }
    resultVec = {};
    CopyVec(&final, &resultVec);
    RenderFrame(&resultVec);
}

void Update()
{
    if(GetActionDownByName("Left"))
    {

    }
    else if(GetActionDownByName("Right"))
    {

    }
    else if(GetActionDownByName("Up"))
    {

    }
    else if(GetActionDownByName("Down"))
    {

    }

    if(GetActionByName("Up2"))
    {
        offsetY += deltaTime * 100;
        RenderFrame(&resultVec);
    }
    else if(GetActionByName("Down2"))
    {
        offsetY -= deltaTime * 100;
        RenderFrame(&resultVec);
    }

    if(GetActionByName("Left2"))
    {
        offsetX += deltaTime * 100;
        RenderFrame(&resultVec);
    }
    else if(GetActionByName("Right2"))
    {
        offsetX -= deltaTime * 100;
        RenderFrame(&resultVec);
    }
}