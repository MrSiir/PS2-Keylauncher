#ifndef __INCLUDE_CDSWAP_H__
#define __INCLUDE_CDSWAP_H__

unsigned char *systemcnf = (unsigned char *)0x800000;
unsigned char CD_DIRELF[256];
int CD_mode = 0;
int CD_invalid = 1;

void CD_loadmodules();
int loadmodulemem(char *buf,int len,int lenarg,char *arg);
int iop_heap_dma_upload(void *src,u32 dst,int size);
int CD_ReadCNF();
void CD_Reset(int mode);
void CD_disco(int type);

#endif //__INCLUDE_CDSWAP_H__

