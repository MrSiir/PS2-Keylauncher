#include <tamtypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <kernel.h>
#include <loadfile.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <libpad.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include "iopmod.h"

unsigned char *systemcnf = (unsigned char *)0x800000;
unsigned char CD_DIRELF[256];
int CD_mode = 0;
int CD_invalid = 1;

void CD_memcpy2(unsigned char *dest,unsigned char *org,int ndata)
{
int n;
for(n=0;n<ndata;n++) dest[n]=org[n];
}

int CD_ReadCNF()
{
	int fd,n,m;
    fd=fioOpen("cdrom0:\\SYSTEM.CNF;1",1);
	if(fd>=0)
		{
		m=fioLseek(fd,0,2);
		fioLseek(fd,0,0);
		if(m>=4095) m=4094;
		fioRead(fd,systemcnf,m);
		systemcnf[m+1]=0;
		m=0;
        while(systemcnf[m]!=0)
			{
			if(systemcnf[m]=='B' && systemcnf[m+1]=='O' && systemcnf[m+2]=='O' && systemcnf[m+3]=='T' && systemcnf[m+4]=='2')
				{m+=5;break;}
			m++;
			}
		while(systemcnf[m]!=0 && systemcnf[m]==32) m++;
		if(systemcnf[m]!=0 )m++; // salta '='
		while(systemcnf[m]!=0 && systemcnf[m]==32) m++;
		if(systemcnf[m]==0) return 0;
		for(n=0;n<255;n++)
			{
			CD_DIRELF[n]=systemcnf[m];
			if(systemcnf[m]==0) break;
			if(n>2) if(CD_DIRELF[n-1]==';' && CD_DIRELF[n]=='1')  {CD_DIRELF[n+1]=0;break;}
            m++;
			}
		CD_DIRELF[255]=0;
		fioClose(fd);
		return 1;
		}
    return 0;
}

//////////////////////////////////////////////////////////
// loadmodulemem
//////////////////////////////////////////////////////////

// a old routine for send data to IOP...
u8 send_buffer2[16] __attribute__((aligned(16)));
u8 memsend[512] __attribute__((aligned(16)));
int iop_heap_dma_upload(void *src,u32 dst,int size)
{
	int i=0,len=0,size2;
  u8  *pkt;
	int ret=0;
	int ret2=0;
	int p=0;

	int cont1=0;
while(size>0) // send data with src unaligned
{
if(size>512) size2=512; else size2=size;
CD_memcpy2(memsend,((unsigned char *) src)+p,size2);
do

{
	FlushCache(0);

  /* build packet */
  pkt = send_buffer2; 
  PUSHDATA( u32, pkt, (u32)memsend, i);
  pkt += i; len += i;
  PUSHDATA( u32, pkt, dst, i);
  pkt += i; len += i;
  PUSHDATA( int, pkt, size2, i);
  pkt += i; len += i;
  PUSHDATA( int, pkt, 0, i);
  pkt += i; len += i;

	ret = SifSetDma((SifDmaTransfer_t*)send_buffer2,1);

	if(ret==0)  {nopdelay();cont1++;}
	if(ret==0 && cont1>=3) {cont1=0;SifSetDChain();}
}while(ret==0); // modificado por Hermes

	while((ret2 = SifDmaStat(ret))>=0);


	FlushCache(0);
size-=size2;
p+=size2;
dst+=size2;
}
	return (ret2 < -1);
}

int loadmodulemem(char *buf,int len,int lenarg,char *arg)
{
	unsigned iopmem=0;
	int ret=0;
	SifInitIopHeap();

	iopmem=(unsigned int)SifAllocIopHeap(len+15);
	printf("iopmem=%d\n", iopmem);
	
	if(iopmem==0) {
		return -1;
	}
	
	if(iop_heap_dma_upload(buf, iopmem,len+15)<0) {
		SifFreeIopHeap((void*)iopmem);
		return -1;
	}
	
	if(SifLoadModuleBuffer((void *)iopmem, lenarg, arg) < 0) {
		SifFreeIopHeap((void*)iopmem);
		return -1;
	}
	
	return ret;
}

void CD_loadmodules()
{
	loadmodulemem(iopmod,size_iopmod,0,0); // control module cd/dvd via qwerty device ...
}

/////////////////////////////////////////////////////////////////////
// reboot IOP
/////////////////////////////////////////////////////////////////////
void CD_Reset(int mode)
{

if(mode==1) SifIopReset("rom0:UDNL rom0:EELOADCNF",0); // normal
else
	{fioOpen("cdrom0:\\SYSTEM.CNF;1",1); // hacking in the night... `_Ž. Prepare 
	SifIopReset("rom0:UDNL cdrom0:\\SYSTEM.CNF;1", 0); // three point!!
	}
   while (SifIopSync()) ;
   
fioExit();
SifExitIopHeap();
SifLoadFileExit();
SifExitRpc();
SifExitCmd();
EI();

SifInitRpc(0);

FlushCache(0);
FlushCache(2);
	

}

void CD_disco(int type)
{
	
	putScreen(0, 0, "");
	putScreen(0, 0, "");
	
	unsigned discColor = 0;
	char discName[256];
		
	if(type==4)
	{
		sprintf(discName, "%s", "USE SWAP");
		discColor = GS_SET_RGBA(0x5A,0xD2,0x43,0x80);
	}
	if(type==2)
	{
		sprintf(discName, "%s", "PS2 CD-ROM");
		discColor = GS_SET_RGBA(0x12,0x00,0xFF,0x80);
	}
	if(type==3)
	{
		sprintf(discName, "%s", "PS2 DVD-ROM");
		discColor = GS_SET_RGBA(0xE3,0xCD,0x3B,0x80);
	}
	if(type==1)
	{
		sprintf(discName, "%s", "PSX CD-ROM");
		discColor = GS_SET_RGBA(0x00,0x00,0x00,0x80);
	}
	
	putBackground(img_fondo);
	myGsDriver->drawPipe.RectFlat(0,0,tvmx,60,0,colorNegro);
	myGsDriver->drawPipe.RectGouraud(0,420,colorNegro,tvmx,tvmy,colorAzul,0);
	myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
	myFont.Print(420, tvmx, 15, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_1);
	myFont.Print(420, tvmx, 30, 1, colorNaranja, GSFONT_ALIGN_LEFT, "http://ps2dev.siirlabs.com");
	myGsDriver->drawPipe.RectFlat(55,125,591,367,0,colorBlanco);
	myGsDriver->drawPipe.Line(0,59,tvmx,59,0,colorGris2);
	myGsDriver->drawPipe.Line(0,420,tvmx,420,0,colorGris2);
	
	myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, "CogSwap Loader");
	
	if(type > 0)
	{
		fillCircle(175, 245, 100, discColor);
		fillCircle(175, 245, 20, GS_SET_RGBA(0xFF,0xFF,0xFF,0x80));
		myFont.Print(304, 640, 196, 1, GS_SET_RGBA(0x0E,0x23,0xA1,0x80), GSFONT_ALIGN_LEFT, "Formato del disco:");
		myFont.Print(304, 640, 216, 1, GS_SET_RGBA(0x00,0x00,0x00,0x80), GSFONT_ALIGN_LEFT, (char *)discName);
	}
	else
	{
		myFont.Print(230, 640, 216, 1, GS_SET_RGBA(0x00,0x00,0x00,0x80), GSFONT_ALIGN_LEFT, "No Hay Disco");
	}
	
	putImage(img_logo, 21, 18);
	myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
	myFont.Print(15, tvmx-15, 435, 1, colorGris2, GSFONT_ALIGN_LEFT, txt_40);
	
	myGsDriver->drawPipe.Flush();
	myGsDriver->WaitForVSync();
	myGsDriver->swapBuffers();
}
