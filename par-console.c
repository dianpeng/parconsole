#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

/* A simple console based particle to make people happy */
static const int WIDTH = 60;
static const int HEIGHT= 100;
static const int FPS = 24;
static const float SLEEP_TIME = 1.0f/(float)FPS;
static const int MAX_PARTICALE = 300;
static const int FACTOR = 1;

struct par {
    float pos_x , pos_y ; /* currrent position of the particles */
    char c; /* current characters */
    float gravity; /* gravity */
    float radius ; /* radius speed */
    float tangle ; /* tangtial speed */
    float vec_x,vec_y; /* velocity */
    int color; /* current color of characters */
    char c_min , c_max; /* character range */
    float lifespan; /* life */
};

#define BLUE 0
#define BLACK 1
#define RED 2
#define GREEN 3
#define BROWN 4
#define MAGENTA 5
#define CYAN 6
#define GRAY 7
#define WHITE 8


struct minmaxF {
    float min;
    float max;
};

struct minmaxI {
    int min;
    int max;
};

struct par_sys {
    struct par particles[ MAX_PARTICALE ];
    int alive;
    int par_sz;
    float life;
    float c_life;

    struct minmaxF pos_x, pos_y;
    struct minmaxF init_c;
    struct minmaxI c_min, c_max;
    struct minmaxF gravity;
    struct minmaxF radius;
    struct minmaxF tangle;
    struct minmaxF lifespan;
    struct minmaxF vec_x,vec_y;
    struct minmaxI col;
};


void init_par_sys( struct par_sys* parsys ) {
    memset( parsys->particles , 0 , sizeof(parsys->particles) );
    parsys->alive = 0;
    parsys->life = 4.0f;
    parsys->c_life = 0;

    parsys->pos_x.min = WIDTH/2-5; parsys->pos_x.max = WIDTH/2+5;
    parsys->pos_y.min = HEIGHT/2-5; parsys->pos_y.max = HEIGHT/2+5;
    parsys->init_c.min = 'A'; parsys->init_c.max= 'Z';
    parsys->c_min.min = 'A' ; parsys->c_min.min = 'U';
    parsys->c_max.min = 'V' ; parsys->c_max.max = 'Z';
    parsys->gravity.min = -10 ; parsys->gravity.max= 10;
    parsys->radius.min = -15 ; parsys->radius.max = 15;
    parsys->tangle.min = -15 ; parsys->tangle.max = 15;
    parsys->lifespan.min = 0; parsys->lifespan.max = 40;
    parsys->col.min = BLUE ; parsys->col.max = WHITE;
    parsys->vec_x.min = -5.5f ; parsys->vec_x.max = 5.5f;
    parsys->vec_y.min = -4.5f ; parsys->vec_y.max = 4.5f;

}

int is_out( int posx, int posy ) {
    if( posx > WIDTH || posx < 0 ||
        posy > HEIGHT|| posy < 0 )
        return true;
    else
        return false;
}

int rndI( int min , int max ) {
    float val = random();
    int diff = max-min;
    float r = val / RAND_MAX;
    return min + (int)(diff*r);
}

float rndF( float min, float max ) {
    float val = random();
    float diff = max-min;
    float r = val / RAND_MAX;
    return min + (diff*r);
}

int update_par_sys( struct par_sys* parsys ) {
    parsys->c_life += SLEEP_TIME;
    if( parsys->c_life > parsys->life ) 
        return 1;

    if( parsys->alive > 0 ) {
        for( int i = 0 ; i < MAX_PARTICALE ; ++i ) {
            struct par* p = parsys->particles + i;
            if( p->lifespan <= 0 ) {
                continue;
            }

            p->lifespan += SLEEP_TIME;
            if( p->lifespan > parsys->life ) {
                p->lifespan = 0;
                parsys->alive--;
            }

            p->c = rndI( p->c_min , p->c_max );

            /* calculate the velocity here */
            float r = sqrt( p->vec_x*p->vec_x + p->vec_y * p->vec_y );
            float n_vec_x = p->vec_x + p->vec_x / r * p->radius;
            float n_vec_y = p->vec_y + p->vec_y / r * p->radius;

            float deg_x = asin( p->vec_x / r );
            float deg_y = asin( p->vec_y / r );

            n_vec_x += p->tangle * cos(deg_x);
            n_vec_y += p->tangle * sin(deg_y);

            float a_vec_x = (n_vec_x + p->vec_x)/2.0f;
            float a_vec_y = (n_vec_y + p->vec_y)/2.0f;

            p->pos_x += a_vec_x * SLEEP_TIME * FACTOR ;
            p->pos_y += a_vec_y * SLEEP_TIME * FACTOR;

            if( is_out(p->pos_x,p->pos_y) ) {
                p->lifespan = 0;
                parsys->alive--;
                continue;
            }

            p->vec_x = n_vec_x;
            p->vec_y = n_vec_y;

            p->color = rndI( parsys->col.min , parsys->col.max );
        }
    } 

    if( parsys->alive < MAX_PARTICALE ) {
        for( int i = 0; i < MAX_PARTICALE ; ++i ) {
            struct par* p = parsys->particles + i;
            if( p->lifespan <= 0 ) {
                p->c = rndI( parsys->init_c.min , parsys->init_c.max );
                p->c_min = rndI( parsys->c_min.min , parsys->c_min.max );
                p->c_max = rndI( parsys->c_max.min , parsys->c_max.max );
                p->pos_x = rndF (parsys->pos_x.min , parsys->pos_x.max );
                p->pos_y = rndF (parsys->pos_y.min , parsys->pos_y.max );
                p->gravity = rndF(parsys->gravity.min, parsys->gravity.max );
                p->radius = rndF(parsys->radius.min,parsys->radius.max);
                p->tangle = rndF(parsys->tangle.min,parsys->tangle.max);
                p->color = rndI( parsys->col.min , parsys->col.max );
                p->vec_x = rndF( parsys->vec_x.min,parsys->vec_x.max );
                p->vec_y = rndF( parsys->vec_y.min,parsys->vec_y.max );
                p->lifespan = SLEEP_TIME;
                parsys->alive++;
                if( parsys->alive == MAX_PARTICALE )
                    break;
            }
        }
    }
    return 0;
}

void render_par_sys( struct par_sys* parsys ) {
    printf("\033[2J\033[1;1H");
    int cnt = 0;
    for( int i = 0 ; i < MAX_PARTICALE ; ++i ) {
        struct par* p = parsys->particles + i;
        if( p->lifespan <= 0 ) 
            continue;
        else {
            const char* color;
            const char* reset = "\x1b[0m";
            switch( p->color ) {
                case BLUE:
                    color = "\x1b[34m";
                    break; 
                case RED:
                    color = "\x1b[31m";
                    break;
                case BLACK:
                    color = "\x1b[0m";
                    break;
                case BROWN:
                    color = "\x1b[33m";
                    break;
                case GREEN:
                    color = "\x1b[32m";
                    break;
                case CYAN:
                    color = "\x1b[36m";
                    break;
                case MAGENTA:
                    color = "\x1b[35m";
                    break;
                default:
                    color = "\x1b[0m";
                    break;
            }
            printf("%s",color); /* change the color of current console */
            printf("\033[%d;%dH%c",(int)(p->pos_x),(int)(p->pos_y),p->c);
            printf("%s",reset);
            ++cnt;
            if( cnt >= parsys->alive )
                break;
        }
    }
}

int main() {
    struct par_sys parsys;
    srand(0);
    init_par_sys(&parsys);

    while( true ) {
        if( update_par_sys(&parsys) ) {
            break;
        }

        render_par_sys(&parsys);
        usleep( 1000000/FPS );
    }
    printf("\033[2J\033[1;1H");
    return 0;
}

