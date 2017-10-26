#include "bmp.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

int bmp_init(BitMap *bmp, int w, int h)
{
    bmp->w = w;
    bmp->h = h;
    bmp->m = (char *)malloc(w * h);
    if (bmp->m)
        bmp_clear(bmp);
    return bmp->m != NULL;
}

void bmp_fin(BitMap *bmp)
{
    if (bmp->m) {
        free(bmp->m);
        bmp->m = NULL;
    }
}

char bmp_get(const BitMap *bmp, int x, int y)
{
    if (x < 0 || x >= bmp->w)
        return EMPTY_PT;
    if (y < 0 || y >= bmp->h)
        return EMPTY_PT;
    return bmp->m[y * bmp->w + x];
}

char bmp_put(BitMap *bmp, int x, int y, char c)
{
    int i;
    char t;
    if (x < 0 || x >= bmp->w)
        return EMPTY_PT;
    if (y < 0 || y >= bmp->h)
        return EMPTY_PT;
    i = y * bmp->w + x;
    t = bmp->m[i];
    bmp->m[i] = c;
    return t;
}

void bmp_rect(BitMap *bmp, int x, int y, int w, int h, char c)
{
    int i, j;
    for (i = y; i < y + h; ++i) {
        bmp_put(bmp, x, i, c);
        bmp_put(bmp, x + w - 1, i, c);
    }
    for (j = x; j < x + w; ++j) {
        bmp_put(bmp, j, y, c);
        bmp_put(bmp, j, y + h - 1, c);
    }
}

void bmp_clear(BitMap *bmp)
{
    memset(bmp->m, EMPTY_PT, bmp->w * bmp->h);
}

void bmp_print(FILE *f, const BitMap *bmp, int dir, int mirror)
{
    int i, j;
    for (i = 0; i < bmp->h; ++i) {
        for (j = 0; j < bmp->w; ++j) {
            int c;
            c = (unsigned char)bmp_get(bmp, j, i);
            fputc(c, f);
        }
        fputc('\n', f);
    }
}

double my_round(double x)
{
    return floor(x + 0.5);
}

