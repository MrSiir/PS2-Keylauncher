/*
  _____     ___ ____
   ____|   |    ____|      MrSiir / SiirLabs
  |     ___|   |____       (C)2005, MrSiir (mrsiir@gmail.com)
  ------------------------------------------------------------------------
  KL.cpp
                        PS2 Key Launcher 0.5.4
  ------------------------------------------------------------------------
    
  * Para compilar este programa se necesita la ps2sdk y gslib 0.5.1
  
  ------------------------------------------------------------------------
*/
/*
	TODO LIST:
	
	v0.5.4
	- [NO] Reproducir MP3 desde el Navegador
	- Pantalla de Ayuda en el Navegador! pulsando L1
	
	v0.5.5
	- [¿?] Añadir pantalla de información
	- [¿?] Arreglar bug lanzamiento desde CD-ROM
	- Añadir copiar/eliminar/mover de manera recursiva
	- Borrar directorios
	- Añadir la creación de particiones en el HD
	
	v0.5.6
	- ¿Reproductor de MP3?
 */


#include <tamtypes.h>
#include <stdarg.h>
#include <kernel.h>
#include <sifrpc.h>
#include <malloc.h>
#include <fileio.h>
#include <iopcontrol.h>
#include <loadfile.h>
#include <libpad.h>
#include <libmc.h>
#include <iopheap.h>
#include <sifcmd.h>
#include <osd_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "iopmod.h"

#include <gsDefs.h>
#include <gsDriver.h>
#include <gsPipe.h>
#include <gsFont.h>

#include "KL.h"
#include "langs.h"
#include "mass_rpc.h"

#include <sbv_patches.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fileXio_rpc.h>
#include <errno.h>
#include <libhdd.h>

#include "cdvd_rpc.h"
#include "CDSwap.cpp"

// MP3
#if defined(MP3_SOUND)
	#include <audiolib.h>
	#include <audiolibmp3.h>
	int volumen		= 0x7fff;
	int fdmp3		= -1;
	int foundMP3	= 0;
#endif

int fileMode =  FIO_S_IRUSR | FIO_S_IWUSR | FIO_S_IXUSR | FIO_S_IRGRP | FIO_S_IWGRP | FIO_S_IXGRP | FIO_S_IROTH | FIO_S_IWOTH | FIO_S_IXOTH;

void printDir(
	char myFiles[MAX_ENTRY][MAX_PATH],
	int myTypeFiles[MAX_ENTRY],
	int nCountmyFiles,
	char gmyPath[256],
	int mx1, int my1,
	int mx2,
	int mbx1, int mbx2,
	int nmyActualEntry,
	int nPanel)
{
	int j = 0;
	int n = 0;
	int nCountEntryInPage = 0;
	unsigned text_color;
	int nStartEntry = 0;
	
	if(nmyActualEntry > (nCountmyFiles - 1))
	{
		nmyActualEntry = (nCountmyFiles - 1);
	}
	
	if(nCountmyFiles > MAX_ENTRY_PAGE2)
	{
		if((nmyActualEntry >= MAX_ENTRY_PAGE2) && (nmyActualEntry < nCountmyFiles))
		{
			nStartEntry = (nmyActualEntry - MAX_ENTRY_PAGE2) + 1;
		}
		else
		{
			nStartEntry = 0;
		}
	}
	
	for(n=nStartEntry; n<nCountmyFiles; n++)
	{
		for(j=30;j<MAX_PATH;j++) myFiles[n][j] = 0;
		
		if(myTypeFiles[n]==0) {
			if(strstr(myFiles[n], ".ELF") || strstr(myFiles[n], ".elf")) {
				putImage(img_fileelf, mx2, my1);
			} else if(strstr(myFiles[n], ".NPO") || strstr(myFiles[n], ".npo")) {
				putImage(img_npo, mx2, my1);
			} else {
				putImage(img_fileold, mx2, my1);
			}
		} else if(myTypeFiles[n]==1 || myTypeFiles[n]==4) {
			putImage(img_folder, mx2, my1);
		} else if(myTypeFiles[n]==2 || myTypeFiles[n]==3) {
			putImage(img_folderup, mx2, my1);
		}
		
		text_color = text_color1;
		if((nmyActualEntry == n) && (nPanel == selectPanelCopy)) {
			text_color = text_color2;
			myGsDriver->drawPipe.RectFlat(mbx1,my1,mbx2,my1+17,1,color_back);
		}
		if(myTypeFiles[n]==2 || myTypeFiles[n]==3) {
			myFont.Print(mx1, 640, my1, 1, text_color, GSFONT_ALIGN_LEFT, "..");
		} else {
			if(myTypeFiles[n]==4) text_color = colorRojo2;
			myFont.Print(mx1, 640, my1, 1, text_color, GSFONT_ALIGN_LEFT, (char *)myFiles[n]);
		}
		nCountEntryInPage++;
		if(nCountEntryInPage >= MAX_ENTRY_PAGE2) break;
		my1+=20;
	}
}

void readCDDir(char *dir)
{
	int root = 0;
	nActualEntry = 0;
	nCountFiles = 0;
	if(strlen(dir)==1) root = 1;
	struct TocEntry* TocEntryList = (TocEntry*)memalign(64,4000*sizeof(struct TocEntry));
	
	if(root == 0)
	{
		sprintf(Files[nCountFiles], "%s", "");
		TypeFiles[nCountFiles] = 2;
		nCountFiles++;
	}
	else
	{
		sprintf(Files[nCountFiles], "%s", "");
		TypeFiles[nCountFiles] = 3;
		nCountFiles++;
	}
	
	CDVD_Init();
	int num_files = CDVD_GetDir(dir, NULL, CDVD_GET_FILES_AND_DIRS, TocEntryList, 4000, dir);
	CDVD_Stop();
	CDVD_FlushCache();
	
	for(int z=0; z<num_files; z++)
	{
		if((TocEntryList[z].fileProperties & 0x02) && (TocEntryList[z].filename[0] != '.'))
		{
			sprintf(Files[nCountFiles], "%s", TocEntryList[z].filename);
			TypeFiles[nCountFiles] = 1;
			nCountFiles++;
		}
		else if(TocEntryList[z].filename[0] != '.')
		{
			sprintf(Files[nCountFiles], "%s", TocEntryList[z].filename);
			TypeFiles[nCountFiles] = 0;
			nCountFiles++;
		}
	}						
	free(TocEntryList);
}

void loadModulesHDD()
{
	int ret;
	static char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
	static char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40" /*"\0" "-debug"*/;
	
	if(HDD_IRX_LOAD == 0)
	{
		SifExecModuleBuffer(&iomanx, size_iomanx, 0, NULL, &ret);
		SifExecModuleBuffer(&filexio, size_filexio, 0, NULL, &ret);
		SifExecModuleBuffer(&ps2dev9, size_ps2dev9, 0, NULL, &ret);
		SifExecModuleBuffer(&ps2atad, size_ps2atad, 0, NULL, &ret);
		SifExecModuleBuffer(&ps2hdd, size_ps2hdd, sizeof(hddarg), hddarg, &ret);
		SifExecModuleBuffer(&ps2fs, size_ps2fs, sizeof(pfsarg), pfsarg, &ret);	
		HDD_IRX_LOAD = 1;
	}
}

int getRootHDDDir()
{
	nActualEntry = 0;
	nCountFiles = 0;
	ROOT_HDD = 1;
	
	int i, nparty;
	t_hddFilesystem parties[MAX_PARTITIONS] __attribute__((aligned(64)));
	
	if(READ_HDD == 0)
	{	
		if(hddCheckPresent() < 0)
		{
			return -1;
		}
		if(hddCheckFormatted() < 0)
		{
			return -2;
		}
		
		TypeFiles[nCountFiles] = 3;
		nCountFiles++;
		
		i = hddGetFilesystemList(parties, MAX_PARTITIONS);
		nparty = i-1;
		
		for(i=nparty;i>=0;i--)
		{
			if(parties[i].formatted == 1)
			{
				if(parties[i].name[0] != '.')
				{
					if(parties[i].fileSystemGroup == 0x00)	sprintf(Files[nCountFiles], "%s%s", "__", parties[i].name);
					if(parties[i].fileSystemGroup == 0x01)	sprintf(Files[nCountFiles], "%s%s", "+", parties[i].name);
					if(parties[i].fileSystemGroup == 0x02)	sprintf(Files[nCountFiles], "%s", parties[i].name);				
					TypeFiles[nCountFiles] = 1;
					nCountFiles++;
				}
			}
			else
			{
				char fakeName[255];
				sprintf(fakeName, "%s", parties[i].name);
				if(strstr(fakeName, "PP.HDL."))
				{
					int j = strlen(fakeName);
					int z = 0;
					int k = 0;
					for(z=7; z<j; z++) {
						fakeName[k] = fakeName[z];
						k++;
					}
					for(z=k; z<j; z++) fakeName[z] = 0;
				}
				sprintf(Files[nCountFiles], "%s", fakeName);
				TypeFiles[nCountFiles] = 4;
				nCountFiles++;
			}
		}
		READ_HDD = 1;		
		copyCharArray2(HDD_Files, Files);
		copyCharArrayN((char *)HDD_TypeFiles, (char *)TypeFiles);
		HDD_nCountFiles = nCountFiles;
	}
	else
	{
		copyCharArray2(Files, HDD_Files);
		copyCharArrayN((char *)TypeFiles, (char *)HDD_TypeFiles);
		nCountFiles = HDD_nCountFiles;
	}
	return 1;
}

int getHDDDir(char *pfsfolder)
{
	ROOT_HDD = 0;	
	nActualEntry = 0;
	nCountFiles = 0;
	char *mountpoint = "pfs0:";
	char folder[256];
	
	if(strlen(pfsfolder)==0 || pfsfolder[0]==0 || pfsfolder=="")
	{
		//return getRootHDDDir();
	}
	
	sprintf(folder, "%s%s", mountpoint, pfsfolder);

	sprintf(Files[nCountFiles], "%s", "");
	TypeFiles[nCountFiles] = 2;
	nCountFiles++;

	int fd, rv;
	iox_dirent_t dirbuf;
	
	if(fileXioMount(mountpoint, (char *)hddpart, 0x01) == 0)
	{
		if((fd = fileXioDopen(folder)) < 0)
		{
			return -1;
		}
		while((rv = fileXioDread(fd, &dirbuf)) > 0)
		{
			if(FIO_S_ISDIR(dirbuf.stat.mode) && (dirbuf.name[0] != '.'))
			{
				sprintf(Files[nCountFiles], "%s", dirbuf.name);
				TypeFiles[nCountFiles] = 1;
				nCountFiles++;
			}
			else if(FIO_S_ISREG(dirbuf.stat.mode) && (dirbuf.name[0] != '.'))
			{
				sprintf(Files[nCountFiles], "%s", dirbuf.name);
				TypeFiles[nCountFiles] = 0;
				nCountFiles++;
			}
		}
		fileXioDclose(fd);
		fileXioUmount(mountpoint);
	}
	else
	{
		return -2;
	}
	return 0;
}


/***************************************************************
 * recursive (MASS)
 * Lee todos los ficheros del USB
 ***************************************************************/
void getMassDir(char *folder)
{
	int root = 0;
	nActualEntry = 0;
	nCountFiles = 0;
	if(strlen(folder)==1) root = 1;
	
	int ret;
	fat_dir_record record;
	
	if(root == 0)
	{
		sprintf(Files[nCountFiles], "%s", "");
		TypeFiles[nCountFiles] = 2;
		nCountFiles++;
	}
	else
	{
		sprintf(Files[nCountFiles], "%s", "");
		TypeFiles[nCountFiles] = 3;
		nCountFiles++;
	}
	
	ret = usb_mass_getFirstDirentry(folder, &record);
	while(ret > 0 )
	{
		if(((record.attr & 0x10) == 0x10) && (record.name[0] != '.'))
		{
			sprintf(Files[nCountFiles], "%s", record.name);
			TypeFiles[nCountFiles] = 1;
			nCountFiles++;
		}
		else if((record.attr & 0x08) == 0x08)
		{
			//Volumen
		}
		else if(record.name[0] != '.')
		{
			sprintf(Files[nCountFiles], "%s", record.name);
			TypeFiles[nCountFiles] = 0;
			nCountFiles++;
		} 
		ret = usb_mass_getNextDirentry(&record);
	}
}

void getFioMCDir(char *folder)
{
	int root = 0;
	nActualEntry = 0;
	nCountFiles = 0;
	if(strlen(folder)==5) root = 1;
	
	int rv, fd=0;
	fio_dirent_t hostbuf;
	
	if(root == 0)
	{
		sprintf(Files[nCountFiles], "%s", "");
		TypeFiles[nCountFiles] = 2;
		nCountFiles++;
	}
	else
	{
		sprintf(Files[nCountFiles], "%s", "");
		TypeFiles[nCountFiles] = 3;
		nCountFiles++;
	}

	fd = fioDopen(folder);
	if(fd > 0)
	{
		while((rv=fioDread(fd, &hostbuf)))
		{			
			if(hostbuf.stat.mode & 0x0010 && strncmp((char *)&hostbuf.name,".",1))
			{
				sprintf(Files[nCountFiles], "%s", hostbuf.name);
				TypeFiles[nCountFiles] = 0;
				nCountFiles++;
			}
			else if(hostbuf.stat.mode & 0x0020 && strncmp((char *)&hostbuf.name,".",1))
			{
				sprintf(Files[nCountFiles], "%s", hostbuf.name);
				TypeFiles[nCountFiles] = 1;
				nCountFiles++;
			}
		}
		fioDclose(fd);
	}
}


/***************************************************************
 * saveMyCnf
 * Graba la configuracion en la MC
 ***************************************************************/
int saveMyCnf()
{
	int fd=0;
	int n=0, m=0;
	char fileSave[1024];
	
	for(n=0; n<1023; n++) fileSave[n] = 0;

	fileSave[m] = 'A'; m++;
	fileSave[m] = 'U'; m++;
	fileSave[m] = 'T'; m++;
	fileSave[m] = 'O'; m++;
	fileSave[m] = '='; m++;
	if(TIME_AUTO[0] != 0)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = TIME_AUTO[n]; m++;
			if(TIME_AUTO[n+1]==0 || TIME_AUTO[n+1]==13 || TIME_AUTO[n+1]==10) break;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = '['; m++;
	fileSave[m] = ']'; m++;
	fileSave[m] = '='; m++;
	if(DIRELF_1[0]!=0 && DIRELF_1[0]!=10 && DIRELF_1[0]!=13 && DIRELF_1[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_1[n]; m++;
			if(DIRELF_1[n+1]==0 || DIRELF_1[n+1]==13 || DIRELF_1[n+1]==10) break;
		}
		fileSave[m] = ';'; 	m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_1_TXT[n]; m++;
			if(DIRELF_1_TXT[n+1]==0 || DIRELF_1_TXT[n+1]==13 || DIRELF_1_TXT[n+1]==10) break;
		}
		if(bautoloadelf==1) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = '/';  m++;
	fileSave[m] = '\\'; m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_2[0]!=0 && DIRELF_2[0]!=10 && DIRELF_2[0]!=13 && DIRELF_2[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_2[n]; m++;
			if(DIRELF_2[n+1]==0 || DIRELF_2[n+1]==13 || DIRELF_2[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_2_TXT[n]; m++;
			if(DIRELF_2_TXT[n+1]==0 || DIRELF_2_TXT[n+1]==13 || DIRELF_2_TXT[n+1]==10) break;
		}
		if(bautoloadelf==2) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'O';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_3[0]!=0 && DIRELF_3[0]!=10 && DIRELF_3[0]!=13 && DIRELF_3[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_3[n]; m++;
			if(DIRELF_3[n+1]==0 || DIRELF_3[n+1]==13 || DIRELF_3[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_3_TXT[n]; m++;
			if(DIRELF_3_TXT[n+1]==0 || DIRELF_3_TXT[n+1]==13 || DIRELF_3_TXT[n+1]==10) break;
		}
		if(bautoloadelf==3) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'X';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_4[0]!=0 && DIRELF_4[0]!=10 && DIRELF_4[0]!=13 && DIRELF_4[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_4[n]; m++;
			if(DIRELF_4[n+1]==0 || DIRELF_4[n+1]==13 || DIRELF_4[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_4_TXT[n]; m++;
			if(DIRELF_4_TXT[n+1]==0 || DIRELF_4_TXT[n+1]==13 || DIRELF_4_TXT[n+1]==10) break;
		}
		if(bautoloadelf==4) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'R';  m++;
	fileSave[m] = '1';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_R1[0]!=0 && DIRELF_R1[0]!=10 && DIRELF_R1[0]!=13 && DIRELF_R1[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_R1[n]; m++;
			if(DIRELF_R1[n+1]==0 || DIRELF_R1[n+1]==13 || DIRELF_R1[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_R1_TXT[n]; m++;
			if(DIRELF_R1_TXT[n+1]==0 || DIRELF_R1_TXT[n+1]==13 || DIRELF_R1_TXT[n+1]==10) break;
		}
		if(bautoloadelf==5) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'R';  m++;
	fileSave[m] = '2';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_R2[0]!=0 && DIRELF_R2[0]!=10 && DIRELF_R2[0]!=13 && DIRELF_R2[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_R2[n]; m++;
			if(DIRELF_R2[n+1]==0 || DIRELF_R2[n+1]==13 || DIRELF_R2[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_R2_TXT[n]; m++;
			if(DIRELF_R2_TXT[n+1]==0 || DIRELF_R2_TXT[n+1]==13 || DIRELF_R2_TXT[n+1]==10) break;
		}
		if(bautoloadelf==6) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'R';  m++;
	fileSave[m] = '3';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_R3[0]!=0 && DIRELF_R3[0]!=10 && DIRELF_R3[0]!=13 && DIRELF_R3[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_R3[n]; m++;
			if(DIRELF_R3[n+1]==0 || DIRELF_R3[n+1]==13 || DIRELF_R3[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_R3_TXT[n]; m++;
			if(DIRELF_R3_TXT[n+1]==0 || DIRELF_R3_TXT[n+1]==13 || DIRELF_R3_TXT[n+1]==10) break;
		}
		if(bautoloadelf==7) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'L';  m++;
	fileSave[m] = '1';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_L1[0]!=0 && DIRELF_L1[0]!=10 && DIRELF_L1[0]!=13 && DIRELF_L1[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_L1[n]; m++;
			if(DIRELF_L1[n+1]==0 || DIRELF_L1[n+1]==13 || DIRELF_L1[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_L1_TXT[n]; m++;
			if(DIRELF_L1_TXT[n+1]==0 || DIRELF_L1_TXT[n+1]==13 || DIRELF_L1_TXT[n+1]==10) break;
		}
		if(bautoloadelf==8) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'L';  m++;
	fileSave[m] = '2';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_L2[0]!=0 && DIRELF_L2[0]!=10 && DIRELF_L2[0]!=13 && DIRELF_L2[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_L2[n]; m++;
			if(DIRELF_L2[n+1]==0 || DIRELF_L2[n+1]==13 || DIRELF_L2[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_L2_TXT[n]; m++;
			if(DIRELF_L2_TXT[n+1]==0 || DIRELF_L2_TXT[n+1]==13 || DIRELF_L2_TXT[n+1]==10) break;
		}
		if(bautoloadelf==9) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 'L';  m++;
	fileSave[m] = '3';  m++;
	fileSave[m] = '=';  m++;
	if(DIRELF_L3[0]!=0 && DIRELF_L3[0]!=10 && DIRELF_L3[0]!=13 && DIRELF_L3[0]!=59)
	{
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_L3[n]; m++;
			if(DIRELF_L3[n+1]==0 || DIRELF_L3[n+1]==13 || DIRELF_L3[n+1]==10) break;
		}
		fileSave[m] = ';'; m++;
		for(n=0;n<255;n++) {
			fileSave[m] = DIRELF_L3_TXT[n]; m++;
			if(DIRELF_L3_TXT[n+1]==0 || DIRELF_L3_TXT[n+1]==13 || DIRELF_L3_TXT[n+1]==10) break;
		}
		if(bautoloadelf==10) {
			fileSave[m] = '@'; m++;
		}
	}
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;
	fileSave[m] = 13; m++;
	fileSave[m] = 10; m++;

	fd = fioOpen(kl_dat, O_WRONLY | O_CREAT);
	if(fd > 0) {
		fioWrite(fd, fileSave, m);
		fioClose(fd);
		return 0;
	} else {
		return -2;
	}
	return -1;
}


/***************************************************************
 * setELFToButton
 * Asignamos un ELF a un boton
 ***************************************************************/
void setELFToButton(unsigned char *fileelf, int button)
{
	int n = 0;
	for(n=0;n<255;n++) fileelf[n] = 0;
	strcpy((char *)fileelf, selectectedELFName);
	for(n=0;n<255;n++) selectectedELFName[n] = 0;
	inEditButton = button;
	mainStatus = STATUS_SNAME;
	saStat = 1;
}

/***************************************************************
 * setTextToButton
 * Asignamos el texto a un boton
 ***************************************************************/
void setTextToButton(unsigned char *nameelf)
{
	int n = 0;
	for(n=0;n<255;n++) nameelf[n] = 0;
	for(n=0;n<255;n++) {
		nameelf[n] = nameEFLKB[n];
		if(nameEFLKB[n]==0) break;
	}
	
	if(saveMyCnf() < 0)
	{
		while(1)
		{
			if(readPad())
			{
				putScreen(133, 200, txt_9);
				if(new_pad & PAD_CROSS)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	
	for(n=0;n<255;n++) nameEFLKB[n] = 0;
	nPosKB = 0;
	selectCharKB = 0;
	saStat = 1;
	mainStatus = STATUS_MAIN;
}

/***************************************************************
 * setTimeForAuto
 * Asignamos el tiempo para la carga
 ***************************************************************/
void setTimeForAuto()
{
	int n = 0;
	
	TIME_AUTO[0] = nameNumKB[0];
	TIME_AUTO[1] = nameNumKB[1];	
	
	if(saveMyCnf() < 0)
	{
		while(1)
		{
			if(readPad())
			{
				putScreen(133, 200, txt_9);
				if(new_pad & PAD_CROSS)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	
	for(n=0;n<255;n++) nameNumKB[n] = 0;
	nPosNumKB = 0;
	selectNumKB = 0;
	saStat = 1;
	mainStatus = STATUS_MAIN;
}


/***************************************************************
 * setELFForAuto
 * Asignamos el ELF para la carga
 ***************************************************************/
void setELFForAuto(int what)
{
	bautoloadelf = what;
	
	if(saveMyCnf() < 0)
	{
		while(1)
		{
			if(readPad())
			{
				putScreen(133, 200, txt_9);
				if(new_pad & PAD_CROSS)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	
	mainStatus = STATUS_MAIN;
}

/***************************************************************
 * saveIcon
 * Ponemos el icono en la MC
 ***************************************************************/
int saveIcon(void)
{
	int mc_fd;
	int icon_fd;
	mcIcon icon_sys;

	static iconIVECTOR bgcolor[4] = {
		{  68,  23, 116,  0 }, // top left
		{ 255, 255, 255,  0 }, // top right
		{ 255, 255, 255,  0 }, // bottom left
		{  68,  23, 116,  0 }, // bottom right
	};

	static iconFVECTOR lightdir[3] = {
		{ 0.5, 0.5, 0.5, 0.0 },
		{ 0.0,-0.4,-0.1, 0.0 },
		{-0.5,-0.5, 0.5, 0.0 },
	};

	static iconFVECTOR lightcol[3] = {
		{ 0.3, 0.3, 0.3, 0.00 },
		{ 0.4, 0.4, 0.4, 0.00 },
		{ 0.5, 0.5, 0.5, 0.00 },
	};

	static iconFVECTOR ambient = { 0.50, 0.50, 0.50, 0.00 };
		
	memset(&icon_sys, 0, sizeof(mcIcon));
	strcpy((char *)icon_sys.head, "PS2D");
	strcpy_sjis((short *)&icon_sys.title, "PS2 KeyLauncher\nMrSiir");
	icon_sys.nlOffset = 16;
	icon_sys.trans = 0x60;
	memcpy(icon_sys.bgCol, bgcolor, sizeof(bgcolor));
	memcpy(icon_sys.lightDir, lightdir, sizeof(lightdir));
	memcpy(icon_sys.lightCol, lightcol, sizeof(lightcol));
	memcpy(icon_sys.lightAmbient, ambient, sizeof(ambient));
	strcpy((char *)icon_sys.view, "kl.icn");
	strcpy((char *)icon_sys.copy, "kl.icn");
	strcpy((char *)icon_sys.del, "kl.icn");
	mc_fd = fioOpen(kl_icon_sys, O_WRONLY | O_CREAT);
	if(mc_fd < 0) return -2;
	fioWrite(mc_fd, &icon_sys, sizeof(icon_sys));
	fioClose(mc_fd);
	icon_fd = fioOpen(kl_icon_icn, O_WRONLY | O_CREAT);
	if(icon_fd < 0) return -6;
	fioWrite(icon_fd, binary_klicn_start, 33688);
	fioClose(icon_fd);
	return 0;
}

/***************************************************************
 * readPad
 * Funcion para leer el PAD, de BOOT.c de Hermes
 ***************************************************************/
int readPad(void)
{
	int ret;
	
	if(padGetState(0, 0) == PAD_STATE_STABLE) {
		port=0;
	} else if(padGetState(1, 0) == PAD_STATE_STABLE) {
		port=1;
	} else {
		return 0;
	}
	
	ret = padRead(port, 0, &buttons); 
	if(ret != 0)
	{
		paddata = 0xffff ^ buttons.btns;
		new_pad = paddata & ~old_pad;
		old_pad = paddata;
		return 1;
	}
	return 0;
}

/***************************************************************
 * MEMORIA
 * memcpy2 y memset2 de BOOT.c de Hermes
 ***************************************************************/
void memcpy2(unsigned char *dest,unsigned char *org,int ndata)
{
	int n;
	for(n=0;n<ndata;n++)
	{
		dest[n]=org[n];
	}
}

void memset2(unsigned char *dest,unsigned char val,int ndata)
{
	int n;
	for(n=0;n<ndata;n++)
	{
		dest[n]=val;
	}
}

/***************************************************************
 * CARGA DE MODULOS
 * loadModules
 * Modulos necesarios para el programa
 ***************************************************************/
void delay(int count)
{
	int i;
	int ret;
	for (i  = 0; i < count; i++)
	{
		ret = 0x01000000;
		while(ret--) asm("nop\nnop\nnop\nnop");
	}
} 
void loadModules(void)
{
	int ret = -1;
	
	ret = SifLoadModule("rom0:SIO2MAN", 0, 0);
	ret = SifLoadModule("rom0:PADMAN", 0, 0);
	ret = SifLoadModule("rom0:MCMAN", 0, 0);
	ret = SifLoadModule("rom0:MCSERV", 0, 0);
	ret = SifLoadModule("rom0:LIBSD", 0, 0);
	
	ret = SifExecModuleBuffer(&CDVD, 	  size_CDVD,	0, NULL, NULL);
	
	#ifdef USB_USE
		ret = SifExecModuleBuffer(&NPMUSBD, size_NPMUSBD, 0, NULL, NULL);
		ret = SifExecModuleBuffer(&USBMASS, size_USBMASS, 0, NULL, NULL);
		delay(1);
		ret = usb_mass_bindRpc();
		if(ret < 0 )
		{
			HAVE_USB = -1;
		}
		else
		{
			HAVE_USB = 1;
		}
	#endif
	
	//MP3
	#if defined(MP3_SOUND)
		ret = SifExecModuleBuffer(&audio, size_audio, 0, NULL, NULL);
	#endif
}

/***************************************************************
 * EJECUTAR .ELF
 * LoadElf (original source by mrbrown in payload.c)
 ***************************************************************/
void Reset()
{
	SifIopReset("rom0:UDNL rom0:EELOADCNF",0);
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

int RunElf(char *name)
{
	int fd,size,i;
	u8 *boot_elf = (u8 *) 0;//&_end;
	elf_header_t *eh = &elfh;
	elf_pheader_t *eph;
	char *argv[1];
	void *pdata;
	fd=-1;
	if(name[0]=='m' && name[1]=='c') // if mc, test mc0 and mc1
	{
		if((fd = fioOpen(name, O_RDONLY)) < 0) 
		{
			name[2]='1';
		}
	}
	if(fd < 0)
		if((fd = fioOpen(name, O_RDONLY)) < 0) 
		{
			return -1;
		}
		size = fioLseek(fd, 0, SEEK_END);
		if(!size) {
			fioClose(fd);
			return -2;
		}

		fioLseek(fd, 0, 0);
		fioRead(fd, eh, sizeof(elf_header_t)); // read the elf header

		// crazy code for crazy man :P
		boot_elf=(u8 *)0x1800000-size-256; 
		//if((eh->entry+size)>=boot_elf) boot_elf=(u8 *)eh->entry-size-256;
		boot_elf=(u8 *) (((unsigned)boot_elf) &0xfffffff0);

		// read rest file elf
		fioRead(fd, boot_elf+sizeof(elf_header_t), size-sizeof(elf_header_t));
		fioClose(fd);

		// mrbrown machine gun ;)
		eph = (elf_pheader_t *)(boot_elf + eh->phoff);
		// Scan through the ELF's program headers and copy them into RAM, then
		// zero out any non-loaded regions.
		for (i = 0; i < eh->phnum; i++) {
	   	   if (eph[i].type != ELF_PT_LOAD)
			   continue;
			   
		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy2((unsigned char *)eph[i].vaddr, (unsigned char *)pdata, (int)eph[i].filesz);

		if (eph[i].memsz > eph[i].filesz)
			memset2((unsigned char *)eph[i].vaddr + eph[i].filesz, (unsigned char)0, (int)eph[i].memsz - eph[i].filesz);
	}

	// Let's go.
	argv[0] = name;

	fioExit();
	Reset();
	FlushCache(0);
	FlushCache(2);
	ExecPS2((void *)eh->entry, 0, 1, argv);
	return 0;
}

/***************************************************************
 * EJECUTAR .ELF del HDD
 * RunHDDElf (version retocada del original de mrbrown (payload.c)
 ***************************************************************/
int RunHDDElf(char *hddpart, char *filename)
{
	int fd,size,i,ret;
	u8 *boot_elf = (u8 *) 0;//&_end;
	elf_header_t *eh = &elfh;
	elf_pheader_t *eph;
	char *argv[1];
	void *pdata;
	fd=-1;
	
	ret = fileXioMount("pfs0:", hddpart, 0x01);
	if((fd = fileXioOpen(filename, O_RDONLY, fileMode)) < 0)
	{
		return -1;
	}
	size = fileXioLseek(fd, 0, SEEK_END);
	if (!size)
	{
		fileXioClose(fd);
		return -2;
	}
	
	fileXioLseek(fd, 0, 0);
	fileXioRead(fd, (unsigned char *)eh, sizeof(elf_header_t));
	
	// crazy code for crazy man :P
	boot_elf=(u8 *)0x1800000-size-256; 
	//if((eh->entry+size)>=boot_elf) boot_elf=(u8 *)eh->entry-size-256;
	boot_elf=(u8 *) (((unsigned)boot_elf) &0xfffffff0);

	// read rest file elf
	fileXioRead(fd, boot_elf+sizeof(elf_header_t), size-sizeof(elf_header_t));
	fileXioClose(fd);
	fileXioUmount("pfs0:");

	// mrbrown machine gun ;)
	eph = (elf_pheader_t *)(boot_elf + eh->phoff);
	// Scan through the ELF's program headers and copy them into RAM, then
	// zero out any non-loaded regions.
	for (i = 0; i < eh->phnum; i++) {
   	   if (eph[i].type != ELF_PT_LOAD)
		   continue;
			   
		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy2((unsigned char *)eph[i].vaddr, (unsigned char *)pdata, (int)eph[i].filesz);

		if (eph[i].memsz > eph[i].filesz)
			memset2((unsigned char *)eph[i].vaddr + eph[i].filesz, (unsigned char)0, (int)eph[i].memsz - eph[i].filesz);
	}

	// Let's go.
	argv[0] = filename;

	fioExit();
	Reset();
	FlushCache(0);
	FlushCache(2);
	ExecPS2((void *)eh->entry, 0, 1, argv);
	return 0;
}

/***************************************************************
 * CIRCULOS
 * Pintamos un circulo usando lineas de la gsLib
 ***************************************************************/
void fillCircle(int x, int y, int r, unsigned color)
{
	int x1;
	int x2;
	int counter=(y+r);
	
	for(int count=(y-r);count<=counter;count++)
	{
		x1=(int)(x+sqrt((r*r)-((count-y)*(count-y)))+1);
		x2=int(x-sqrt((r*r)-((count-y)*(count-y)))+1);
		myGsDriver->drawPipe.Line(x1,count,x2,count,0,color);
	}
}

/***************************************************************
 * CONFIGURACION
 * Leemos el fichero de configuración y lo parseamos para
 * asignar los ELF a cada boton
 ***************************************************************/
int readMyCNF()
{
	int fd,n,m;
	int c1, c2;
	int isName = 0;
		
	if(HAVE_USB==1) {
		fd = fioOpen(kl_usb_dat, O_RDONLY);
	} else {
		fd = -1;
	}
	
	if(fd < 0)
	{
		fd = fioOpen(kl_dat, O_RDONLY);
		if(fd < 0)
		{
			return -1;
		}
	}
	else
	{
		USB_MODE = 1;
	}
	
	if(fd >= 0)
	{
		m=fioLseek(fd, 0, 2);
		fioLseek(fd, 0, 0);
		if(m>=4095) m=4094;
		fioRead(fd, charbuffer, m);
		fioClose(fd);
		charbuffer[m+1] = 0;
		m=0;
		
		while(charbuffer[m]!=0)
		{
			isName = 0;
			c1=0;
			c2=0;
			
			if(charbuffer[m]=='[' && charbuffer[m+1]==']' && charbuffer[m+2]=='=') {
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_1_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_1[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 1;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='/' && charbuffer[m+1]=='\\' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_2_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_2[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 2;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='O' && charbuffer[m+1]=='=')
			{
				m+=2;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_3_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_3[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 3;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='X' && charbuffer[m+1]=='=')
			{
				m+=2;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_4_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_4[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 4;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='R' && charbuffer[m+1]=='1' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_R1_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_R1[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 5;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='R' && charbuffer[m+1]=='2' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_R2_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_R2[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 6;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='R' && charbuffer[m+1]=='3' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_R3_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_R3[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 7;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='L' && charbuffer[m+1]=='1' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_L1_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_L1[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 8;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='L' && charbuffer[m+1]=='2' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_L2_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_L2[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 9;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='L' && charbuffer[m+1]=='3' && charbuffer[m+2]=='=')
			{
				m+=3;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<255;n++) {
					if(charbuffer[m]==';') {
						isName = 1;
					}
					if(isName==2) {
						DIRELF_L3_TXT[c1] = charbuffer[m];
						c1++;
					} else if(isName==0) {
						DIRELF_L3[c2] = charbuffer[m];
						c2++;
					}
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					if(charbuffer[m+1]==64) {
						autoloadelf = 10;
						break;
					}
					if(isName==1) isName = 2;
					m++;
				}
			}
			if(charbuffer[m]=='A' && charbuffer[m+1]=='U' && charbuffer[m+2]=='T' && charbuffer[m+3]=='O' && charbuffer[m+4]=='=')
			{
				m+=5;
				if(charbuffer[m]==0) return 0;
				for(n=0;n<2;n++) {
					TIME_AUTO[n] = charbuffer[m];
					if(charbuffer[m]==0 || charbuffer[m]==13) break;
					m++;
				}
			}
			m++;
		}
	
		if(DIRELF_1_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_1_TXT[n] = DIRELF_1[n];
				if(DIRELF_1[n]==0) break;
			}
		}
		if(DIRELF_2_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_2_TXT[n] = DIRELF_2[n];
				if(DIRELF_2[n]==0) break;
			}
		}
		if(DIRELF_3_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_3_TXT[n] = DIRELF_3[n];
				if(DIRELF_3[n]==0) break;
			}
		}
		if(DIRELF_4_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_4_TXT[n] = DIRELF_4[n];
				if(DIRELF_4[n]==0) break;
			}
		}
		
		if(DIRELF_R1_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_R1_TXT[n] = DIRELF_R1[n];
				if(DIRELF_R1[n]==0) break;
			}
		}
		if(DIRELF_R2_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_R2_TXT[n] = DIRELF_R2[n];
				if(DIRELF_R2[n]==0) break;
			}
		}
		if(DIRELF_R3_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_R3_TXT[n] = DIRELF_R3[n];
				if(DIRELF_R3[n]==0) break;
			}
		}
		
		if(DIRELF_L1_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_L1_TXT[n] = DIRELF_L1[n];
				if(DIRELF_L1[n]==0) break;
			}
		}
		if(DIRELF_L2_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_L2_TXT[n] = DIRELF_L2[n];
				if(DIRELF_L2[n]==0) break;
			}
		}
		if(DIRELF_L3_TXT[0] == 0) {
			for(n=0;n<255;n++) {
				DIRELF_L3_TXT[n] = DIRELF_L3[n];
				if(DIRELF_L3[n]==0) break;
			}
		}
		return 1;
	}
	else
	{
		return -2;
	}
	return 0;
}

/***************************************************************
 * PANTALLITA
 * Pantalla negra con texto en blanco
 ***************************************************************/
void putScreen(int x, int y, const char *txt)
{
	myGsDriver->drawPipe.RectFlat(0,0,tvmx,tvmy,0,colorNegro);
	myFont.Print(x, tvmx, y, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt);
	myGsDriver->drawPipe.Flush();
	myGsDriver->WaitForVSync();
	myGsDriver->swapBuffers();
}

void putMessageScreen(int x, int y, const char *txt)
{
	myGsDriver->drawPipe.RectFlat(209, 159, 427, 266, 0, GS_SET_RGBA(0x00,0x00,0x00,0x80));
	myGsDriver->drawPipe.RectFlat(211, 161, 425, 264, 0, GS_SET_RGBA(0x8B,0x8B,0x8B,0x80));
	myFont.Print(x, tvmx, y, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt);
	myGsDriver->drawPipe.Flush();
	myGsDriver->WaitForVSync();
	myGsDriver->swapBuffers();
}

void putScreen2(int x, int y, const char *txt)
{
	myGsDriver->drawPipe.RectFlat(0,0,tvmx,tvmy,0,colorBlanco);
	myFont.Print(x, tvmx, y, 1, colorNegro, GSFONT_ALIGN_LEFT, txt);
	myGsDriver->drawPipe.Flush();
	myGsDriver->WaitForVSync();
	myGsDriver->swapBuffers();
}

/***************************************************************
 * PRECARGA DEL ELF
 * Ejecutamos el ELF con un pantallita de carga
 ***************************************************************/
int runMyElf(unsigned char elf[256])
{ 
	int retelf = 0;
	autoloadelf = 0;
	
	#if defined(MP3_SOUND)
		stopMyMP3();
		SoundOff();
		PCMSilence();
	#endif
	
	if(elf[0] == 0 || elf[0] == 13)
	{		
		while(1)
		{
			if(readPad())
			{
				putScreen(150, 200, txt_6);
				if(new_pad & PAD_CROSS)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		putScreen(280, 200, txt_7);
		
		if(elf[0]=='m' &&
		   elf[1]=='a' &&
		   elf[2]=='s' &&
		   elf[3]=='s' &&
		   elf[4]==':' &&
		   elf[5]=='/')
		{
			// Mass
			putScreen(200, 200, txt_39);
		}
		else if(elf[0]=='h' &&
			   elf[1]=='d' &&
			   elf[2]=='d' &&
			   elf[3]=='0' &&
			   elf[4]==':')
		{
			// HDD
			int i=0, j=0, top=256;
			char hddpart[top];
			char pfsfolder[top];
			char fileelf[top];
			
			for(i=0; i<top; i++)
			{
				hddpart[i] = elf[i];
				if(elf[i]==0) break;
			}
			for(i=0; i<top; i++)
			{
				if(hddpart[i]=='/' || hddpart[i] == 0) break;
			}
			j=0;
			for(i=i; i<top; i++)
			{
				pfsfolder[j] = hddpart[i];
				if(hddpart[i]==0) break;
				hddpart[i] = 0;
				j++;
			}
			
			sprintf(fileelf, "%s%s", "pfs0:", pfsfolder);
			putScreen(233, 200, txt_29);
			loadModulesHDD();
			putScreen(220, 200, txt_30);
			padPortClose(0,0);
			padPortClose(1,0);
			int load = RunLoaderElf((char *)hddpart, (char *)fileelf);
			padPortOpen(0, 0, padBuf);
			padPortOpen(1, 0, padBuf2);
			if(load < 0)
			{
				while(1)
				{
					if(readPad())
					{
						putScreen(133, 200, txt_8);
						if(new_pad & PAD_CROSS)
						{
							return -1;
							break;
						}
					}
				}
			}
			return 0;
		
		}
		else if(elf[0]=='m' &&
			   elf[1]=='c' &&
			   (elf[2]=='0' || elf[2]=='1') &&
			   elf[3]==':' &&
			   elf[4]=='/')
		{
			// MC
			putScreen(200, 200, txt_31);
		}
		
		padPortClose(0,0);
		padPortClose(1,0);
		
		retelf = RunLoaderElf((char *)elf, (char *)elf);
		
		padPortOpen(0, 0, padBuf);
		padPortOpen(1, 0, padBuf2);
		if(retelf < 0)
		{
			while(1)
			{
				if(readPad())
				{
					putScreen(133, 200, txt_8);
					if(new_pad & PAD_CROSS)
					{
						return -1;
						break;
					}
				}
			}
		}
	}
	return 0;
}


/***************************************************************
 * TEXTOS SIN GSLIB
 * m_putchar y m_putcad de BOOT.c de Hermes
 ***************************************************************/
extern unsigned char msx[];

void m_putchar(int x, int y, unsigned color, unsigned char ch)
{
	int i,j;
	unsigned char *font;
	
	font = &msx[ (int)ch * 8];
	for (i=0; i < 8; i++, font++)
	{
		for (j=0; j < 8; j++)
		{
			if ((*font & (128 >> j)))
			{
				myGsDriver->drawPipe.Point(x+j,y+i,0,color);
			}
		}
	}
}

void m_putcad(int x, int y, unsigned color, char *cad)
{
	while(cad[0]!=0)
	{
		if(cad[0]==13) break;
		m_putchar(x, y, color, cad[0]);
		cad++;
		x+=8;
	}
}

void m_putcharG(int x, int y, unsigned color, unsigned char ch)
{
	int i,j;
	unsigned char *font;
	
	font = &msx[ (int)ch * 8];
	for (i=0; i < 8; i++, font++)
	{
		for (j=0; j < 8; j++)
		{
			if ((*font & (128 >> j)))
			{
				int x1 = x+(j<<1);
				int x2 = x1+2;
				int y1 = y+(i<<2);
				int y2 = y1+4;
				myGsDriver->drawPipe.RectFlat(x1,y1,x2,y2,1,color);
			}
		}
	}
}

void m_putcadG(int x, int y, unsigned color, char *cad)
{
	while(cad[0]!=0)
	{
		if(cad[0]==13) break;
		m_putcharG(x, y, color, cad[0]);
		cad++;
		x+=16;
	}
}


/***************************************************************
 * TIMER POR VA
 * 
 ***************************************************************/
int count = 500;

void va_handler()
{
	if(autoloadelf > 0)
	{
		count--;
		if(count < 0) count = 0;
	}
}

unsigned getColorAuto(int btn, unsigned char fileelf[256])
{
	if(btn == autoloadelf)
	{
		return colorAzul;
	}
	else
	{
		if(fileelf[0]=='m' &&
		   fileelf[1]=='a' &&
		   fileelf[2]=='s' &&
		   fileelf[3]=='s' &&
		   fileelf[4]==':' &&
		   fileelf[5]=='/')
		{
			return colorVerde;
		}
		else
		{
			if(fileelf[0]=='h' &&
			   fileelf[1]=='d' &&
			   fileelf[2]=='d' &&
			   fileelf[3]=='0' &&
			   fileelf[4]==':')
			{
				return colorRojo;
			}
		}
	}
	return colorNegro;
}

void setTimeAuto(void)
{
	if(TIME_AUTO[0] != 0 && TIME_AUTO[0] != 13)
	{
		if(isdigit((int)TIME_AUTO[0]))
		{
			count = (int)atoi((const char *)TIME_AUTO);
			if(count >= 0)
			{
				count = count*100;
			}
			else
			{
				count = 0;
				autoloadelf = 0;
			}
		}
		else
		{
			count = 0;
			autoloadelf = 0;
		}
	}
}


/***************************************************************
 * IMAGENES
 * 
 ***************************************************************/
unsigned int getMinTextureSizeID(int size)
{
	int ret = 		GS_TEX_SIZE_2;
	if(size>2)		ret=GS_TEX_SIZE_4;
	if(size>4)		ret=GS_TEX_SIZE_8;
	if(size>8)		ret=GS_TEX_SIZE_16;
	if(size>16)		ret=GS_TEX_SIZE_32;
	if(size>32)		ret=GS_TEX_SIZE_64;
	if(size>64)		ret=GS_TEX_SIZE_128;
	if(size>128)	ret=GS_TEX_SIZE_256;
	if(size>256)	ret=GS_TEX_SIZE_512;
	if(size>512)	ret=GS_TEX_SIZE_1024;
	return ret;
}

unsigned int getMinTextureSize(int size)
{
	int 			ret=2;
	if(size>2)		ret=4;
	if(size>4)		ret=8;
	if(size>8)		ret=16;
	if(size>16)		ret=32;
	if(size>32)		ret=64;
	if(size>64)		ret=128;
	if(size>128)	ret=256;
	if(size>256)	ret=512;
	if(size>512)	ret=1024;
	return ret;
}

void putImage(IIF *img, int x, int y)
{
	myGsDriver->drawPipe.TextureUpload(
		myGsDriver->getTextureBufferBase(), 
		getMinTextureSize(img->width), 0, 0, (int)img->psm, (const unsigned char*)&(img->pixel_data), img->width, img->height
	);
	myGsDriver->drawPipe.TextureSet(
		myGsDriver->getTextureBufferBase(), 
		getMinTextureSize(img->width), (gsTexSize)getMinTextureSizeID(img->width), (gsTexSize)getMinTextureSizeID(img->height), 
		img->psm, 0, 0, 0, 0
	); 
	myGsDriver->drawPipe.RectTexture(
		x, y, 0, 0, x+img->width, y+img->height, img->width, img->height, 
		1, GS_SET_RGBA(0x80,0x80,0x80,0x80)
	);
	
	free(img);
}

void putBackground(IIF *img)
{		
	myGsDriver->drawPipe.TextureUpload(
		myGsDriver->getTextureBufferBase(), 
		getMinTextureSize(img->width), 0, 0, (int)img->psm, (const unsigned char*)&(img->pixel_data), img->width, img->height
	);
	myGsDriver->drawPipe.TextureSet(
		myGsDriver->getTextureBufferBase(), 
		getMinTextureSize(img->width), (gsTexSize)getMinTextureSizeID(img->width), (gsTexSize)getMinTextureSizeID(img->height), 
		img->psm, 0, 0, 0, 0
	);
	
	int x = 0;
	int y = 0;
	
	for(x=0; x<tvmx; x+=8)
	{
		for(y=60; y<420; y+=8)
		{
			myGsDriver->drawPipe.RectTexture(
				x, y, 0, 0, x+img->width, y+img->height, img->width, img->height, 
				1, GS_SET_RGBA(0x80,0x80,0x80,0x80)
			);
		}
	}
	
	free(img);
}

/***************************************************************
 * getCharFromNum
 * Funcion tonta para el teclado numerico
 ***************************************************************/
char getCharFromNum(int num)
{
	if((num >= 0) && (num <= 9))
	{
		return (char)(num+48);
	}
	else
	{
		return '\0';
	}
}

/***************************************************************
 * main
 * 
 ***************************************************************/
int main(int argc, char *argv[])
{
	
	// Variables
	int x2 = 111;
	int y = 140;
	char cad[256];
	int ret, n;
	int mx1, my1, mx2, mbx1, mbx2;
	unsigned text_color;
	int sax, say;
	
	// inicializa las RPC
	SifInitRpc(0);
	
	// Reset
	#if !defined(DEVELOP)
		Reset();
	#endif
		
	SifInitRpc(0); // inicializa las RPC
	SifInitCmd();
	
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();
	
	// gsLib
	myGsDriver = new gsDriver;
	myGsDriver->setDisplayMode(tvmx, tvmy, tvx, tvy, GS_PSMCT32, 2, GS_TV_AUTO, GS_TV_INTERLACE, GS_ENABLE, GS_PSMZ32);
	myFont.assignPipe(&myGsDriver->drawPipe);
	myGsDriver->drawPipe.setAlphaEnable(GS_ENABLE);
	myGsDriver->drawPipe.setZTestEnable(GS_DISABLE);
	fontTex = (gsFontTex*)binary_arial_start;
	
	myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
	putScreen(233, 200, txt_70);
	
	// Cargamos los modulos
	loadModules();
	
	// Iniciamos el PAD
	padInit(0);
	// Open...
	padPortOpen(0, 0, padBuf);
	padPortOpen(1, 0, padBuf2);
	// ¿Esta apunto?
	ret = padGetState(0, 0);
	while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1))
	{
		if(ret==PAD_STATE_DISCONN)
		{
			// Ops! No PAD!!
			break;
		}
		ret=padGetState(0, 0);
	}
	// Iniciamos en modo analogico para usar el R3 y el L3
	padSetMainMode(0, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_UNLOCK);
	padSetMainMode(1, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_UNLOCK);
	
	// Ficheros
	strcpy(baseFullPath, argv[0]);
	int i = strlen(baseFullPath)-1;
	
	if(i > 0)
	{
		baseFullPath[i] = '\0';
		while(baseFullPath[i] != '/' && baseFullPath[i] != ':' && baseFullPath[i] != '\\')
		{
			baseFullPath[i]='\0';
			i--;
		}
	}
	
	// Paths para los Ficheros
	if(strncmp(baseFullPath, "cdrom", 5) == 0)
	{
		sprintf(kl_dat, "%s", "cdfs:/KL.DAT");
		sprintf(kl_mp3, "%s", "cdfs:/KL.MP3");
	}
	else
	{
		sprintf(kl_dat,			"%s%s",	baseFullPath,	kl_dat_file);
		sprintf(kl_icon_sys,	"%s%s",	baseFullPath,	kl_icon_sys_file);
		sprintf(kl_icon_icn,	"%s%s",	baseFullPath,	kl_icon_icn_file);
		sprintf(kl_mp3,			"%s%s",	baseFullPath,	kl_mp3_file);
	}
		
	// Leemos el fichero de configuración de los botones
	int read = readMyCNF(); 
	if(read < 0)
	{		
		myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
		sprintf(txt_32, "%s%s%s%s%s%d", txt_32, "\n", kl_dat, "\n", "read:", read);
		while(1)
		{
			putScreen(133, 200, txt_32);
		}
	}
	// Establecemos el tiempo para el Autorun
	setTimeAuto();
	
	// ¿Icono?
	if(baseFullPath[0]=='m' && baseFullPath[1]=='c')
	{
		saveIcon();
	}
	
	// Autoload
	if(autoloadelf > 0) {
		id_va_handler = myGsDriver->AddVSyncCallback(va_handler);
		bautoloadelf = autoloadelf;
	}
	
	// Idioma
	#ifndef EMULATOR
		s32 lang = configGetLanguage();
		if(lang == SPANISH)
		{
			// Spanish
			currentLang = SPANISH;
		}
		else if(lang == FRENCH)
		{
			// French
			setLangFrench();
			currentLang = FRENCH;
		}
		else
		{
			// Other
			setLangEnglish();
			currentLang = ENGLISH;
		}
	#endif
		
	// MP3
	#if defined(MP3_SOUND)
		
		// Buscamos el MP3
		int fdFileExists = fioOpen(kl_mp3, O_RDONLY);
		if(fdFileExists > 0)
		{
			foundMP3 = 1;
			fioClose(fdFileExists);
		}
			
		if(foundMP3 == 1)
		{
			if(SoundInit()) SleepThread(); // inicializa la RPC de sonido
			InitMP3(1); // inicializa la lib MP3 con codec de alta calidad
			PCMSilence();
			PCMInit(44100, 2);
			playMyMP3();
		}
	#endif
		
	// Paramos el disco
	CDVD_Init();
	CDVD_Stop();
	CDVD_FlushCache();
	
	// Bucle pricipal
	while(1)
	{
		
		// Fondo
		putBackground(img_fondo);
		myGsDriver->drawPipe.RectFlat(0,0,tvmx,60,0,colorNegro);
		myGsDriver->drawPipe.RectGouraud(0,420,colorNegro,tvmx,tvmy,colorAzul,0);
		// Subimos la textura para la fuente
		myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
		// Textos
		myFont.Print(420, tvmx, 15, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_1);
		myFont.Print(420, tvmx, 30, 1, colorNaranja, GSFONT_ALIGN_LEFT, "http://ps2dev.siirlabs.com");
		// Parte central
		//myGsDriver->drawPipe.RectGouraud(53,123,colorNegro,593,369,colorAzul,0);
		myGsDriver->drawPipe.RectFlat(55,125,591,367,0,colorBlanco);
		// Lineas blancas
		myGsDriver->drawPipe.Line(0,59,tvmx,59,0,colorGris2);
		myGsDriver->drawPipe.Line(0,420,tvmx,420,0,colorGris2);
		
		
		/*
		 * mainStatus
		 *
		 * 0 = Pantalla inicial
		 * 1 = Seleccion del ELF
		 * 2 = Asignacion del Boton
		 * 3 = Nombre para el ELF
		 *
		 */
		if(mainStatus==STATUS_MAIN)
		{
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_3);
			
			// Logo
			putImage(img_mrsiir, 463, 322);
			
			// Subimos la textura para la fuente
			myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
		
			// Textos de los botones X,O,R1.....
			y = 131;
			myFont.Print(x2, 4000, y, 1, getColorAuto(1, DIRELF_1), GSFONT_ALIGN_LEFT, (char *)DIRELF_1_TXT);	y+=24;
			myFont.Print(x2, 4000, y, 1, getColorAuto(2, DIRELF_2), GSFONT_ALIGN_LEFT, (char *)DIRELF_2_TXT);	y+=24;
			myFont.Print(x2, 4000, y, 1, getColorAuto(3, DIRELF_3), GSFONT_ALIGN_LEFT, (char *)DIRELF_3_TXT);	y+=24;
			myFont.Print(x2, 4000, y, 1, getColorAuto(4, DIRELF_4), GSFONT_ALIGN_LEFT, (char *)DIRELF_4_TXT);	y+=29;
			
	
			myFont.Print(x2, 4000, y, 1, getColorAuto(8, DIRELF_L1),GSFONT_ALIGN_LEFT, (char *)DIRELF_L1_TXT);	y+=20;
			myFont.Print(x2, 4000, y, 1, getColorAuto(9, DIRELF_L2),GSFONT_ALIGN_LEFT, (char *)DIRELF_L2_TXT);	y+=20;
			myFont.Print(x2, 4000, y, 1, getColorAuto(10,DIRELF_L3),GSFONT_ALIGN_LEFT, (char *)DIRELF_L3_TXT);	y+=30;
			
			myFont.Print(x2, 4000, y, 1, getColorAuto(5, DIRELF_R1),GSFONT_ALIGN_LEFT, (char *)DIRELF_R1_TXT);	y+=20;
			myFont.Print(x2, 4000, y, 1, getColorAuto(6, DIRELF_R2),GSFONT_ALIGN_LEFT, (char *)DIRELF_R2_TXT);	y+=20;
			myFont.Print(x2, 4000, y, 1, getColorAuto(7, DIRELF_R3),GSFONT_ALIGN_LEFT, (char *)DIRELF_R3_TXT);
			
			// Autoload
			if(autoloadelf > 0)
			{
				unsigned tmp = (count+100) / 100;
				int xt = 562, yt = 85;
				fillCircle(565, 95, 30, GS_SET_RGBA(0x00,0x00,0xFF,0x40));
				myFont.Print(460, tvmx, 65, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_5);
				sprintf(cad, "%u", tmp);
				if(tmp > 9) xt -= 8;
				m_putcadG(xt, yt, colorBlanco, cad);
			}
			
			if(USB_MODE==1)
			{
				putImage(img_usb, 520, 130);
			}
			
			// Botones
			int tx = 75;
			int ty = 127;
			int tinc = 24;
			putImage(img_cuadrado, tx, ty);		ty+=tinc;
			putImage(img_triangulo, tx, ty);	ty+=tinc;
			putImage(img_circulo, tx, ty);		ty+=tinc;
			putImage(img_cruz, tx, ty);			ty+=tinc;
			
			tinc = 20;
			ty+=8;
			tx = 70;
			
			putImage(img_L1, tx, ty);		ty+=tinc;
			putImage(img_L2, tx, ty);		ty+=tinc;
			putImage(img_L3, tx, ty);		ty+=(tinc+10);
			putImage(img_R1, tx, ty);		ty+=tinc;
			putImage(img_R2, tx, ty);		ty+=tinc;
			putImage(img_R3, tx, ty);		ty+=tinc;
			
		}
		else if(mainStatus == STATUS_SELF)
		{
			if(browseStatus == BROWSE_MAIN)
			{
				myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_10);
				// Dispositivos
				sax = 70;
				say = 140;
				putImage(img_icomc, sax, say-2);
				myFont.Print(sax+30, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_33);
				if(browseStatusSel==1) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
				say+=25;
				putImage(img_icousb, sax, say-2);
				myFont.Print(sax+30, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_34);
				if(browseStatusSel==2) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
				say+=25;
				putImage(img_icomano, sax, say-2);
				myFont.Print(sax+30, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_35);
				if(browseStatusSel==3) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
				say+=25;
				putImage(img_icohdd, sax, say-2);
				myFont.Print(sax+30, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_36);
				if(browseStatusSel==4) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
			}
			else if(browseStatus == BROWSE_HAND)
			{
				myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_37);
				//Uffff!!!!!
				myGsDriver->drawPipe.RectFlat(65, 137, 583, 157, 0, GS_SET_RGBA(0x74,0x74,0x74,0x80));
				myGsDriver->drawPipe.RectFlat(64, 136, 582, 156, 0, colorAzul);
				myGsDriver->drawPipe.RectFlat(66, 138, 580, 154, 0, colorBlanco);
				myFont.Print(68, tvmx, 138, 1, colorNegro, GSFONT_ALIGN_LEFT, (char *)gPath);
				kbx1 = 70;
				kby1 = 170;
				int w2 = 520;
				int h2 = 120;
				int j1=64;
				int j2=164;
				for(j1=64; j1<604; j1+=20)
				{
					myGsDriver->drawPipe.Line(j1,j2,j1,j2+h2,0,colorGris2);
				}
				j1=64;
				j2=164;
				for(j2=164; j2<=284; j2+=40)
				{
					myGsDriver->drawPipe.Line(j1,j2,j1+w2,j2,0,colorGris2);
				}
				int nC = 0;
				for(n=0; n<78; n++)
				{
					if(selectCharKB == n)
					{
						myGsDriver->drawPipe.RectFlat(kbx1-5, kby1-5, kbx1+14, kby1+34, 1, colorBlanco);
						myGsDriver->drawPipe.RectFlat(kbx1-5, kby1-5, kbx1+14, kby1+34, 2, color_back);
					}
					m_putcharG(kbx1, kby1, colorNegro, keysKEY[n]);
					kbx1 += 20;
					nC++;
					if(nC > 25)
					{
						nC=0;
						kby1 += 40;
						kbx1 = 70;
					}
				}
				myFont.Print(155, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_18);
				myFont.Print(270, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_19);
				myFont.Print(355, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_20);
				myFont.Print(445, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_42);
				putImage(img_cruz,		130,	337);
				putImage(img_circulo,	244,	337);
				putImage(img_cuadrado,	326,	337);
				putImage(img_triangulo,	419,	337);
			}
			else
			{
				if(browseStatus == BROWSE_HDD) {
					myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_36);
				} else if(browseStatus == BROWSE_MC) {
					myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_33);
				} else {
					myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_34);
				}
				mx1=100;
				my1=140;
				mx2=77;
				n = 0;
				mbx1=55;
				mbx2=575;
				nCountEntryInPage = 0;
				altolinea = 20;
				atotal =  altolinea * nCountFiles;
				max_height = 200;
				height1 = 0;
				diff = 0;
				sbx1 = 576;
				sby1 = 124;
				sbx2 = 592;
				sby2 = 368;
				sbintx = sbx1+3;
				sbinty = sby1+22;
				sbintw = 11;
				sbinth = sbinty+max_height;
				
				
				//int pzTot = (altolinea * 100) / atotal;
				//juas juas juas TODO: hacerlo
				double yplus = 0; //nActualEntry * 2;
				
				if(nCountFiles > MAX_ENTRY_PAGE)
				{
					height1 = (MAX_ENTRY_PAGE * max_height) / nCountFiles;
					diff = max_height - height1;
				}
					
				myGsDriver->drawPipe.RectFlat(sbx1, sby1, sbx2, sby2, 0, colorAzul);
				myGsDriver->drawPipe.RectFlat(sbx1+1, sby1+20, sbx2, sby2-20, 0, colorBlanco);
				myGsDriver->drawPipe.RectFlat(int(sbintx), int(sbinty+yplus), int(sbintx+sbintw), int((sbinth-diff)+yplus), 0, GS_SET_RGBA(0x81,0x81,0x81,0x80));
				myGsDriver->drawPipe.TriangleFlat(581, 136, 0, 590, 136, 0, 585, 131, 0, colorBlanco);
				myGsDriver->drawPipe.TriangleFlat(581, 357, 0, 589, 358, 0, 585, 362, 0, colorBlanco);
			
				nStartEntry = 0;
				if(nCountFiles > MAX_ENTRY_PAGE)
				{
					if((nActualEntry >= MAX_ENTRY_PAGE) && (nActualEntry < nCountFiles))
					{
						nStartEntry = (nActualEntry - MAX_ENTRY_PAGE) + 1;
					}
					else
					{
						nStartEntry = 0;
					}
				}
				for(n=nStartEntry; n<nCountFiles; n++)
				{
					if(TypeFiles[n]==0) {
						if(strstr(Files[n], ".ELF") || strstr(Files[n], ".elf")) {
							putImage(img_fileelf, mx2, my1);
						} else if(strstr(Files[n], ".NPO") || strstr(Files[n], ".npo")) {
							putImage(img_npo, mx2, my1);
						} else {
							putImage(img_fileold, mx2, my1);
						}
					} else if(TypeFiles[n]==1 || TypeFiles[n]==4) {
						putImage(img_folder, mx2, my1);
					} else if(TypeFiles[n]==2 || TypeFiles[n]==3) {
						putImage(img_folderup, mx2, my1);
					}
					
					text_color = text_color1;
					if(nActualEntry == n) {
						text_color = text_color2;
						myGsDriver->drawPipe.RectFlat(mbx1,my1,mbx2,my1+17,1,color_back);
					}
					if(TypeFiles[n]==2 || TypeFiles[n]==3) {
						myFont.Print(mx1, 640, my1, 1, text_color, GSFONT_ALIGN_LEFT, "..");
					} else {
						if(TypeFiles[n]==4) text_color = colorRojo2;
						myFont.Print(mx1, 640, my1, 1, text_color, GSFONT_ALIGN_LEFT, (char *)Files[n]);
					}
					
					my1+=20;
					nCountEntryInPage++;
					if(nCountEntryInPage >= MAX_ENTRY_PAGE) break;
				}
			}
		}
		else if(mainStatus == STATUS_SBUTON)
		{		
			if(browseStatus == BROWSE_MC)
			{
				sprintf(selectectedELFName, "%s%s", gPath, Files[nActualEntry]);
			}
			else if(browseStatus == BROWSE_USB)
			{
				sprintf(selectectedELFName, "%s%s%s", "mass:", gPath, Files[nActualEntry]);
			}
			else if(browseStatus == BROWSE_HAND)
			{
				sprintf(selectectedELFName, "%s", Files[nActualEntry]);
			}
			else if(browseStatus == BROWSE_HDD)
			{
				sprintf(selectectedELFName, "%s%s%s", hddpart, gPath, Files[nActualEntry]);
			}
		
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_13);
			myFont.Print(70, tvmx, 140, 2, colorNegro, GSFONT_ALIGN_LEFT, txt_14);
			myFont.Print(70, 500, 160, 1, colorAzul, GSFONT_ALIGN_LEFT, (char *)selectectedELFName);
			myFont.Print(70, tvmx, 180, 2, colorNegro, GSFONT_ALIGN_LEFT, txt_15);
			myFont.Print(395, tvmx, 345, 2, colorRojo, GSFONT_ALIGN_LEFT, txt_16);
		}
		else if(mainStatus == STATUS_SNAME)
		{
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_17);
			myGsDriver->drawPipe.RectFlat(65, 137, 583, 157, 0, GS_SET_RGBA(0x74,0x74,0x74,0x80));
			myGsDriver->drawPipe.RectFlat(64, 136, 582, 156, 0, colorAzul);
			myGsDriver->drawPipe.RectFlat(66, 138, 580, 154, 0, colorBlanco);
			myFont.Print(68, tvmx, 138, 1, colorNegro, GSFONT_ALIGN_LEFT, (char *)nameEFLKB);
			kbx1 = 70;
			kby1 = 170;

			int w2 = 520;
			int h2 = 120;
			int j1=64;
			int j2=164;
			// Lineas
			for(j1=64; j1<604; j1+=20)
			{
				myGsDriver->drawPipe.Line(j1,j2,j1,j2+h2,0,colorGris2);
			}
			j1=64;
			j2=164;
			for(j2=164; j2<=284; j2+=40)
			{
				myGsDriver->drawPipe.Line(j1,j2,j1+w2,j2,0,colorGris2);
			}
			
			// Teclas
			int nC = 0;
			for(n=0; n<78; n++)
			{
				if(selectCharKB == n)
				{
					myGsDriver->drawPipe.RectFlat(kbx1-5, kby1-5, kbx1+14, kby1+34, 1, colorBlanco);
					myGsDriver->drawPipe.RectFlat(kbx1-5, kby1-5, kbx1+14, kby1+34, 2, color_back);
				}
				
				m_putcharG(kbx1, kby1, colorNegro, keysKEY[n]);
				kbx1 += 20;
				nC++;
				if(nC > 25)
				{
					nC=0;
					kby1 += 40;
					kbx1 = 70;
				}
			}
			
			// Textos botones
			myFont.Print(155, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_18);
			myFont.Print(270, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_19);
			myFont.Print(355, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_20);
			myFont.Print(445, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_21);
			// Graficos botones
			putImage(img_cruz,		130,	337);
			putImage(img_circulo,	244,	337);
			putImage(img_cuadrado,	326,	337);
			putImage(img_triangulo,	419,	337);
		}
		else if(mainStatus == STATUS_SACTION)
		{
			// Acciones
			sax = 70;
			say = 140;
			// Titulo
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_22);
			// Opcion 1
			myFont.Print(sax, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_23);
			if(saStat==1) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
			// Opcion 2
			say+=25;
			myFont.Print(sax, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_24);
			if(saStat==2) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
			// Opcion 3
			say+=25;
			myFont.Print(sax, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_25);
			if(saStat==3) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);			
			// Opcion 4
			say+=25;
			myFont.Print(sax, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_43);
			if(saStat==4) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
			// Opcion 5
			/**/
			say+=25;
			myFont.Print(sax, tvmx, say, 1, colorNegro, GSFONT_ALIGN_LEFT, "5. Cargador de Backups");
			if(saStat==5) myGsDriver->drawPipe.RectFlat(sax-7, say-4, sax+513, say+19, 2, color_back);
			
			// Texto Cancelar
			myFont.Print(395, tvmx, 345, 2, colorRojo, GSFONT_ALIGN_LEFT, txt_16);
		}
		else if(mainStatus == STATUS_SAUTO)
		{
			// Titulo
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_26);
			// Texto Cancelar
			myFont.Print(395, tvmx, 345, 2, colorRojo, GSFONT_ALIGN_LEFT, txt_16);
			// Texto
			myFont.Print(70, tvmx, 140, 2, colorNegro, GSFONT_ALIGN_LEFT, txt_27);
		}
		else if(mainStatus == STATUS_STIME)
		{
			// Graficos botones
			putImage(img_cruz,		130,	337);
			putImage(img_circulo,		244,	337);
			putImage(img_triangulo,		326,	337);
			// Subimos la textura para la fuente
			myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
			// Titulo
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_28);
			// Texto Cancelar
			myFont.Print(405, tvmx, 97, 2, colorGris2, GSFONT_ALIGN_LEFT, txt_16);
			
			myGsDriver->drawPipe.RectFlat(65, 137, 583, 157, 0, GS_SET_RGBA(0x74,0x74,0x74,0x80));
			myGsDriver->drawPipe.RectFlat(64, 136, 582, 156, 0, colorAzul);
			myGsDriver->drawPipe.RectFlat(66, 138, 580, 154, 0, colorBlanco);
			myFont.Print(68, tvmx, 138, 1, colorNegro, GSFONT_ALIGN_LEFT, (char *)nameNumKB);
			
			kbx1 = 80;
			kby1 = 170;
			for(n=0; n<=9; n++)
			{
				if(selectNumKB == n)
				{
					myGsDriver->drawPipe.RectFlat(kbx1-5, kby1, kbx1+15, kby1+30, 0, colorNegro);
					myGsDriver->drawPipe.RectFlat(kbx1-4, kby1+1, kbx1+14, kby1+29, 1, colorBlanco);
					myGsDriver->drawPipe.RectFlat(kbx1-4, kby1+1, kbx1+14, kby1+29, 2, color_back);
				}
				m_putcharG(kbx1, kby1, colorNegro, getCharFromNum(n));
				kbx1+=20;
			}
			
			// Textos botones
			myFont.Print(155, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_18);
			myFont.Print(270, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_19);
			myFont.Print(355, tvmx, 341, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_21);
			
		}
		else if(mainStatus == STATUS_COPION)
		{	
			int diff = 304;
			
			myFont.Print(53, tvmx, 97, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_44);
			myGsDriver->drawPipe.RectFlat(11, 68, 631, 415, 0, GS_SET_RGBA(0xBB,0xBB,0xBB,0x80));
			myGsDriver->drawPipe.RectFlat(13, 70, 629, 413, 0, GS_SET_RGBA(0x8B,0x8B,0x8B,0x80));
			
			myGsDriver->drawPipe.RectFlat(19, 76, 317, 94, 0, colorBlanco);
			myGsDriver->drawPipe.RectFlat(19, 99, 317, 407, 0, colorBlanco);
			
			myGsDriver->drawPipe.RectFlat(19+diff, 76, 317+diff, 94, 0, colorBlanco);
			myGsDriver->drawPipe.RectFlat(19+diff, 99, 317+diff, 407, 0, colorBlanco);
			
			char printPath1[256];
			copyCharArray((char *)printPath1, (char *)gPathLeft);
			for(n=35;n<255;n++) printPath1[n] = 0;
			
			char printPath2[256];
			copyCharArray((char *)printPath2, (char *)gPathRight);
			for(n=35;n<255;n++) printPath2[n] = 0;
			
			myFont.Print(22, tvmx, 77, 1, colorNegro, GSFONT_ALIGN_LEFT, (char *)printPath1);
			myFont.Print(22+diff, tvmx, 77, 1, colorNegro, GSFONT_ALIGN_LEFT, (char *)printPath2);
			
			printDir(LeftFiles, LeftTypeFiles, nCountLeftFiles, (char *)gPathLeft, 50, 105, 25, 20, 315, nLeftActualEntry, 1);
			printDir(RightFiles, RightTypeFiles, nCountRightFiles, (char *)gPathRight, 50+diff, 105, 25+diff, 20+diff, 315+diff, nRightActualEntry, 2);
			
			if(selectDevice==1)
			{
				myGsDriver->drawPipe.RectFlat(209, 159, 427, 271, 0, GS_SET_RGBA(0x00,0x00,0x00,0x80));
				myGsDriver->drawPipe.RectFlat(211, 161, 425, 269, 0, GS_SET_RGBA(0x8B,0x8B,0x8B,0x80));
				int txt = 165;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_45);
				if(selectedDeviceMenu==1) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_46);
				if(selectedDeviceMenu==2) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_47);
				if(selectedDeviceMenu==3) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_48);
				if(selectedDeviceMenu==4) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_49);
				if(selectedDeviceMenu==5) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
			}
			
			if(selectActionFile==1)
			{
				myGsDriver->drawPipe.RectFlat(209, 159, 427, 266, 0, GS_SET_RGBA(0x00,0x00,0x00,0x80));
				myGsDriver->drawPipe.RectFlat(211, 161, 425, 264, 0, GS_SET_RGBA(0x8B,0x8B,0x8B,0x80));
				int txt = 165;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_50);
				if(selectedFileMenu==1) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_51);
				if(selectedFileMenu==2) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_52);
				if(selectedFileMenu==3) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_53);
				if(selectedFileMenu==4) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
				txt+=20;
				myFont.Print(224, tvmx, txt, 1, colorBlanco, GSFONT_ALIGN_LEFT, txt_54);
				if(selectedFileMenu==5) myGsDriver->drawPipe.RectFlat(211, txt-2, 425, txt+17, 2, colorGris2);
			}
			
			if(selectMenuInfo == 1)
			{
				myGsDriver->drawPipe.RectFlat(109, 159, 527, 266, 0, GS_SET_RGBA(0x00,0x00,0x00,0x80));
				myGsDriver->drawPipe.RectFlat(111, 161, 525, 264, 0, GS_SET_RGBA(0x8B,0x8B,0x8B,0x80));
				
				char txtName[1024];
				
				if(selectPanelCopy == 1) {
					myActualEntry = nLeftActualEntry;
					sprintf(txtName, "%s", LeftFiles[nLeftActualEntry]);
				} else {
					myActualEntry = nRightActualEntry;
					sprintf(txtName, "%s", RightFiles[nRightActualEntry]);
				}
				
				if(myActualEntry <= 0)
				{
					sprintf(txtInfo, "%s", txt_55);
				}
				else
				{
					sprintf(txtInfo, "%s", txt_56);
					sprintf(txtInfo, "%s%s%s%s", txtInfo, "Nombre: ", txtName, "\n");
					sprintf(txtInfo, "%s%s%s%s", txtInfo, "Nombre real: ", txtRealName_IconSys, "\n");
					sprintf(txtInfo, "%s%s", txtInfo, "\n");
					sprintf(txtInfo, "%s%s", txtInfo, "En construcción!!!\n");
					sprintf(txtInfo, "%s%s", txtInfo, "\n");
				}
				myFont.Print(124, tvmx, 165, 1, colorBlanco, GSFONT_ALIGN_LEFT, txtInfo);
			}
			
			if(selectHelp == 1)
			{
				myGsDriver->drawPipe.RectFlat(89, 129, 547, 346, 0, GS_SET_RGBA(0x00,0x00,0x00,0x80));
				myGsDriver->drawPipe.RectFlat(91, 131, 545, 344, 0, GS_SET_RGBA(0x8B,0x8B,0x8B,0x80));
				myFont.Print(110, tvmx, 140, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_83);
				myFont.Print(110, tvmx, 170, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\bL1\b");
				myFont.Print(110, tvmx, 190, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\bR1\b");
				myFont.Print(110, tvmx, 210, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\bR2\b");
				myFont.Print(110, tvmx, 230, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\bX\b");
				myFont.Print(110, tvmx, 250, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\bX\b");
				myFont.Print(110, tvmx, 270, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\b[]\b");
				myFont.Print(110, tvmx, 290, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\b/\\\b");
				myFont.Print(110, tvmx, 310, 1, colorBlanco, GSFONT_ALIGN_LEFT, "\bSTART\b");
				myFont.Print(162, tvmx, 170, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_75);
				myFont.Print(162, tvmx, 190, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_76);
				myFont.Print(162, tvmx, 210, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_77);
				myFont.Print(162, tvmx, 230, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_78);
				myFont.Print(162, tvmx, 250, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_79);
				myFont.Print(162, tvmx, 270, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_80);
				myFont.Print(162, tvmx, 290, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_81);
				myFont.Print(162, tvmx, 310, 1, colorBlanco, GSFONT_ALIGN_LEFT, (char *)txt_82);
			}
			
		}
		else if(mainStatus == STATUS_CSL)
		{
			
			CD_disco(0);
			
			CD_loadmodules();
			CDVD_Init();
			CDVD_DiskReady(0);
			
			while(1)
			{	
				if(readPad())
				{
					CD_mode = fioOpen("qwerty:t",1) & 255;
					if(new_pad & (PAD_CROSS | PAD_CIRCLE))
					{
						fioOpen("qwerty:r",1);
						if(CD_ReadCNF())
						{
							padPortClose(0,0);
							padPortClose(1,0);
							padEnd();
							CD_Reset(2);
							SifInitRpc(0);
							break;
						}
					}
					if(new_pad & (PAD_TRIANGLE))
					{
						goto CD_TheEnd;
					}
				}
				
				if(CD_mode==0x14) {
					if(CD_invalid) {
						CD_disco(3);
						CD_invalid = 0;
						fioOpen("qwerty:s",1);
					}
				} else if(CD_mode==0x12 || CD_mode==0x13) {
					if(CD_invalid) {
						CD_disco(2);
						CD_invalid = 0;
						fioOpen("qwerty:s",1);
					}
				} else if(CD_mode==0x10 || CD_mode==0x11) {
					if(CD_invalid) {
						CD_disco(1);
						CD_invalid=0;
						fioOpen("qwerty:s",1);
					}
				}
				else
				{
					if(CD_invalid==0) {
						CD_disco(0);
						CD_invalid=1;
					}
				}		
			}
			
			if(CD_mode==0x14 || (new_pad &  PAD_CIRCLE)) // DVDR
			{
				CD_Reset(1);
				SifInitRpc(0);
				CD_loadmodules();
				
				padInit(0);
				padPortOpen(0, 0, padBuf);
				padPortOpen(1, 0, padBuf2);
				ret = padGetState(0, 0);
				while((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1))
				{
					if(ret==PAD_STATE_DISCONN)
					{
						goto CD_TheEnd;
					}
					ret=padGetState(0, 0);
				}
				padSetMainMode(0, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_UNLOCK);
				padSetMainMode(1, 0, PAD_MMODE_DUALSHOCK, PAD_MMODE_UNLOCK);
				
				CDVD_Stop();
				CDVD_FlushCache();
			
				fioOpen("qwerty:c",1); // Put LBA 0
				fioOpen("qwerty:s",1); // Stop Disk
			
				CD_disco(4);
				
				CDVD_Init();
				CDVD_DiskReady(0);
				
				while(1)
				{
					if(readPad())
					{
						CD_mode = fioOpen("qwerty:t",1) & 255;
						if(new_pad & PAD_CROSS)
						{
							fioOpen("qwerty:r",1);
							if(CD_ReadCNF())
							{	
								padPortClose(0, 0);
								padPortClose(1, 0);
								padEnd();
								break;
							}
						}
						if(new_pad & (PAD_TRIANGLE))
						{
							goto CD_TheEnd;
						}
					}
				}
			}
			
			putScreen(100, 100, "Loading...");
			RunElf((char *)CD_DIRELF);
			////////////////////////////////////////////////////////
	CD_TheEnd:
			mainStatus = STATUS_SACTION;
			
			
		}
		
		// Logo
		putImage(img_logo, 21, 18);
		
		// Texto Thank You!
		// Subimos la textura para la fuente
		myFont.uploadFont(fontTex, myGsDriver->getTextureBufferBase(), fontTex->TexWidth, 0, 0 );
		if(mainStatus != STATUS_COPION)
		{
			myFont.Print(15, tvmx-15, 435, 1, colorGris2, GSFONT_ALIGN_LEFT, txt_40);
		}
		else
		{
			myFont.Print(0, tvmx, 460, 1, colorGris2, GSFONT_ALIGN_CENTRE, txt_57);
		}
	
		// Leemos el PAD
		if(readPad())
		{	
			checkButtons++;  // Analogico
			checkButtons2++; // Analogico
			checkButtons3++; // Analogico
			checkButtons4++; // Analogico
			checkButtons5++; // Analogico			
			
			if(mainStatus == STATUS_COPION)
			{
				
				if(selectDevice == 0 && selectActionFile == 0 && selectMenuInfo == 0)
				{
					if(new_pad & PAD_L1)
					{
						if(selectHelp == 0)
						{
							selectHelp = 1;
						}
						else
						{
							selectHelp = 0;
						}
					}
				}
				
				if(selectDevice == 0 && selectActionFile == 0 && selectHelp == 0)
				{
					if(new_pad & PAD_R2)
					{
						if(selectMenuInfo == 0)
						{
							selectMenuInfo = 1;
						}
						else
						{
							selectMenuInfo = 0;
						}
					}
				}
				
				if(selectActionFile == 0 && selectMenuInfo == 0 && selectHelp == 0)
				{
					if(new_pad & PAD_R1)
					{
						if(selectDevice == 0)
						{
							selectedDeviceMenu = 1;
							selectDevice = 1;
						}
						else
						{
							selectDevice = 0;
						}
					}
				}
				
				if(selectDevice == 1 && selectActionFile == 0 && selectMenuInfo == 0 && selectHelp == 0)
				{
					/* ANALOG */
					#ifndef EMULATOR
						if(checkButtons3 > 5)
						{
							if(buttons.ljoy_v > 200)
							{
								if(selectedDeviceMenu < 5) selectedDeviceMenu++;
							}
							if(buttons.ljoy_v < 50)
							{
								if(selectedDeviceMenu > 1) selectedDeviceMenu--;
							}
							checkButtons3 = 0;
						}
					#endif
					if(new_pad & PAD_UP)
					{
						if(selectedDeviceMenu > 1) selectedDeviceMenu--;
					}
					if(new_pad & PAD_DOWN)
					{
						if(selectedDeviceMenu < 5) selectedDeviceMenu++;
					}
					
					if(new_pad & PAD_CROSS)
					{
						
						if(selectedDeviceMenu==1)
						{
							//MC0
							sprintf(gPath, "%s", "mc0:/");
							getFioMCDir((char *)gPath);
							if(selectPanelCopy == PANEL_LEFT) nLeftDevice = DEVICE_MC0;
							if(selectPanelCopy == PANEL_RIGHT) nRightDevice = DEVICE_MC0;
						}
						else if(selectedDeviceMenu==2)
						{
							//MC1
							sprintf(gPath, "%s", "mc1:/");
							getFioMCDir((char *)gPath);
							if(selectPanelCopy == PANEL_LEFT) nLeftDevice = DEVICE_MC1;
							if(selectPanelCopy == PANEL_RIGHT) nRightDevice = DEVICE_MC1;
						}
						else if(selectedDeviceMenu==3)
						{
							//USB
							sprintf(gPath, "%s", "/");
							getMassDir((char *)gPath);
							if(selectPanelCopy == PANEL_LEFT) nLeftDevice = DEVICE_USB;
							if(selectPanelCopy == PANEL_RIGHT) nRightDevice = DEVICE_USB;
						}
						else if(selectedDeviceMenu==4)
						{
							//HDD
							sprintf(gPath, "%s", "/");
							putScreen(233, 200, txt_29);
							loadModulesHDD();
							putScreen(180, 200, txt_38);
							getRootHDDDir();
							if(selectPanelCopy == PANEL_LEFT) nLeftDevice = DEVICE_HDD;
							if(selectPanelCopy == PANEL_RIGHT) nRightDevice = DEVICE_HDD;
						}
						else if(selectedDeviceMenu==5)
						{
							//CDVD
							sprintf(gPath, "%s", "/");
							readCDDir((char *)gPath);
							if(selectPanelCopy == PANEL_LEFT) nLeftDevice = DEVICE_CDVD;
							if(selectPanelCopy == PANEL_RIGHT) nRightDevice = DEVICE_CDVD;
						}
							
						if(selectPanelCopy == 1)
						{
							copyCharArray2(LeftFiles, Files);
							copyCharArrayN((char *)LeftTypeFiles, (char *)TypeFiles);
							nCountLeftFiles = nCountFiles;
							nLeftActualEntry = 0;
							copyCharArray((char *)gPathLeft, (char *)gPath);
						}
						else
						{
							copyCharArray2(RightFiles, Files);
							copyCharArrayN((char *)RightTypeFiles, (char *)TypeFiles);
							nCountRightFiles = nCountFiles;
							nRightActualEntry = 0;
							copyCharArray((char *)gPathRight, (char *)gPath);
						}
						
						selectDevice = 0;
					}
					
				}
				else if(selectActionFile == 0 && selectMenuInfo == 0 && selectHelp == 0)
				{
					/* ANALOG */
					#ifndef EMULATOR
						if(checkButtons4 > 5)
						{
							if(buttons.ljoy_v > 200)
							{
								if(selectPanelCopy == 1) //Left
								{
									if(nLeftActualEntry < (nCountLeftFiles-1)) nLeftActualEntry++;
								}
								else
								{
									if(nRightActualEntry < (nCountRightFiles-1)) nRightActualEntry++;
								}
							}
							if(buttons.ljoy_v < 50)
							{
								if(selectPanelCopy == 1) //Left
								{
									if(nLeftActualEntry > 0) nLeftActualEntry--;
								}
								else
								{
									if(nRightActualEntry > 0) nRightActualEntry--;
								}
							}
							checkButtons4 = 0;
						}
					#endif
					
					if(new_pad & PAD_UP)
					{
						if(selectPanelCopy == 1) //Left
						{
							if(nLeftActualEntry > 0) nLeftActualEntry--;
						}
						else
						{
							if(nRightActualEntry > 0) nRightActualEntry--;
						}
					}
					if(new_pad & PAD_DOWN)
					{
						if(selectPanelCopy == 1) //Left
						{
							if(nLeftActualEntry < (nCountLeftFiles-1)) nLeftActualEntry++;
						}
						else
						{
							if(nRightActualEntry < (nCountRightFiles-1)) nRightActualEntry++;
						}
					}
					
					if(new_pad & PAD_TRIANGLE)
					{
						deleteDirFromPanels();
						selectActionFile = 0;
						refreshDir(PANEL_LEFT);
						refreshDir(PANEL_RIGHT);
					}
					
					if(new_pad & PAD_SQUARE)
					{
						char name[256];
						keyBoard((char *)name);
						if(name[0] != '\0')
						{
							createDirFromPanels((char *)name);
							refreshDir(PANEL_LEFT);
							refreshDir(PANEL_RIGHT);
						}
					}
					
					if(new_pad & PAD_CROSS)
					{
						if(selectPanelCopy == 1)
						{
							copyCharArray2(Files, LeftFiles);
							copyCharArrayN((char *)TypeFiles, (char *)LeftTypeFiles);
							copyCharArray((char *)gPath, (char *)gPathLeft);
							nActualEntry = nLeftActualEntry;
						}
						else
						{
							copyCharArray2(Files, RightFiles);
							copyCharArrayN((char *)TypeFiles, (char *)RightTypeFiles);
							copyCharArray((char *)gPath, (char *)gPathRight);
							nActualEntry = nRightActualEntry;
						}
						
						if(TypeFiles[nActualEntry]==3)
						{
							//printf("ROOT\n");
						}
						else if(TypeFiles[nActualEntry]==2)
						{
							int i = strlen(gPath)-1;				
							if(i > 0)
							{
								gPath[i] = '\0';
								while(gPath[i] != '/')
								{
									gPath[i]='\0';
									i--;
								}
							}
							else
							{
								gPath[0]='\0';
							}
							
							if(selectPanelCopy == 1)
							{
								if(nLeftDevice==DEVICE_MC0 || nLeftDevice==DEVICE_MC1) {
									if(gPath[0]==0) sprintf(gPath, "%s", "mc0:/");
									getFioMCDir((char *)gPath);
								} else if(nLeftDevice==DEVICE_USB) {
									if(gPath[0]==0) sprintf(gPath, "%s", "/");
									getMassDir((char *)gPath);
								} else if(nLeftDevice==DEVICE_HDD) {
									if(gPath[0]==0) sprintf(gPath, "%s", "/");
									getHDDDir((char *)gPath);
								} else if(nLeftDevice==DEVICE_CDVD) {
									if(gPath[0]==0) sprintf(gPath, "%s", "/");
									readCDDir((char *)gPath);
								}
								copyCharArray2(LeftFiles, Files);
								copyCharArrayN((char *)LeftTypeFiles, (char *)TypeFiles);
								copyCharArray((char *)gPathLeft, (char *)gPath);
								nCountLeftFiles = nCountFiles;
								nLeftActualEntry = 0;
							}
							else
							{
								if(nRightDevice==DEVICE_MC0 || nRightDevice==DEVICE_MC1) {
									if(gPath[0]==0) sprintf(gPath, "%s", "mc0:/");
									getFioMCDir((char *)gPath);
								} else if(nRightDevice==DEVICE_USB) {
									if(gPath[0]==0) sprintf(gPath, "%s", "/");
									getMassDir((char *)gPath);
								} else if(nRightDevice==DEVICE_HDD) {
									if(gPath[0]==0) sprintf(gPath, "%s", "/");
									getHDDDir((char *)gPath);
								} else if(nRightDevice==DEVICE_CDVD) {
									if(gPath[0]==0) sprintf(gPath, "%s", "/");
									readCDDir((char *)gPath);
								}

								copyCharArray2(RightFiles, Files);
								copyCharArrayN((char *)RightTypeFiles, (char *)TypeFiles);
								copyCharArray((char *)gPathRight, (char *)gPath);
								nCountRightFiles = nCountFiles;
								nRightActualEntry = 0;
							}
						}
						else if(TypeFiles[nActualEntry]==1)
						{
																				
							if(selectPanelCopy == 1)
							{
								if(nLeftDevice==DEVICE_MC0 || nLeftDevice==DEVICE_MC1) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									getFioMCDir((char *)gPath);
								} else if(nLeftDevice==DEVICE_USB) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									getMassDir((char *)gPath);
								} else if(nLeftDevice==DEVICE_HDD) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									if(ROOT_HDD==1) {
										sprintf(hddpart, "%s%s", "hdd0:", Files[nActualEntry]);
										sprintf(gPath, "%s", "/");
									}
									getHDDDir((char *)gPath);
								} else if(nLeftDevice==DEVICE_CDVD) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									readCDDir((char *)gPath);
								}
								copyCharArray2(LeftFiles, Files);
								copyCharArrayN((char *)LeftTypeFiles, (char *)TypeFiles);
								copyCharArray((char *)gPathLeft, (char *)gPath);
								nCountLeftFiles = nCountFiles;
								nLeftActualEntry = 0;
							}
							else
							{
								if(nRightDevice==DEVICE_MC0 || nRightDevice==DEVICE_MC1) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									getFioMCDir((char *)gPath);
								} else if(nRightDevice==DEVICE_USB) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									getMassDir((char *)gPath);
								} else if(nRightDevice==DEVICE_HDD) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									if(ROOT_HDD==1) {
										sprintf(hddpart, "%s%s", "hdd0:", Files[nActualEntry]);
										sprintf(gPath, "%s", "/");
									}
									getHDDDir((char *)gPath);
								} else if(nRightDevice==DEVICE_CDVD) {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
									readCDDir((char *)gPath);
								}
								copyCharArray2(RightFiles, Files);
								copyCharArrayN((char *)RightTypeFiles, (char *)TypeFiles);
								copyCharArray((char *)gPathRight, (char *)gPath);
								nCountRightFiles = nCountFiles;
								nRightActualEntry = 0;
							}
						}
						else if(TypeFiles[nActualEntry]==0)
						{
							if(Files[nActualEntry][0] != 0)
							{
								selectActionFile = 1;
							}
						}
					}
					
					/* ANALOG */
					#ifndef EMULATOR
						if(checkButtons5 > 5)
						{
							if(buttons.ljoy_h > 200)
							{
								if(selectPanelCopy == 1)
								{
									selectPanelCopy = 2;
								}
							}
							if(buttons.ljoy_h < 50)
							{
								if(selectPanelCopy == 2)
								{
									selectPanelCopy = 1;
								}
							}
							checkButtons5 = 0;
						}
					#endif
					
					if(new_pad & PAD_RIGHT)
					{
						if(selectPanelCopy == 1)
						{
							selectPanelCopy = 2;
						}
					}
					if(new_pad & PAD_LEFT)
					{
						if(selectPanelCopy == 2)
						{
							selectPanelCopy = 1;
						}
					}
					
					if(new_pad & PAD_L1)
					{
						/*
						copyRecursiveFromPanels();
						selectActionFile = 0;
						refreshDir(PANEL_LEFT);
						refreshDir(PANEL_RIGHT);
						*/
					}
					
				}
				else if(selectActionFile == 1)
				{
					// Fichero
					/* ANALOG */
					#ifndef EMULATOR
						if(checkButtons5 > 5)
						{
							if(buttons.ljoy_v > 200)
							{
								if(selectedFileMenu < 5) selectedFileMenu++;
							}
							if(buttons.ljoy_v < 50)
							{
								if(selectedFileMenu > 1) selectedFileMenu--;
							}
							checkButtons5 = 0;
						}
					#endif

					if(new_pad & PAD_UP)
					{
						if(selectedFileMenu > 1) selectedFileMenu--;
					}
					if(new_pad & PAD_DOWN)
					{
						if(selectedFileMenu < 5) selectedFileMenu++;
					}
					
					if(new_pad & PAD_CROSS)
					{
						if(selectedFileMenu == 1) // Copiar
						{
							copyFromPanels();
							selectActionFile = 0;
							refreshDir(PANEL_LEFT);
							refreshDir(PANEL_RIGHT);
						}
						else if(selectedFileMenu == 2) // Borrar
						{
							deleteFromPanels();
							selectActionFile = 0;
							refreshDir(PANEL_LEFT);
							refreshDir(PANEL_RIGHT);
						}
						else if(selectedFileMenu == 3) // Mover
						{
							copyFromPanels();
							deleteFromPanels();
							selectActionFile = 0;
							refreshDir(PANEL_LEFT);
							refreshDir(PANEL_RIGHT);
						}
						else if(selectedFileMenu == 4) // Ejecutar
						{
							int fromDevice = -1;
							char szFile[256];
							
							if(selectPanelCopy==1) {
								sprintf(szFile, "%s%s", gPathLeft, LeftFiles[nLeftActualEntry]);
								fromDevice = nLeftDevice;
							} else {
								sprintf(szFile, "%s%s", gPathRight, RightFiles[nRightActualEntry]);
								fromDevice = nRightDevice;
							}
							// Hack!
							if(fromDevice==DEVICE_USB) {
								char tmpPath[256];
								sprintf(tmpPath, "%s%s", "mass:", szFile);
								copyCharArray((char *)szFile, (char *)tmpPath);
							} else if(fromDevice==DEVICE_HDD) {
								char tmpPath[256];
								sprintf(tmpPath, "%s%s", "hdd0:", szFile);
								copyCharArray((char *)szFile, (char *)tmpPath);
							} else if(fromDevice==DEVICE_CDVD) {
								char tmpPath[256];
								sprintf(tmpPath, "%s%s", "cdfs:/", szFile);
								copyCharArray((char *)szFile, (char *)tmpPath);
							}
							
							if(strstr(szFile, ".NPO") || strstr(szFile, ".npo"))
							{								
								putMessageScreen(261, 195, "Copiando...");
								npoExtract((char *)szFile);
								selectActionFile = 0;
								refreshDir(PANEL_LEFT);
								refreshDir(PANEL_RIGHT);
							}
							else
							{
								runMyElf((unsigned char *)szFile);
							}
							
						}
						else if(selectedFileMenu == 5) // Cancelar
						{
							selectedFileMenu = 1;
							selectActionFile = 0;
						}
					}
				}
			}
			else if(mainStatus == STATUS_SAUTO)
			{
				int selectFA = 0;
				
				if(new_pad & PAD_SQUARE)	selectFA = 1;
				if(new_pad & PAD_TRIANGLE)	selectFA = 2;
				if(new_pad & PAD_CIRCLE)	selectFA = 3;
				if(new_pad & PAD_CROSS)		selectFA = 4;
				if(new_pad & PAD_R1)		selectFA = 5;
				if(new_pad & PAD_R2)		selectFA = 6;
				if(new_pad & PAD_R3)	selectFA = 7;
				if(new_pad & PAD_L1)		selectFA = 8;
				if(new_pad & PAD_L2)		selectFA = 9;
				if(new_pad & PAD_L3)		selectFA = 10;
				
				if(selectFA > 0)
				{
					// Han pulsado algo ... Guardar
					setELFForAuto(selectFA);
				}
				
			}
			else if(mainStatus == STATUS_STIME)
			{
				if(new_pad & PAD_LEFT)
				{
					if(selectNumKB > 0) selectNumKB--;
				}
				if(new_pad & PAD_RIGHT)
				{
					if(selectNumKB < 9) selectNumKB++;
				}
				if(new_pad & PAD_CROSS)
				{
					nameNumKB[nPosNumKB] = getCharFromNum(selectNumKB);
					nPosNumKB++;
					nameNumKB[nPosNumKB] = 0;
				}
				if(new_pad & PAD_CIRCLE)
				{
					if(nPosNumKB > 0)
					{
						nPosNumKB--;
						nameNumKB[nPosNumKB] = 0;
					}
				}
				if(new_pad & PAD_TRIANGLE)
				{
					// Guardar
					setTimeForAuto();
				}
				// PAD Analogico
				#ifndef EMULATOR
					if(checkButtons > 5)
					{
						if(buttons.ljoy_h > 200)
						{
							if(selectNumKB < 9) selectNumKB++;
						}
						if(buttons.ljoy_h < 50)
						{
							if(selectNumKB > 0) selectNumKB--;
						}
						checkButtons = 0;
					}
				#endif
			}
			else if(mainStatus == STATUS_SACTION)
			{
				if(new_pad & PAD_UP)
				{
					if(saStat > 1) saStat--;
				}
				if(new_pad & PAD_DOWN)
				{
					if(saStat < 5) saStat++;
				}
				if(new_pad & PAD_CROSS)
				{
					if(saStat==1)
					{
						mainStatus = STATUS_SELF;
					}
					else if(saStat==2)
					{
						mainStatus = STATUS_STIME;
					}
					else if(saStat==3)
					{
						mainStatus = STATUS_SAUTO;
					}
					else if(saStat==4)
					{
						sprintf(gPathLeft, "%s", "mc0:/");
						getFioMCDir((char *)gPathLeft);
						copyCharArray2(LeftFiles, Files);
						copyCharArrayN((char *)LeftTypeFiles, (char *)TypeFiles);
						nCountLeftFiles = nCountFiles;
						nLeftActualEntry = 0;
						nLeftDevice = DEVICE_MC0;
						
						sprintf(gPathRight, "%s", "mc0:/");
						getFioMCDir((char *)gPathRight);
						copyCharArray2(RightFiles, Files);
						copyCharArrayN((char *)RightTypeFiles, (char *)TypeFiles);
						nCountRightFiles = nCountFiles;
						nRightActualEntry = 0;
						nRightDevice = DEVICE_MC0;
						
						selectPanelCopy = PANEL_LEFT;
						mainStatus = STATUS_COPION;
					}
					else if(saStat==5)
					{
						mainStatus = STATUS_CSL;
					}
				}
			
				// PAD Analogico
				#ifndef EMULATOR
					if(checkButtons2 > 7)
					{
						if(buttons.ljoy_v > 240)
						{
							if(saStat < 5) saStat++;
						}
						if(buttons.ljoy_v < 10)
						{
							if(saStat > 1) saStat--;
						}
						checkButtons2 = 0;
					}
				#endif
			}
			else if(mainStatus == STATUS_MAIN)
			{
				// Para cada boton lanzamos su ELF, si lo tiene
				
				if(new_pad & PAD_DOWN)
				{
					// Paramos el MP3
					#if defined(MP3_SOUND)
						stopMyMP3();
					#endif
				}
				
				if(new_pad & PAD_SQUARE)	runMyElf(DIRELF_1);
				if(new_pad & PAD_TRIANGLE)	runMyElf(DIRELF_2);
				if(new_pad & PAD_CIRCLE)	runMyElf(DIRELF_3);
				if(new_pad & PAD_CROSS)		runMyElf(DIRELF_4);
				if(new_pad & PAD_R1)		runMyElf(DIRELF_R1);
				if(new_pad & PAD_R2)		runMyElf(DIRELF_R2);
				if(new_pad & PAD_R3)		runMyElf(DIRELF_R3);
				if(new_pad & PAD_L1)		runMyElf(DIRELF_L1);
				if(new_pad & PAD_L2)		runMyElf(DIRELF_L2);
				if(new_pad & PAD_L3)		runMyElf(DIRELF_L3);
				
				// Autoload
				if(new_pad & PAD_SELECT)
				{
					if(autoloadelf > 0)
					{
						autoloadelf = 0;
					}
					else if(bautoloadelf > 0)
					{
						setTimeAuto();
						autoloadelf = bautoloadelf;
					}
				}
			}
			else if(mainStatus == STATUS_SELF)
			{
				if(browseStatus == BROWSE_HAND)
				{
					int nlinc = 26;					
					int status = 0;
					if(selectCharKB >= nlinc) status = 1;
					if(selectCharKB >= (nlinc*2)) status = 2;
					if(new_pad & PAD_LEFT)
					{
						selectCharKB--;
						if(status==0) {
							if(selectCharKB < 0) selectCharKB = 25;
						} else if(status==1) {
							if(selectCharKB < 26) selectCharKB = 51;
						} else if(status==2) {
							if(selectCharKB < 52) selectCharKB = 77;
						}
					}
					if(new_pad & PAD_RIGHT)
					{
						selectCharKB++;
						if(status==0) {
							if(selectCharKB > 25) selectCharKB = 0;
						} else if(status==1) {
							if(selectCharKB > 51) selectCharKB = 26;
						} else if(status==2) {
							if(selectCharKB > 77) selectCharKB = 52;
						}
					}
					if(new_pad & PAD_DOWN)
					{
						if(status < 2) selectCharKB+=nlinc;
					}
					if(new_pad & PAD_UP)
					{
						if(status > 0) selectCharKB-=nlinc;
					}
					if(new_pad & PAD_CROSS)
					{
						gPath[nPosKB] = keysKEY[selectCharKB];
						nPosKB++;
						gPath[nPosKB] = 0;
					}
					if(new_pad & PAD_CIRCLE)
					{
						if(nPosKB > 0)
						{
							nPosKB--;
							gPath[nPosKB] = 0;
						}
					}
					if(new_pad & PAD_SQUARE)
					{
						gPath[nPosKB] = ' ';
						nPosKB++;
						gPath[nPosKB] = 0;
					}
					if(new_pad & PAD_TRIANGLE)
					{
						gPath[nPosKB] = '\n';
						nPosKB++;
						gPath[nPosKB] = 0;
						nActualEntry = 0;
						sprintf(Files[nActualEntry], "%s", gPath);
						mainStatus = STATUS_SBUTON;
						saStat = 1;
						nPosKB = 0;
					}
					// PAD Analogico
					#ifndef EMULATOR
						if(checkButtons > 5)
						{
							if(buttons.ljoy_h > 200)
							{
								selectCharKB++;
								if(status==0) {
									if(selectCharKB > 25) selectCharKB = 0;
								} else if(status==1) {
									if(selectCharKB > 51) selectCharKB = 26;
								} else if(status==2) {
									if(selectCharKB > 77) selectCharKB = 52;
								}
							}
							if(buttons.ljoy_h < 50)
							{
								selectCharKB--;
								if(status==0) {
									if(selectCharKB < 0) selectCharKB = 25;
								} else if(status==1) {
									if(selectCharKB < 26) selectCharKB = 51;
								} else if(status==2) {
									if(selectCharKB < 52) selectCharKB = 77;
								}
							}
							if(buttons.ljoy_v > 200)
							{
								if(status < 2) selectCharKB+=nlinc;
							}
							if(buttons.ljoy_v < 40)
							{
								if(status > 0) selectCharKB-=nlinc;
							}
							checkButtons = 0;
						}
					#endif
				}
				else if(browseStatus == BROWSE_MAIN)
				{
					if(new_pad & PAD_UP)
					{
						if(browseStatusSel > 1) browseStatusSel--;
					}
					if(new_pad & PAD_DOWN)
					{
						if(browseStatusSel < 4) browseStatusSel++;
					}
					// PAD Analogico
					#ifndef EMULATOR
						if(checkButtons2 > 7)
						{
							if(buttons.ljoy_v > 240)
							{
								if(browseStatusSel < 4) browseStatusSel++;
							}
							if(buttons.ljoy_v < 10)
							{
							   if(browseStatusSel > 1) browseStatusSel--;
							}
							checkButtons2 = 0;
						}
					#endif
					if(new_pad & PAD_CROSS)
					{
						if(browseStatusSel==1)
						{
							sprintf(gPath, "%s", "mc0:/");
							getFioMCDir((char *)gPath);
							browseStatus = BROWSE_MC;
						}
						else if(browseStatusSel==2)
						{
							sprintf(gPath, "%s", "/");
							getMassDir((char *)gPath);
							browseStatus = BROWSE_USB;
						}
						else if(browseStatusSel==3)
						{
							// Manual
							sprintf(gPath, "%s", "");
							gPath[0] = 0;
							browseStatus = BROWSE_HAND;
						}
						else if(browseStatusSel==4)
						{
							// HDD
							#ifdef EMULATOR							
								nCountFiles = 0;
								sprintf(Files[nCountFiles], "%s", "");				TypeFiles[nCountFiles] = 3;	nCountFiles++;
								/*
								sprintf(Files[nCountFiles], "%s", "Eye Toy");		TypeFiles[nCountFiles] = 4;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "Spiderman 2");	TypeFiles[nCountFiles] = 4;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "+DEMOS");		TypeFiles[nCountFiles] = 1;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "PS2OS");			TypeFiles[nCountFiles] = 1;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "Driv3r");		TypeFiles[nCountFiles] = 4;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "GT4 Prologue");	TypeFiles[nCountFiles] = 4;	nCountFiles++;
								*/
								sprintf(Files[nCountFiles], "%s", "DEMO1.ELF"); 	TypeFiles[nCountFiles] = 0;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "PGEN.ELF"); 		TypeFiles[nCountFiles] = 0;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "IPCONFIG.DAT"); 	TypeFiles[nCountFiles] = 0;	nCountFiles++;
								sprintf(Files[nCountFiles], "%s", "PS2KL04.ELF"); 	TypeFiles[nCountFiles] = 0;	nCountFiles++;
								browseStatus = BROWSE_HDD;
							#else
								sprintf(gPath, "%s", "pfs0:/");
								putScreen(233, 200, txt_29);
								loadModulesHDD();
								putScreen(180, 200, txt_38);
								int ret = getRootHDDDir();
								if(ret >= 0)
								{
									browseStatus = BROWSE_HDD;
								}
								else
								{
									browseStatus = BROWSE_MAIN;
								}
							#endif
						}
					}
				}
				else
				{
					if(new_pad & PAD_LEFT)
					{
						nActualEntry=0;
					}
					if(new_pad & PAD_RIGHT)
					{
						if((nActualEntry+5) < (nCountFiles-1)) {
							nActualEntry += 5;
						} else {
							nActualEntry = nCountFiles-1;
						}
					}
					if(new_pad & PAD_UP)
					{
						if(nActualEntry > 0) nActualEntry--;
					}
					if(new_pad & PAD_DOWN)
					{
						if(nActualEntry < (nCountFiles-1)) nActualEntry++;
					}
					// PAD Analogico
					#ifndef EMULATOR
						if(checkButtons2 > 7)
						{
							if(buttons.ljoy_v > 240)
							{
								if(nActualEntry < (nCountFiles-1)) nActualEntry++;
							}
							if(buttons.ljoy_v < 10)
							{
							   if(nActualEntry > 0) nActualEntry--;
							}
							checkButtons2 = 0;
						}
					#endif
					if(new_pad & PAD_CROSS)
					{
						if(TypeFiles[nActualEntry]==3)
						{
							nCountFiles = 0;
							nActualEntry = 0;
							sprintf(gPath, "%s", "");
							browseStatus = BROWSE_MAIN;
						}
						else if(TypeFiles[nActualEntry]==2)
						{
							int i = strlen(gPath)-1;						
							if(i > 0)
							{
								gPath[i] = '\0';
								while(gPath[i] != '/')
								{
									gPath[i]='\0';
									i--;
								}
							}
							else
							{
								gPath[0]='\0';
							}
							
							if(browseStatus == BROWSE_MC)	getFioMCDir((char *)gPath);
							if(browseStatus == BROWSE_USB)	getMassDir((char *)gPath);
							if(browseStatus == BROWSE_HDD)	getHDDDir((char *)gPath);
						}
						else if(TypeFiles[nActualEntry]==1)
						{
							if(browseStatus == BROWSE_MC)
							{
								sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
								getFioMCDir((char *)gPath);
							}
							if(browseStatus == BROWSE_USB)
							{
								sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
								getMassDir((char *)gPath);
							}
							if(browseStatus == BROWSE_HDD)
							{
								if(ROOT_HDD==1) {
									sprintf(hddpart, "%s%s", "hdd0:", Files[nActualEntry]);
									sprintf(gPath, "%s", "/");
								} else {
									sprintf(gPath, "%s%s%s", gPath, Files[nActualEntry], "/");
								}
								getHDDDir((char *)gPath);
							}
						}
						else if(TypeFiles[nActualEntry]==0)
						{
							if(Files[nActualEntry][0] != 0)
							{
								mainStatus = STATUS_SBUTON;
							}
						}
					}
				}
			}
			else if(mainStatus == STATUS_SBUTON)
			{
				if(new_pad & PAD_SQUARE)	setELFToButton(DIRELF_1, 1);
				if(new_pad & PAD_TRIANGLE)	setELFToButton(DIRELF_2, 2);
				if(new_pad & PAD_CIRCLE)	setELFToButton(DIRELF_3, 3);
				if(new_pad & PAD_CROSS)		setELFToButton(DIRELF_4, 4);
				if(new_pad & PAD_R1)		setELFToButton(DIRELF_R1, 5);
				if(new_pad & PAD_R2)		setELFToButton(DIRELF_R2, 6);
				if(new_pad & PAD_R3)	setELFToButton(DIRELF_R3, 7);
				if(new_pad & PAD_L1)		setELFToButton(DIRELF_L1, 8);
				if(new_pad & PAD_L2)		setELFToButton(DIRELF_L2, 9);
				if(new_pad & PAD_L3)		setELFToButton(DIRELF_L3, 10);
			}
			else if(mainStatus == STATUS_SNAME)
			{
				/*
				3 Lineas de:
				0  a 25
				26 a 51
				52 a 77
				*/
				int nlinc = 26;
				
				int status = 0;
				if(selectCharKB >= nlinc) status = 1;
				if(selectCharKB >= (nlinc*2)) status = 2;
				
				if(new_pad & PAD_LEFT)
				{
					selectCharKB--;
					if(status==0) {
						if(selectCharKB < 0) selectCharKB = 25;
					} else if(status==1) {
						if(selectCharKB < 26) selectCharKB = 51;
					} else if(status==2) {
						if(selectCharKB < 52) selectCharKB = 77;
					}
				}
				if(new_pad & PAD_RIGHT)
				{
					selectCharKB++;
					if(status==0) {
						if(selectCharKB > 25) selectCharKB = 0;
					} else if(status==1) {
						if(selectCharKB > 51) selectCharKB = 26;
					} else if(status==2) {
						if(selectCharKB > 77) selectCharKB = 52;
					}
				}
				if(new_pad & PAD_DOWN)
				{
					if(status < 2) selectCharKB+=nlinc;
				}
				if(new_pad & PAD_UP)
				{
					if(status > 0) selectCharKB-=nlinc;
				}
				
				if(new_pad & PAD_CROSS)
				{
					nameEFLKB[nPosKB] = keysKEY[selectCharKB];
					nPosKB++;
					nameEFLKB[nPosKB] = 0;
				}
				if(new_pad & PAD_CIRCLE)
				{
					if(nPosKB > 0)
					{
						nPosKB--;
						nameEFLKB[nPosKB] = 0;
					}
				}
				if(new_pad & PAD_SQUARE)
				{
					nameEFLKB[nPosKB] = ' ';
					nPosKB++;
					nameEFLKB[nPosKB] = 0;
				}
				if(new_pad & PAD_TRIANGLE)
				{
					nameEFLKB[nPosKB] = '\n';
					nPosKB++;
					nameEFLKB[nPosKB] = 0;
					
					if(inEditButton==1)		setTextToButton(DIRELF_1_TXT);
					if(inEditButton==2)		setTextToButton(DIRELF_2_TXT);
					if(inEditButton==3)		setTextToButton(DIRELF_3_TXT);
					if(inEditButton==4)		setTextToButton(DIRELF_4_TXT);
					if(inEditButton==5)		setTextToButton(DIRELF_R1_TXT);
					if(inEditButton==6)		setTextToButton(DIRELF_R2_TXT);
					if(inEditButton==7)		setTextToButton(DIRELF_R3_TXT);
					if(inEditButton==8)		setTextToButton(DIRELF_L1_TXT);
					if(inEditButton==9)		setTextToButton(DIRELF_L2_TXT);
					if(inEditButton==10)	setTextToButton(DIRELF_L3_TXT);
				}
				// PAD Analogico
				#ifndef EMULATOR
					if(checkButtons > 5)
					{
						if(buttons.ljoy_h > 200)
						{
							selectCharKB++;
							if(status==0) {
								if(selectCharKB > 25) selectCharKB = 0;
							} else if(status==1) {
								if(selectCharKB > 51) selectCharKB = 26;
							} else if(status==2) {
								if(selectCharKB > 77) selectCharKB = 52;
							}
						}
						if(buttons.ljoy_h < 50)
						{
							selectCharKB--;
							if(status==0) {
								if(selectCharKB < 0) selectCharKB = 25;
							} else if(status==1) {
								if(selectCharKB < 26) selectCharKB = 51;
							} else if(status==2) {
								if(selectCharKB < 52) selectCharKB = 77;
							}
						}
						if(buttons.ljoy_v > 200)
						{
							if(status < 2) selectCharKB+=nlinc;
						}
						if(buttons.ljoy_v < 40)
						{
							if(status > 0) selectCharKB-=nlinc;
						}
						checkButtons = 0;
					}
				#endif
			}
			
			// START para todos los estados
			if(new_pad & PAD_START)
			{
				autoloadelf = 0;
				if(USB_MODE==0)
				{
					if(mainStatus == STATUS_MAIN)
					{
						// Paramos el MP3
						#if defined(MP3_SOUND)
							//stopMyMP3();
						#endif
						saStat = 1;
						mainStatus = STATUS_SACTION;
					}
					else if(mainStatus == STATUS_SELF)
					{
						if(browseStatus == BROWSE_MAIN)
						{
							mainStatus = STATUS_SACTION;
						}
						else
						{
							browseStatus = BROWSE_MAIN;
						}
						sprintf(gPath, "%s", "");
					}
					else if(mainStatus == STATUS_SBUTON)
					{
						mainStatus = STATUS_SELF;
					}
					else if(mainStatus == STATUS_SACTION)
					{
						mainStatus = STATUS_MAIN;
					}
					else if(mainStatus == STATUS_STIME)
					{
						mainStatus = STATUS_SACTION;
					}
					else if(mainStatus == STATUS_SAUTO)
					{
						mainStatus = STATUS_SACTION;
					}
					else if(mainStatus == STATUS_COPION)
					{
						if(selectHelp==0 && selectMenuInfo==0 && selectDevice==0)
						{
							saStat = 4;
							mainStatus = STATUS_SACTION;
						}
					}
				}
			}
		}
		
		// Autoload
		if(autoloadelf > 0 && count == 0)
		{
			switch(autoloadelf)
			{
				case 1:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_1);	break;
				case 2:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_2);	break;
				case 3:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_3);	break;
				case 4:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_4);	break;
				case 5:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_R1);	break;
				case 6:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_R2);	break;
				case 7:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_R3);	break;
				case 8:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_L1);	break;
				case 9:		copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_L2);	break;
				case 10:	copyCharArray((char *)DIRELF_AUTO, (char *)DIRELF_L3);	break;
			}
			runMyElf(DIRELF_AUTO);
		}
		
		myGsDriver->drawPipe.Flush();
		
		// MP3
		#if defined(MP3_SOUND)
			if(foundMP3 == 1 && fdmp3>=0)
			{
				ReadAndDecodeMP3();
				SendSamplesMP3();
			}
		#endif
			
		// Pintamos la pantalla, ...
		myGsDriver->drawPipe.Flush();
		myGsDriver->WaitForVSync();
		myGsDriver->swapBuffers();
		
	}
}



int RunLoaderElf(char *partition, char *filename)
{
	u8 *boot_elf = (u8 *)&loader;
	elf_header_t *eh = (elf_header_t *)boot_elf;
	elf_pheader_t *eph;
	void *pdata;
	int i;
	char *argv[1];

	/* Load the ELF into RAM.  */
	if (_lw((u32)&eh->ident) != ELF_MAGIC)
	{
		return -1;
	}
	eph = (elf_pheader_t *)(boot_elf + eh->phoff);
	/* Scan through the ELF's program headers and copy them into RAM, then
										zero out any non-loaded regions.  */
	for (i = 0; i < eh->phnum; i++)
	{
		if (eph[i].type != ELF_PT_LOAD)
			continue;

		pdata = (void *)(boot_elf + eph[i].offset);
		memcpy(eph[i].vaddr, pdata, eph[i].filesz);

		if (eph[i].memsz > eph[i].filesz)
			memset((char *)eph[i].vaddr + eph[i].filesz, 0, eph[i].memsz - eph[i].filesz);
	}
	/* Let's go.  */
	fioExit();
	SifInitRpc(0);
	SifExitRpc();
	FlushCache(0);
	FlushCache(2);

	argv[0] = filename;
	argv[1] = partition; //party[parties].filename;
	ExecPS2((void *)eh->entry, 0, 2, argv);
	return 0;
	
}

int deleteDir(char *szFile, int mode)
{
	if(mode==1)
	{
		int ret = fioRmdir(szFile);
		return ret;
	}
	else if(mode==2)
	{
		fileXioMount("pfs0:", hddpart, FIO_MT_RDWR);
		fileXioRmdir(szFile);
		fileXioUmount("pfs0:");
		return 0;
	}
	return -1;
}

int deleteFile(char *szFile, int mode)
{
	if(mode==1)
	{
		int ret = fioRemove(szFile);
		fioRmdir(szFile);
		return ret;
	}
	else if(mode==2)
	{
		fileXioMount("pfs0:", hddpart, FIO_MT_RDWR);
		fileXioRemove(szFile);
		fileXioRmdir(szFile);
		fileXioUmount("pfs0:");
		return 0;
	}
	return -1;
}

char *readFile(char *szFile, int mode)
{
	int fd=0;
	char* buffer;
	int size=0;

	if(mode==1) //FIO
	{
		int fd = fioOpen(szFile, O_RDONLY);
		if(fd > 0) {
			size = fioLseek(fd, 0, SEEK_END);
			fioLseek(fd, 0, SEEK_SET);
			buffer = (char *)malloc(size);
			fioRead(fd, buffer, size);
			fioClose(fd);
		} else {
			return NULL;
		}
	}
	else if(mode==2) //FILEXIO
	{
		fileXioMount("pfs0:", hddpart, FIO_MT_RDONLY);
		if((fd = fileXioOpen(szFile, O_RDONLY, fileMode)) < 0) {
			return NULL;
		}
		size = fileXioLseek(fd, 0, SEEK_END);
		buffer = (char *)malloc(size);
		fileXioLseek(fd, 0, 0);
		fileXioRead(fd, (unsigned char *)buffer, size);
		fileXioClose(fd);
		fileXioUmount("pfs0:");
	}
	sizeForCopy = size;
	return buffer;
}

int writeFile(char *szFile, char *file, int mode)
{
	int fd;
	
	if(mode==1) //FIO
	{
		fd = fioOpen(szFile, O_WRONLY | O_CREAT);
		if(fd > 0) {
			fioWrite(fd, file, sizeForCopy);
			fioClose(fd);
		} else {
			return -1;
		}
	}
	else if(mode==2) //FILEXIO
	{
		fileXioMount("pfs0:", hddpart, FIO_MT_RDWR);
		if((fd = fileXioOpen(szFile, O_WRONLY | O_TRUNC | O_CREAT, fileMode)) < 0) {
			return -1;
		}
		fileXioWrite(fd, (unsigned char *)file, sizeForCopy);
		fileXioClose(fd);
		fileXioUmount("pfs0:");
	}
	sizeForCopy = 0;
	return 0;
}

void refreshDir(int panel)
{
	putMessageScreen(261, 195, txt_62);
	if(panel==PANEL_LEFT)
	{
		if(nLeftDevice==DEVICE_MC0 || nLeftDevice==DEVICE_MC1) {
			getFioMCDir((char *)gPathLeft);
		} else if(nLeftDevice==DEVICE_USB) {
			getMassDir((char *)gPathLeft);
		} else if(nLeftDevice==DEVICE_HDD) {
			getHDDDir((char *)gPathLeft);
		} else if(nLeftDevice==DEVICE_CDVD) {
			readCDDir((char *)gPathLeft);
		}
		copyCharArray2(LeftFiles, Files);
		copyCharArrayN((char *)LeftTypeFiles, (char *)TypeFiles);
		nCountLeftFiles = nCountFiles;
		if(nRightActualEntry > (nCountLeftFiles - 1))
		{
			nLeftActualEntry = (nCountLeftFiles - 1);
		}
	}
	else if(panel==PANEL_RIGHT)
	{
		if(nRightDevice==DEVICE_MC0 || nRightDevice==DEVICE_MC1) {
			getFioMCDir((char *)gPathRight);
		} else if(nRightDevice==DEVICE_USB) {
			getMassDir((char *)gPathRight);
		} else if(nRightDevice==DEVICE_HDD) {
			getHDDDir((char *)gPathRight);
		} else if(nRightDevice==DEVICE_CDVD) {
			readCDDir((char *)gPathRight);
		}
		copyCharArray2(RightFiles, Files);
		copyCharArrayN((char *)RightTypeFiles, (char *)TypeFiles);
		nCountRightFiles = nCountFiles;
		if(nRightActualEntry > (nCountRightFiles - 1))
		{
			nRightActualEntry = (nCountRightFiles - 1);
		}
	}
}


void copyFromPanels()
{
	char szFileFrom[256];
	char szFileTo[256];
	int fromDevice = -1;
	int toDevice = -1;
	
	if(selectPanelCopy==PANEL_LEFT) {
		sprintf(szFileFrom, "%s%s", gPathLeft, LeftFiles[nLeftActualEntry]);
		sprintf(szFileTo, "%s%s", gPathRight, LeftFiles[nLeftActualEntry]);
		fromDevice = nLeftDevice;
		toDevice = nRightDevice;
	} else {
		sprintf(szFileFrom, "%s%s", gPathRight, RightFiles[nRightActualEntry]);
		sprintf(szFileTo, "%s%s", gPathLeft, RightFiles[nRightActualEntry]);
		fromDevice = nRightDevice;
		toDevice = nLeftDevice;
	}
						
	char* buffer;
	int fmode = 1;
	int tmode = 1;
	
	if(fromDevice==DEVICE_USB) {
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "mass:", szFileFrom);
		copyCharArray((char *)szFileFrom, (char *)tmpPath);
	} else if(fromDevice==DEVICE_HDD) {
		fmode = 2;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "pfs0:", szFileFrom);
		copyCharArray((char *)szFileFrom, (char *)tmpPath);
	} else if(fromDevice==DEVICE_CDVD) {
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "cdfs:/", szFileFrom);
		copyCharArray((char *)szFileFrom, (char *)tmpPath);
	}

	if(toDevice==DEVICE_USB)
	{
		tmode = 1;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "mass:", szFileTo);
		copyCharArray((char *)szFileTo, (char *)tmpPath);

	}
	else if(toDevice==DEVICE_HDD)
	{
		tmode = 2;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "pfs0:", szFileTo);
		copyCharArray((char *)szFileTo, (char *)tmpPath);
	}

	if(toDevice==DEVICE_CDVD)
	{
		if(toDevice==DEVICE_CDVD) sprintf(txtError, txt_64);
		while(1)
		{
			if(readPad())
			{
				putScreen(200, 200, txtError);
				if(new_pad & PAD_CROSS) break;
			}
		}
	}
	else
	{
		putMessageScreen(261, 195, "Leyendo...");	
		buffer = readFile(szFileFrom, fmode);
		if(buffer == NULL)
		{
			sprintf(txtError, "%s%s", txt_65, szFileFrom);
			while(1)
			{
				if(readPad())
				{
					putScreen(133, 200, txtError);
					if(new_pad & PAD_CROSS) break;
				}
			}
		}
		else
		{
			putMessageScreen(261, 195, txt_66);
			int wrt = writeFile(szFileTo, buffer, tmode);
			if(wrt < 0)
			{
				sprintf(txtError, "%s%s", txt_65, szFileTo);
				while(1)
				{
					if(readPad())
					{
						putScreen(133, 200, txtError);
						if(new_pad & PAD_CROSS) break;
					}
				}
			}
		}
	}
}


void deleteFromPanels()
{
	char szFile[256];
	char szMoreFile[256];
	int mode = 1;
	int rDevice = 0;

	if(selectPanelCopy==1) {
		rDevice = nLeftDevice;
	} else {
		rDevice = nRightDevice;
	}

	putMessageScreen(261, 195, txt_67);

	if(rDevice==DEVICE_USB)
	{
		mode = 1;
		sprintf(szMoreFile, "%s", "mass:");
	}
	else if(rDevice==DEVICE_HDD)
	{
		mode = 2;
		sprintf(szMoreFile, "%s", "pfs0:");
	}
	else
	{
		mode = 1;
		sprintf(szMoreFile, "%s", "\0");		
	}
	

	if(rDevice==DEVICE_USB || rDevice==DEVICE_HDD)
	{
		if(selectPanelCopy==1) {
			sprintf(szFile, "%s%s%s", szMoreFile, gPathLeft, LeftFiles[nLeftActualEntry]);
		} else {
			sprintf(szFile, "%s%s%s", szMoreFile, gPathRight, RightFiles[nRightActualEntry]);
		}
	}
	else
	{
		if(selectPanelCopy==1) {
			sprintf(szFile, "%s%s", gPathLeft, LeftFiles[nLeftActualEntry]);
		} else {
			sprintf(szFile, "%s%s", gPathRight, RightFiles[nRightActualEntry]);
		}
	}
	
	if(rDevice==DEVICE_HDD)
	{
		mode = 2;
	}
	if(deleteFile(szFile, mode) < 0)
	{
		while(1)
		{
			sprintf(txtError, "%s%s", txt_68, szFile);
			if(readPad())
			{
				putScreen(133, 200, txtError);
				if(new_pad & PAD_CROSS) break;
			}
		}
	}
}

void deleteDirFromPanels()
{
	char szFile[256];
	char szMoreFile[256];
	int mode = 1;
	int rDevice = 0;

	if(selectPanelCopy==1) {
		rDevice = nLeftDevice;
	} else {
		rDevice = nRightDevice;
	}

	if(rDevice==DEVICE_USB)
	{
		mode = 1;
		sprintf(szMoreFile, "%s", "mass0:");
	}
	else if(rDevice==DEVICE_HDD)
	{
		mode = 2;
		sprintf(szMoreFile, "%s", "pfs0:");
	}
	else
	{
		mode = 1;
		sprintf(szMoreFile, "%s", "\0");
	}
	
	putMessageScreen(261, 195, txt_67);
	
	
	if(rDevice==DEVICE_USB || rDevice==DEVICE_HDD)
	{
		if(selectPanelCopy==1) {
			sprintf(szFile, "%s%s%s", szMoreFile, gPathLeft, LeftFiles[nLeftActualEntry]);
		} else {
			sprintf(szFile, "%s%s%s", szMoreFile, gPathRight, RightFiles[nRightActualEntry]);
		}
	}
	else
	{
		if(selectPanelCopy==1) {
			sprintf(szFile, "%s%s", gPathLeft, LeftFiles[nLeftActualEntry]);
		} else {
			sprintf(szFile, "%s%s", gPathRight, RightFiles[nRightActualEntry]);
		}
	}
	
	if(deleteDir(szFile, mode) < 0)
	{
		while(1)
		{
			sprintf(txtError, "%s%s", txt_68, szFile);
			if(readPad())
			{
				putScreen(133, 200, txtError);
				if(new_pad & PAD_CROSS) break;
			}
		}
	}
}


void copyCharArray(char to[], char from[])
{
	int i = 0;
	while((to[i] = from[i]) != '\0')
	{
		i++;
	}
}

void copyCharArrayN(char to[], char from[])
{
	int i = 0;
	for(i=0; i<MAX_ENTRY; i++)
	{
		to[i] = from[i];
	}
}


void copyCharArray2(char to[MAX_ENTRY][MAX_PATH], char from[MAX_ENTRY][MAX_PATH])
{
	int i = 0;
	int n = 0;
	for(i=0; i<MAX_ENTRY; i++)
	{
		for(n=0; n<MAX_PATH; n++)
		{
			to[i][n] = from[i][n];
		}
	}
}

/*
void copyRecursiveFromPanels()
{
	char szDirFo[256];
	char szDirTo[256];
	int foDevice = -1;
	int toDevice = -1;
	char* buffer;
	int fmode = 1;
	int tmode = 1;
	
	if(selectPanelCopy==PANEL_LEFT) {
		sprintf(szDirFo, "%s%s", gPathLeft, LeftFiles[nLeftActualEntry]);
		sprintf(szDirTo, "%s%s", gPathRight, LeftFiles[nLeftActualEntry]);
		foDevice = nLeftDevice;
		toDevice = nRightDevice;
	} else {
		sprintf(szDirFo, "%s%s", gPathRight, RightFiles[nRightActualEntry]);
		sprintf(szDirTo, "%s%s", gPathLeft, RightFiles[nRightActualEntry]);
		foDevice = nRightDevice;
		toDevice = nLeftDevice;
	}
	
	if(foDevice==3) {
		fmode = 3;
		//char tmpPath[256];
		//sprintf(tmpPath, "%s%s", "mass:", szDirFo);
		//copyCharArray((char *)szDirFo, (char *)tmpPath);
	} else if(foDevice==4) {
		fmode = 2;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "pfs0:", szDirFo);
		copyCharArray((char *)szDirFo, (char *)tmpPath);
	} else if(foDevice==5) {
		fmode = 1;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "cdfs:/", szDirFo);
		copyCharArray((char *)szDirFo, (char *)tmpPath);
	}

	if(toDevice==3)
	{
		tmode = 1;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "mass:", szDirTo);
		copyCharArray((char *)szDirTo, (char *)tmpPath);

	}
	else if(toDevice==4)
	{
		tmode = 2;
		char tmpPath[256];
		sprintf(tmpPath, "%s%s", "pfs0:", szDirTo);
		copyCharArray((char *)szDirTo, (char *)tmpPath);
	}

	if(toDevice==5)
	{
		if(toDevice==5) sprintf(txtError, txt_64);
		while(1)
		{
			if(readPad())
			{
				putScreen(200, 200, txtError);
				if(new_pad & PAD_CROSS) break;
			}
		}
	}
	else
	{	
		putMessageScreen(261, 195, "Copiando...");
		recursiveCopy((char *)szDirFo, (char *)szDirTo, fmode, tmode);
	}
}

void recursiveCopy(char *folder, char *destination, int fmode, int tmode)
{
	char *buffer;
	printf("---------------------------------------------\n");
	printf("Copia recursiva de %s a %s\n",folder,destination);
	printf("---------------------------------------------\n");

	if(fmode==1) // FIO
	{
		int rv, fd=0;
		fio_dirent_t hostbuf;
		
		fd = fioDopen(folder);
		if(fd > 0)
		{
			int ret = fioMkdir(destination);
			printf("ret = %d\n", ret);
			
			while((rv=fioDread(fd, &hostbuf)))
			{			
				if(hostbuf.stat.mode & 0x0010 && strncmp((char *)&hostbuf.name,".",1))
				{
					//File
					printf("FILE:%s\n", hostbuf.name);
					char fileFCopy[256];
					char fileTCopy[256];
					sprintf(fileFCopy, "%s%s%s", folder, "/", hostbuf.name);
					sprintf(fileTCopy, "%s%s%s", destination, "/", hostbuf.name);
					
					printf("Copiamos %s a %s\n", fileFCopy, fileTCopy);
					
					buffer = readFile(fileFCopy, 1);
					if(buffer != NULL)
					{
						writeFile(fileTCopy, buffer, 1);
					}
					
					
				}
				else if(hostbuf.stat.mode & 0x0020 && strncmp((char *)&hostbuf.name,".",1))
				{
					//Dir
					printf("-----------------------------------\n");
					printf("DIR:%s\n", hostbuf.name);
					printf("-----------------------------------\n");
					char newFDir[256];
					char newTDir[256];
					sprintf(newFDir, "%s%s%s", folder, "/", hostbuf.name);
					sprintf(newTDir, "%s%s%s", destination, "/", hostbuf.name);
					recursiveCopy(newFDir, newTDir, fmode, tmode);
				}
			}
			fioDclose(fd);
		}
	}
	else if(fmode==2) //FILEXIO
	{
	}
	else if(fmode==3) //MASS
	{
		int ret;
		fat_dir_record record;
		
		ret = fioMkdir(destination);
		ret = usb_mass_getFirstDirentry(folder, &record);
		
		while(ret > 0 )
		{
			if(((record.attr & 0x10) == 0x10) && (record.name[0] != '.'))
			{
				// Dir
				printf("-----------------------------------\n");
				printf("DIR:%s\n", record.name);
				printf("-----------------------------------\n");
				char newFDir[256];
				char newTDir[256];
				sprintf(newFDir, "%s%s%s", folder, "/", record.name);
				sprintf(newTDir, "%s%s%s", destination, "/", record.name);
				recursiveCopy(newFDir, newTDir, fmode, tmode);
			}
			else if((record.attr & 0x08) == 0x08)
			{
				// Volumen
			}
			else if(record.name[0] != '.')
			{
				// File
				printf("FILE:%s\n", record.name);
				char fileFCopy[256];
				char fileTCopy[256];
				sprintf(fileFCopy, "%s%s%s%s", "mass:", folder, "/", record.name);
				sprintf(fileTCopy, "%s%s%s", destination, "/", record.name);
				printf("Copiamos %s a %s\n", fileFCopy, fileTCopy);
				buffer = readFile(fileFCopy, 1);
				if(buffer != NULL)
				{
					writeFile(fileTCopy, buffer, 1);
				}
			} 
			ret = usb_mass_getNextDirentry(&record);
		}
	}
	else if(fmode==4) //CDVD
	{
	}
}
*/

//Extracts an nPort npo file to mc0:/
int npoExtract(char *filename)
{
	
	npoHeader *npofile;
	int fd, size=0, nfile,i;
	char npocontent[48];
	u32 nposize;
	unsigned char *npoptr;
	char *strip;
	char fullpath[256];
	
	if((fd = fioOpen(filename, O_RDONLY)) < 0)
	{
		goto error;
	}
	size = fioLseek(fd, 0, SEEK_END);
	if (!size)
	{
		fioClose(fd);
		goto error;
	}
	fioLseek(fd, 0, SEEK_SET);
	npofile = (npoHeader *)malloc(size);
	fioRead(fd, npofile, size);
	fioClose(fd);
	
	npoptr = (unsigned char *)npofile + 24;
	size -= 24;
	nfile = 1;
	
	if(!strncmp((const char*)npofile->nPort,"nPort",5))
	{
		strip = strrchr(filename, '/');
		if(strip == NULL)
		{
			strip = strrchr(filename, '\\');
			if (strip == NULL)
			{
				strip = strrchr(filename, ':');
			}
		}
		strip++;
		strcpy(npocontent, strip);
		i = strlen(npocontent);
		npocontent[i-4] = '\0';
		
		sprintf(fullpath, "%s%s", "mc0:/", npocontent);
		fioMkdir(fullpath);
		while(size > 56)
		{
			strcpy(npocontent,(char *)npoptr);
			npoptr+=48;
			size-=48;
			nposize=*npoptr++;
			nposize+=(*npoptr++)*256;
			nposize+=(*npoptr++)*65536;
			npoptr++;
			npoptr+=4;
			size-=8;
			if(nposize)
			{
				char newFile[256];
				sprintf(newFile, "%s%s%s", fullpath, "/", npocontent);	
				fd = fioOpen(newFile, O_WRONLY | O_CREAT);
				if(fd>=0)
				{
					fioWrite(fd, npoptr, nposize);
					fioClose(fd);
				}
			}
			else
			{
				if(strlen(npocontent))
				{
					char newDir[256];
					sprintf(newDir, "%s%s%s", fullpath, "/", npocontent);	
					fioMkdir(newDir);
				}
			}
			npoptr += nposize;
			size -= nposize;
			nfile++;
		}
	}
	free(npofile);
	return 0;
	
error:
	return -1;
}

#if defined(MP3_SOUND)
	int dummy()
	{
		double p = pow(0, 0);
		float t = tan(0);
		float s = sin(0);
		return 0;
	}

	void playMyMP3()
	{
		fdmp3 = OpenMP3(kl_mp3);
		if(fdmp3 >= 0)
		{
			RepeatMP3(fdmp3, 1);
			PCMVolume(volumen, volumen); // volumen a la mitad
		}
		else
		{
			foundMP3 = 0;
		}
	}
	
	void stopMyMP3()
	{
		fdmp3 = -1;
		CloseMP3(fdmp3);
	}
#endif

	
void keyBoard(char *returnText)
{
	int w2 = 520;
	int h2 = 120;
	int j1=64;
	int j2=164; //y?
	int n = 0;
	
	returnText[0] = '\0';
	int nPosKB2 = 0;
	int selectCharKB2 = 0;
	int checkButtons = 0;
	
	while(1)
	{
		myGsDriver->drawPipe.RectFlat(50,120,596,372,0,colorGris);
		myGsDriver->drawPipe.RectFlat(55,125,591,367,0,colorBlanco);
		
		myGsDriver->drawPipe.RectFlat(65, 137, 583, 157, 0, GS_SET_RGBA(0x74,0x74,0x74,0x80));
		myGsDriver->drawPipe.RectFlat(64, 136, 582, 156, 0, colorAzul);
		myGsDriver->drawPipe.RectFlat(66, 138, 580, 154, 0, colorBlanco);
		myFont.Print(68, tvmx, 138, 1, colorNegro, GSFONT_ALIGN_LEFT, (char *)returnText);
		kbx1 = 70;
		kby1 = 170;
		
		// Lineas
		j1=64;
		j2=164;
		for(j1=64; j1<604; j1+=20)
		{
			myGsDriver->drawPipe.Line(j1,j2,j1,j2+h2,0,colorGris2);
		}
		j1=64;
		j2=164;
		for(j2=164; j2<=284; j2+=40)
		{
			myGsDriver->drawPipe.Line(j1,j2,j1+w2,j2,0,colorGris2);
		}
		
		// Teclas
		int nC = 0;
		for(n=0; n<78; n++)
		{
			if(selectCharKB2 == n)
			{
				myGsDriver->drawPipe.RectFlat(kbx1-5, kby1-5, kbx1+14, kby1+34, 1, colorBlanco);
				myGsDriver->drawPipe.RectFlat(kbx1-5, kby1-5, kbx1+14, kby1+34, 2, color_back);
			}
			
			m_putcharG(kbx1, kby1, colorNegro, keysKEY[n]);
			kbx1 += 20;
			nC++;
			if(nC > 25)
			{
				nC=0;
				kby1 += 40;
				kbx1 = 70;
			}
		}
		
		// Textos botones
		myFont.Print(105, tvmx, 331, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_18);
		myFont.Print(220, tvmx, 331, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_19);
		myFont.Print(305, tvmx, 331, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_20);
		myFont.Print(395, tvmx, 331, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_71);
		myFont.Print(485, tvmx, 331, 1, colorNegro, GSFONT_ALIGN_LEFT, txt_72);
		
		// Graficos botones
		putImage(img_cruz,		80,		327);
		putImage(img_circulo,	194,	327);
		putImage(img_cuadrado,	277,	327);
		putImage(img_triangulo,	369,	327);
		putImage(img_R1,		450,	330);
		
		if(readPad())
		{
			checkButtons++;  // Analogico
			
			int nlinc = 26;
			int status = 0;
			if(selectCharKB2 >= nlinc) status = 1;
			if(selectCharKB2 >= (nlinc*2)) status = 2;
			
			if(new_pad & PAD_LEFT)
			{
				selectCharKB2--;
				if(status==0) {
					if(selectCharKB2 < 0) selectCharKB2 = 25;
				} else if(status==1) {
					if(selectCharKB2 < 26) selectCharKB2 = 51;
				} else if(status==2) {
					if(selectCharKB2 < 52) selectCharKB2 = 77;
				}
			}
			if(new_pad & PAD_RIGHT)
			{
				selectCharKB2++;
				if(status==0) {
					if(selectCharKB2 > 25) selectCharKB2 = 0;
				} else if(status==1) {
					if(selectCharKB2 > 51) selectCharKB2 = 26;
				} else if(status==2) {
					if(selectCharKB2 > 77) selectCharKB2 = 52;
				}
			}
			if(new_pad & PAD_DOWN)
			{
				if(status < 2) selectCharKB2+=nlinc;
			}
			if(new_pad & PAD_UP)
			{
				if(status > 0) selectCharKB2-=nlinc;
			}
			
			if(new_pad & PAD_CROSS)
			{
				returnText[nPosKB2] = keysKEY[selectCharKB2];
				nPosKB2++;
				returnText[nPosKB2] = 0;
			}
			if(new_pad & PAD_CIRCLE)
			{
				if(nPosKB2 > 0)
				{
					nPosKB2--;
					returnText[nPosKB2] = 0;
				}
			}
			if(new_pad & PAD_SQUARE)
			{
				returnText[nPosKB2] = ' ';
				nPosKB2++;
				returnText[nPosKB2] = 0;
			}
			if(new_pad & PAD_TRIANGLE)
			{
				break;
			}
			if(new_pad & PAD_R1)
			{
				returnText[0] = '\0';
				break;
			}
			// PAD Analogico
			#ifndef EMULATOR
				if(checkButtons > 5)
				{
					if(buttons.ljoy_h > 200)
					{
						selectCharKB2++;
						if(status==0) {
							if(selectCharKB2 > 25) selectCharKB2 = 0;
						} else if(status==1) {
							if(selectCharKB2 > 51) selectCharKB2 = 26;
						} else if(status==2) {
							if(selectCharKB2 > 77) selectCharKB2 = 52;
						}
					}
					if(buttons.ljoy_h < 50)
					{
						selectCharKB2--;
						if(status==0) {
							if(selectCharKB2 < 0) selectCharKB2 = 25;
						} else if(status==1) {
							if(selectCharKB2 < 26) selectCharKB2 = 51;
						} else if(status==2) {
							if(selectCharKB2 < 52) selectCharKB2 = 77;
						}
					}
					if(buttons.ljoy_v > 200)
					{
						if(status < 2) selectCharKB2+=nlinc;
					}
					if(buttons.ljoy_v < 40)
					{
						if(status > 0) selectCharKB2-=nlinc;
					}
					checkButtons = 0;
				}
			#endif
		}
		
		// MP3
		#if defined(MP3_SOUND)
			if(foundMP3 == 1 && fdmp3>=0)
			{
				ReadAndDecodeMP3();
				SendSamplesMP3();
			}
		#endif
		
		// Pintamos la pantalla, ...
		myGsDriver->drawPipe.Flush();
		myGsDriver->WaitForVSync();
		myGsDriver->swapBuffers();
	}
}

int createDirFromPanels(char *name)
{
	char szFile[256];
	char szPath[256];
	int mode = 1;
	int rDevice = 0;

	if(selectPanelCopy==1) {
		copyCharArray(szPath, gPathLeft);
		rDevice = nLeftDevice;
	} else {
		copyCharArray(szPath, gPathRight);
		rDevice = nRightDevice;
	}

	if(rDevice==DEVICE_MC0 || rDevice==DEVICE_MC1)
	{
		mode = 1;
		sprintf(szFile, "%s%s", szPath, name);
	}
	if(rDevice==DEVICE_USB)
	{
		mode = 1;
		sprintf(szFile, "%s%s%s", "mass:", szPath, name);
	}
	else if(rDevice==DEVICE_HDD)
	{
		mode = 2;
		sprintf(szFile, "%s%s%s", "pfs0:", szPath, name);
	}
	else if(rDevice==DEVICE_CDVD)
	{
		while(1)
		{
			if(readPad())
			{
				putScreen(133, 200, txt_74);
				if(new_pad & PAD_CROSS) break;
			}
		}
		return -1;
	}
	
	return createDir(szFile, mode);
}

int createDir(char *szFile, int mode)
{	
	if(mode==1)
	{
		return fioMkdir(szFile);
	}
	else if(mode==2)
	{
		fileXioMount("pfs0:", hddpart, FIO_MT_RDWR);
		fileXioMkdir(szFile, fileMode);
		fileXioUmount("pfs0:");
		return 0;
	}
	return -1;
}
