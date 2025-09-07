#include "syscalls/syscalls.h"

gpu_point p1paddle, p2paddle, ball;
draw_ctx ctx = {};

#define SCALE 10

#define BG_COLOR 0
#define PADDLE_COLOR 0xFFFFFFFF
#define BALL_COLOR 0xFFFFFFFF

void draw_paddle(gpu_point point){
    fb_fill_rect(&ctx, point.x * SCALE, point.y * SCALE, SCALE, SCALE*3, PADDLE_COLOR);
}

void draw_ball(){
    fb_fill_rect(&ctx, ball.x * SCALE, ball.y * SCALE, SCALE, SCALE, BALL_COLOR);
}

int main(int argc, char* argv[]){
    request_draw_ctx(&ctx);
    //Setup function
    gpu_size size = { ctx.width/SCALE, ctx.height/SCALE };
    //TODO: bounds checks
    p1paddle.x = 1;
    p1paddle.y = (size.height/2) - 1;
    p2paddle.x = size.width - 2;
    p2paddle.y = (size.height/2) - 1;
    ball = (gpu_point){(size.width/2) - 1, (size.height/2) - 1};
    //Game loop function
    while (true){
        fb_clear(&ctx, BG_COLOR);
        draw_paddle(p1paddle);
        draw_paddle(p2paddle);
        draw_ball();
        commit_draw_ctx(&ctx);
    }
    return 0;
}