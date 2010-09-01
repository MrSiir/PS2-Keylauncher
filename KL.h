/*
  _____     ___ ____
   ____|   |    ____|      MrSiir / SiirLabs
  |     ___|   |____       (C)2005, MrSiir (mrsiir@gmail.com)
  ------------------------------------------------------------------------
  KL.h
                        PS2 Key Launcher 0.5.4
  ------------------------------------------------------------------------
    
  * Para compilar este programa se necesita la ps2sdk y gslib 0.5.1
  
  ------------------------------------------------------------------------
*/
#ifndef __INCLUDE_KL_H__
#define __INCLUDE_KL_H__

#define MP3_SOUND		1
//#define EMULATOR		1
//#define DEBUG			1
#define USB_USE			1
#define DEVELOP         1

// Estados
#define STATUS_MAIN		0
#define STATUS_SELF		1
#define STATUS_SBUTON	2
#define STATUS_SNAME	3
#define STATUS_SACTION	4
#define STATUS_SAUTO	5
#define STATUS_STIME	6
#define STATUS_COPION	7
#define STATUS_CSL	    8

#define BROWSE_MAIN	10
#define BROWSE_MC	11
#define BROWSE_USB	12
#define BROWSE_HAND	13
#define BROWSE_HDD	14

#define DEVICE_MC0	1
#define DEVICE_MC1	1
#define DEVICE_USB	1
#define DEVICE_HDD	1
#define DEVICE_CDVD	1

//------------------------------------------------
// Paths y Ficheros
//------------------------------------------------
char kl_dat[256];
char kl_usb_dat[] = "mass:/KL/KL.DAT";
char kl_icon_sys[256];
char kl_icon_icn[256];
char kl_mp3[256];
//------------------------------------------------
char kl_dat_file[7] = "KL.DAT";
char kl_icon_sys_file[9] = "icon.sys";
char kl_icon_icn_file[7] = "kl.icn";
char kl_mp3_file[7] = "KL.MP3";
//------------------------------------------------

// Graficos
extern char binary_logo_start[];
extern char binary_mrsiir_start[];
extern char binary_cuadrado_start[];
extern char binary_triangulo_start[];
extern char binary_circulo_start[];
extern char binary_cruz_start[];
extern char binary_fileelf_start[];
extern char binary_folder_start[];
extern char binary_fondo_start[];
extern char binary_usb_start[];
extern char binary_L1_start[];
extern char binary_L2_start[];
extern char binary_L3_start[];
extern char binary_R1_start[];
extern char binary_R2_start[];
extern char binary_R3_start[];
extern char binary_icomc_start[];
extern char binary_icomano_start[];
extern char binary_icousb_start[];
extern char binary_icohdd_start[];
extern char binary_folderup_start[];
extern char binary_fileold_start[];
extern char binary_npo_start[];

// IRX
extern u8 *NPMUSBD;
extern int size_NPMUSBD;
extern u8 *USBMASS;
extern int size_USBMASS;

// MP3
#if defined(MP3_SOUND)
	extern u8 *audio;
	extern int size_audio;
#endif

// HDD
extern u8 *filexio;
extern u8 *iomanx;
extern u8 *ps2atad;
extern u8 *ps2dev9;
extern u8 *ps2fs;
extern u8 *ps2hdd;
extern u8 *loader;
extern u8 *CDVD;

extern int size_filexio;
extern int size_iomanx;
extern int size_ps2atad;
extern int size_ps2dev9;
extern int size_ps2fs;
extern int size_ps2hdd;
extern int size_loader;
extern int size_CDVD;

// Icono
extern char binary_klicn_start[];

// Fuente
extern char binary_arial_start[];

unsigned char *charbuffer = (unsigned char *)0x800000;
int autoloadelf = 0;
int bautoloadelf = 0;
unsigned int id_va_handler = 0;

// ELFs
unsigned char DIRELF_1[256];  // Cuadrado
unsigned char DIRELF_2[256];  // Triangulo
unsigned char DIRELF_3[256];  // Circulo
unsigned char DIRELF_4[256];  // Cruz
unsigned char DIRELF_R1[256]; // etc...
unsigned char DIRELF_R2[256];
unsigned char DIRELF_R3[256];
unsigned char DIRELF_L1[256];
unsigned char DIRELF_L2[256];
unsigned char DIRELF_L3[256];
unsigned char DIRELF_AUTO[256];

unsigned char TIME_AUTO[2];

// Nombres
unsigned char DIRELF_1_TXT[256];  // Cuadrado
unsigned char DIRELF_2_TXT[256];  // Triangulo
unsigned char DIRELF_3_TXT[256];  // Circulo
unsigned char DIRELF_4_TXT[256];  // Cruz
unsigned char DIRELF_R1_TXT[256]; // etc...
unsigned char DIRELF_R2_TXT[256];
unsigned char DIRELF_R3_TXT[256];
unsigned char DIRELF_L1_TXT[256];
unsigned char DIRELF_L2_TXT[256];
unsigned char DIRELF_L3_TXT[256];

// Colores
unsigned colorVerde	= GS_SET_RGBA(0x08,0xBC,0x08,0x80);
unsigned colorAzul 	= GS_SET_RGBA(0x0B,0x1B,0x7A,0x80);
unsigned colorAzul2 	= GS_SET_RGBA(0x0B,0x1B,0x7A,0x40);
unsigned colorRojo 	= GS_SET_RGBA(0xFF,0x00,0x00,0x80);
unsigned colorRojo2 	= GS_SET_RGBA(0xFF,0x00,0x00,0x40);
unsigned colorRojo3 	= GS_SET_RGBA(0xEC,0x06,0x06,0x80);
unsigned colorNegro 	= GS_SET_RGBA(0x00,0x00,0x00,0x80);
unsigned colorBlanco 	= GS_SET_RGBA(0xFF,0xFF,0xFF,0x80);
unsigned colorBlanco2 	= GS_SET_RGBA(0xFF,0xFF,0xFF,0x80);
unsigned colorAmarillo 	= GS_SET_RGBA(0xFF,0xFF,0x00,0x80);
unsigned colorAmarillo2 = GS_SET_RGBA(0xFF,0xFF,0x00,0x40);
unsigned colorAmarillo3 = GS_SET_RGBA(0xFF,0xFF,0x00,0x20);
unsigned colorGris 	    = GS_SET_RGBA(0xBB,0xBB,0xBB,0x80);
unsigned colorGris2 	= GS_SET_RGBA(0xC0,0xC0,0xC0,0x40);
unsigned colorGris3 	= GS_SET_RGBA(0xC0,0xC0,0xC0,0x80);
unsigned text_color1 	= GS_SET_RGBA(0x00,0x00,0x00,0x80);
unsigned text_color2 	= GS_SET_RGBA(0xFF,0xFF,0xFF,0x80);
unsigned color_back 	= GS_SET_RGBA(0x00,0x00,0xFF,0x30);
unsigned colorNaranja 	= GS_SET_RGBA(0xFF,0x3D,0x00,0x80);

#define MAX_ENTRY	1024
#define MAX_PATH	64
#define MAX_ENTRY_PAGE	11
#define MAX_ENTRY_PAGE2	15
#define MC_MAX_PATH	1025
#define MAX_CD_ENTRY	2048

// gsLib
gsDriver* myGsDriver;
gsFontTex* fontTex;
gsFont myFont;

// Las imagenes
IIF* img_logo = (IIF*)binary_logo_start;
IIF* img_mrsiir = (IIF*)binary_mrsiir_start;
IIF* img_cuadrado = (IIF*)binary_cuadrado_start;
IIF* img_circulo = (IIF*)binary_circulo_start;
IIF* img_triangulo = (IIF*)binary_triangulo_start;
IIF* img_cruz = (IIF*)binary_cruz_start;
IIF* img_fileelf = (IIF*)binary_fileelf_start;
IIF* img_folder = (IIF*)binary_folder_start;
IIF* img_fondo = (IIF*)binary_fondo_start;
IIF* img_usb = (IIF*)binary_usb_start;

IIF* img_L1 = (IIF*)binary_L1_start;
IIF* img_L2 = (IIF*)binary_L2_start;
IIF* img_L3 = (IIF*)binary_L3_start;
IIF* img_R1 = (IIF*)binary_R1_start;
IIF* img_R2 = (IIF*)binary_R2_start;
IIF* img_R3 = (IIF*)binary_R3_start;
IIF* img_icomc = (IIF*)binary_icomc_start;
IIF* img_icousb = (IIF*)binary_icousb_start;
IIF* img_icohdd = (IIF*)binary_icohdd_start;
IIF* img_icomano = (IIF*)binary_icomano_start;
IIF* img_folderup = (IIF*)binary_folderup_start;
IIF* img_fileold = (IIF*)binary_fileold_start;
IIF* img_npo = (IIF*)binary_npo_start;

// TV
int tvmx = 640;
int tvmy = 512;
int tvx = 170;
int tvy = 80;

// USB
int HAVE_USB = 0;
int USB_MODE = 0;

char nameEFLKB[256];
char nameNumKB[256];
int nPosKB = 0;
int nPosNumKB = 0;
char selectCharKB = 0;
int selectNumKB = 0;
char selectNumCharKB = 0;
int inEditButton = -1;
int mainStatus = STATUS_MAIN; //STATUS_MAIN;
int browseStatus = BROWSE_MAIN; //BROWSE_MAIN;
int browseStatusSel = 1;

char Files[MAX_ENTRY][MAX_PATH] __attribute__((aligned(64)));
int TypeFiles[MAX_ENTRY] __attribute__((aligned(64)));
int nCountFiles = 0;
char gPath[256];
int nCountEntryInPage = 0;
int nActualEntry = 0;
int nStartEntry = 0;
int saStat = 1;
int HDD_IRX_LOAD = 0;
int ROOT_HDD = 0;
int READ_HDD = 0;
char hddpart[256];
#define MAX_PARTITIONS	100

char HDD_Files[MAX_ENTRY][MAX_PATH] __attribute__((aligned(64)));
int HDD_TypeFiles[MAX_ENTRY] __attribute__((aligned(64)));
int HDD_nCountFiles = 0;


char selectectedELFName[256];


// PAD
int port = 0;
static char padBuf[256] __attribute__((aligned(64)));
static char padBuf2[256] __attribute__((aligned(64)));
struct padButtonStatus buttons;
u32 paddata;
u32 old_pad = 0;
u32 new_pad;
int checkButtons = 0;
int checkButtons2 = 0;
int checkButtons3 = 0;
int checkButtons4 = 0;
int checkButtons5 = 0;

// Funciones
int saveMyCnf();
void setELFToButton(unsigned char *fileelf, int button);
void setTextToButton(unsigned char *nameelf);
void setTimeForAuto();
void setELFForAuto(int what);
int saveIcon(void);
int readPad(void);
void memcpy2(unsigned char *dest,unsigned char *org,int ndata);
void memset2(unsigned char *dest,unsigned char val,int ndata);
//float sqrt(float a);
void loadModules(void);
void Reset();
int RunElf(char *name);
void fillCircle(int x, int y, int r, unsigned color);
int readMyCNF();
void putScreen(int x, int y, const char *txt);
int runMyElf(unsigned char elf[256]);
void m_putchar(int x, int y, unsigned color, unsigned char ch);
void m_putcad(int x, int y, unsigned color, char *cad);
void m_putcharG(int x, int y, unsigned color, unsigned char ch);
void m_putcadG(int x, int y, unsigned color, char *cad);
void va_handler();
unsigned getColorAuto(int btn);
void setTimeAuto(void);
unsigned int getMinTextureSizeID(int size);
unsigned int getMinTextureSize(int size);
void putImage(IIF *img, int x, int y);
void putBackground(IIF *img);
void setLangEnglish();
void setLangFrench();
void delay(int count);
int RunLoaderElf(char *partition, char *filename);
void deleteFromPanels();
void copyFromPanels();
void deleteDirFromPanels();
int deleteDir(char *szFile, int mode);

#define ELF_MAGIC	0x464c457f
#define ELF_PT_LOAD	1

typedef struct {
	u8	ident[16];
	u16	type;
	u16	machine;
	u32	version;
	u32	entry;
	u32	phoff;
	u32	shoff;
	u32	flags;
	u16	ehsize;
	u16	phentsize;
	u16	phnum;
	u16	shentsize;
	u16	shnum;
	u16	shstrndx;
} elf_header_t;

typedef struct {
	u32	type;
	u32	offset;
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

elf_header_t elfh;

const char *eeloadimg = "rom0:UDNL rom0:EELOADCNF";
char *imgcmd;



//--------------
int showDevices = 0;
int selectDevice = 0;
int selectedDeviceMenu = 1;
int selectMenuInfo = 0;

int myActualEntry = 0;
char txtInfo[1024];

int selectPanelCopy = 1;

char LeftFiles[MAX_ENTRY][MAX_PATH] __attribute__((aligned(64)));
int LeftTypeFiles[MAX_ENTRY] __attribute__((aligned(64)));
int nCountLeftFiles = 0;
int nLeftActualEntry = 0;
char gPathLeft[256] = "mc0:/";
int nLeftDevice = -1;

char RightFiles[MAX_ENTRY][MAX_PATH] __attribute__((aligned(64)));
int RightTypeFiles[MAX_ENTRY] __attribute__((aligned(64)));
int nCountRightFiles = 0;
int nRightActualEntry = 0;
char gPathRight[256] = "mc0:/";
int nRightDevice = -1;

int selectActionFile = 0;
int selectedFileMenu = 1;

#define	PANEL_LEFT	1
#define	PANEL_RIGHT	2

/*
 * selectDevice
 * ======================
 * 1 - MC0
 * 2 - MC1
 * 3 - USB
 * 4 - HDD
 * 5 - CDVD
 *
 */
 
#define FIO_MT_RDWR			0x00
#define	FIO_MT_RDONLY		0x01

int sizeForCopy = 0;
char txtError[256];

int writeFile(char *szFile, char *file, int mode);
char *readFile(char *szFile, int mode);
int deleteFile(char *szFile, int mode);
void refreshDir(int panel);


char txtRealName_IconSys[32];

void copyCharArray(char to[], char from[]);
void copyCharArrayN(char to[], char from[]);
void copyCharArray2(char to[MAX_ENTRY][MAX_PATH], char from[MAX_ENTRY][MAX_PATH]);


typedef struct
{
	unsigned char nPort[5];
	u8 padding1[3];
	u32 notsure1;
	u32 iconptr;
	u32 notsure2;
	u32 notsure3;
} npoHeader;

//void recursiveCopy(char *folder, char *destination, int fmode, int tmode);
//void copyRecursiveFromPanels();
int npoExtract(char *filename);

int nCountScreens = 0;
void takeScreenshoot();

char baseFullPath[256];


void playMyMP3();
void stopMyMP3();


// Perdido
int createDirFromPanels(char *name);
int createDir(char *szFile, int mode);

int kbx1 = 0;
int kby1 = 0;
char keysKEY[80] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,:;@/+-*[]{}()=\0";
int altolinea,atotal,max_height,height1,diff;
int sbx1,sby1,sbx2,sby2,sbintx,sbinty,sbintw,sbinth;
int selectHelp = 0;

void keyBoard(char *returnText);

#endif //__INCLUDE_KL_H__
