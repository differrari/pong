#include "syscalls/syscalls.h"
#include "input_keycodes.h"
#include "math/math.h"

gpu_rect p1paddle, p2paddle, ball, border1, border2;
draw_ctx ctx = {};

typedef struct ivector2 {
    int32_t x,y;
} ivector2;

ivector2 ball_velocity = {};

#define SCALE 10

#define BG_COLOR 0
#define PADDLE_COLOR 0xFFFFFFFF
#define BORDER_COLOR 0xFFFFFFFF
#define BALL_COLOR 0xFFB4DD13

bool single_player = true;

void draw(gpu_rect rect, color color){
    fb_fill_rect(&ctx, rect.point.x, rect.point.y, rect.size.width, rect.size.height, color);
}

bool collide(gpu_rect a, gpu_rect b){
    uint32_t ax2 = a.point.x + a.size.width;
    uint32_t ay2 = a.point.y + a.size.height;
    uint32_t bx2 = b.point.x + b.size.width;
    uint32_t by2 = b.point.y + b.size.height;

    return (ax2 >= b.point.x && a.point.x <= bx2 && ay2 >= b.point.y && a.point.y <= by2);
}

void reset(){
    p1paddle = (gpu_rect){
        .point = { SCALE, (ctx.height/2) - SCALE},
        .size = { SCALE, SCALE * 3}
    };
    p2paddle = (gpu_rect){
        .point = { ctx.width - (single_player ? (SCALE + 1) : (2 * SCALE)), single_player ? SCALE : (ctx.height/2) - SCALE},
        .size = { SCALE, single_player ? ctx.height - SCALE*2 - 1 : SCALE * 3}
    };
    ball = (gpu_rect){
        .point = { (ctx.width/2) - SCALE, (ctx.height/2) - SCALE},
        .size = { SCALE, SCALE}
    };
    border1 = (gpu_rect) {
        .point = { 0, 0 },
        .size = { ctx.width - 1, SCALE}
    };
    border2 = (gpu_rect) {
        .point = { 0, ctx.height - SCALE - 1 },
        .size = { ctx.width - 1, SCALE}
    };
    ball_velocity = (ivector2){ SCALE, SCALE };
}

void update_ball(){
    gpu_rect next_ball = ball;
    next_ball.point.x += ball_velocity.x;
    next_ball.point.y += ball_velocity.y;

    if (collide(next_ball, border2)){
        ball_velocity = (ivector2){ball_velocity.x,-ball_velocity.y};
    } else if (collide(next_ball, p2paddle)){
        ball_velocity = (ivector2){-ball_velocity.x,ball_velocity.y};
    } else if (collide(next_ball, border1)){
        ball_velocity = (ivector2){ball_velocity.x,-ball_velocity.y};
    } else if (collide(next_ball, p1paddle)){
        ball_velocity = (ivector2){-ball_velocity.x,ball_velocity.y};
    } else if (!collide(next_ball, (gpu_rect){0,0,ctx.width, ctx.height})){
        reset();
    } else ball = next_ball;
}

int main(int argc, char* argv[]){
    request_draw_ctx(&ctx);
    //Setup function
    reset();
    //Game loop function
    while (true){
        //Input section
        keypress kp = {};
        if (read_key(&kp)){
            if (kp.keys[0] == KEY_UP) p1paddle.point.y = max(p1paddle.point.y - SCALE, SCALE);
            if (kp.keys[0] == KEY_DOWN) p1paddle.point.y = min(p1paddle.point.y + SCALE, ctx.height - (SCALE * 4) - 1);
        }
        //Render section
        // ctx.full_redraw = true;
        fb_clear(&ctx, BG_COLOR);
        draw(p1paddle, PADDLE_COLOR);
        draw(p2paddle, single_player ? BORDER_COLOR : PADDLE_COLOR);
        draw(border1, BORDER_COLOR);
        draw(border2, BORDER_COLOR);
        draw(ball, BALL_COLOR);
        update_ball();
        commit_draw_ctx(&ctx);
    }
    return 0;
}