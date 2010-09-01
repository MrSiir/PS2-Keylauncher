/*
  _____     ___ ____
   ____|   |    ____|      MrSiir / SiirLabs
  |     ___|   |____       (C)2004, MrSiir (mrsiir@gmail.com)
  ------------------------------------------------------------------------
  langs.h
                        PS2 Key Launcher 0.5.4
  ------------------------------------------------------------------------
    
  * Para compilar este programa se necesita la ps2sdk y gslib 0.5.1
  
  ------------------------------------------------------------------------
*/
#define ENGLISH	1
#define FRENCH	2
#define SPANISH	3
//------------------------------------------------------------------------
int currentLang = SPANISH;
//------------------------------------------------------------------------
// Español
//------------------------------------------------------------------------
// Traduccion por MrSiir
//------------------------------------------------------------------------
char txt_1[256] = "Mas info en:";
char txt_3[256] = "Botones configurados:";
char txt_5[256] = "Carga automática:";
char txt_6[256] = "El botón no esta configurado, pulsa X para volver";
char txt_7[256] = "Cargando...";
char txt_8[256] = "ERROR:\nLa carga del programa ha fallado, pulsa X para volver";
//------------------------------------------------------------------------
char txt_9[256] = "ERROR:\nNo se ha podido grabar el fichero de configuración\nPulsa X para volver";
char txt_10[256] = "Modo Selección";
char txt_11[256] = "Para enlazar un fichero pulsa X";
char txt_12[256] = "Ficheros";
char txt_13[256] = "Asignar botón";
char txt_14[256] = "Para asignar el fichero:";
char txt_15[256] = "Pulsa el botón al que lo quieras asignar";
char txt_16[256] = "Para cancelar pulsa START";
char txt_17[256] = "Pon nombre al fichero enlazado:";
char txt_18[256] = "Seleccionar";
char txt_19[256] = "Borrar";
char txt_20[256] = "Espacio";
char txt_21[256] = "Guardar";
char txt_22[256] = "Selecciona una opción:";
char txt_23[256] = "1. Enlazar un ELF a un botón";
char txt_24[256] = "2. Establecer el tiempo para la carga automática";
char txt_25[256] = "3. Seleccionar ELF para la carga automática";
char txt_26[256] = "Selecciona el ELF para la carga automática:";
char txt_27[256] = "Para seleccionar un ELF para la carga automática, pulsa el botón que\nquieras que cargue automáticamente";
char txt_28[256] = "Selecciona el tiempo:";
//------------------------------------------------------------------------
char txt_29[256] = "Cargando módulos del HD...";
char txt_30[256] = "Cargando desde el HD...";
char txt_31[256] = "Cargando desde la Memory Card...";
char txt_32[256] = "ERROR:\nNo se ha podido leer el fichero de configuración";
char txt_33[256] = "Memory Card";
char txt_34[256] = "Dispositivo USB";
char txt_35[256] = "Manual";
char txt_36[256] = "Dispositivo HD";
char txt_37[256] = "Fichero Manual:";
char txt_38[256] = "Leyendo la tabla de particiones del HD...";
char txt_39[256] = "Cargando desde el USB...";
char txt_40[400] = "\bGracias a:\b  Hermes, Rocio, Joju, Gustavo Scotti, David Ryan, Nicholas Van Veen, Pukko, Doug Knight, Marcus R. Brown, A.Lee, Marek Olejnik (USB), Mavy, BigBoss, Marcansoft, Neofar, Cambios, Crothgar, puterucu, Vermis, ZeusII, t0mb0la, lion, D@NO, a la gente de ELOTROLADO.NET, a la gente de PS2REALITY.NET y a la gente de PS2SWAP.NET";
//------------------------------------------------------------------------
char txt_42[256] = "Fin";
char txt_43[256] = "4. Navegador!";
char txt_44[256] = "Navegador!";
char txt_45[256] = "1. Memory Card 1";
char txt_46[256] = "2. Memory Card 2";
char txt_47[256] = "3. USB Mass";
char txt_48[256] = "4. HD";
char txt_49[256] = "5. CDVD-ROM";
char txt_50[256] = "1. Copiar";
char txt_51[256] = "2. Borrar";
char txt_52[256] = "3. Mover";
char txt_53[256] = "4. Ejecutar";
char txt_54[256] = "5. Cancelar";
char txt_55[256] = "No ha ningún fichero/directorio seleccionado";
char txt_56[256] = "\bInformación:\b\n";
char txt_57[256] = "Pulsa L1 para mostrar/ocultar la AYUDA";
char txt_62[256] = "Recargando...";
char txt_63[256] = "No se puede copiar en el USB";
char txt_64[256] = "No se puede copiar en el CDVD";
char txt_65[256] = "No se puede abrir ";
char txt_66[256] = "Copiando...";
char txt_67[256] = "Borrando...";
char txt_68[256] = "No se puede borrar ";
//------------------------------------------------------------------------
char txt_69[256] = "5. Lanzar Backup de PSOne (solo v9)";
char txt_70[256] = "PS2 KeyLauncher 0.5.4\nCargando...";
//------------------------------------------------------------------------
char txt_71[256] = "Crear";
char txt_72[256] = "Cancelar";
char txt_73[256] = "No se puede crear el directorio:";
char txt_74[256] = "El CDVD-ROM es de solo lectura";
//------------------------------------------------------------------------
char txt_75[256] = ": Muestra/Oculta esta ayuda";
char txt_76[256] = ": Muestra/Oculta la selección de dispositivos";
char txt_77[256] = ": Muestra/Oculta información de los ficheros";
char txt_78[256] = ": [Fichero] Muestra las opciones de un fichero";
char txt_79[256] = ": [Directorio] Entra en el directorio";
char txt_80[256] = ": Crear directorio en la MC, USB o HD";
char txt_81[256] = ": Borra un directorio o un fichero de la MC, USB o HD";
char txt_82[256] = ": Volver a la pantalla inicial";
char txt_83[256] = "\bAyuda para los botones del Navegador!:\b";
//------------------------------------------------------------------------
// English
//------------------------------------------------------------------------
// Traduccion por MrSiir
//------------------------------------------------------------------------
char txt_1_eng[256] = "More info:";
char txt_3_eng[256] = "Configured buttons:";
char txt_5_eng[256] = "Automatic loading:";
char txt_6_eng[256] = "The button is not assigned, press X to return";
char txt_7_eng[256] = "Loading...";
char txt_8_eng[256] = "ERROR:\nThe load of the program has failed, press X to return";
//------------------------------------------------------------------------
char txt_9_eng[256] = "ERROR:\nIt has not been possible to record the configuration file, Press X to return";
char txt_10_eng[256] = "Selecction Mode";
char txt_11_eng[256] = "To link file press X";
char txt_12_eng[256] = "Files";
char txt_13_eng[256] = "Assign button";
char txt_14_eng[256] = "To assign file:";
char txt_15_eng[256] = "Press the button to which you want to assign it";
char txt_16_eng[256] = "To cancel press START";
char txt_17_eng[256] = "Put name to linked file:";
char txt_18_eng[256] = "Select";
char txt_19_eng[256] = "Delete";
char txt_20_eng[256] = "Space";
char txt_21_eng[256] = "Save";
char txt_22_eng[256] = "Select Option:";
char txt_23_eng[256] = "1. Link an ELF to any button";
char txt_24_eng[256] = "2. Set the time for automatic load";
char txt_25_eng[256] = "3. Select an ELF for automatic load";
char txt_26_eng[256] = "Select ELF for automatic load:";
char txt_27_eng[256] = "To select an ELF for the automatic load, press the button that you want\n that it loads automatically";
char txt_28_eng[256] = "Select the time:";
//------------------------------------------------------------------------
char txt_29_eng[256] = "Loading HD modules...";
char txt_30_eng[256] = "Loading from HD...";
char txt_31_eng[256] = "Loading from Memory Card...";
char txt_32_eng[256] = "ERROR:\nCan't read the configuration file";
char txt_33_eng[256] = "Memory Card";
char txt_34_eng[256] = "USB Device";
char txt_35_eng[256] = "Manual";
char txt_36_eng[256] = "HD Device";
char txt_37_eng[256] = "File Manual:";
char txt_38_eng[256] = "Reading the table of partitions...";
char txt_39_eng[256] = "Loading from USB...";
char txt_40_eng[400] = "\bThanks to:\b Hermes, Rocio, Joju, Gustavo Scotti, David Ryan, Nicholas Van Veen, Pukko, Doug Knight, Marcus R. Brown, A.Lee, Marek Olejnik, Mavy, BigBoss, Marcansoft, Neofar, Cambios, Crothgar, puterucu, Vermis, ZeusII, t0mb0la, lion, D@NO, to ELOTROLADO.NET people, to PS2Reality.net people, to PS2SWAP.NET people and PS2NFO.COM";
//------------------------------------------------------------------------
char txt_42_eng[256] = "End";
char txt_43_eng[256] = "4. Browser!";
char txt_44_eng[256] = "Browser!";
char txt_45_eng[256] = "1. Memory Card 1";
char txt_46_eng[256] = "2. Memory Card 2";
char txt_47_eng[256] = "3. USB Mass";
char txt_48_eng[256] = "4. HD";
char txt_49_eng[256] = "5. CDVD-ROM";
char txt_50_eng[256] = "1. Copy";
char txt_51_eng[256] = "2. Delete";
char txt_52_eng[256] = "3. Move";
char txt_53_eng[256] = "4. Execute";
char txt_54_eng[256] = "5. Cancel";
char txt_55_eng[256] = "There is no selected directory or file";
char txt_56_eng[256] = "\bInformation:\b\n";
char txt_57_eng[256] = "Press L1 for show/hide HELP";
char txt_62_eng[256] = "Loading...";
char txt_63_eng[256] = "It is not possible to be copied in the USB";
char txt_64_eng[256] = "It is not possible to be copied in the CDVD";
char txt_65_eng[256] = "Cannot be open ";
char txt_66_eng[256] = "Copying...";
char txt_67_eng[256] = "Erasing...";
char txt_68_eng[256] = "Cannot be delete ";
//------------------------------------------------------------------------
char txt_69_eng[256] = "5. Launch PSOne Backup (only v9)";
char txt_70_eng[256] = "PS2 KeyLauncher 0.5.4\nLoading...";
//------------------------------------------------------------------------
char txt_71_eng[256] = "Create";
char txt_72_eng[256] = "Cancel";
char txt_73_eng[256] = "I don't create directory:";
char txt_74_eng[256] = "The CDVD-ROM is read only";
//------------------------------------------------------------------------
char txt_75_eng[256] = ": Show/Hide this help";
char txt_76_eng[256] = ": Show/Hide device selection";
char txt_77_eng[256] = ": Show/Hide file information";
char txt_78_eng[256] = ": [File] Show options file";
char txt_79_eng[256] = ": [Directory] Entra en el directorio";
char txt_80_eng[256] = ": Create directory in MC, USB or HD";
char txt_81_eng[256] = ": Delete directory or file from MC, USB or HD";
char txt_82_eng[256] = ": Go back to main screen";
char txt_83_eng[256] = "\bHelp for Browser! buttons:\b";
//------------------------------------------------------------------------
// Frances
//------------------------------------------------------------------------
// Traduccion por Chandler <brucewilnos13@hotmail.com>
//------------------------------------------------------------------------
char txt_1_fre[256] = "Plus d'Info:";
char txt_3_fre[256] = "Boutons pré-configurés:";
char txt_5_fre[256] = "Chargement automatique:";
char txt_6_fre[256] = "Le bouton n'est pas assigné, pressez X pour revenir";
char txt_7_fre[256] = "Chargement...";
char txt_8_fre[256] = "Le chargement du programme a échoué, pressez X pour revenir";
//------------------------------------------------------------------------
char txt_9_fre[256] = "Il n'a pas été possible d'enregistrer le fichier de configuration,\nAppuyer X pour revenir";
char txt_10_fre[256] = "Mode Select";
char txt_11_fre[256] = "Pour lier le fichier presser X";
char txt_12_fre[256] = "Fichiers";
char txt_13_fre[256] = "Assigner bouton";
char txt_14_fre[256] = "Pour assigner un fichier:";
char txt_15_fre[256] = "Presser le bouton auquel vous voulez l'assigner";
char txt_16_fre[256] = "Pour effacer presser START";
char txt_17_fre[256] = "Mettre un nom au fichier lié:";
char txt_18_fre[256] = "Select";
char txt_19_fre[256] = "Effacer";
char txt_20_fre[256] = "Espace";
char txt_21_fre[256] = "Sauveguarder";
char txt_22_fre[256] = "Selectionner Option:";
char txt_23_fre[256] = "1. Lier un ELF à n'importe quel bouton";
char txt_24_fre[256] = "2. Regler le temps avant la lecture automatique";
char txt_25_fre[256] = "3. Selectionner un ELF pour la lecture automatique";
char txt_26_fre[256] = "Selectionner ELF pour la lecture automatique:";
char txt_27_fre[256] = "Pour selectionner un ELF pour la lecture automatique, pressez le bouton\nque vous voulez pour le lire automatiquement";
char txt_28_fre[256] = "Choisir le temps:";
//------------------------------------------------------------------------
char txt_29_fre[256] = "Cargando módulos del HD...";
char txt_30_fre[256] = "Cargando desde el HD...";
char txt_31_fre[256] = "Cargando desde la Memory Card...";
char txt_32_fre[256] = "ERROR:\nNo se ha podido leer el fichero de configuración";
char txt_33_fre[256] = "Memory Card";
char txt_34_fre[256] = "Dispositivo USB";
char txt_35_fre[256] = "Manual";
char txt_36_fre[256] = "Dispositivo HD";
char txt_37_fre[256] = "Fichero Manual:";
char txt_38_fre[256] = "Leyendo la table de particiones del HD...";
char txt_39_fre[256] = "Cargando desde el USB...";
char txt_40_fre[400] = "\bMerci à:\b Hermes, Rocio, Joju, Gustavo Scotti, David Ryan, Nicholas Van Veen, Pukko, Doug Knight, Marcus R. Brown, A.Lee, Marek Olejnik, Mavy, BigBoss, Marcansoft, Neofar, Cambios, Crothgar, puterucu, Vermis, ZeusII, t0mb0la, lion, D@NO, to ELOTROLADO.NET people, to PS2Reality.net people, to PS2SWAP.NET people and PS2NFO.COM";
//------------------------------------------------------------------------
char txt_42_fre[256] = "End";
char txt_43_fre[256] = "4. Browser!";
char txt_44_fre[256] = "Browser!";
char txt_45_fre[256] = "1. Memory Card 1";
char txt_46_fre[256] = "2. Memory Card 2";
char txt_47_fre[256] = "3. USB Mass";
char txt_48_fre[256] = "4. HD";
char txt_49_fre[256] = "5. CDVD-ROM";
char txt_50_fre[256] = "1. Copy";
char txt_51_fre[256] = "2. Delete";
char txt_52_fre[256] = "3. Move";
char txt_53_fre[256] = "4. Execute";
char txt_54_fre[256] = "5. Cancel";
char txt_55_fre[256] = "There is no selected directory or file";
char txt_56_fre[256] = "\bInformation:\b\n";
char txt_57_fre[256] = "Press L1 for show/hide HELP";
char txt_62_fre[256] = "Loading...";
char txt_63_fre[256] = "It is not possible to be copied in the USB";
char txt_64_fre[256] = "It is not possible to be copied in the CDVD";
char txt_65_fre[256] = "Cannot be open ";
char txt_66_fre[256] = "Copying...";
char txt_67_fre[256] = "Erasing...";
char txt_68_fre[256] = "Cannot be delete ";
//------------------------------------------------------------------------
char txt_69_fre[256] = "5. Launch PSOne Backup (only v9)";
char txt_70_fre[256] = "PS2 KeyLauncher 0.5.4\nLoading...";
//------------------------------------------------------------------------
char txt_71_fre[256] = "Create";
char txt_72_fre[256] = "Cancel";
char txt_73_fre[256] = "I don't create directory:";
char txt_74_fre[256] = "The CDVD-ROM is read only";
//------------------------------------------------------------------------
char txt_75_fre[256] = ": Show/Hide this help";
char txt_76_fre[256] = ": Show/Hide device selection";
char txt_77_fre[256] = ": Show/Hide file information";
char txt_78_fre[256] = ": [File] Show options file";
char txt_79_fre[256] = ": [Directory] Entra en el directorio";
char txt_80_fre[256] = ": Create directory in MC, USB or HD";
char txt_81_fre[256] = ": Delete directory or file from MC, USB or HD";
char txt_82_fre[256] = ": Go back to main screen";
char txt_83_fre[256] = "\bHelp for Browser! buttons:\b";
/***************************************************************
 * IDIOMA
 * 
 ***************************************************************/
void setLangEnglish()
{
	copyCharArray((char *)txt_1, (char *)txt_1_eng);
	copyCharArray((char *)txt_3, (char *)txt_3_eng);
	copyCharArray((char *)txt_5, (char *)txt_5_eng);
	copyCharArray((char *)txt_6, (char *)txt_6_eng);
	copyCharArray((char *)txt_7, (char *)txt_7_eng);
	copyCharArray((char *)txt_8, (char *)txt_8_eng);
	copyCharArray((char *)txt_9, (char *)txt_9_eng);
	copyCharArray((char *)txt_10, (char *)txt_10_eng);
	copyCharArray((char *)txt_11, (char *)txt_11_eng);
	copyCharArray((char *)txt_12, (char *)txt_12_eng);
	copyCharArray((char *)txt_13, (char *)txt_13_eng);
	copyCharArray((char *)txt_14, (char *)txt_14_eng);
	copyCharArray((char *)txt_15, (char *)txt_15_eng);
	copyCharArray((char *)txt_16, (char *)txt_16_eng);
	copyCharArray((char *)txt_17, (char *)txt_17_eng);
	copyCharArray((char *)txt_18, (char *)txt_18_eng);
	copyCharArray((char *)txt_19, (char *)txt_19_eng);
	copyCharArray((char *)txt_20, (char *)txt_20_eng);
	copyCharArray((char *)txt_21, (char *)txt_21_eng);
	copyCharArray((char *)txt_22, (char *)txt_22_eng);
	copyCharArray((char *)txt_23, (char *)txt_23_eng);
	copyCharArray((char *)txt_24, (char *)txt_24_eng);
	copyCharArray((char *)txt_25, (char *)txt_25_eng);
	copyCharArray((char *)txt_26, (char *)txt_26_eng);
	copyCharArray((char *)txt_27, (char *)txt_27_eng);
	copyCharArray((char *)txt_28, (char *)txt_28_eng);
	copyCharArray((char *)txt_29, (char *)txt_29_eng);
	copyCharArray((char *)txt_30, (char *)txt_30_eng);
	copyCharArray((char *)txt_31, (char *)txt_31_eng);
	copyCharArray((char *)txt_32, (char *)txt_32_eng);
	copyCharArray((char *)txt_33, (char *)txt_33_eng);
	copyCharArray((char *)txt_34, (char *)txt_34_eng);
	copyCharArray((char *)txt_35, (char *)txt_35_eng);
	copyCharArray((char *)txt_36, (char *)txt_36_eng);
	copyCharArray((char *)txt_37, (char *)txt_37_eng);
	copyCharArray((char *)txt_38, (char *)txt_38_eng);
	copyCharArray((char *)txt_39, (char *)txt_39_eng);
	copyCharArray((char *)txt_40, (char *)txt_40_eng);
	copyCharArray((char *)txt_42, (char *)txt_42_eng);
	copyCharArray((char *)txt_43, (char *)txt_43_eng);
	copyCharArray((char *)txt_44, (char *)txt_44_eng);
	copyCharArray((char *)txt_45, (char *)txt_45_eng);
	copyCharArray((char *)txt_46, (char *)txt_46_eng);
	copyCharArray((char *)txt_47, (char *)txt_47_eng);
	copyCharArray((char *)txt_48, (char *)txt_48_eng);
	copyCharArray((char *)txt_49, (char *)txt_49_eng);
	copyCharArray((char *)txt_50, (char *)txt_50_eng);
	copyCharArray((char *)txt_51, (char *)txt_51_eng);
	copyCharArray((char *)txt_52, (char *)txt_52_eng);
	copyCharArray((char *)txt_53, (char *)txt_53_eng);
	copyCharArray((char *)txt_54, (char *)txt_54_eng);
	copyCharArray((char *)txt_55, (char *)txt_55_eng);
	copyCharArray((char *)txt_56, (char *)txt_56_eng);
	copyCharArray((char *)txt_57, (char *)txt_57_eng);
	copyCharArray((char *)txt_62, (char *)txt_62_eng);
	copyCharArray((char *)txt_63, (char *)txt_63_eng);
	copyCharArray((char *)txt_64, (char *)txt_64_eng);
	copyCharArray((char *)txt_65, (char *)txt_65_eng);
	copyCharArray((char *)txt_66, (char *)txt_66_eng);
	copyCharArray((char *)txt_67, (char *)txt_67_eng);
	copyCharArray((char *)txt_68, (char *)txt_68_eng);
	copyCharArray((char *)txt_69, (char *)txt_69_eng);
	copyCharArray((char *)txt_70, (char *)txt_70_eng);
	copyCharArray((char *)txt_71, (char *)txt_71_eng);
	copyCharArray((char *)txt_72, (char *)txt_72_eng);
	copyCharArray((char *)txt_73, (char *)txt_73_eng);
	copyCharArray((char *)txt_74, (char *)txt_74_eng);
	copyCharArray((char *)txt_75, (char *)txt_75_eng);
	copyCharArray((char *)txt_76, (char *)txt_76_eng);
	copyCharArray((char *)txt_77, (char *)txt_77_eng);
	copyCharArray((char *)txt_78, (char *)txt_78_eng);
	copyCharArray((char *)txt_79, (char *)txt_79_eng);
	copyCharArray((char *)txt_80, (char *)txt_80_eng);
	copyCharArray((char *)txt_81, (char *)txt_81_eng);
	copyCharArray((char *)txt_82, (char *)txt_82_eng);
	copyCharArray((char *)txt_83, (char *)txt_83_eng);
}

void setLangFrench()
{
	copyCharArray((char *)txt_1, (char *)txt_1_fre);
	copyCharArray((char *)txt_3, (char *)txt_3_fre);
	copyCharArray((char *)txt_5, (char *)txt_5_fre);
	copyCharArray((char *)txt_6, (char *)txt_6_fre);
	copyCharArray((char *)txt_7, (char *)txt_7_fre);
	copyCharArray((char *)txt_8, (char *)txt_8_fre);
	copyCharArray((char *)txt_9, (char *)txt_9_eng);
	copyCharArray((char *)txt_10, (char *)txt_10_fre);
	copyCharArray((char *)txt_11, (char *)txt_11_fre);
	copyCharArray((char *)txt_12, (char *)txt_12_fre);
	copyCharArray((char *)txt_13, (char *)txt_13_fre);
	copyCharArray((char *)txt_14, (char *)txt_14_fre);
	copyCharArray((char *)txt_15, (char *)txt_15_fre);
	copyCharArray((char *)txt_16, (char *)txt_16_fre);
	copyCharArray((char *)txt_17, (char *)txt_17_fre);
	copyCharArray((char *)txt_18, (char *)txt_18_fre);
	copyCharArray((char *)txt_19, (char *)txt_19_fre);
	copyCharArray((char *)txt_20, (char *)txt_20_fre);
	copyCharArray((char *)txt_21, (char *)txt_21_fre);
	copyCharArray((char *)txt_22, (char *)txt_22_fre);
	copyCharArray((char *)txt_23, (char *)txt_23_fre);
	copyCharArray((char *)txt_24, (char *)txt_24_fre);
	copyCharArray((char *)txt_25, (char *)txt_25_fre);
	copyCharArray((char *)txt_26, (char *)txt_26_fre);
	copyCharArray((char *)txt_27, (char *)txt_27_fre);
	copyCharArray((char *)txt_28, (char *)txt_28_fre);
	copyCharArray((char *)txt_29, (char *)txt_29_fre);
	copyCharArray((char *)txt_30, (char *)txt_30_fre);
	copyCharArray((char *)txt_31, (char *)txt_31_fre);
	copyCharArray((char *)txt_32, (char *)txt_32_fre);
	copyCharArray((char *)txt_33, (char *)txt_33_fre);
	copyCharArray((char *)txt_34, (char *)txt_34_fre);
	copyCharArray((char *)txt_35, (char *)txt_35_fre);
	copyCharArray((char *)txt_36, (char *)txt_36_fre);
	copyCharArray((char *)txt_37, (char *)txt_37_fre);
	copyCharArray((char *)txt_38, (char *)txt_38_fre);
	copyCharArray((char *)txt_39, (char *)txt_39_fre);
	copyCharArray((char *)txt_40, (char *)txt_40_fre);
	copyCharArray((char *)txt_42, (char *)txt_42_fre);
	copyCharArray((char *)txt_43, (char *)txt_43_fre);
	copyCharArray((char *)txt_44, (char *)txt_44_fre);
	copyCharArray((char *)txt_45, (char *)txt_45_fre);
	copyCharArray((char *)txt_46, (char *)txt_46_fre);
	copyCharArray((char *)txt_47, (char *)txt_47_fre);
	copyCharArray((char *)txt_48, (char *)txt_48_fre);
	copyCharArray((char *)txt_49, (char *)txt_49_fre);
	copyCharArray((char *)txt_50, (char *)txt_50_fre);
	copyCharArray((char *)txt_51, (char *)txt_51_fre);
	copyCharArray((char *)txt_52, (char *)txt_52_fre);
	copyCharArray((char *)txt_53, (char *)txt_53_fre);
	copyCharArray((char *)txt_54, (char *)txt_54_fre);
	copyCharArray((char *)txt_55, (char *)txt_55_fre);
	copyCharArray((char *)txt_56, (char *)txt_56_fre);
	copyCharArray((char *)txt_57, (char *)txt_57_fre);
	copyCharArray((char *)txt_62, (char *)txt_62_fre);
	copyCharArray((char *)txt_63, (char *)txt_63_fre);
	copyCharArray((char *)txt_64, (char *)txt_64_fre);
	copyCharArray((char *)txt_65, (char *)txt_65_fre);
	copyCharArray((char *)txt_66, (char *)txt_66_fre);
	copyCharArray((char *)txt_67, (char *)txt_67_fre);
	copyCharArray((char *)txt_68, (char *)txt_68_fre);
	copyCharArray((char *)txt_69, (char *)txt_69_fre);
	copyCharArray((char *)txt_70, (char *)txt_70_fre);
	copyCharArray((char *)txt_71, (char *)txt_71_fre);
	copyCharArray((char *)txt_72, (char *)txt_72_fre);
	copyCharArray((char *)txt_73, (char *)txt_73_fre);
	copyCharArray((char *)txt_74, (char *)txt_74_fre);
	copyCharArray((char *)txt_75, (char *)txt_75_fre);
	copyCharArray((char *)txt_76, (char *)txt_76_fre);
	copyCharArray((char *)txt_77, (char *)txt_77_fre);
	copyCharArray((char *)txt_78, (char *)txt_78_fre);
	copyCharArray((char *)txt_79, (char *)txt_79_fre);
	copyCharArray((char *)txt_80, (char *)txt_80_fre);
	copyCharArray((char *)txt_81, (char *)txt_81_fre);
	copyCharArray((char *)txt_82, (char *)txt_82_fre);
	copyCharArray((char *)txt_83, (char *)txt_83_fre);
}

s32 configGetLanguage(void)
{
	u32 config = 0;
	GetOsdConfigParam(&config);
	return(config >> 16) & 0x1F;
}
