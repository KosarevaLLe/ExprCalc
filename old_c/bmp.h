#ifndef BMP_H__INCLUDED
#define BMP_H__INCLUDED

#include <stdio.h>

typedef struct {
    char *m;
    int w, h;
} BitMap;

#define EMPTY_PT ' '

int bmp_init(BitMap *bmp, int w, int h);
void bmp_fin(BitMap *bmp);
char bmp_get(const BitMap *bmp, int x, int y);
char bmp_put(BitMap *bmp, int x, int y, char c);
void bmp_rect(BitMap *bmp, int x, int y, int w, int h, char c);
void bmp_clear(BitMap *bmp);
void bmp_print(FILE *f, const BitMap *bmp, int dir, int mirror);

double my_round(double x);

#endif
