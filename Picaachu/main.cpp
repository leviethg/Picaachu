#include <bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <ctime>

using namespace std;


SDL_Window *window;
SDL_Renderer *renderer; //khai báo biến màn hình
Mix_Music *gMusic; // nhạc nền

const SDL_Color CYAN_COLOR = {0, 255, 255};
const SDL_Color BLUE_COLOR = {0, 0, 255};
const SDL_Color ORANGE_COLOR = {255, 165, 0};
const SDL_Color YELLOW_COLOR = {255, 255, 0};
const SDL_Color LIME_COLOR = {0, 255, 0};
const SDL_Color PURPLE_COLOR = {128, 0, 128};
const SDL_Color RED_COLOR = {255, 0, 0};
const SDL_Color WHITE_COLOR = {255, 255, 255};
const SDL_Color BLACK_COLOR = {0, 0, 0};
const SDL_Color GREEN_COLOR = {0, 128, 0};
const SDL_Color DEFAULT_COLOR = BLACK_COLOR;
const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 750;
const int h_w = 50; // kích thước của một ô
const int row = 18; // số lượng cột
const int column = 11; // số lượng hàng
const int FRAME_DELAY = 12; // độ trễ
const int levelMax = 9; // số cấp độ max
const string WINDOW_TITLE = "PIKACHU";

SDL_Color getColor (const std::string& colorName) {
    if  (colorName == "CYAN") {
        return CYAN_COLOR;
    } else if  (colorName == "BLUE") {
        return BLUE_COLOR;
    } else if  (colorName == "ORANGE") {
        return ORANGE_COLOR;
    } else if  (colorName == "YELLOW") {
        return YELLOW_COLOR;
    } else if  (colorName == "LIME") {
        return LIME_COLOR;
    } else if  (colorName == "PURPLE") {
        return PURPLE_COLOR;
    } else if  (colorName == "RED") {
        return RED_COLOR;
    } else if  (colorName == "WHITE") {
        return WHITE_COLOR;
    } else if  (colorName == "BLACK") {
        return BLACK_COLOR;
    } else if  (colorName == "GREEN") {
        return GREEN_COLOR;
    } else {
        return DEFAULT_COLOR;
    }
}


const int const_row = 4 * h_w; // khoảng cách Map tới lề trái
const int const_column = 3 * h_w / 2; // khoảng cách Map tới lề trên

const SDL_Rect FULL_SCREEN = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
const SDL_Rect CHANGE_GAME = {1000, 25, 70, 70};
const SDL_Rect SOUND_GAME = {800, 25, 70, 70};
const SDL_Rect MAIN_GAME = {900, 25, 70, 70};

const SDL_Rect PLAY_MENU = {430, 333, 240, 96};
const SDL_Rect SCORE_MENU = {430, 45, 240, 96};
const SDL_Rect INTRO_MENU = {430, 590, 240, 96};

const SDL_Rect MENU_WAIT = {430, 333, 240, 96};
const SDL_Rect RESUME_WAIT = {430, 463, 240, 96};
const SDL_Rect RESTART_WAIT = {430, 593, 240, 96};

const SDL_Rect MENU_WIN = {410, 500, 280, 96};
const SDL_Rect NEW_WIN = {410, 622, 280, 96};
const SDL_Rect NEXT_LEVEL_WIN = {410, 622, 280, 96};

const SDL_Rect MENU_LOSE = {410, 382, 280, 96};
const SDL_Rect NEW_LOSE = {410, 500, 280, 96};
const SDL_Rect QUIT_LOSE = {410, 622, 280, 96};

const SDL_Rect BACK = {90, 90, 150, 60};

const SDL_Rect TITLE_END = {410, 252, 280, 120};
const SDL_Rect SCORE_END = {490, 350, 120, 90};
const SDL_Rect MENU_END = {410, 452, 280, 96};
const SDL_Rect QUIT_END = {410, 572, 280, 96};



map<int, int> m; // lưu trữ thời gian đã trôi qua
int arr[column][row] = {0}; // ma trận Map
int rank_score[11] = {0}; // lưu trữ bảng điểm

int score = 0; // Điểm nhận được
int level = 1; // cấp độ bắt đầu
int sound = 0; // âm thanh mở  (1 là âm thanh tắt)
int chance = 10; // số lượt thay đổi vị trí Map còn lại

// bảng menu
bool menu = true;
bool menu_score = false;
bool menu_intro = false;
bool menu_back = false;

// Ingame
bool game = false;
bool game_change = false;
bool game_win = false;
bool game_lose = false;

bool wait = false;
bool wait_resume = false;

//nếu win hoặc lose

bool end_game = false; // true nếu win hết
bool end_game2 = false;

bool new_game = false;
bool quit_game = false;

bool first_Check = false;
bool second_Check = true;

struct Point
{
int x;
int y;
};

Point first_Step, second_Step, tmp1, tmp2;

void reset_Game ()
{
    score = 0;
    level = 1;
    sound = 0;
    chance = 10;

    game = true;
    game_change = false;
    game_win = false;
    game_lose = false;

    end_game = false;
    end_game2 = false;

    first_Check = false;
    second_Check = true;

    tmp1 = {0, 0};
    tmp2 = {0, 0};
}

void reset_Next_Level ()
{
    level += 1;
    chance = max (chance + 2, 10);

    game = true;
    game_change = false;
    game_win = false;
    game_lose = false;
    end_game = false;
    end_game2 = false;

    first_Check = false;
    second_Check = true;

    tmp1 = {0, 0};
    tmp2 = {0, 0};
}

void logSDLError (ostream& os, const string &msg, bool fatal = false)
{
    os << msg << "Error: " << SDL_GetError () << endl;
    if (fatal)
    {
        SDL_Quit ();
        exit (1);
    }
}

void initSDL (SDL_Window* &window, SDL_Renderer* &renderer)
{
    if (SDL_Init (SDL_INIT_EVERYTHING) != 0) logSDLError (cout, "SDL_Init", true);

    window = SDL_CreateWindow (WINDOW_TITLE.c_str (), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) logSDLError (cout, "CreateWindow", true);

    renderer = SDL_CreateRenderer (window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    Mix_OpenAudio ( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );

    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize (renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void quitSDL (SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_DestroyRenderer (renderer);
    SDL_DestroyWindow (window);

    SDL_Quit ();
    IMG_Quit ();
    TTF_Quit ();
    Mix_Quit ();
}

void waitUntilKeyPressed ()
{
    SDL_Event e;
    while (true)
    {
        if (SDL_WaitEvent (&e) != 0 &&  (e.type == SDL_QUIT)) return;
        SDL_Delay (100);
    }
}

SDL_Texture* loadTexture (string path, SDL_Renderer* renderer)
{
    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load (path.c_str ());

    if (loadedSurface == nullptr)
    {
        cout<<"Unable to  load image " << path << " SDL_image Error: "
            << IMG_GetError () << endl;
    }
    else
    {
        newTexture = SDL_CreateTextureFromSurface (renderer, loadedSurface);
        if (newTexture == nullptr)
            cout<<"Unable to create texture from " << path << " SDL Error: "
                << SDL_GetError ()<< endl;
        SDL_FreeSurface ( loadedSurface);
    }

    return newTexture;
}

// tải ảnh
void renderTexture (string path, int x, int y, int w, int h)
{
    SDL_Rect rect = {x, y, w, h};
    SDL_Texture *file = loadTexture (path.c_str (), renderer);
    if (file == nullptr)
    {
        logSDLError (cout, "Fail", true);
        SDL_DestroyTexture (file);
        quitSDL (window, renderer);
    }
    SDL_RenderCopy (renderer, file, NULL, &rect);
    SDL_DestroyTexture (file);
}

void drawCircle (SDL_Renderer* renderer, int x, int y, int r)
{
    SDL_SetRenderDrawColor (renderer, 255, 255, 25, 0);

    for (double i = -r / sqrt (2); i <= r / sqrt (2); i += 0.01)
    {
        SDL_RenderDrawPoint (renderer, x + i, y + sqrt (r * r - i * i));
        SDL_RenderDrawPoint (renderer, x + i, y - sqrt (r * r - i * i));
    }

    for  (double i = -r / sqrt (2); i <= r / sqrt (2); i += 0.2)
    {
        SDL_RenderDrawPoint (renderer, x + sqrt (r * r - i * i), y + i);
        SDL_RenderDrawPoint (renderer, x - sqrt (r * r - i * i), y + i);
    }
}

// Bật hoặc tắt âm thanh
void mix ()
{
    sound =  (sound + 1) % 2;
    if (sound == 0) renderTexture ("photos/mix.png", SOUND_GAME.x, SOUND_GAME.y, SOUND_GAME.w, SOUND_GAME.h);
    else renderTexture ("photos/no_mix.png", SOUND_GAME.x, SOUND_GAME.y, SOUND_GAME.w, SOUND_GAME.h);
    if ( Mix_PlayingMusic () == 0 )
    {
        Mix_PlayMusic ( gMusic, -1 );
    }
    else
    {
        if ( Mix_PausedMusic () == 1 )
        {
            Mix_ResumeMusic ();
        }
        else
        {
            Mix_PauseMusic ();
        }
    }
}

void sound_Track ()
{
    gMusic = Mix_LoadMUS ( "sounds/music.mp3" );
    if ( gMusic == NULL )
    {
        printf ( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError () );
        Mix_Quit ();
    }
    else Mix_PlayMusic (gMusic, -1);
}

void sound_Delete ()
{
    Mix_Chunk *gDelete = Mix_LoadWAV ( "sounds/delete.wav" );
    Mix_PlayChannel ( -1, gDelete, 0 );
}

void sound_NoDelete ()
{
    Mix_Chunk *gNoDelete = Mix_LoadWAV ( "sounds/no_delete.wav" );
    Mix_PlayChannel ( -1, gNoDelete, 0 );
}

void sound_Turn ()
{
    Mix_Chunk *gTurn = Mix_LoadWAV ( "sounds/turn.wav" );
    Mix_PlayChannel ( -1, gTurn, 0 );
}

void load_Font (string path, int x, int y, int w, int h, SDL_Color c)
{
    if (game) renderTexture ("photos/sky.png", x, y, w, h);
    if  (TTF_Init () < 0)
    {
        SDL_Log ("%s", TTF_GetError ());
        SDL_Quit ();
    }

    TTF_Font *font = TTF_OpenFont ("font/ariblk.ttf", 70);

    SDL_Color fg = c;

    SDL_Surface *surface = TTF_RenderText_Solid (font, path.c_str (), fg);
    SDL_Texture *texture = SDL_CreateTextureFromSurface (renderer, surface);
    SDL_FreeSurface (surface);

    SDL_Rect srcRest;
    SDL_Rect desRect = {x, y, w, h};

    TTF_SizeText (font, path.c_str (), &srcRest.w, &srcRest.h);

    SDL_RenderCopy (renderer, texture, NULL, &desRect);
    SDL_RenderPresent (renderer);

    SDL_DestroyTexture (texture);
}

void background (const string &file)
{
    renderTexture (file, FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    SDL_RenderPresent (renderer);
}

int convertX (int n)
{
    return  (n - 1) * h_w + const_column;
}

int convertY (int n)
{
    return  (n - 1) * h_w + const_row;
}

int Ox (int n)
{
    return convertX (n) + h_w / 2;
}

int Oy (int n)
{
    return convertY (n) + h_w / 2;
}

void createArr ()
{
    srand (time (NULL));

    vector<Point> point;
    vector<int> val;

    for (int i = 1; i < column - 1; ++i)
    {
        for (int j = 1; j < row - 1; ++j)
        {
            point.push_back (Point{i, j});
        }
    }

    for (int i = 1; i <= 36; ++i)
    {
        val.insert (val.end (), 4, i);
    }

    random_shuffle (val.begin (), val.end ());

    int n = point.size ();
    for (int i = 0; i < n; ++i)
    {
        arr[point[i].x][point[i].y] = val[i];
    }
}void random ()
{
    vector<Point> point;
    vector<int> val;

    for (int i = 1; i < column - 1; ++i)
    {
        for (int j = 1; j < row - 1; ++j)
        {
            if (arr[i][j] > 0)
            {
                point.push_back ({i, j});
                val.push_back (arr[i][j]);
            }
        }
    }

    random_shuffle (val.begin (), val.end ());

    for (int i = 0; i < point.size (); ++i)
    {
        arr[point[i].x][point[i].y] = val[i];
    }
}

vector<pair<Point, Point>> checkX (Point A, Point B)
{
    vector<pair<Point, Point>> step_A;
    int minX_A = A.x, maxX_A = A.x;
    int minX_B = B.x, maxX_B = B.x;

    if (A.y == B.y || arr[A.x][A.y] != arr[B.x][B.y] || arr[A.x][A.y] * arr[B.x][B.y] == 0) return step_A;

    for (int i = A.x - 1; i >= 0; --i)
    {
        if (arr[i][A.y] == 0) minX_A = i;
        else break;
    }

    for (int i = B.x - 1; i >= 0; --i)
    {
        if (arr[i][B.y] == 0) minX_B = i;
        else break;
    }

    for (int i = A.x + 1; i < column; ++i)
    {
        if (arr[i][A.y] == 0) maxX_A = i;
        else break;
    }

    for (int i = B.x + 1; i < column; ++i)
    {
        if (arr[i][B.y] == 0) maxX_B = i;
        else break;
    }

    if (minX_A > maxX_B || maxX_A < minX_B) return step_A;

    int min_max = min (maxX_A, maxX_B);
    int max_min = max (minX_A, minX_B);

    for (int i = min (min_max, max_min); i <= max (min_max, max_min); ++i)
    {
        bool check = true;
        for (int j = min (A.y, B.y) + 1; j <= max (A.y, B.y) - 1; ++j)
        {
            if (arr[i][j] != 0) check = false;
        }

        if (check)
        {
            step_A.push_back ({{i, A.y}, {i, B.y}});
        }
    }

    return step_A;
}

vector<pair<Point, Point>> checkXY (Point A, Point B)
{
    vector<pair<Point, Point>> step_A = checkX (A, B);
    int minY_A = A.y, maxY_A = A.y;
    int minY_B = B.y, maxY_B = B.y;

    if (A.x == B.x || arr[A.x][A.y] != arr[B.x][B.y] || arr[A.x][A.y] * arr[B.x][B.y] == 0) return step_A;

    for (int i = A.y - 1; i >= 0; --i)
    {
        if (arr[A.x][i] == 0) minY_A = i;
        else break;
    }

    for (int i = B.y - 1; i >= 0; --i)
    {
        if (arr[B.x][i] == 0) minY_B = i;
        else break;
    }

    for (int i = A.y + 1; i < row; ++i)
    {
        if (arr[A.x][i] == 0) maxY_A = i;
        else break;
    }

    for (int i = B.y + 1; i < row; ++i)
    {
        if (arr[B.x][i] == 0) maxY_B = i;
        else break;
    }

    if (minY_A > maxY_B || maxY_A < minY_B) return step_A;

    int min_max = min (maxY_A, maxY_B);
    int max_min = max (minY_A, minY_B);

    for (int i = min (min_max, max_min); i <= max (min_max, max_min); ++i)
    {
        bool check = true;
        for (int j = min (A.x, B.x) + 1; j <= max (A.x, B.x) - 1; ++j)
        {
            if (arr[j][i] != 0) check = false;
        }

        if (check)
        {
            step_A.push_back ({{A.x, i}, {B.x, i}});
        }
    }

    return step_A;
}

pair<Point, Point> point_Road (Point A, Point B)
{
    pair<Point, Point> d = {A, B};
    int sum = 100;

    vector<pair<Point, Point>> s = checkXY (A, B);

    for (auto c : s)
    {
        Point A1 = c.first;
        Point B1 = c.second;

        int ans = abs (A.x - A1.x + A.y - A1.y) + abs (A1.x - B1.x + A1.y - B1.y) + abs (B1.x - B.x + B1.y - B.y) + 1;

        if (ans < sum ||  (ans == sum && A1.x == A.x && A1.y == A.y) ||  (ans == sum && B.x == B1.x && B.y == B1.y))
        {
            sum = ans;
            d = {A1, B1};
        }
    }

    return d;
}

bool check_Exit_Pair ()
{
    vector<Point> point;

    for (int i = 1; i < column - 1; ++i)
    {
        for (int j = 1; j <= row - 2; ++j)
        {
            if (arr[i][j] > 0) point.push_back ({i, j});
        }
    }

    for (int i = 0; i < point.size (); ++i)
    {
        for (int j = i + 1; j < point.size (); ++j)
        {
            Point A = {point[i].x, point[i].y};
            Point B = {point[j].x, point[j].y};
            if (checkXY (A, B).size () != 0)
            {
                return true;;
            }
        }
    }

    return false;
}

bool is_Game_Over ()
{
    for (int i = 0; i < column; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            if (arr[i][j] > 0) return false;
        }
    }

    return true;
}

void print_Road (Point A, Point B)
{
    pair<Point, Point> p = point_Road (A, B);

    Point B1 = p.second;
    Point A1 = p.first;

    SDL_SetRenderDrawColor (renderer, 204, 21, 16, 2);

    SDL_RenderDrawLine (renderer, Ox (A.y), Oy (A.x), Ox (A1.y), Oy (A1.x));
    SDL_RenderDrawLine (renderer, Ox (A1.y), Oy (A1.x), Ox (B1.y), Oy (B1.x));
    SDL_RenderDrawLine (renderer, Ox (B.y), Oy (B.x), Ox (B1.y), Oy (B1.x));
    SDL_RenderDrawLine (renderer, Ox (A.y) + 1, Oy (A.x) + 1, Ox (A1.y) + 1, Oy (A1.x) + 1);
    SDL_RenderDrawLine (renderer, Ox (A1.y) + 1, Oy (A1.x) + 1, Ox (B1.y) + 1, Oy (B1.x) + 1);
    SDL_RenderDrawLine (renderer, Ox (B.y) + 1, Oy (B.x) + 1, Ox (B1.y) + 1, Oy (B1.x) + 1);
    SDL_RenderDrawLine (renderer, Ox (A.y) - 1, Oy (A.x) - 1, Ox (A1.y) - 1, Oy (A1.x) - 1);
    SDL_RenderDrawLine (renderer, Ox (A1.y) - 1, Oy (A1.x) - 1, Ox (B1.y) - 1, Oy (B1.x) - 1);
    SDL_RenderDrawLine (renderer, Ox (B.y) - 1, Oy (B.x) - 1, Ox (B1.y) - 1, Oy (B1.x) - 1);
}

void print_Box (Point A)
{
    if (arr[A.x][A.y] > 0) return;
    renderTexture ("photos/sky.png", convertX (A.y), convertY (A.x), h_w, h_w);
}

void print_Road_Box (Point A, Point B)
{
    arr[A.x][A.y] = 0;
    arr[B.x][B.y] = 0;
    pair<Point, Point> p = point_Road (A, B);
    Point B1 = p.second;
    Point A1 = p.first;
    if (A.x == A1.x)
    {
        for (int i = min (A1.y, A.y); i <= max (A.y, A1.y); ++i)
        {
            print_Box ({A.x, i});
        }
    }
    else
    {
        for (int i = min (A1.x, A.x); i <= max (A.x, A1.x); ++i)
        {
            print_Box ({i, A.y});
        }
    }

    if (B1.x == A1.x)
    {
        for (int i = min (B1.y, A1.y); i <= max (B1.y, A1.y); ++i)
        {
            print_Box ({A1.x, i});
        }
    }
    else
    {
        for (int i = min (A1.x, B1.x); i <= max (B1.x, A1.x); ++i)
        {
            print_Box ({i, A1.y});
        }
    }

    if (B.x == B1.x)
    {
        for (int i = min (B1.y, B.y); i <= max (B1.y, B.y); ++i)
        {
            print_Box ({B.x, i});
        }
    }
    else
    {
        for (int i = min (B1.x, B.x); i <= max (B.x, B1.x); ++i)
        {
            print_Box ({i, B.y});
        }
    }
}

void print_Random ()
{
    for (int i = 0; i < column; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            SDL_Rect rect = {convertX (j), convertY (i), h_w, h_w};

            if (arr[i][j] > 0)
            {
                string s = "pokemons/" + to_string (arr[i][j]) + "_down.png";
                SDL_Texture *abc = loadTexture (s, renderer);
                SDL_RenderCopy (renderer, abc, NULL, &rect);
                SDL_DestroyTexture (abc);
            }
            else
            {
                print_Box ({i, j});
            }
        }
    }

    SDL_RenderPresent (renderer);
}

void print_Board ()
{
    for (int i = 0; i < column; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            cout << arr[i][j] << " ";
            if (arr[i][j] < 10) cout << "  ";
            else cout << " ";
        }
        cout << endl;
    }
    cout << "---------------------------" << endl << endl;
}

void print_Map ()
{
    renderTexture ("photos/sky.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);

    for (int i = 0; i < column; ++i)
    {
        for (int j = 0; j < row; ++j)
        {
            SDL_Rect rect = {convertX (j), convertY (i), h_w, h_w};

            if (arr[i][j] > 0)
            {
                string s = "pokemons/" + to_string (arr[i][j]) + "_down.png";
                SDL_Texture *abc = loadTexture (s, renderer);
                SDL_RenderCopy (renderer, abc, NULL, &rect);
                SDL_DestroyTexture (abc);
            }
            else
            {
                print_Box ({i, j});
            }
        }
    }
    renderTexture ("photos/change.png", CHANGE_GAME.x, CHANGE_GAME.y, CHANGE_GAME.w, CHANGE_GAME.h);
    renderTexture ("photos/main.png", MAIN_GAME.x, MAIN_GAME.y, MAIN_GAME.w, MAIN_GAME.h);
    if (sound == 0) renderTexture ("photos/mix.png", SOUND_GAME.x, SOUND_GAME.y, SOUND_GAME.w, SOUND_GAME.h);
    else renderTexture ("photos/no_mix.png", SOUND_GAME.x, SOUND_GAME.y, SOUND_GAME.w, SOUND_GAME.h);

    load_Font ("SCORE", 940, 190, 130, 70, PURPLE_COLOR);
    load_Font (to_string (score), 955, 250, 100, 70, BLACK_COLOR);

    load_Font ("CHANCE", 930, 350, 150, 70, PURPLE_COLOR);
    load_Font (to_string (chance), 985, 410, 40, 70, BLACK_COLOR);

    load_Font ("LEVEL", 940, 510, 130, 70, PURPLE_COLOR);
    load_Font (to_string (level), 985, 570, 40, 70, BLACK_COLOR);
}

void leVel (Point A, Point B, int n)
{
    switch (n)
    {
    case 1:
        print_Random ();
        break;
    case 2:
        if (A.x == B.x)
        {
            vector<int> v;
            for (int i = 1; i <= row - 2; ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v.push_back (arr[A.x][i]);

                    arr[A.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[A.x][i] = v[i - 1];
        }
        else
        {
            vector<int> v1, v2;
            for (int i = 1; i <= row - 2; ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v1.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
                if (arr[B.x][i] > 0)
                {
                    v2.push_back (arr[B.x][i]);
                    arr[B.x][i] = 0;
                }
            }
            for (int i = 1; i <= v1.size (); ++i) arr[A.x][i] = v1[i - 1];
            for (int i = 1; i <= v2.size (); ++i) arr[B.x][i] = v2[i - 1];
        }
        print_Random ();

        break;

    case 3:

if (A.y == B.y)
        {
            vector<int> v;

            for (int i = 1; i < column - 1; ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[i][A.y] = v[i - 1];
        }
        else
        {
            vector<int> v1, v2;

            for (int i = 1; i < column - 1; ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v1.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
                if (arr[i][B.y] > 0)
                {
                    v2.push_back (arr[i][B.y]);
                    arr[i][B.y] = 0;
                }
            }
            for (int i = 1; i <= v1.size (); ++i) arr[i][A.y] = v1[i - 1];

            for (int i = 1; i <= v2.size (); ++i) arr[i][B.y] = v2[i - 1];
        }
        print_Random ();
        break;

    case 4:
	if (A.y == B.y)
        {
            vector<int> v;

            for (int i = 1; i < column - 1; ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
            }
         for (int i = 1; i <= v.size (); ++i) arr[column - 2 - v.size () + i][A.y] = v[i - 1];
        }
        else
        {
            vector<int> v1, v2;
            for (int i = 1; i < column - 1; ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v1.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }

                if (arr[i][B.y] > 0)
                {
                    v2.push_back (arr[i][B.y]);
                    arr[i][B.y] = 0;
                }
            }
            for (int i = 1; i <= v1.size (); ++i) arr[column - 2 - v1.size () + i][A.y] = v1[i - 1];
            for (int i = 1; i <= v2.size (); ++i) arr[column - 2 - v2.size () + i][B.y] = v2[i - 1];
        }
        print_Random ();

        break;

    case 5:
if (A.y <= row/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= row/2 - 1; ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[A.x][row/2 - 1 - v.size () +i] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = row/2; i <=  (row - 2); ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[A.x][row/2 - 1 + i] = v[i - 1];
        }
        if (B.y <= row/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= row/2 - 1; ++i)
            {
                if (arr[B.x][i] > 0)
                {
                    v.push_back (arr[B.x][i]);
                    arr[B.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[B.x][row/2 - 1 - v.size () +i] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for  (int i = row/2; i < row - 1; ++i)
            {
                if (arr[B.x][i] > 0)
                {
                    v.push_back (arr[B.x][i]);
                    arr[B.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[B.x][row/2 - 1 + i] = v[i - 1];
        }
        print_Random ();

        break;

    case 6:
         if (A.x == B.x)
        {
            vector<int> v;
            for (int i = 1; i <= row - 2; ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[A.x][row - 2 - v.size () + i] = v[i - 1];
        }
        else
        {
            vector<int> v1, v2;
            for (int i = 1; i <= row - 2; ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v1.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
                if (arr[B.x][i] > 0)
                {
                    v2.push_back (arr[B.x][i]);
                    arr[B.x][i] = 0;
                }
            }
            for (int i = 1; i <= v1.size (); ++i) arr[A.x][row - 2 - v1.size () + i] = v1[i - 1];
            for (int i = 1; i <= v2.size (); ++i) arr[B.x][row - 2 - v2.size () + i] = v2[i - 1];
        }
        print_Random ();

        break;



    case 7:
	        if (A.x <= column/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= column/2 - 1; ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[i][A.y] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = column/2; i <=  (column - 2); ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[column - 2 - v.size () + i][A.y] = v[i - 1];
        }
        if (B.x <= column/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= column/2 - 1; ++i)
            {
                if (arr[i][B.y] > 0)
                {
                    v.push_back (arr[i][B.y]);
                    arr[i][B.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[i][B.y] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = column/2; i <=  (column - 2); ++i)
            {
                if (arr[i][B.y] > 0)
                {
                    v.push_back (arr[i][B.y]);
                    arr[i][B.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[column - 2 - v.size () + i][B.y] = v[i - 1];
        }
        print_Random ();
        break;

    case 8:
        if (A.y <= row/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= row/2 - 1; ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[A.x][i] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = row/2; i <=  (row - 2); ++i)
            {
                if (arr[A.x][i] > 0)
                {
                    v.push_back (arr[A.x][i]);
                    arr[A.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[A.x][row - 2 - v.size () + i] = v[i - 1];
        }
        if (B.y <= row/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= row/2 - 1; ++i)
            {
                if (arr[B.x][i] > 0)
                {
                    v.push_back (arr[B.x][i]);
                    arr[B.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[B.x][i] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = row/2; i <=  (row - 2); ++i)
            {
                if (arr[B.x][i] > 0)
                {
                    v.push_back (arr[B.x][i]);
                    arr[B.x][i] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[B.x][row - 2 - v.size () + i] = v[i - 1];
        }
        print_Random ();
        break;

    case 9:
        if (A.x <= column/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= column/2 - 1; ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[column/2 - 1 - v.size () + i][A.y] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = column/2; i <=  (column - 2); ++i)
            {
                if (arr[i][A.y] > 0)
                {
                    v.push_back (arr[i][A.y]);
                    arr[i][A.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[column/2 - 1 + i][A.y] = v[i - 1];
        }
        if (B.x <= column/2 - 1)
        {
            vector<int> v;
            for (int i = 1; i <= column/2 - 1; ++i)
            {
                if (arr[i][B.y] > 0)
                {
                    v.push_back (arr[i][B.y]);
                    arr[i][B.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[column/2 - 1 - v.size () + i][B.y] = v[i - 1];
        }
        else
        {
            vector<int> v;
            for (int i = column/2; i <=  (column - 2); ++i)
            {
                if (arr[i][B.y] > 0)
                {
                    v.push_back (arr[i][B.y]);
                    arr[i][B.y] = 0;
                }
            }
            for (int i = 1; i <= v.size (); ++i) arr[column/2 - 1 + i][B.y] = v[i - 1];
        }
        print_Random ();

        break;

    default:

        break;
    }
}


void push_Score ()
{
    ifstream f ("highscore.txt");
    for (int i = 0; i < 11; ++i)
    {
        f >> rank_score[i];
    }
    f.close ();
    ofstream file ("highscore.txt");

    rank_score[10] = max (rank_score[10], score);
    sort (rank_score, rank_score + 11);
    reverse (rank_score, rank_score + 11);
    for (int i = 0; i < 11; ++i)
    {
        file << rank_score[i] << endl;
    }
    file.close ();

}

bool return_Mouse (int x, int y, SDL_Rect rect)
{
    return  (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

bool return_Mouse1 (int x, int y, SDL_Rect rect)
{
    if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) return true;
    return false;
}

void mouse ()
{
    SDL_Event e;
    SDL_PollEvent (&e);

    if (e.type == SDL_QUIT)
    {
        SDL_Quit ();
        quit_game = true;
    }
    if (e.type == SDL_MOUSEBUTTONUP)
    {
        int x = e.button.x;
        int y = e.button.y;

        // đang hiện menu
        if (menu)
        {
            if (return_Mouse (x, y, INTRO_MENU))
            {
                menu_intro = true;
            }
            if (return_Mouse (x, y, SCORE_MENU))
            {
                menu_score = true;
            }
            if (return_Mouse (x, y, PLAY_MENU))
            {
                sound_Track ();
                new_game = true;
                game = true;
            }

            menu = false;
        }

        else if (menu_back && return_Mouse (x, y, BACK))
        {
            menu_intro = false;
            menu_score = false;
            menu = true;
            menu_back = false;

        }

        // đang hiện màn hình chờ
        else if (wait)
        {
            if (return_Mouse (x, y, MENU_WAIT))
            {
                Mix_HaltMusic ();
                menu = true;
                push_Score ();
            }
            if (return_Mouse (x, y, RESUME_WAIT))
            {
                wait_resume = true;
            }
            if (return_Mouse (x, y, RESTART_WAIT))
            {
                game = true;
                new_game = true;
                first_Check = false;
                second_Check = true;
            }

            wait = false;
        }

        //đang ở trong game
        else if (game)
        {
            if (return_Mouse (x, y, MAIN_GAME))
            {
                wait = true;
                game = false;
            }

            if (return_Mouse (x, y, SOUND_GAME))
            {
                mix ();
                if (sound == 1) Mix_PauseMusic ();
                else Mix_ResumeMusic ();
            }

            if (return_Mouse (x, y, CHANGE_GAME))
            {
                game_change = true;
            }

            x -= const_column;
            x =  (x + h_w - 1) / h_w;
            y -= const_row;
            y =  (y + h_w - 1) / h_w;

            if (x >= 1 && y >= 1 && y < column - 1 && x <= row - 2 && arr[y][x] > 0)
            {
                if (first_Check && !second_Check)
                {
                    second_Step = {y, x};
                    tmp2 = {y, x};
                    second_Check = true;
                    first_Check = false;
                    sound_Turn ();
                    renderTexture ("pokemons/" + to_string (arr[tmp2.x][tmp2.y]) + "_up.png", convertX (tmp2.y), convertY (tmp2.x), h_w, h_w);
                }
                else if (!first_Check && second_Check)
                {
                    first_Step = {y, x};
                    tmp1 = {y, x};
                    second_Check = false;
                    first_Check = true;
                    sound_Turn ();
                    renderTexture ("pokemons/" + to_string (arr[tmp1.x][tmp1.y]) + "_up.png", convertX (tmp1.y), convertY (tmp1.x), h_w, h_w);
                }
            }
        }

        else if (end_game2)
        {
            if (return_Mouse (x, y, MENU_END))
            {
                menu = true;
                end_game2 = false;
            }

            if (return_Mouse (x, y, QUIT_END))
            {
                quit_game = true;
                end_game2 = false;
            }
        }


        else if (game_win)
        {
            if (return_Mouse (x, y, MENU_WIN))
            {
                menu = true;
                game_win = false;
            }

            if (return_Mouse (x, y, NEW_WIN))
            {
                new_game = true;
                game = true;
                game_win = false;
            }

            if (return_Mouse (x, y, NEXT_LEVEL_WIN))
            {
                reset_Next_Level ();
                createArr ();
                print_Map ();
                game = true;
                game_win = false;
            }
        }


        else if (game_lose)
        {
            if (return_Mouse (x, y, MENU_LOSE))
            {
                menu = true;
            }

            if (return_Mouse (x, y, NEW_LOSE))
            {
                new_game = true;
                game = true;
            }

            if (return_Mouse (x, y, QUIT_LOSE))
            {
                quit_game = true;
            }
            game_lose = false;
        }
        game_lose = false;
    }
}

 void print_MENU ()
{
    SDL_RenderClear (renderer);
    renderTexture  ("photos/sky_menu.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    renderTexture  ("photos/play.png", PLAY_MENU.x, PLAY_MENU.y, PLAY_MENU.w, PLAY_MENU.h);
    renderTexture  ("photos/intro.png", INTRO_MENU.x, INTRO_MENU.y, INTRO_MENU.w, INTRO_MENU.h);
    renderTexture  ("photos/scr.png", SCORE_MENU.x, SCORE_MENU.y, SCORE_MENU.w, SCORE_MENU.h);
}

 void print_WAIT ()
{
    SDL_RenderClear  (renderer);
    renderTexture  ("photos/sky_menu.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    renderTexture  ("photos/home.png", PLAY_MENU.x, PLAY_MENU.y, PLAY_MENU.w, PLAY_MENU.h);
    renderTexture  ("photos/resume.png", RESUME_WAIT.x, RESUME_WAIT.y, RESUME_WAIT.w, RESUME_WAIT.h);
    renderTexture  ("photos/restart.png", RESTART_WAIT.x, RESTART_WAIT.y, RESTART_WAIT.w, RESTART_WAIT.h);
}

 void print_RANK ()
{
     SDL_RenderClear  (renderer);
    renderTexture  ("photos/sky_menu.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    renderTexture  ("photos/scoreBoard.png", 150, 300, 800, 300);
    renderTexture  ("photos/back.png", BACK.x, BACK.y, BACK.w, BACK.h);

    load_Font  ("Top 1 : " + to_string (rank_score[0]), 210, 330, 180, 70, RED_COLOR);
    load_Font  ("Top 2 : " + to_string (rank_score[1]), 210, 410, 180, 70, RED_COLOR);
    load_Font  ("Top 3 : " + to_string (rank_score[2]), 210, 490, 180, 70, RED_COLOR);
    load_Font  ("Top 4 : " + to_string (rank_score[3]), 465, 300, 180, 70, RED_COLOR);
    load_Font  ("Top 5 : " + to_string (rank_score[4]), 465, 370, 180, 70, RED_COLOR);
    load_Font  ("Top 6 : " + to_string (rank_score[5]), 465, 450, 180, 70, RED_COLOR);
    load_Font  ("Top 7 : " + to_string (rank_score[6]), 465, 530, 180, 70, RED_COLOR);
    load_Font  ("Top 8 : " + to_string (rank_score[7]), 720, 330, 180, 70, RED_COLOR);
    load_Font  ("Top 9 : " + to_string (rank_score[8]), 720, 410, 180, 70, RED_COLOR);
    load_Font  ("Top 10 : " + to_string (rank_score[9]), 720, 490, 180, 70, RED_COLOR);
}

 void print_END ()
{
     SDL_RenderClear (renderer);
    renderTexture  ("photos/youwin.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);

    load_Font  ("YOUR SCORE", TITLE_END.x, TITLE_END.y, TITLE_END.w, TITLE_END.h, PURPLE_COLOR);
    load_Font  (to_string (score), SCORE_END.x, SCORE_END.y, SCORE_END.w, SCORE_END.h, BLUE_COLOR);

    renderTexture  ("photos/menu_end.png", MENU_END.x, MENU_END.y, MENU_END.w, MENU_END.h);

    renderTexture  ("photos/quit_end.png", QUIT_END.x, QUIT_END.y, QUIT_END.w, QUIT_END.h);

}
 void print_INTRO ()
{
     SDL_RenderClear (renderer);
    renderTexture  ("photos/sky_menu.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    renderTexture  ("photos/back.png", BACK.x, BACK.y, BACK.w, BACK.h);
    renderTexture  ("photos/intro_menu.png", 125, 250, 850, 450);
}


void print_LOSE ()
{
    renderTexture  ("photos/youlose.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    renderTexture  ("photos/menu_end.png", MENU_LOSE.x, MENU_LOSE.y, MENU_LOSE.w, MENU_LOSE.h);
    renderTexture  ("photos/new_end.png", NEW_LOSE.x, NEW_LOSE.y, NEW_LOSE.w, NEW_LOSE.h);
    renderTexture  ("photos/quit_end.png", QUIT_LOSE.x, QUIT_LOSE.y, QUIT_LOSE.w, QUIT_LOSE.h);
}

void print_NEXT_LEVEL ()
{
    SDL_RenderClear  (renderer);
    renderTexture  ("photos/youwin.png", FULL_SCREEN.x, FULL_SCREEN.y, FULL_SCREEN.w, FULL_SCREEN.h);
    renderTexture  ("photos/menu_end.png", MENU_WIN.x, MENU_WIN.y, MENU_WIN.w, MENU_WIN.h);
    renderTexture  ("photos/new_end.png", NEW_WIN.x, NEW_WIN.y, NEW_WIN.w, NEW_WIN.h);
    renderTexture  ("photos/next_level.png", NEXT_LEVEL_WIN.x, NEXT_LEVEL_WIN.y, NEXT_LEVEL_WIN.w, NEXT_LEVEL_WIN.h);
}

 void Render ()
{
    if  (new_game)
    {
        reset_Game ();
        createArr ();
        print_Map ();
        new_game = false;
    }

    if  (game)
    {
        if  (!new_game)
        {

        }
        if  (second_Check && arr[first_Step.x][first_Step.y] * arr[second_Step.x][second_Step.y] > 0)
        {
            if  (checkXY (first_Step, second_Step).size () != 0)
            {
                score += 10 +  (level - 1) * 2;
                load_Font  ("SCORE", 940, 190, 130, 70, PURPLE_COLOR);
                load_Font  (to_string (score), 955, 250, 100, 70, YELLOW_COLOR);

                sound_Delete ();
                print_Road  (first_Step, second_Step);
                SDL_RenderPresent  (renderer);
                print_Road_Box  (first_Step, second_Step);
                leVel  (first_Step, second_Step, level);
                SDL_RenderPresent  (renderer);
            }
            else
            {
               if  (tmp2.x != 0 && tmp2.y != 0 && tmp1.x != 0 && tmp1.y != 0)
                {
                    renderTexture  ("pokemons/" + to_string (arr[tmp2.x][tmp2.y]) + "_down.png", convertX (tmp2.y), convertY (tmp2.x), h_w, h_w);
                    renderTexture  ("pokemons/" + to_string (arr[tmp1.x][tmp1.y]) + "_down.png", convertX (tmp1.y), convertY (tmp1.x), h_w, h_w);
                    sound_NoDelete ();
                }
            }

            tmp1 = {0, 0};
            tmp2 = {0, 0};
        }

        if  (is_Game_Over ())
        {
            if  (level < levelMax)
            {
                game_win = true;
            }
            else if  (level == levelMax)
            {
                end_game = true;
            }
            game = false;

        }

        else if  (!check_Exit_Pair ())
        {
            if  (chance == 0)
            {
                push_Score ();
                game_lose = true;
                game = false;
            }
            else
            {
                chance--;
                random ();
                print_Random ();
                load_Font  ("CHANCE", 930, 350, 150, 70, PURPLE_COLOR);
                load_Font  (to_string (chance), 985, 410, 40, 70, YELLOW_COLOR);
            }
        }

        if  (game_change && chance > 0)
        {
            random ();
            print_Random ();
            game_change = false;
            chance--;
            load_Font  ("CHANCE", 930, 350, 150, 70, PURPLE_COLOR);
            load_Font  (to_string (chance), 985, 410, 40, 70, YELLOW_COLOR);
            first_Check = false;
            second_Check = true;
        }


    }

    if  (end_game)
    {
        push_Score ();
        print_END ();
        end_game = false;
        end_game2 = true;
    }

    if  (game_win)
    {
        print_NEXT_LEVEL ();
    }

    if  (game_lose)
    {
        print_LOSE ();
    }

    if  (menu)
    {
        print_MENU ();
    }

    if  (menu_score)
    {
        SDL_RenderClear (renderer);
        print_RANK ();
        menu_score = false;
        menu_back = true;
    }

    if (menu_intro)
    {
        print_INTRO ();
        menu_back = true;

    }

    if  (wait)
    {
        print_WAIT ();
    }

    if  (wait_resume)
    {
        print_Map ();
        wait_resume = false;
        wait = false;
        game = true;
    }
}

int main  (int argc, char* argv[])
{
    ios_base::sync_with_stdio  (false);

    initSDL  (window, renderer);

    Uint32 frameStart;
    int frameTime;

    createArr ();

    while  (!quit_game)
    {
        frameStart = SDL_GetTicks ();

        Render ();
        mouse ();

        SDL_RenderPresent  (renderer);
        frameTime = SDL_GetTicks () - frameStart;

        if  (FRAME_DELAY > frameTime)
        {
            SDL_Delay  (FRAME_DELAY - frameTime);
        }

    }
    quitSDL  (window, renderer);
    return 0;
}
