#ifndef TABS_H
#define TABS_H

extern u64 N1s; //0000..00000000000000011111
extern u64 R1s; //0000..00100001000100010001
extern int count[1<<N];
extern u32 hashNumbers[2][N][N]; //pro kazdou barvu a hranu
extern u32 hashNumbers2[2][N][1<<N]; //pro kazdou barvu, vrchol a sousedy

void tabsInit();

#endif //TABS_H

