/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: ManagePanel */
#define  PANEL_LOCKIN_PARAMETERS          2       /* control type: table, callback function: SetLockinParameters */
#define  PANEL_RANGE                      3       /* control type: slide, callback function: SetRange */
#define  PANEL_BALANCE_TARGET             4       /* control type: ring, callback function: SetBalance */
#define  PANEL_ACTIVE_CHANNEL             5       /* control type: ring, callback function: SetActiveChannel */
#define  PANEL_CHECK_THRESHOLD            6       /* control type: numeric, callback function: SetBalance */
#define  PANEL_BALANCE_THRESHOLD          7       /* control type: numeric, callback function: SetBalance */
#define  PANEL_LOCKIN_R                   8       /* control type: numeric, callback function: (none) */
#define  PANEL_LOCKIN_Y                   9       /* control type: numeric, callback function: (none) */
#define  PANEL_REAL_FREQUENCY             10      /* control type: numeric, callback function: (none) */
#define  PANEL_LOCKIN_X                   11      /* control type: numeric, callback function: (none) */
#define  PANEL_CLOCKFREQUENCY             12      /* control type: numeric, callback function: SetFrequency */
#define  PANEL_FREQUENCY                  13      /* control type: numeric, callback function: SetFrequency */
#define  PANEL_PHASE                      14      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_MDAC2_VAL                  15      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_MDAC1_OFFSET               16      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_MDAC2_CODE                 17      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_IMAG                       18      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_REAL                       19      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_AMPLITUDE                  20      /* control type: numeric, callback function: SetWaveformParameters */
#define  PANEL_LOCKIN_RECORD              21      /* control type: command, callback function: RecordLockin */
#define  PANEL_LOCKIN_READ                22      /* control type: command, callback function: ReadLockin */
#define  PANEL_COPY_REAL_FREQUENCY        23      /* control type: command, callback function: CopyWaveformParameters */
#define  PANEL_COPY_SAMPLES               24      /* control type: command, callback function: CopyWaveformParameters */
#define  PANEL_COPY_PHASOR_1              25      /* control type: command, callback function: CopyWaveformParameters */
#define  PANEL_PASTE_PHASOR               26      /* control type: command, callback function: SetWaveformParameters */
#define  PANEL_COPY_PHASOR                27      /* control type: command, callback function: CopyWaveformParameters */
#define  PANEL_DECORATION_7               28      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_5               29      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_8               30      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_6               31      /* control type: deco, callback function: (none) */
#define  PANEL_DECORATION_4               32      /* control type: deco, callback function: (none) */
#define  PANEL_START_STOP                 33      /* control type: command, callback function: StartStop */
#define  PANEL_START_STOP_LED             34      /* control type: LED, callback function: (none) */
#define  PANEL_BALANCE                    35      /* control type: command, callback function: Balance */
#define  PANEL_OUT_OF_RANGE_LED           36      /* control type: LED, callback function: (none) */
#define  PANEL_AUTOZERO_LED               37      /* control type: LED, callback function: (none) */
#define  PANEL_CONNECT                    38      /* control type: command, callback function: Connect */
#define  PANEL_CONNECT_LED                39      /* control type: LED, callback function: (none) */
#define  PANEL_TEXTMSG_11                 40      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_6                  41      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_10                 42      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_5                  43      /* control type: textMsg, callback function: (none) */
#define  PANEL_IS_LOCKED                  44      /* control type: radioButton, callback function: ToggleLock */
#define  PANEL_LABEL                      45      /* control type: string, callback function: SetLabel */
#define  PANEL_PHASE_ADD_PIHALF           46      /* control type: command, callback function: SetWaveformParameters */
#define  PANEL_PHASE_SUBTRACT_PIHALF      47      /* control type: command, callback function: SetWaveformParameters */
#define  PANEL_PHASE_ADD_PI               48      /* control type: command, callback function: SetWaveformParameters */
#define  PANEL_PHASE_SUBTRACT_PI          49      /* control type: command, callback function: SetWaveformParameters */
#define  PANEL_ACTIVE_SWITCH_MODE         50      /* control type: ring, callback function: SetActiveSwitchMode */
#define  PANEL_ACTIVE_MODE                51      /* control type: ring, callback function: SetActiveMode */
#define  PANEL_SWAP_ACTIVE_CHANNEL        52      /* control type: command, callback function: SwapActiveChannel */
#define  PANEL_VERSICAL_LOGO              53      /* control type: picture, callback function: (none) */
#define  PANEL_UPDATE_MODE                54      /* control type: command, callback function: UpdateActiveMode */
#define  PANEL_TEXTMSG_8                  55      /* control type: textMsg, callback function: (none) */
#define  PANEL_LOCKIN_AUTORANGE_LED       56      /* control type: LED, callback function: (none) */
#define  PANEL_RECORD_TIMER               57      /* control type: timer, callback function: RecordLockinOnTimer */
#define  PANEL_CURRENT_REPETITION         58      /* control type: numeric, callback function: (none) */
#define  PANEL_BALANCE_REPETITIONS        59      /* control type: numeric, callback function: (none) */
#define  PANEL_TEXTMSG                    60      /* control type: textMsg, callback function: (none) */
#define  PANEL_GIQS_LOGO                  61      /* control type: picture, callback function: (none) */
#define  PANEL_TEXTMSG_3                  62      /* control type: textMsg, callback function: (none) */
#define  PANEL_TEXTMSG_2                  63      /* control type: textMsg, callback function: (none) */
#define  PANEL_SAMPLES                    64      /* control type: numeric, callback function: SetFrequency */

#define  PANEL_AZ                         2
#define  PANEL_AZ_LOCKIN_READINGS         2       /* control type: graph, callback function: (none) */
#define  PANEL_AZ_INTERRUPT               3       /* control type: command, callback function: (none) */
#define  PANEL_AZ_ZERO_Y                  4       /* control type: textMsg, callback function: (none) */
#define  PANEL_AZ_X_LABEL                 5       /* control type: textMsg, callback function: (none) */
#define  PANEL_AZ_ZERO_X                  6       /* control type: textMsg, callback function: (none) */
#define  PANEL_AZ_X_MAX                   7       /* control type: numeric, callback function: (none) */
#define  PANEL_AZ_X_MIN                   8       /* control type: numeric, callback function: (none) */
#define  PANEL_AZ_Y_MIN                   9       /* control type: numeric, callback function: (none) */
#define  PANEL_AZ_Y_MAX                   10      /* control type: numeric, callback function: (none) */
#define  PANEL_AZ_Y_LABEL                 11      /* control type: textMsg, callback function: (none) */

#define  PANEL_BRI                        3
#define  PANEL_BRI_INJECTION_CHANNEL      2       /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_KELVIN_ARM_CHANNEL     3       /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_CURRENT_CHANNEL_B      4       /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_OK                     5       /* control type: command, callback function: (none) */
#define  PANEL_BRI_CANCEL                 6       /* control type: command, callback function: (none) */
#define  PANEL_BRI_CURRENT_RESISTANCE_B   7       /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_VOLTAGE_CHANNEL_B      8       /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_VOLTAGE_RESISTANCE_B   9       /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_CURRENT_CHANNEL_A      10      /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_CURRENT_RESISTANCE_A   11      /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_VOLTAGE_CHANNEL_A      12      /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_VOLTAGE_RESISTANCE_A   13      /* control type: numeric, callback function: (none) */
#define  PANEL_BRI_TEXTMSG                14      /* control type: textMsg, callback function: (none) */
#define  PANEL_BRI_TEXTMSG_2              15      /* control type: textMsg, callback function: (none) */

#define  PANEL_CHSW                       4
#define  PANEL_CHSW_ADD                   2       /* control type: command, callback function: SetChannelSwitchModes */
#define  PANEL_CHSW_OK                    3       /* control type: command, callback function: SetChannelSwitchModes */
#define  PANEL_CHSW_DUPLICATE             4       /* control type: command, callback function: SetChannelSwitchModes */
#define  PANEL_CHSW_REMOVE                5       /* control type: command, callback function: SetChannelSwitchModes */
#define  PANEL_CHSW_LIST                  6       /* control type: table, callback function: SetChannelSwitchModes */
#define  PANEL_CHSW_SWITCH_SETTINGS       7       /* control type: command, callback function: SetChannelSwitchPorts */

#define  PANEL_CON1                       5
#define  PANEL_CON1_PROGRESSBAR           2       /* control type: scale, callback function: (none) */

#define  PANEL_CON2                       6
#define  PANEL_CON2_CANCEL                2       /* control type: command, callback function: SetConnection */
#define  PANEL_CON2_OK                    3       /* control type: command, callback function: SetConnection */
#define  PANEL_CON2_SWITCH_PROCESS_NAME   4       /* control type: string, callback function: (none) */
#define  PANEL_CON2_LOCKIN_PROCESS_NAME   5       /* control type: string, callback function: (none) */
#define  PANEL_CON2_SOURCE_PROCESS_NAME   6       /* control type: string, callback function: (none) */
#define  PANEL_CON2_SWITCH_ISPRESENT      7       /* control type: radioButton, callback function: (none) */
#define  PANEL_CON2_NV_SERVER_NAME        8       /* control type: string, callback function: (none) */
#define  PANEL_CON2_LOCKIN_ISPRESENT      9       /* control type: radioButton, callback function: (none) */

#define  PANEL_MOD                        7
#define  PANEL_MOD_ADD                    2       /* control type: command, callback function: SetModes */
#define  PANEL_MOD_OK                     3       /* control type: command, callback function: SetModes */
#define  PANEL_MOD_DUPLICATE              4       /* control type: command, callback function: SetModes */
#define  PANEL_MOD_REMOVE                 5       /* control type: command, callback function: SetModes */
#define  PANEL_MOD_LIST                   6       /* control type: table, callback function: SetModes */
#define  PANEL_MOD_SWITCH_SETTINGS        7       /* control type: command, callback function: SetSwitchPorts */
#define  PANEL_MOD_BRIDGE_SETTINGS        8       /* control type: command, callback function: SetBridge */

#define  PANEL_PRE                        8
#define  PANEL_PRE_IMPEDANCE_B            2       /* control type: ring, callback function: PresetBridgeParameters */
#define  PANEL_PRE_SET                    3       /* control type: command, callback function: PresetBridgeParameters */
#define  PANEL_PRE_CANCEL                 4       /* control type: command, callback function: PresetBridgeParameters */
#define  PANEL_PRE_SECONDARY_B            5       /* control type: numeric, callback function: (none) */
#define  PANEL_PRE_UNIT_SECONDARY_B       6       /* control type: textMsg, callback function: (none) */
#define  PANEL_PRE_IMPEDANCE_A            7       /* control type: ring, callback function: PresetBridgeParameters */
#define  PANEL_PRE_RMS_CURRENT            8       /* control type: numeric, callback function: (none) */
#define  PANEL_PRE_PRIMARY_B              9       /* control type: numeric, callback function: (none) */
#define  PANEL_PRE_UNIT_PRIMARY_B         10      /* control type: textMsg, callback function: (none) */
#define  PANEL_PRE_SECONDARY_A            11      /* control type: numeric, callback function: (none) */
#define  PANEL_PRE_UNIT_SECONDARY_A       12      /* control type: textMsg, callback function: (none) */
#define  PANEL_PRE_TEXTMSG_2              13      /* control type: textMsg, callback function: (none) */
#define  PANEL_PRE_PRIMARY_A              14      /* control type: numeric, callback function: (none) */
#define  PANEL_PRE_UNIT_PRIMARY_A         15      /* control type: textMsg, callback function: (none) */
#define  PANEL_PRE_TEXTMSG                16      /* control type: textMsg, callback function: (none) */
#define  PANEL_PRE_TEXTMSG_3              17      /* control type: textMsg, callback function: (none) */

#define  PANEL_REC                        9
#define  PANEL_REC_BROWSE                 2       /* control type: command, callback function: SetRecord */
#define  PANEL_REC_CANCEL                 3       /* control type: command, callback function: SetRecord */
#define  PANEL_REC_OK                     4       /* control type: command, callback function: SetRecord */
#define  PANEL_REC_DATA_FILE_NAME         5       /* control type: string, callback function: (none) */
#define  PANEL_REC_SAMPLING_PERIOD        6       /* control type: numeric, callback function: (none) */
#define  PANEL_REC_SAMPLES                7       /* control type: numeric, callback function: (none) */

#define  PANEL_SCH1                       10      /* callback function: CloseHelp */
#define  PANEL_SCH1_PICTURE               2       /* control type: picture, callback function: (none) */

#define  PANEL_SCH2                       11      /* callback function: CloseHelp */
#define  PANEL_SCH2_PICTURE               2       /* control type: picture, callback function: (none) */

#define  PANEL_SWAP                       12
#define  PANEL_SWAP_LIST                  2       /* control type: table, callback function: (none) */
#define  PANEL_SWAP_OK                    3       /* control type: command, callback function: SetSwapDestinationChannel */
#define  PANEL_SWAP_CANCEL                4       /* control type: command, callback function: SetSwapDestinationChannel */

#define  PANEL_SWI                        13
#define  PANEL_SWI_PORTS                  2       /* control type: table, callback function: (none) */
#define  PANEL_SWI_OK                     3       /* control type: command, callback function: (none) */
#define  PANEL_SWI_CANCEL                 4       /* control type: command, callback function: (none) */
#define  PANEL_SWI_TEXTMSG                5       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENUBAR                          1
#define  MENUBAR_FILE                     2
#define  MENUBAR_FILE_NEW                 3       /* callback function: FileMenuNew */
#define  MENUBAR_FILE_SAVE                4       /* callback function: FileMenuSave */
#define  MENUBAR_FILE_CLOSE               5       /* callback function: FileMenuClose */
#define  MENUBAR_FILE_SEPARATOR_3         6
#define  MENUBAR_FILE_EXIT                7       /* callback function: FileMenuExit */
#define  MENUBAR_SETTINGS                 8
#define  MENUBAR_SETTINGS_CONNECTION      9       /* callback function: SettingsMenuConnection */
#define  MENUBAR_SETTINGS_SEPARATOR_4     10
#define  MENUBAR_SETTINGS_MODES           11      /* callback function: SettingsMenuModes */
#define  MENUBAR_SETTINGS_CHANNEL_SWITCH  12      /* callback function: SettingsMenuChannelSwitch */
#define  MENUBAR_SETTINGS_PRESET          13      /* callback function: SettingsMenuPreset */
#define  MENUBAR_SETTINGS_SEPARATOR_5     14
#define  MENUBAR_SETTINGS_BALANCE         15
#define  MENUBAR_SETTINGS_BALANCE_SUBMENU 16
#define  MENUBAR_SETTINGS_BALANCE_TWO_PORT 17     /* callback function: SettingsMenuBalance */
#define  MENUBAR_SETTINGS_BALANCE_VERSICAL 18     /* callback function: SettingsMenuBalance */
#define  MENUBAR_SETTINGS_BALANCE_VERSICAL_NO_KELVIN 19 /* callback function: SettingsMenuBalance */
#define  MENUBAR_SETTINGS_BALANCE_GIQS    20      /* callback function: SettingsMenuBalance */
#define  MENUBAR_SETTINGS_BALANCE_GIQS_NO_KELVIN 21 /* callback function: SettingsMenuBalance */
#define  MENUBAR_SETTINGS_RECORD          22      /* callback function: SettingsMenuRecord */
#define  MENUBAR_SETTINGS_SEPARATOR_2     23
#define  MENUBAR_SETTINGS_LOAD            24      /* callback function: SettingsMenuLoad */
#define  MENUBAR_SETTINGS_SAVE            25      /* callback function: SettingsMenuSave */
#define  MENUBAR_SETTINGS_SEPARATOR       26
#define  MENUBAR_SETTINGS_RESET           27      /* callback function: SettingsMenuReset */
#define  MENUBAR_HELP                     28
#define  MENUBAR_HELP_VERSICAL_BRIDGE     29
#define  MENUBAR_HELP_VERSICAL_BRIDGE_SUBMENU 30
#define  MENUBAR_HELP_VERSICAL_BRIDGE_SCHEMATIC 31 /* callback function: HelpMenuVersicalBridge */
#define  MENUBAR_HELP_VERSICAL_BRIDGE_BALANCE_SHORTING 32 /* callback function: HelpMenuVersicalBridge */
#define  MENUBAR_HELP_VERSICAL_BRIDGE_BALANCE_NONSHORTING 33 /* callback function: HelpMenuVersicalBridge */
#define  MENUBAR_HELP_GIQS_BRIDGE         34
#define  MENUBAR_HELP_GIQS_BRIDGE_SUBMENU 35
#define  MENUBAR_HELP_GIQS_BRIDGE_SCHEMATIC 36    /* callback function: HelpMenuGiqsBridge */
#define  MENUBAR_HELP_GIQS_BRIDGE_BALANCE 37      /* callback function: HelpMenuGiqsBridge */


     /* Callback Prototypes: */

int  CVICALLBACK Balance(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CloseHelp(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Connect(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CopyWaveformParameters(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK FileMenuClose(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK FileMenuExit(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK FileMenuNew(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK FileMenuSave(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK HelpMenuGiqsBridge(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK HelpMenuVersicalBridge(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ManagePanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PresetBridgeParameters(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ReadLockin(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RecordLockin(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RecordLockinOnTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetActiveChannel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetActiveMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetActiveSwitchMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetBalance(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetBridge(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetChannelSwitchModes(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetChannelSwitchPorts(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetConnection(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetFrequency(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetLabel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetLockinParameters(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetModes(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetRange(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetRecord(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetSwapDestinationChannel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetSwitchPorts(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK SettingsMenuBalance(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuChannelSwitch(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuConnection(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuLoad(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuModes(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuPreset(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuRecord(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuReset(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SettingsMenuSave(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK SetWaveformParameters(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartStop(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SwapActiveChannel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ToggleLock(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK UpdateActiveMode(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif