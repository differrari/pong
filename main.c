#include "syscalls/syscalls.h"
#include "input_keycodes.h"
#include "math/math.h"

typedef struct ivector2 {
    float x,y;
} ivector2;

typedef struct primitive_rect {
    ivector2 point;
    ivector2 size;
} primitive_rect;

primitive_rect p1paddle, p2paddle, ball, border1, border2;
primitive_rect safe_area;
draw_ctx ctx = {};

ivector2 ball_velocity = {};

#define SCALE 10

#define BG_COLOR 0
#define PADDLE_COLOR 0xFFFFFFFF
#define BORDER_COLOR 0xFFFFFFFF
#define BALL_COLOR 0xFFB4DD13

#define PADDLE_SPEED 20

bool single_player = true;

void draw(primitive_rect rect, color color){
    fb_fill_rect(&ctx, rect.point.x, rect.point.y, rect.size.x, rect.size.y, color);
}

void redraw(primitive_rect old_rect, primitive_rect rect, color color){
    fb_fill_rect(&ctx, old_rect.point.x, old_rect.point.y, old_rect.size.x, old_rect.size.y, BG_COLOR);
    fb_fill_rect(&ctx, rect.point.x, rect.point.y, rect.size.x, rect.size.y, color);
}

bool collide(primitive_rect a, primitive_rect b){
    uint32_t ax2 = a.point.x + a.size.x;
    uint32_t ay2 = a.point.y + a.size.y;
    uint32_t bx2 = b.point.x + b.size.x;
    uint32_t by2 = b.point.y + b.size.y;

    return (ax2 >= b.point.x && a.point.x <= bx2 && ay2 >= b.point.y && a.point.y <= by2);
}

bool compare_rects(primitive_rect a, primitive_rect b){
    return a.point.x == b.point.x && a.point.y == b.point.y && a.size.x == b.size.x && a.size.y == b.size.y;
}

void reset(){
    p1paddle = (primitive_rect){
        .point = { SCALE, (ctx.height/2) - SCALE},
        .size = { SCALE, SCALE * 3}
    };
    p2paddle = (primitive_rect){
        .point = { ctx.width - (single_player ? (SCALE + 1) : (2 * SCALE)), single_player ? SCALE : (ctx.height/2) - SCALE},
        .size = { SCALE, single_player ? ctx.height - SCALE * 2 - 1 : SCALE * 3}
    };
    ball = (primitive_rect){
        .point = { (ctx.width/2) - SCALE, (ctx.height/2) - SCALE},
        .size = { SCALE, SCALE}
    };
    border1 = (primitive_rect) {
        .point = { 0, 0 },
        .size = { ctx.width - 1, SCALE}
    };
    border2 = (primitive_rect) {
        .point = { 0, ctx.height - SCALE - 1 },
        .size = { ctx.width - 1, SCALE}
    };
    ball_velocity = (ivector2){ SCALE*10, SCALE*10 };
    fb_clear(&ctx, BG_COLOR);

    draw(p1paddle, PADDLE_COLOR);
    draw(p2paddle, single_player ? BORDER_COLOR : PADDLE_COLOR);
    draw(border1, BORDER_COLOR);
    draw(border2, BORDER_COLOR);
    draw(ball, BALL_COLOR);
}

void update_ball(float delta_time){
    primitive_rect next_ball = ball;
    next_ball.point.x += ball_velocity.x * delta_time;
    next_ball.point.y += ball_velocity.y  * delta_time;

    if (collide(next_ball, safe_area)) {
        redraw(ball, next_ball, BALL_COLOR);
        ball = next_ball;
    } else if (collide(next_ball, border2)){
        ball_velocity = (ivector2){ball_velocity.x,-ball_velocity.y};
    } else if (collide(next_ball, p2paddle)){
        ball_velocity = (ivector2){-ball_velocity.x,ball_velocity.y};
    } else if (collide(next_ball, border1)){
        ball_velocity = (ivector2){ball_velocity.x,-ball_velocity.y};
    } else if (collide(next_ball, p1paddle)){
        ball_velocity = (ivector2){-ball_velocity.x,ball_velocity.y};
    } else if (!collide(next_ball, (primitive_rect){0,0,ctx.width, ctx.height})){
        reset();
    } else {
        redraw(ball, next_ball, BALL_COLOR);
        ball = next_ball;
    }
}

void setup(){
    safe_area = (primitive_rect){
        .point = { SCALE * 3, SCALE * 3},
        .size = { ctx.width - (SCALE * 6), ctx.height - (SCALE * 6)}
    };
    reset();
}

void update(float delta){
    keypress kp = {};
    for (int i = 0; i < 5 && read_key(&kp); i++){
        primitive_rect old_rect = p1paddle;
        if (kp.keys[0] == KEY_UP) p1paddle.point.y = max(p1paddle.point.y - (SCALE * PADDLE_SPEED * delta), SCALE);
        if (kp.keys[0] == KEY_DOWN) p1paddle.point.y = min(p1paddle.point.y + (SCALE * PADDLE_SPEED * delta), ctx.height - (SCALE * 4) - 1);
        if (kp.keys[0] == KEY_ESC) {
            halt(0);
            return;
        }
        if (!compare_rects(old_rect, p1paddle)){
            redraw(old_rect, p1paddle, PADDLE_COLOR);
        }
    }
    update_ball(delta);
}

int run_game(uint64_t target_fps){
    request_draw_ctx(&ctx);
    setup();
    uint64_t time = get_time();
    uint64_t delta_time;
    uint64_t target_dt = target_fps == 0 ? 0 : (1/30.f)*1000;
    while (true){
        float dt = delta_time/1000.f;
        ctx.full_redraw = true;
        update(dt);

        commit_draw_ctx(&ctx);
        uint64_t new_time = get_time();
        delta_time = new_time - time;
        time = new_time;
        if (delta_time < target_dt){
            sleep(target_dt - delta_time);
            delta_time = target_dt;
        }
    }
}

int main(int argc, char* argv[]){
    return run_game(30);
}