############################################################
#
#
#
#
############################################################

#Descomentar para soporte MP3
MP3_SOUND=1

EE_OBJS = KL.o iopmod.o klicn.o arial.o logo.o mrsiir.o cuadrado.o triangulo.o circulo.o cruz.o fileelf.o fondo.o mass_rpc.o usb.o NPMUSBD.o USBMASS.o L1.o L2.o L3.o R1.o R2.o R3.o icomc.o icomano.o icousb.o icohdd.o folder.o folderup.o fileold.o filexio.o iomanx.o ps2atad.o ps2dev9.o ps2fs.o ps2hdd.o loader.o CDVD.o npo.o
EE_BIN=KL.ELF
EE_LIBS=-ldebug -lkernel -lsyscall -lm -lpad -lhdd -lfileXio -lgcc -lgs -lmc -lcdvdfs -lpatches
EE_INCS=-I$(GSLIB)/source -I$(LIBCDVD)/common -I$(LIBCDVD)/ee -I$(PS2SDK)/sbv/include
EE_LDFLAGS=-L$(GSLIB)/lib -L$(LIBCDVD)/lib -L$(PS2SDK)/sbv/lib
PS2IP=192.168.2.3

ifdef MP3_SOUND
	EE_OBJS += audio.o
	EE_LIBS +=-laudio -laudiomp3 -lffmpegavi
	EE_INCS +=-I$(PS2DEV)/audiolib/includes
	EE_LDFLAGS +=-L$(PS2DEV)/audiolib/libs
endif

all: elf

super: clean res elf run

test: elf pack run

elf: $(EE_BIN)

res:
	############################################################
	# Images
	############################################################
	./bin/rawtoasm ./resource/kl.icn klicn.S klicn
	./bin/rawtoasm ./resource/cuadrado.iif cuadrado.S cuadrado
	./bin/rawtoasm ./resource/triangulo.iif triangulo.S triangulo
	./bin/rawtoasm ./resource/circulo.iif circulo.S circulo
	./bin/rawtoasm ./resource/cruz.iif cruz.S cruz
	./bin/rawtoasm ./resource/logo.iif logo.S logo
	./bin/rawtoasm ./resource/mrsiir.iif mrsiir.S mrsiir
	./bin/rawtoasm ./resource/fileelf.iif fileelf.S fileelf
	./bin/rawtoasm ./resource/kl.icn klicn.S klicn
	./bin/rawtoasm ./resource/fondo.iif fondo.S fondo
	./bin/rawtoasm ./resource/usb.iif usb.S usb
	./bin/rawtoasm ./resource/folder.iif folder.S folder
	./bin/rawtoasm ./resource/folderup.iif folderup.S folderup
	./bin/rawtoasm ./resource/fileold.iif fileold.S fileold
	./bin/rawtoasm ./resource/l1.iif L1.S L1
	./bin/rawtoasm ./resource/l2.iif L2.S L2
	./bin/rawtoasm ./resource/l3.iif L3.S L3
	./bin/rawtoasm ./resource/r1.iif R1.S R1
	./bin/rawtoasm ./resource/r2.iif R2.S R2
	./bin/rawtoasm ./resource/r3.iif R3.S R3
	./bin/rawtoasm ./resource/icomc.iif icomc.S icomc
	./bin/rawtoasm ./resource/icomano.iif icomano.S icomano
	./bin/rawtoasm ./resource/icousb.iif icousb.S icousb
	./bin/rawtoasm ./resource/icohdd.iif icohdd.S icohdd
	./bin/rawtoasm ./resource/arial.fnt arial.S arial
	./bin/rawtoasm ./resource/npo.iif npo.S npo
	################################################################
	# HDD IRXs
	################################################################
	./bin/bin2s ./hdd/filexio.irx filexio.S filexio
	./bin/bin2s ./hdd/iomanx.irx iomanx.S iomanx
	./bin/bin2s ./hdd/ps2atad.irx ps2atad.S ps2atad
	./bin/bin2s ./hdd/ps2dev9.irx ps2dev9.S ps2dev9
	./bin/bin2s ./hdd/ps2fs.irx ps2fs.S ps2fs
	./bin/bin2s ./hdd/ps2hdd.irx ps2hdd.S ps2hdd
	./bin/bin2s ./hdd/loader.elf loader.S loader
	./bin/bin2s ./hdd/cdvd.irx CDVD.S CDVD
	###############################################################
	# USB IRXs and Audio
	###############################################################
	./bin/bin2s ./irx/npm-usbd.irx NPMUSBD.S NPMUSBD
	./bin/bin2s ./irx/usb_mass.irx USBMASS.S USBMASS
	./bin/bin2s ./irx/audio.irx audio.S audio

pack:
	############################################
	# Pack ELF
	###########################################
	./bin/ps2packer/ps2-packer $(EE_BIN) TMP.ELF
	rm -f $(EE_BIN)
	mv TMP.ELF $(EE_BIN)

run:
	############################################
	# Run ELF
	###########################################
	./bin/ps2client -h $(PS2IP) execee host:$(EE_BIN)

reset:
	############################################
	# Reset PS2Client
	###########################################
	./bin/ps2client -h $(PS2IP) reset

upload:
	############################################
	# Upload ELF
	###########################################
	./bin/ps2client -h $(PS2IP) copyto $(EE_BIN) mc0:/BEDATA-SYSTEM/BOOT.ELF
	

clean:
	rm -f $(EE_BIN) *.o *.a *.elf *.S

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp
