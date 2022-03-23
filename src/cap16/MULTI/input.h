#ifndef _INPUT_H
#define _INPUT_H

/* OBS: este pequeno truque serve para evitar que um cabeçalho
        seja incluído mais de uma vez em caso de referências recursivas. */

extern char look;

void initInput();
void nextChar();

#endif