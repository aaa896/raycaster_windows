#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdint.h>
#include <math.h>
#define PI 3.14

bool global_running = true;
struct Input {
    bool left_down;
    bool left_was_down;
    bool down_down;
    bool down_was_down;
    bool up_down;
    bool up_was_down;
    bool right_down;
    bool right_was_down;
    bool tab_down;
    bool tab_was_down;
    bool tab_just_pressed;
};


Input global_input = {};

enum State {
    topdown,
    lines,
};

struct Buffer {
    BITMAPINFO bm_info;
    uint32_t  *data;
    int width;
    int height;
};


#define MAP_ROWS  20
#define MAP_COLS  20
#define WALL_COLOR 0x009000FF
#define EMPTY_COLOR 0x28282828
int map[MAP_ROWS][MAP_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


void clear_bitmap( Buffer *buffer , uint32_t color) 
{
    for (int row = 0; row < buffer->height; ++row) {
        for (int col = 0; col <buffer->width; ++col) {
            buffer->data[ (row * buffer->width) + col] = color;
        }
    }
}


void draw_map_lines(Buffer *buffer) {
    int cell_width = buffer->width/MAP_COLS ;
    int cell_height = buffer->height/MAP_ROWS ;
    for (int y = 0; y <buffer->height; y += (cell_height -1)) {
        for (int x = 0; x <buffer->width; ++x) {
            buffer->data[ (y * buffer->width) + x] = 0x00AE0000;
        }
        y+=1;
    }

    for (int x = 0; x <buffer->width; x += (cell_width -1)) {
        for (int y = 0; y <buffer->height; ++y) {
            buffer->data[ (y * buffer->width) + x] = 0x00AE0000;
        }
        ++x;
    }
}


void draw_map(Buffer *buffer) 
{
    int cell_width = buffer->width/MAP_COLS ;
    int cell_height = buffer->height/MAP_ROWS ;
    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col <MAP_COLS; ++col) {
            if (map[row][col] == 1) {

                int start_y = row * cell_height;
                for (int y = start_y; y <(start_y + cell_height); ++y) {

                    int start_x = col * cell_width;
                    for (int x = start_x; x < (start_x + cell_width ); ++x) {
                        buffer->data[ ( (y*buffer->width) + x) ] = WALL_COLOR;
                    }
                }
            }
            else if (map[row][col] == 0) {
                int start_y = row * cell_height;
                for (int y = start_y; y <(start_y + cell_height); ++y) {
                    int start_x = col * cell_width;
                    for (int x = start_x; x < (start_x + cell_width); ++x) {
                        buffer->data[ ( (y*buffer->width) + x) ] = EMPTY_COLOR;
                    }
                }
            }

        }

    }
}


void draw_rectangle(Buffer *buffer, int x, int y, int width, int height, uint32_t color) 
{
    for (int row  = y; row < (y + height); ++row) {
        for (int col = x;  col < (x + width); ++col) {
            buffer->data[(row * buffer->width) + col] = color;
        }
    }
}


void draw_circle(Buffer *buffer, int x, int y, int radius, uint32_t color) 
{
    int y_dist = radius;
    int x_dist = 0;
    while (y_dist > x_dist) {
        buffer->data[((y - y_dist) * buffer->width) + (x - x_dist)] = color;
        buffer->data[((y - y_dist) * buffer->width) + (x + x_dist)] = color;
        buffer->data[((y + y_dist) * buffer->width) + (x - x_dist)] = color;
        buffer->data[((y + y_dist) * buffer->width) + (x + x_dist)] = color;

        buffer->data[((y - x_dist) * buffer->width) + (x - y_dist)] = color;
        buffer->data[((y - x_dist) * buffer->width) + (x + y_dist)] = color;
        buffer->data[((y + x_dist) * buffer->width) + (x - y_dist)] = color;
        buffer->data[((y + x_dist) * buffer->width) + (x + y_dist)] = color;

        ++x_dist;
        y_dist = sqrt( (radius * radius) - (x_dist * x_dist) );

    }
}


void render_bitmap(HDC hdc, int window_width, int window_height, Buffer *buffer) 
{
    StretchDIBits(
            hdc,
            0,0, buffer->width, buffer->height,
            0,0, buffer->width, buffer->height,
            buffer->data, &buffer->bm_info,
            DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) 
{
    LRESULT result = 0;

    switch (msg) 
    {
        case WM_KEYUP:
        case WM_KEYDOWN: {
                             uint32_t KEY_WAS_DOWN = ((l_param & (1<<30)) != 0);
                             uint32_t KEY_IS_DOWN = ((l_param & (1<<31)) == 0);
                             if (w_param == VK_LEFT) {
                                 if (KEY_WAS_DOWN) {
                                     global_input.left_was_down = true;
                                 }
                                 else {
                                     global_input.left_was_down = false;
                                 }

                                 if (KEY_IS_DOWN) {
                                     global_input.left_down = true;
                                 }
                                 else {
                                     global_input.left_down = false;
                                 }
                             }
                             if (w_param == VK_RIGHT) {
                                 if (KEY_WAS_DOWN) {
                                     global_input.right_was_down = true;
                                 }
                                 else {
                                     global_input.right_was_down = false;
                                 }

                                 if (KEY_IS_DOWN) {
                                     global_input.right_down = true;
                                 }
                                 else {
                                     global_input.right_down = false;
                                 }
                             }
                             if (w_param == VK_DOWN) {
                                 if (KEY_WAS_DOWN) {
                                     global_input.down_was_down = true;
                                 }
                                 else {
                                     global_input.down_was_down = false;
                                 }

                                 if (KEY_IS_DOWN) {
                                     global_input.down_down = true;
                                 }
                                 else {
                                     global_input.down_down = false;
                                 }
                             }
                             if (w_param == VK_UP) {
                                 if (KEY_WAS_DOWN) {
                                     global_input.up_was_down = true;
                                 }
                                 else {
                                     global_input.up_was_down = false;
                                 }

                                 if (KEY_IS_DOWN) {
                                     global_input.up_down = true;
                                 }
                                 else {
                                     global_input.up_down = false;
                                 }
                             }
                             if (w_param == VK_TAB) {
                                 if (KEY_WAS_DOWN) {
                                     global_input.tab_was_down = true;
                                 }
                                 else {
                                     global_input.tab_was_down = false;
                                 }

                                 if (KEY_IS_DOWN) {
                                     global_input.tab_down = true;
                                 }
                                 else {
                                     global_input.tab_down = false;
                                 }
                             }
                             if (w_param == VK_ESCAPE) {
                                 DestroyWindow(hwnd);
                             }
                         }break;
        case WM_CLOSE:{
                          DestroyWindow(hwnd);
                      }break;
        case WM_DESTROY:{
                            PostQuitMessage(0);
                            global_running = false;
                        }break;
        case WM_PAINT:{
                          PAINTSTRUCT ps = {};
                          HDC hdc = BeginPaint(hwnd, &ps);
                          EndPaint(hwnd, &ps);
                      }break;
        default:{
                    return DefWindowProc(hwnd, msg, w_param, l_param);
                }break;
    }

    return result;
}


void update_player(int *x, int *y, int width, float* player_rot, float rot_speed, float player_speed, Buffer *buffer, Input *input) {
    input->left_down = global_input.left_down;
    input->right_down = global_input.right_down;
    input->up_down = global_input.up_down;
    input->down_down = global_input.down_down;

    if (input->left_down ){
        //*x -= player_speed;
        *player_rot -= rot_speed;
    }
    if (input->right_down ){
        //*x += player_speed;
        *player_rot += rot_speed;
    }

    float rot = *player_rot;
    float magnitude = player_speed;
    if (input->up_down ){

        if (rot >=0 && rot <90) {
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y -= y_dist;
            *x += x_dist;
            //buffer->data[((y - y_dist) * buffer->width) + (x + x_dist) ] = color;
        }
        else if (rot >=270 && rot <360) {
            rot -= 270;
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y += x_dist;
            *x += y_dist;
            //   buffer->data[((y + x_dist) * buffer->width) + (x + y_dist) ] = color;
        }
        else if (rot >=90 && rot <180) {
            rot -=90;
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y -= x_dist;
            *x -= y_dist;
            //buffer->data[((y - x_dist) * buffer->width) + (x - y_dist) ] = color;
        }
        else if (rot >=180 && rot <270) {
            rot -= 180;
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y += y_dist;
            *x -= x_dist;
            //buffer->data[((y + y_dist) * buffer->width) + (x - x_dist) ] = color;
        }

    }
    if (input->down_down){
        if (rot >=0 && rot <90) {
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y += y_dist;
            *x -= x_dist;
        }
        else if (rot >=270 && rot <360) {
            rot -= 270;
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y -= x_dist;
            *x -= y_dist;
        }
        else if (rot >=90 && rot <180) {
            rot -=90;
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y += x_dist;
            *x += y_dist;
        }
        else if (rot >=180 && rot <270) {
            rot -= 180;
            float radian = (PI/180.0) * rot;
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            *y -= y_dist;
            *x += x_dist;
            //buffer->data[((y + y_dist) * buffer->width) + (x - x_dist) ] = color;
        }
    }

    if (*player_rot <0.0f)         *player_rot += 360.0f ;
    else if (*player_rot >360.0f)  *player_rot -= 360.0f;
}


int draw_ray(Buffer *buffer, int x, int y, int len, float rot, uint32_t color) {
    if (rot >360) rot -= 360;
    if (rot <0) rot += 360;
    int cell_width = buffer->width/MAP_COLS ;
    int cell_height = buffer->height/MAP_ROWS ;
    if (rot >=0 && rot <90) {
        float radian = (PI/180.0) * rot;
        int magnitude = 1;
        while (magnitude <len ) {
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            bool intersect = false;
            if ((x+x_dist) %cell_width == 0)  {
                int y_cell = (y - y_dist)/cell_width;
                int x_cell = (x + x_dist)/cell_width;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if ((y-y_dist) %cell_height == 0) {
                int y_cell = (y - y_dist)/cell_width;
                y_cell -=1;
                int x_cell = (x + x_dist)/cell_width;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            
            if (intersect) draw_circle(buffer, x + x_dist, y - y_dist, 4, 0x00AEAEAE);
            buffer->data[((y - y_dist) * buffer->width) + (x + x_dist) ] = color;
            ++magnitude;
        }
    }
    else if (rot >=270 && rot <360) {
        rot -= 270;
        float radian = (PI/180.0) * rot;

        int magnitude = 1;
        while (magnitude <len ) {
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);
            bool intersect = false;
            if ((x + y_dist) %cell_width == 0)  {
                int y_cell = (y + x_dist)/cell_width;
                int x_cell = (x + y_dist)/cell_width;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if ((y + x_dist) %cell_height == 0) {
                int y_cell = (y + x_dist)/cell_width;
                int x_cell = (x + y_dist)/cell_width;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if (intersect) draw_circle(buffer, x + y_dist, y + x_dist, 4, 0x00AEAEAE);

            buffer->data[((y + x_dist) * buffer->width) + (x + y_dist) ] = color;
            ++magnitude;
        }
    }
    else if (rot >=90 && rot <180) {
        rot -=90;
        float radian = (PI/180.0) * rot;
        int magnitude = 1;
        while (magnitude <len ) {
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);

            bool intersect = false;
            if ((x - y_dist) %cell_width == 0)  {
                int y_cell = (y - x_dist)/cell_width;
                int x_cell = (x - y_dist)/cell_width;
                --x_cell;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if ((y - x_dist) %cell_height == 0) {
                int y_cell = (y - x_dist)/cell_width;
                int x_cell = (x - y_dist)/cell_width;
                --y_cell;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if (intersect) draw_circle(buffer, x - y_dist, y - x_dist, 4, 0x00AEAEAE);

            buffer->data[((y - x_dist) * buffer->width) + (x - y_dist) ] = color;
            ++magnitude;
        }
    }
    else if (rot >=180 && rot <270) {
        rot -= 180;
        float radian = (PI/180.0) * rot;
        int magnitude = 1;
        while (magnitude <len ) {
            int y_dist = magnitude * sin(radian);
            int x_dist = magnitude * cos(radian);

            bool intersect = false;
            if ((x - x_dist) %cell_width == 0)  {
                int y_cell = (y + y_dist)/cell_width;
                int x_cell = (x - x_dist)/cell_width;
                --x_cell;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if ((y + y_dist) %cell_height == 0) {
                int y_cell = (y + y_dist)/cell_width;
                int x_cell = (x - x_dist)/cell_width;
                if (map[y_cell][x_cell] == 1) {
                    draw_rectangle(buffer,x_cell * cell_width, y_cell * cell_height, cell_width, cell_height, 0x00009F90);
                    return magnitude;
                }
                intersect = true;
            }
            if (intersect) draw_circle(buffer, x - x_dist, y + y_dist, 4, 0x00AEAEAE);

            buffer->data[((y + y_dist) * buffer->width) + (x - x_dist) ] = color;
            ++magnitude;
        }
    }
    return 0;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) 
{
    Buffer buffer = {};
    buffer.width  = 600;
    buffer.height = 600;
    buffer.data = (uint32_t *)VirtualAlloc(0, buffer.width * buffer.height * 4, MEM_COMMIT, PAGE_READWRITE);
    BITMAPINFOHEADER bm_info_header = {};
    bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
    bm_info_header.biWidth =  buffer.width;
    //top down
    bm_info_header.biHeight = -buffer.height;
    bm_info_header.biPlanes = 1;
    bm_info_header.biBitCount = 32;
    bm_info_header.biCompression = BI_RGB;
    buffer.bm_info.bmiHeader = bm_info_header;

    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASS wc = {}; 
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    HWND hwnd = CreateWindowEx( 
            0,
            CLASS_NAME,
            L"Window !",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,buffer.width + 40, buffer.height +40 ,
            0,
            0,
            hInstance,
            0);

    if (!hwnd)  return 0;

    HDC hdc = GetDC(hwnd);
    MSG msg = {};
    uint32_t color_change = 0;

    int player_x = 100;
    int player_y = 100;
    int player_width  = 10;
    int player_height = 10;
    int player_radius = 40;
    float player_rot = 0;
    uint32_t player_color = 0x00FFFF00;
    Input player_input = {};
    float player_speed = 4.01;
    float player_rot_speed = 4;
    int player_detect_len = buffer.height;
    float fov = 90;
    const int ray_count = 100;
    int player_rays[ray_count];
    float ray_rot = fov/ray_count;
    State state = topdown;

    int cell_width = buffer.width/MAP_COLS ;
    int cell_height = buffer.height/MAP_ROWS ;
    while (global_running) {
        while ( PeekMessage( &msg,hwnd, 0,0,PM_REMOVE ) ) {
            if (msg.message == WM_QUIT) global_running = false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RECT client_rect;
        GetClientRect(hwnd, &client_rect);
        int client_width = client_rect.right - client_rect.left;
        int client_height = client_rect.bottom - client_rect.top;

        update_player(&player_x, &player_y, player_width, &player_rot,player_rot_speed, player_speed, &buffer, &player_input);
        if (player_x <cell_width) player_x = cell_width;
        if (player_y <cell_height) player_y = cell_height;
        if ((player_x + player_width) > (buffer.width - cell_width)) {
                player_x = buffer.width - cell_width - player_width;
        }
        if ((player_y + player_height) > (buffer.height - cell_height)) {
                player_y = buffer.height - cell_height - player_height;
        }
        if (global_input.tab_down && !global_input.tab_was_down) {
            if (state == topdown) state = lines;
            else state = topdown;
        }
        clear_bitmap(&buffer, 0x000F0F0F);
        if (state == topdown) {
            draw_map(&buffer);
            draw_map_lines(&buffer);
            draw_rectangle(&buffer, player_x, player_y, player_width, player_height, player_color);
            for (int i = 0; i <ray_count; ++i) {
                player_rays[i] = draw_ray(&buffer, player_x + player_width/2, player_y + player_height/2, player_detect_len, player_rot - (ray_count/2 * ray_rot)  + (i * ray_rot), player_color);
            }
            draw_ray(&buffer, player_x + player_width/2, player_y + player_height/2, player_detect_len,  player_rot , 0x0000FF00);
        }
        else if (state == lines) {
            for (int i = 0; i <ray_count; ++i) {
                player_rays[i] = draw_ray(&buffer, player_x + player_width/2, player_y + player_height/2, player_detect_len, player_rot - (ray_count/2 * ray_rot)  + (i * ray_rot), player_color);
            }
            clear_bitmap(&buffer, 0x00300424);
            int wall_width = buffer.width / ray_count;
            int mid_y = buffer.height/2;
            for (int i = 0; i <ray_count; ++i) {

                uint32_t wall_color = 0x0020FFFF;
                int wall_height;
                float wall_scale = 9000.0f;
                if (player_rays[i] != 0) {
                    wall_height =  wall_scale/(player_rays[i]) ;
                }
                else if (i>0 ) {
                    wall_height =  wall_scale/(player_rays[i-1]) ;
                }
                else if (i <(ray_count - 1)) {
                    wall_height =  wall_scale/(player_rays[i+1]) ;
                }
                else {
                    wall_height = (buffer.height/2.0);
                }
                if (wall_height > (buffer.height/2.0)) wall_height = buffer.height/2;
                if (wall_height < 0) wall_height = buffer.height/2;
                wall_color &= wall_height;
                wall_color |= 0x002E0000;
                draw_rectangle(&buffer, wall_width * i, (buffer.height/2) - wall_height, wall_width, wall_height *2,wall_color) ;
                draw_rectangle(&buffer, wall_width * i, (buffer.height/2) + wall_height, wall_width,  buffer.height - ((buffer.height/2) + wall_height ) , 0x00200110);
                
                 
            }
            
        }
        render_bitmap(hdc, client_width, client_height, &buffer);
        Sleep(16.67);
    }

    return 0;
}

