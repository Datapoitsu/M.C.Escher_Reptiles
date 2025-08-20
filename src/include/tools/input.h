#ifndef _INPUTH_
#define _INPUTH_

#include <SDL2/SDL.h>
#include <string.h>
#include <iostream>

struct Buttons //Has all buttons for an action. Action can have more than one buttons struct.
{
    SDL_KeyCode keys[3];
    bool isPressed[3] = {false,false,false};
    bool previouslyPressed[3] = {false,false,false};
};

struct Action //Action is something that you would see in binding settings as an action.
{
    char name[20];
    struct Buttons buttons[2];
};

Action actions[] = {
    {
        {"Up"},
        {
            {
                {SDLK_w,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Down"},
        {
            {
                {SDLK_s,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Left"},
        {
            {
                {SDLK_a,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Right"},
        {
            {
                {SDLK_d,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Up2"},
        {
            {
                {SDLK_UP,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Down2"},
        {
            {
                {SDLK_DOWN,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Left2"},
        {
            {
                {SDLK_LEFT,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
    {
        {"Right2"},
        {
            {
                {SDLK_RIGHT,SDLK_UNKNOWN,SDLK_UNKNOWN},
            }
        }
    },
};  

void UnbindAll(Action a[]) //
{
    for(int i = 0; i < sizeof(*a) / 12; i++){
        for(int k = 0; k < 2; k++){
            for(int j = 0; j < 3; j++){
                a[i].buttons[k].keys[j] = SDLK_UNKNOWN;
            }
        }
    }
}

SDL_Keycode ConfigToKeycode(std::string s)
{
    std::cout << "config to keycode: " << s << std::endl;
    if(s == "a"){return SDLK_a;}
    if(s == "b"){return SDLK_b;}
    if(s == "d"){return SDLK_d;}
    if(s == "e"){return SDLK_e;}
    if(s == "r"){return SDLK_r;}
    if(s == "s"){return SDLK_s;}
    if(s == "t"){return SDLK_t;}
    if(s == "w"){return SDLK_w;}
    if(s == "up"){return SDLK_UP;}
    if(s == "down"){return SDLK_DOWN;}
    if(s == "1"){return SDLK_1;}
    if(s == "2"){return SDLK_2;}
    if(s == "3"){return SDLK_3;}
    if(s == "4"){return SDLK_4;}
    if(s == "5"){return SDLK_5;}
    if(s == "6"){return SDLK_6;}
    if(s == "7"){return SDLK_7;}
    if(s == "8"){return SDLK_8;}
    if(s == "9"){return SDLK_9;}
    return SDLK_UNKNOWN;
}

void ReadConfig(Action a[]) //Sets the values to actions.
{
    FILE *fptr;

    fptr = fopen("Binding.config", "r");

    if(fptr == NULL)
    {
        printf("Not able to open the file.");
        fclose(fptr); 
        return;
    }

    UnbindAll(a); //Clears old bindings for the next one
    
    const int bufferSize = 100;
    char myString[bufferSize];

    while(fgets(myString, bufferSize, fptr)) // Read the content and print it.
    {
        std::cout << "\n" << std::endl;
        printf("%s", myString);
        //Count the size of the name of the action.
        int nameLength = 0;
        for(int i = 0; i < bufferSize; i++)
        {
            if(myString[i] == '{')
            {
                break;
            }
            nameLength++;
        }

        for(int i = 0; i < sizeof(*a) / 12; i++) //Loop actions.
        {
            bool sameName = true;
            for(int k = 0; k < nameLength; k++) //Loop action name
            {
                if(a[i].name[k] != myString[k])
                {
                    sameName = false;
                    break;
                }
            }

            if(sameName == true)
            {
                int holder = nameLength + 1;
                int curButton = 0;
                int curKey = 0;
                std::cout << "Holder: " << holder << ", size:" << sizeof(myString) << std::endl;
                for(int k = holder; k < sizeof(myString); k++)
                {
                    if(myString[k] == ',' || myString[k] == '}')
                    {
                        char h[0];
                        memcpy(h, &myString[holder], k - holder);

                        std::cout << "Cur letter: " << h << "; holder: " << holder << " k: " << k << ", k-holder: " << k - holder << ", size(h): " << sizeof(h) << std::endl;
                        a[i].buttons[curButton].keys[curKey] = (SDL_KeyCode)ConfigToKeycode(h);
                        if(myString[k] == ',')
                        {
                            holder = k + 1;
                            curKey++;
                        }
                        else
                        {
                            holder = k + 3;
                            k += 2;
                            curKey = 0;
                            curButton++;
                        }
                    }
                }
            }
        }  
    }

    fclose(fptr); // Close the file

    printf("\n");
    for(int i = 0; i < sizeof(*a) / 12; i++) //Loop actions.
    {
        std::cout << a[i].name << ", 1: " << (char)a[i].buttons[0].keys[0] << ", " << (char)a[i].buttons[0].keys[1] << ", " << (char)a[i].buttons[0].keys[2] << " 2: " << (char)a[i].buttons[1].keys[0] << ", " << (char)a[i].buttons[1].keys[1] << ", " << (char)a[i].buttons[1].keys[2] << std::endl;
    }
    
}

void UpdateInputs(SDL_Event e) //Checks all defined actions and if the buttons are pressed.
{
    for(int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++)
    {
        for(int k = 0; k < 2; k++)
        {
            for(int j = 0; j < 3; j++)
            {
                if (e.key.keysym.sym == actions[i].buttons[k].keys[j])
                {
                    if(e.type == SDL_KEYDOWN) //Key pressed down
                    {
                        actions[i].buttons[k].isPressed[j] = true;
                    }
                    if(e.type == SDL_KEYUP) //Key pressed up
                    {
                        actions[i].buttons[k].isPressed[j] = false;
                    }
                }
            }
        }
    }
}

void UpdatePreviousInputs(SDL_Event e) //Sets values for previously pressed buttons. Used for GetKeyUp & GetKeyDown.
{
    for(int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++)
    {
        for(int k = 0; k < 2; k++)
        {
            for(int j = 0; j < 3; j++)
            {
                actions[i].buttons[k].previouslyPressed[j] = actions[i].buttons[k].isPressed[j];
            }
        }
    }
}

bool GetAction(struct Action a)
{
    for(int i = 0; i < 2; i++)
    {
        //Checks if there isn't any bindings.
        bool emptyButtons = true;
        for(int k = 0; k < 3; k++){
            if(a.buttons[i].keys[k] != SDLK_UNKNOWN){
                emptyButtons = false;
                break;
            }
        }
        if(emptyButtons == true) //Current actions had no binding at all.
        {
            continue;
        }
        
        //Check buttons.
        bool isPressed = true;
        for(int k = 0; k < 3; k++){
            //If non-empty button isn't pressed, then no input.
            if(a.buttons[i].keys[k] != SDLK_UNKNOWN && a.buttons[i].isPressed[k] == false){
                isPressed = false;
                break;
            }
        }
        if(isPressed)
        {
            return true;
        }
    }
    return false;
}

bool GetActionByName(std::string name) //Version with string.
{
    //Checks through all actions to find the correct action.
    for(int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++){
        if(actions[i].name == name){
            return GetAction(actions[i]);
        }
    }
    //printf("Couldn't find an input for action named: %s \n",name);
    return false;
};

bool GetActionDown(Action a) //Action down is the moment when key goes from up state to down state
{
    for(int i = 0; i < 2; i++)
    {
        //Checks if there isn't any bindings
        bool emptyButtons = true;
        for(int k = 0; k < 3; k++){
            if(a.buttons[i].keys[k] != SDLK_UNKNOWN){
                emptyButtons = false;
                break;
            }
        }
        if(emptyButtons == true) //Current actions had no binding at all.
        {
            continue;
        }
        
        //Check buttons
        bool isPressed = true;
        for(int k = 0; k < 3; k++)
        {
            //If non-empty button isn't pressed, then no input
            if(a.buttons[i].keys[k] != SDLK_UNKNOWN && a.buttons[i].isPressed[k] == false)
            {
                isPressed = false;
                break;
            }
        }

        if(isPressed) //Checking if previously pressed is false
        {
            for(int k = 0; k < 3; k++){
                if(a.buttons[i].previouslyPressed[k] == false && a.buttons[i].keys[k] != SDLK_UNKNOWN)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool GetActionDownByName(std::string name) //Version with string.
{
    for(int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++){
        if(actions[i].name == name){
            return GetActionDown(actions[i]);
        }
    }
    //printf("Couldn't find an input for action named: %s \n",name);
    return false;
}

bool GetActionUp(Action a)
{
    for(int i = 0; i < 2; i++)
    {
        //Checks if there isn't any bindings
        bool emptyButtons = true;
        for(int k = 0; k < 3; k++){
            if(a.buttons[i].keys[k] != SDLK_UNKNOWN){
                emptyButtons = false;
                break;
            }
        }
        if(emptyButtons == true){
            continue;
        }
        
        //Check that if a button isn't pressed
        bool isntPressed = false;
        for(int k = 0; k < 3; k++){
            if(a.buttons[i].keys[k] != SDLK_UNKNOWN && a.buttons[i].isPressed[k] == false){
                isntPressed = true;
                break;
            }
        }

        if(isntPressed)
        {
            //Checking if previously pressed is true
            int prevPressed = 0;
            for(int k = 0; k < 3; k++)
            {
                if(a.buttons[i].keys[k] == SDLK_UNKNOWN || a.buttons[i].previouslyPressed[k] == true)
                { 
                    prevPressed++;
                }
            }
            if(prevPressed == 3){
                return true;
            }
        }
    }
    return false;
}

bool GetActionUpByName(std::string name) //Version with string.
{
    for(int i = 0; i < sizeof(actions) / sizeof(actions[0]); i++){
        if(actions[i].name == name){
            return GetActionUp(actions[i]);
        }
    }
    //printf("Couldn't find an input for action named: %s \n",name);
    return false;
}

#endif