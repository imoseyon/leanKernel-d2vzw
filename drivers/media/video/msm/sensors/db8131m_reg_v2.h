#ifndef __db8131m_REG_H
#define __db8131m_REG_H

/* 1.3M mipi setting-common from PARTRON */
/* =================================================================*/
/* Name     : DB8131M Module                  */
/* Version  :                                 */
/* PLL mode : MCLK - 24MHz                    */
/* fPS      :                                 */
/* PRVIEW   : 640*480                         */
/* Made by  : Dongbu Hitek                    */
/* date     : 12/06/13 (Analog 10.0)           */
/* Model    : Gogh                            */
/* Delay    : 0xE7                            */
/* 주의사항 : 0xE796 셋팅값을 만나면 0xE796을 I2C write 하지 말고 */
/*            150ms delay 후 다음 셋팅값을 I2C write해 주면 됨    */
/*******************************************************************/


static const u16 db8131m_common[] = {
/*==================================*/
/* Preview Command (SXGA)           */
/*==================================*/
0xFFC0, /*Page mode*/
0x1001,
0xFF81,
0x89C9,
0xE764,

/*==================================*/
/* Format                           */
/*==================================*/
0xFFA1, /*Page mode*/
0x7001,
0x710D,

/*==================================*/
/* SensorCon                        */
/*==================================*/

0xFFD0, /* Page Mode*/
0x0E0A,
0x0F0D, /* ABLK_Ctrl_1			*/
0x1300, /* Gain                 */
0x1500, /* IVREFT_REFB          */
0x1834, /* ABLK_Ctrl_3          */
0x1921, /* ABLK_Ctrl_4          */
0x1A07,
0x200F, /* ABLK_Rsrv            */
0x2300, /* IVREFT2_REFB2        */
0x2400, /* IPPG_IVCM2           */
0x39C5, /* RiseSx_CDS_1_L       */
0x511F, /* Fallcreset_1_L       */
0x8365, /* RiseTran_Sig_Even_L  */
0x8567, /* FallTran_Sig_Even_L  */
0x8765, /* RiseTran_Sig_Odd_L   */
0x8967, /* FallTran_Sig_Odd_L   */
0x8B27, /* RiseCNT_EN_1_L       */
0x8D6c, /* FallCNT_EN_1_L       */
0x9115,
0xC509,
0xD1C9,
0xD407,
0xD740, /* ABLK_Ctrl_12*/
0xDBC5, /* FallScanTx_L				  */
0xED01, /* PLL_P                      */
0xEE0F, /* PLL_M                      */
0xEF00, /* PLL_S                      */
0xF840, /* ABLK_Ctrl_8 */
0xF900, /* Vblank Sleep Mode enable   */
0xFB50, /* PostADC_Gain               */



/*==================================*/
/* Analog ADF                       */
/*==================================*/
0xFF85, /* Page Mode*/

0x89C3, /* Th_AP                          */
0x8A0C, /* Th_Clamp                       */
0x8C07, /* ADF_APMinVal_ThrClampH         */
0x8D40, /* ADF_APMinVal_ThrClampL         */
0x8E00,
0x8F0C, /* ADF_APMinVal_AMP2_1_SDM        */
0x9111, /* ADF_APMinVal_AMP4_3_SDM        */
0x921F, /* ADF_APMin_FallIntTx15          */
0x9379, /* ADF_APMinVal_CDSxRange_CtrlPre */
0x9519, /* ADF_APMinVal_REFB_IVCM         */
0x962A, /* ADF_APMinVal_ref_os_PB         */
0x970E, /* ADF_APMinVal_NTx_Range         */
0x980B, /* ADF_APMaxVal_Clmp_rst          */
0x9907, /* ADF_APMaxVal_ThrClampH         */
0x9A00, /* ADF_APMaxVal_ThrClampL         */
0x9B00,
0x9C0C, /* ADF_APMaxVal_AMP2_1_SDM        */
0x9D7E, /* ADF_APMaxVal_FallIntRx         */
0x9E29, /* ADF_APMaxVal_AMP4_3_SDM        */
0x9F3F, /* ADF_APMaxVal_FallIntTx15       */
0xA079, /* ADF_APMaxVal_CDSxRange_CtrlPre */
0xA175, /* ADF_APMaxVal_FallIntLatch      */
0xA218, /* ADF_APMaxVal_REFB_IVCM         */
0xA333, /* ADF_APMaxVal_ref_os_PB         */
0xA40F, /* ADF_APMaxVal_NTx_Range         */
0xFF86, /* Page Mode*/
0x1500, /* ADF_APThrHys					 */
0x16C2, /* ADF_APFallIntTxThrLevel       */
0x1709, /* ADF_APMinVal_BP2_1_SDM        */
0x1800, /* ADF_APMidVal_BP2_1_SDM        */
0x1909, /* ADF_APMaxVal_BP2_1_SDM        */
0x1BF0,
0x1C00,
0x1D0C, /* ADF_APMidVal_AMP2_1_SDM       */
0x1F09, /* ADF_APMidVal_AMP4_3_SDM       */
0x203A, /* ADF_APMidVal_CDSxRange_CtrlPre*/
0x2218, /* ADF_APMidVal_REFB_IVCM        */
0x232A, /* ADF_APMidVal_ref_os_PB        */
0x240E, /* ADF_APMidVal_NTx_Range        */
0x2577, /* ADF_APVal_EnSiSoSht_EnSm      */
0x2DEB,

0xFF87, /* Page Mode*/
0xE120,
0xEA41, /* RiseIntTx15 */
0xDDB0,
0xF139, /* Count icnt		*/
0xFF83, /* Page Mode    */
0x6328, /* Again Table  */
0x6410, /* Again Table  */
0x65A8, /* Again Table  */
0x6650, /* Again Table  */
0x6728, /* Again Table  */
0x6814, /* Again Table  */
0xFF82, /* Pre RGB gain */
0xB909, /* Rgain        */
0xBA80,
0xBB09, /* Ggain*/
0xBC80,
0xBD09, /* Bgain*/
0xBE80,

0xFFD1,
0x0301,
0x0700,
0x0B00,
0xD196,


/*==================================*/
/* AE                               */
/*==================================*/

0xFF82, /*Page mode */
0x9101,
0x9555,
0x9655,
0x97F5,
0x985F,
0x99F5,
0x9A5F,
0x9B55,
0x9C55,
0xA93B, /* 42 OTarget	SAT 1 CCM 1 40*/
0xAA3C, /* ITarget                   */
0x9D88, /* AE Speed                  */
0x9F06, /* AE HoldBnd                */
0xA840, /* STarget                   */
0xC502, /* PeakMvStep                */
0xC638, /* PeakTgMin                 */
0xC724, /* PeakRatioTh1              */
0xC810, /* PeakRatioTh0              */
0xC905, /* PeakLuTh                  */
0xD560, /* LuxGainTB_2               */

0xFF83, /*Page mode		*/
0x2F04, /* TimeNum0   */
0x3005, /* TimeNum1   */
0x4F05, /* FrameOffset*/
0x5D00, /* Lower Fine */
0x5E8F,

0xFF82, /* Page mode			*/
0xA10A, /* AnalogGainMax         */
0xF309, /* SCLK		 24MHz      */
0xF460, /*                       */
0xF900, /* GainMax               */
0xFAB0, /* GainMax B0            */
0xFB55, /* Gain3Lut              */
0xFC30, /* Gain2Lut              */
0xFD14, /* Gain1Lut              */
0xFE12, /* GainMin               */
0xD312, /* LuxTBGainStep0        */
0xD450, /* LuxTBGainStep1        */
0xD560, /* LuxTBGainStep2        */
0xD601, /* LuxTBTimeStep0H       */
0xD700, /* LuxTBTimeStep0L       */
0xD801, /* LuxTBTimeStep1H       */
0xD990, /* LuxTBTimeStep1L       */
0xDA05, /* LuxTBTimeStep2H       */
0xDB00, /* LuxTBTimeStep2L       */
0xFF83, /* Page mode             */
0x030F, /* TimeMax60Hz  : 8fps   */
0x0408, /* Time3Lux60Hz : 15fps  */
0x0504, /* Time2Lut60Hz : 30fps  */
0x0604, /* Time1Lut60Hz : 30fps  */
0x070C, /* TimeMax50Hz  : 8fps   */
0x0807, /* Time3Lux50Hz : 14fps  */
0x0903, /* Time2Lut50Hz : 30fps  */
0x0A03, /* Time1Lut50Hz : 30fps  */
0x0B04,
0x0C4C, /* Frame Rate*/
0x1104,
0x128A, /* Frame Rate*/

0xFF85, /* Page mode*/
0xC803, /* Min Frame*/
0xC921,

0xFF82, /*Page mode*/
0x925D,


/*==================================*/
/* AWB                              */
/*==================================*/
0xFF83,
0x7983,
0x8200,
0x8601,
0x8780,
0x9005,
0x9140,
0x9404,
0x95C0,
0x98D4,
0xA228,
0xA300,
0xA40F,
0xAD65,
0xAE80,
0xAF20,
0xB410,
0xB554,
0xB6BD,
0xB774,
0xB89D,
0xBA4F,
0xBF0C,
0xC080,

0xFF84, /* Page mode			*/
0x3D00, /* AWB_LuxConst1_0	      */
0x3E00, /* AWB_LuxConst1_1	      */
0x3F06, /* AWB_LuxConst1_2	      */
0x4020, /* AWB_LuxConst1_3	      */
0x4107, /* AWB_LuxConst2_0	      */
0x4253, /* AWB_LuxConst2_1	      */
0x4300, /* AWB_LuxConst2_2	      */
0x4400, /* AWB_LuxConst2_3	      */
0x4901, /* Threshold_indoor	      */
0x4A00,
0x4B01, /* Threshold_outdoor	    */
0x4C80,
0x5503, /* AWB_Weight_Genernal_0	*/
0x5610, /* AWB_Weight_Genernal_1	*/
0x5714, /* AWB_Weight_Genernal_2	*/
0x5807, /* AWB_Weight_Genernal_3	*/
0x5904, /* AWB_Weight_Genernal_4	*/
0x5A00, /* AWB_Weight_Genernal_5	*/
0x5B01, /* AWB_Weight_Genernal_6	*/
0x5C01, /* AWB_Weight_Genernal_7	*/
0x5D01, /* AWB_Weight_Indoor_0	  */
0x5E08, /* AWB_Weight_Indoor_1	  */
0x5F12, /* AWB_Weight_Indoor_2	  */
0x6008, /* AWB_Weight_Indoor_3	  */
0x6120, /* AWB_Weight_Indoor_4	  */
0x6200, /* AWB_Weight_Indoor_5	  */
0x6318, /* AWB_Weight_Indoor_6	  */
0x6414, /* AWB_Weight_Indoor_7	  */
0x6503, /* AWB_Weight_Outdoor_0	  */
0x6601, /* AWB_Weight_Outdoor_1	  */
0x6701, /* AWB_Weight_Outdoor_2	  */
0x6840, /* AWB_Weight_Outdoor_3	  */
0x6901, /* AWB_Weight_Outdoor_4	  */
0x6A02, /* AWB_Weight_Outdoor_5	  */
0x6B01, /* AWB_Weight_Outdoor_6	  */
0x6C01, /* AWB_Weight_Outdoor_7	  */
0xFF85, /* Page mode	          */
0xE20C, /* AWB_unicolorzone	      */

0xFF83, /*Page mode			*/
0xCB04, /* Min Rgain	    */
0xCC58, /* 38             */
0xCD07, /* Max Rgain      */
0xCE20, /*                */
0xCF04, /* Min Bgain      */
0xD020, /*                */
0xD106, /* Max BGain 690  */
0xd268, /* 70             */

/*==================================*/
/* AWB STE                          */
/*==================================  */
0xFFA1, /* Page mode                  */
0x9C18, /* AWBLuThL                 */
0x9DF0, /* AWBLuThH				      */
0xA063, /* AWBZone0LTx - Flash	    */
0xA17A, /* AWBZone0LTy	            */
0xA269, /* AWBZone0RBx	            */
0xA36F, /* AWBZone0RBy	            */
0xA47C, /* AWBZone1LTx - Cloudy	    */
0xA54C, /* AWBZone1LTy	            */
0xA68C, /* AWBZone1RBx	            */
0xA73C, /* AWBZone1RBy	            */
0xA86C, /* AWBZone2LTx - D65	      */
0xA95E, /* AWBZone2LTy	            */
0xAA84, /* AWBZone2RBx	            */
0xAB44, /* AWBZone2RBy	            */
0xAC60, /* AWBZone3LTx - Fluorecent	*/
0xAD64, /* AWBZone3LTy	            */
0xAE78, /* AWBZone3RBx	            */
0xAF4B, /* AWBZone3RBy              */
0xB048, /* AWBZone4LTx - CWF	      */
0xB168, /* AWBZone4LTy	            */
0xB264, /* AWBZone4RBx	            */
0xB34D, /* AWBZone4RBy	            */
0xB400, /* AWBZone5LTx - TL84	      */
0xB500, /* AWBZone5LTy	            */
0xB600, /* AWBZone5RBx	            */
0xB700, /* AWBZone5RBy	            */
0xB841, /* AWBZone6LTx - A	        */
0xB97B, /* AWBZone6LTy	            */
0xBA54, /* AWBZone6RBx	            */
0xBB6A, /* AWBZone6RBy	            */
0xBC3C, /* AWBZone7LTx - Horizon	  */
0xBD90, /* AWBZone7LTy	            */
0xBE4C, /* AWBZone7RBx	            */
0xBF73, /* AWBZone7RBy	            */
0xC05B, /* AWBZone8LTx - Skin	      */
0xC185, /* AWBZone8LTy	            */
0xC260, /* AWBZone8RBx	            */
0xC37F, /* AWBZone8RBy              */

/*==================================*/
/* UR                               */
/*==================================*/
0xFF87, /*Page mode                */
0xC922, /* AWBTrim                 */
0xFF86, /*Page mode                */
0x143E, /* CCM sum 1               */
0xFF85, /*Page mode                */
0x0605, /* Saturation CCM 1        */
0x8640, /* saturation level        */
0x0700, /* sup hysteresis          */

/*==================================*/
/* CCM D65                          */
/*==================================*/
0xFF83, /* Page Mode                */
0xEA00, /* CrcMtx11_Addr1       */
0xEB6A, /* CrcMtx11_Addr0       */
0xECFF, /* CrcMtx12_Addr1       */
0xEDD7, /* CrcMtx12_Addr0       */
0xEEFF, /* CrcMtx13_Addr1       */
0xEFFF, /* CrcMtx13_Addr0       */
0xF0FF, /* CrcMtx21_Addr1       */
0xF1F7, /* CrcMtx21_Addr0       */
0xF200, /* CrcMtx22_Addr1       */
0xF353, /* CrcMtx22_Addr0         */
0xF4FF, /* CrcMtx23_Addr1       */
0xF5F6, /* CrcMtx23_Addr0       */
0xF6FF, /* CrcMtx31_Addr1       */
0xF7FF, /* CrcMtx31_Addr0       */
0xF8FF, /* CrcMtx32_Addr1       */
0xF9B7, /* CrcMtx32_Addr0         */
0xFA00, /* CrcMtx33_Addr1       */
0xFB81, /* CrcMtx33_Addr0       */

/*==================================*/
/* CCM Coolwhite                    */
/*==================================*/
0xFF83, /* Page Mode                */
0xFC00, /* CrcMtx11_Addr1       */
0xFD6B, /* CrcMtx11_Addr0       */
0xFF85, /* Page Mode                */
0xE0FF, /* CrcMtx12_Addr1       */
0xE1DB, /* CrcMtx12_Addr0       */
0xFF84, /* Page Mode                */
0x00FF, /* CrcMtx13_Addr1       */
0x01FA, /* CrcMtx13_Addr0       */
0x02FF, /* CrcMtx21_Addr1       */
0x03F3, /* CrcMtx21_Addr0       */
0x0400, /* CrcMtx22_Addr1       */
0x0554, /* CrcMtx22_Addr0       */
0x06FF, /* CrcMtx23_Addr1       */
0x07F9, /* CrcMtx23_Addr0       */
0x08FF, /* CrcMtx31_Addr1       */
0x09FA, /* CrcMtx31_Addr0       */
0x0AFF, /* CrcMtx32_Addr1       */
0x0BC4, /* CrcMtx32_Addr0       */
0x0C00, /* CrcMtx33_Addr1       */
0x0D82, /* CrcMtx33_Addr0       */


/*==================================*/
/* CCM A                            */
/*==================================*/
0xFF84, /* Page Mode                */
0x0E00, /* CrcMtx11_Addr1       */
0x0F6B, /* CrcMtx11_Addr0       */
0x10FF, /* CrcMtx12_Addr1       */
0x11DA, /* CrcMtx12_Addr0       */
0x12FF, /* CrcMtx13_Addr1       */
0x13FC, /* CrcMtx13_Addr0       */
0x14FF, /* CrcMtx21_Addr1       */
0x15F5, /* CrcMtx21_Addr0       */
0x1600, /* CrcMtx22_Addr1       */
0x175B, /* CrcMtx22_Addr0       */
0x18FF, /* CrcMtx23_Addr1       */
0x19F0, /* CrcMtx23_Addr0       */
0x1AFF, /* CrcMtx31_Addr1       */
0x1BFA, /* CrcMtx31_Addr0       */
0x1CFF, /* CrcMtx32_Addr1       */
0x1DC4, /* CrcMtx32_Addr0       */
0x1E00, /* CrcMtx33_Addr1       */
0x1F82, /* CrcMtx33_Addr0       */


/*==================================*/
/* Outdoor D65                      */
/*==================================*/
0xFF86, /* Page mode				*/
0x4500, /* CCM LuxThreshold        */
0x4698, /* CCM LuxThreshold        */
0xFF85, /* Page mode               */
0xFE15, /* Outdoor CCM On          */
0xEC00, /* gAwb_s16AdapCCMTbl_0    */
0xED72, /* gAwb_s16AdapCCMTbl_1    */
0xEEFF, /* gAwb_s16AdapCCMTbl_2    */
0xEFBC, /* gAwb_s16AdapCCMTbl_3    */
0xF000, /* gAwb_s16AdapCCMTbl_4    */
0xF112, /* gAwb_s16AdapCCMTbl_5    */
0xF2FF, /* gAwb_s16AdapCCMTbl_6    */
0xF3F4, /* gAwb_s16AdapCCMTbl_7    */
0xF400, /* gAwb_s16AdapCCMTbl_8    */
0xF54F, /* gAwb_s16AdapCCMTbl_9    */
0xF6FF, /* gAwb_s16AdapCCMTbl_10   */
0xF7FD, /* gAwb_s16AdapCCMTbl_11   */
0xF8FF, /* gAwb_s16AdapCCMTbl_12   */
0xF9F5, /* gAwb_s16AdapCCMTbl_13   */
0xFAFF, /* gAwb_s16AdapCCMTbl_14   */
0xFBCE, /* gAwb_s16AdapCCMTbl_15   */
0xFC00, /* gAwb_s16AdapCCMTbl_16   */
0xFD7D, /* gAwb_s16AdapCCMTbl_17   */


/*==================================*/
/* ADF                              */
/*==================================*/
/* ISP setting   */
0xFFA0, /* Page mode						*/
0x1080, /* BLC Th	                          */
0x1109, /* BLC Separator	                  */
0x6073, /* CDC: Dark CDC ON                 */
0x611F, /* Six CDC_Edge En, Slash EN        */
0x690C, /* Slash direction Line threshold   */
0x6A60, /* Slash direction Pixel threshold  */
0xC204, /* NSF: directional smoothing       */
0xD051, /* DEM: pattern detection           */
0xFFA1, /* Page mode*/
0x3000, /* EDE: Luminane adaptation off*/
0x3240, /* EDE: Adaptation slope       */
0x3400, /* EDE: x1 point               */
0x3516, /* EDE: x1 point               */
0x3600, /* EDE: x2 point               */
0x3730, /* EDE: x2 point               */
0x3A00, /* EDE: Adaptation left margin */
0x3B20, /* EDE: Adaptation right margin*/
0x3CFF, /* EDE: rgb edge threshol      */

/* Adaptive Setting*/
0xFF85, /*Page mode*/
/* BGT*/
0x1721, /* BGT lux level threshold*/
0x260C, /* MinVal               */
0x3c00, /* MaxVal               */

/* BGT2 */
0xFF86, /*Page mode*/
0x680C, /* BGTLux1*/
0x690B, /* BGTLux2*/
0x6a09, /* BGTLux3*/
0x6b00, /* Value1 */
0x6c00, /* Value2 */
0x6d03, /* Value3 */

/* SAT */
0x6F0B, /* SATLux1   */
0x700C, /* SATLux2   */
0x7102, /* SATZone   */
0x7202, /* SATZone   */
0x7302, /* SATValue1 */
0x7400, /* SATValue2 */

/* CNT */
0xFF85, /* Page mode*/
0x1898, /* CNT lux level threshold*/
0x2702, /* MinVal               */
0x3d02, /* MaxVal               */

/* NSF */
0x12A5, /* NSF lux level threshold*/
0x221A, /* u8MinVal_NSF1 28       */
0x2340, /* u8MinVal_NSF2 70       */
0x3810, /* u8MaxVal_NSF1	      */
0x3928, /* u8MaxVal_NSF2	      */
0xFF86, /*Page mode*/
0x1206, /* u8MinVal_NSF3*/
0x1306, /* u8MaxVal_NSF3*/

/* NSF NEW thresold*/
0xFF85, /*Page mode*/
0xE822,
0xE950,
0xEA0E,
0xEB18,
/* GDC*/
0xFF85, /*Page mode*/
0x15F4, /* GDC lux level threshold */
0x2D20, /* u8MinVal_GDC1           */
0x2E30, /* u8MinVal_GDC2           */
0x4320, /* u8MaxVal_GDC1           */
0x4430, /* u8MaxVal_GDC2           */

/* ISP  Edge */
0xFF85, /*Page mode				*/
0x04FB, /* FuncCtlr_ADF         */
0x14A5, /* u8ThrLevel_EDE       */
0x2808, /* u8MinVal_EDE_CP      */
0x2907, /* u8MinVal_EDE1        */
0x2A08, /* u8MinVal_EDE2        */
0x2B05, /* u8MinVal_EDE_OFS     */
0x2C22, /* u8MinVal_SG          */
0x3E02, /* u8MaxVal_EDE_CP      */
0x3F08, /* u8MaxVal_EDE1 Edge   */
0x4009, /* u8MaxVal_EDE2        */
0x4107, /* u8MaxVal_EDE_OFS     */
0x4222, /* u8MaxVal_SG          */

/* Gamma Adaptive*/
0xFF85, /*Page mode*/
0x1670,
0x4700, /* Min_Gamma_0	*/
0x4816, /* Min_Gamma_1    */
0x4925, /* Min_Gamma_2    */
0x4A37, /* Min_Gamma_3    */
0x4B45, /* Min_Gamma_4    */
0x4C51, /* Min_Gamma_5    */
0x4D65, /* Min_Gamma_6    */
0x4E77, /* Min_Gamma_7    */
0x4F87, /* Min_Gamma_8    */
0x5095, /* Min_Gamma_9    */
0x51AF, /* Min_Gamma_10   */
0x52C6, /* Min_Gamma_11   */
0x53DB, /* Min_Gamma_12   */
0x54E5, /* Min_Gamma_13   */
0x55EE, /* Min_Gamma_14   */
0x56F7, /* Min_Gamma_15   */
0x57FF, /* Min_Gamma_16   */

0x5800, /* Max_Gamma_0          */
0x5904, /* Max_Gamma_1          */
0x5a16, /* Max_Gamma_2          */
0x5b30, /* Max_Gamma_3          */
0x5c46, /* Max_Gamma_4		skin  */
0x5d58, /* Max_Gamma_5		skin  */
0x5e6E, /* Max_Gamma_6		skin  */
0x5f7F, /* Max_Gamma_7          */
0x608F, /* Max_Gamma_8          */
0x619F, /* Max_Gamma_9          */
0x62B7, /* Max_Gamma_10         */
0x63CC, /* Max_Gamma_11         */
0x64DE, /* Max_Gamma_12         */
0x65E7, /* Max_Gamma_13         */
0x66F0, /* Max_Gamma_14         */
0x67F8, /* Max_Gamma_15         */
0x68FF, /* Max_Gamma_16         */


/* CCM Saturation Level	*/
0xFF85, /* Page mode         */
0x1A21, /* 64 SUP Threshold	*/
0x3070, /* MinSUP	        */

/* LSC */
0x0F43, /* LVLSC lux level threshold         */
0x10E3, /* LSLSC Light source , threshold lux*/
0x1BA0,
0x31C0,

/* Lens Shading	*/
0xFFA0, /*Page mode   */
0x4380, /* RH7 rrhrhh */
0x4480, /* RV         */
0x4580, /* RH	      */
0x4680, /* RV         */
0x4780, /* GH         */
0x4880, /* GV         */
0x4980, /* GH         */
0x4A80, /* GV	      */
0x4B80, /* BH         */
0x4C80, /* BV         */
0x4D80, /* BH         */
0x4E80, /* BV         */
0x5290, /* GGain      */
0x5320, /* GGain      */
0x5400, /* GGain      */
/*Max Shading */
0xFF86, /* Page mode   */
0x51B8, /* Rgain1      */
0x5230, /* Rgain2      */
0x5300, /* Rgain3      */
0x5490, /* Bgain1      */
0x5510, /* Bgain2      */
0x5600, /* Bgain3      */
0x5790, /* GGain1     */
0x5820, /* GGain2     */
0x5900, /* GGain3     */
/*Min Shading */
0x48B0, /* Rgain1      */
0x4920, /* Rgain2      */
0x4A00, /* Rgain3      */
0x4B90, /* Bgain1      */
0x4C18, /* Bgain2	  */
0x4D00, /* Bgain3      */
0x4E90, /* GGain1     */
0x4F20, /* GGain2     */
0x5000, /* GGain3     */

/*DDC */
0xFF85,
0x6910,
0x6A00,
0x6B00,
0x6C00,
0x6D20,

/* LineLength*/
0xFF87, /*Page mode */
0xDC05,
0xDDB0,
0xd500, /* Flip*/


/*==================================*/
/* MIPI                             */
/*==================================*/
0xFFB0, /*Page mode*/
0x5402, /* MIPI PHY_HS_TX_CTRL*/
0x3805, /* MIPI DPHY_CTRL_SET*/
0x3C81,
0x5011, /* MIPI Current 01 -> 11 */

/* SXGA PR*/
0xFF85, /*Page mode */
0xB71E,
0xB80A, /* gPT_u8PR_Active_SXGA_WORD_COUNT_Addr0*/
0xB900, /* gPT_u8PR_Active_SXGA_WORD_COUNT_Addr1*/
0xBC04, /* gPT_u8PR_Active_SXGA_DPHY_CLK_TIME_Addr3*/
0xFF87, /* Page mode*/
0x0C00, /* start Y*/
0x0D20, /* start Y*/
0x1003, /* end Y*/
0x11E0, /* end Y*/

/* Recoding*/
0xFF86, /*Page mode*/
0x371E,
0x3805, /*gPT_u8PR_Active_720P_WORD_COUNT_Addr0*/
0x3900, /*gPT_u8PR_Active_720P_WORD_COUNT_Addr1*/
0x3C04, /*gPT_u8PR_Active_720P_DPHY_CLK_TIME_Addr3*/

0xFF87,
0x2302, /*gPR_Active_720P_u8SensorCtrl_Addr*/
0x2472, /*gPR_Active_720P_u8SensorMode_Addr*/
0x2501, /*gPR_Active_720P_u8PLL_P_Addr*/
0x260F, /*gPR_Active_720P_u8PLL_M_Addr*/
0x2700, /*gPR_Active_720P_u8PLL_S_Addr*/
0x2800, /*gPR_Active_720P_u8PLL_Ctrl_Addr*/
0x2901, /*gPR_Active_720P_u8src_clk_sel_Addr*/
0x2A00, /*gPR_Active_720P_u8output_pad_status_Addr*/
0x2B3F, /*gPR_Active_720P_u8ablk_ctrl_10_Addr*/
0x2CFF, /*gPR_Active_720P_u8BayerFunc_Addr*/
0x2DFF, /*gPR_Active_720P_u8RgbYcFunc_Addr*/
0x2E00, /*gPR_Active_720P_u8ISPMode_Addr*/
0x2F02, /*gPR_Active_720P_u8SCLCtrl_Addr*/
0x3001, /*gPR_Active_720P_u8SCLHorScale_Addr0*/
0x31FF, /*gPR_Active_720P_u8SCLHorScale_Addr1*/
0x3203, /*gPR_Active_720P_u8SCLVerScale_Addr0*/
0x33FF, /*gPR_Active_720P_u8SCLVerScale_Addr1*/
0x3400, /*gPR_Active_720P_u8SCLCropStartX_Addr0*/
0x3500, /*gPR_Active_720P_u8SCLCropStartX_Addr1*/
0x3600, /*gPR_Active_720P_u8SCLCropStartY_Addr0*/
0x3710, /*gPR_Active_720P_u8SCLCropStartY_Addr1*/
0x3802, /*gPR_Active_720P_u8SCLCropEndX_Addr0*/
0x3980, /*gPR_Active_720P_u8SCLCropEndX_Addr1*/
0x3A01, /*gPR_Active_720P_u8SCLCropEndY_Addr0*/
0x3BF0, /*gPR_Active_720P_u8SCLCropEndY_Addr1*/
0x3C01, /*gPR_Active_720P_u8OutForm_Addr*/
0x3D0C, /*gPR_Active_720P_u8OutCtrl_Addr*/
0x3E04, /*gPR_Active_720P_u8AEWinStartX_Addr*/
0x3F04, /*gPR_Active_720P_u8AEWinStartY_Addr*/
0x4066, /*gPR_Active_720P_u8MergedWinWidth_Addr*/
0x415E, /*gPR_Active_720P_u8MergedWinHeight_Addr*/
0x4204, /*gPR_Active_720P_u8AEHistWinAx_Addr*/
0x4304, /*gPR_Active_720P_u8AEHistWinAy_Addr*/
0x4498, /*gPR_Active_720P_u8AEHistWinBx_Addr*/
0x4578, /*gPR_Active_720P_u8AEHistWinBy_Addr*/
0x4622, /*gPR_Active_720P_u8AWBTrim_Addr*/
0x4728, /*gPR_Active_720P_u8AWBCTWinAx_Addvr*/
0x4820, /*gPR_Active_720P_u8AWBCTWinAy_Addr*/
0x4978, /*gPR_Active_720P_u8AWBCTWinBx_Addr*/
0x4A60, /*gPR_Active_720P_u8AWBCTWinBy_Addr*/
0x4B03, /*gPR_Active_720P_u16AFCFrameLength_0*/
0x4C00, /*gPR_Active_720P_u16AFCFrameLength_1*/

/*VGA PR*/
0xFF86, /*Page mode*/
0x2E1E,
0x2F05, /* gPT_u8PR_Active_VGA_WORD_COUNT_Addr0*/
0x3000, /* gPT_u8PR_Active_VGA_WORD_COUNT_Addr1*/
0x3304, /* gPT_u8PR_Active_VGA_DPHY_CLK_TIME_Addr3*/

0xFF87, /*Page mode*/
0x4D00, /*gPR_Active_VGA_u8SensorCtrl_Addr*/
0x4E72, /*gPR_Active_VGA_u8SensorMode_Addr*/
0x4F01, /*gPR_Active_VGA_u8PLL_P_Addr*/
0x500F, /*gPR_Active_VGA_u8PLL_M_Addr*/
0x5100, /*gPR_Active_VGA_u8PLL_S_Addr*/
0x5200, /*gPR_Active_VGA_u8PLL_Ctrl_Addr*/
0x5301, /*gPR_Active_VGA_u8src_clk_sel_Addr*/
0x5400, /*gPR_Active_VGA_u8output_pad_status_Addr*/
0x553F, /*gPR_Active_VGA_u8ablk_ctrl_10_Addr*/
0x56FF, /*gPR_Active_VGA_u8BayerFunc_Addr*/
0x57FF, /*gPR_Active_VGA_u8RgbYcFunc_Addr*/
0x5800, /*gPR_Active_VGA_u8ISPMode_Addr*/
0x5902, /*gPR_Active_VGA_u8SCLCtrl_Addr*/
0x5A01, /*gPR_Active_VGA_u8SCLHorScale_Addr0*/
0x5BFF, /*gPR_Active_VGA_u8SCLHorScale_Addr1*/
0x5C01, /*gPR_Active_VGA_u8SCLVerScale_Addr0*/
0x5DFF, /*gPR_Active_VGA_u8SCLVerScale_Addr1*/
0x5E00, /*gPR_Active_VGA_u8SCLCropStartX_Addr0*/
0x5F00, /*gPR_Active_VGA_u8SCLCropStartX_Addr1*/
0x6000, /*gPR_Active_VGA_u8SCLCropStartY_Addr0*/
0x6110, /*gPR_Active_VGA_u8SCLCropStartY_Addr1*/
0x6202, /*gPR_Active_VGA_u8SCLCropEndX_Addr0*/
0x6380, /*gPR_Active_VGA_u8SCLCropEndX_Addr1*/
0x6401, /*gPR_Active_VGA_u8SCLCropEndY_Addr0*/
0x65F0, /*gPR_Active_VGA_u8SCLCropEndY_Addr1*/



0xFFC0, /*Page mode*/
0x1041,
0xE764, /*Wait  100*/

/* Self-Cam END of Initial*/
};

/* Set-data based on SKT VT standard ,when using 3G network
* VGA 8fps
*//*Fixed 8fps Mode*/
static const u16 db8131m_vt_common[] = {
0xFFC0,
0x1001,
0xFF81, /*Page mode*/
0x89C9,
0xE764, /*Wait  100*/

/*==================================*/
/* Format                           */
/*==================================*/
0xFFA1, /*Page mode*/
0x7001,
0x710D,

/*==================================*/
/* SensorCon                        */
/*==================================*/

0xFFD0, /* Page Mode*/
0x0E0A,
0x0F0D, /* ABLK_Ctrl_1			*/
0x1300, /* Gain                 */
0x1500, /* IVREFT_REFB          */
0x1834, /* ABLK_Ctrl_3          */
0x1921, /* ABLK_Ctrl_4          */
0x1A07,
0x200F, /* ABLK_Rsrv            */
0x2300, /* IVREFT2_REFB2        */
0x2400, /* IPPG_IVCM2           */
0x39C5, /* RiseSx_CDS_1_L       */
0x511F, /* Fallcreset_1_L       */
0x8365, /* RiseTran_Sig_Even_L  */
0x8567, /* FallTran_Sig_Even_L  */
0x8765, /* RiseTran_Sig_Odd_L   */
0x8967, /* FallTran_Sig_Odd_L   */
0x8B27, /* RiseCNT_EN_1_L       */
0x8D6c, /* FallCNT_EN_1_L       */
0x9115,
0xC509,
0xD1C9,
0xD407,
0xD740, /* ABLK_Ctrl_12*/
0xDBC5, /* FallScanTx_L				  */
0xED01, /* PLL_P                      */
0xEE0F, /* PLL_M                      */
0xEF00, /* PLL_S                      */
0xF840, /* ABLK_Ctrl_8 */
0xF900, /* Vblank Sleep Mode enable   */
0xFB50, /* PostADC_Gain               */


/*==================================*/
/* Analog ADF                       */
/*==================================*/
0xFF85, /* Page Mode*/

0x89C3, /* Th_AP                          */
0x8A0C, /* Th_Clamp                       */
0x8C07, /* ADF_APMinVal_ThrClampH         */
0x8D40, /* ADF_APMinVal_ThrClampL         */
0x8E00,
0x8F0C, /* ADF_APMinVal_AMP2_1_SDM        */
0x9111, /* ADF_APMinVal_AMP4_3_SDM        */
0x921F, /* ADF_APMin_FallIntTx15          */
0x9379, /* ADF_APMinVal_CDSxRange_CtrlPre */
0x9519, /* ADF_APMinVal_REFB_IVCM         */
0x962A, /* ADF_APMinVal_ref_os_PB         */
0x970E, /* ADF_APMinVal_NTx_Range         */
0x980B, /* ADF_APMaxVal_Clmp_rst          */
0x9907, /* ADF_APMaxVal_ThrClampH         */
0x9A00, /* ADF_APMaxVal_ThrClampL         */
0x9B00,
0x9C0C, /* ADF_APMaxVal_AMP2_1_SDM        */
0x9D7E, /* ADF_APMaxVal_FallIntRx         */
0x9E29, /* ADF_APMaxVal_AMP4_3_SDM        */
0x9F3F, /* ADF_APMaxVal_FallIntTx15       */
0xA079, /* ADF_APMaxVal_CDSxRange_CtrlPre */
0xA175, /* ADF_APMaxVal_FallIntLatch      */
0xA218, /* ADF_APMaxVal_REFB_IVCM         */
0xA333, /* ADF_APMaxVal_ref_os_PB         */
0xA40F, /* ADF_APMaxVal_NTx_Range         */

0xFF86, /* Page Mode*/
0x1500, /* ADF_APThrHys					 */
0x16C2, /* ADF_APFallIntTxThrLevel       */
0x1709, /* ADF_APMinVal_BP2_1_SDM        */
0x1800, /* ADF_APMidVal_BP2_1_SDM        */
0x1909, /* ADF_APMaxVal_BP2_1_SDM        */
0x1BF0,
0x1C00,
0x1D0C, /* ADF_APMidVal_AMP2_1_SDM       */
0x1F09, /* ADF_APMidVal_AMP4_3_SDM       */
0x203A, /* ADF_APMidVal_CDSxRange_CtrlPre*/
0x2218, /* ADF_APMidVal_REFB_IVCM        */
0x232A, /* ADF_APMidVal_ref_os_PB        */
0x240E, /* ADF_APMidVal_NTx_Range        */
0x2577, /* ADF_APVal_EnSiSoSht_EnSm      */
0x2DEB,

0xFF87, /* Page Mode*/
0xE120,
0xEA41, /* RiseIntTx15 */
0xDDB0,
0xF139, /* Count icnt		*/

0xFF83, /* Page Mode    */
0x6328, /* Again Table  */
0x6410, /* Again Table  */
0x65A8, /* Again Table  */
0x6650, /* Again Table  */
0x6728, /* Again Table  */
0x6814, /* Again Table  */

0xFF82, /* Pre RGB gain */
0xB909, /* Rgain        */
0xBA80,
0xBB09, /* Ggain*/
0xBC80,
0xBD09, /* Bgain*/
0xBE80,

0xFFD1,
0x0301,
0x0700,
0x0B00,
0xD196,



0xFF82, /*Page mode */
0x9102,
0x9555,
0x9655,
0x97F5,
0x985F,
0x99F5,
0x9A5F,
0x9B55,
0x9C55,
0xA93B, /* 42 OTarget	SAT 1 CCM 1 40*/
0xAA3C, /* ITarget                   */
0x9D88, /* AE Speed                  */
0x9F06, /* AE HoldBnd                */
0xA840, /* STarget                   */
0xC502, /* PeakMvStep                */
0xC638, /* PeakTgMin                 */
0xC724, /* PeakRatioTh1              */
0xC810, /* PeakRatioTh0              */
0xC905, /* PeakLuTh                  */
0xD560, /* LuxGainTB_2               */

0xFF83, /*Page mode		*/
0x2F04, /* TimeNum0   */
0x3005, /* TimeNum1   */
0x4F05, /* FrameOffset*/
0x5D00, /* Lower Fine */
0x5E8F,

0xFF82, /* Page mode			*/
0xA10A, /* AnalogGainMax         */
0xF309, /* SCLK		 24MHz      */
0xF460,
0xF900,
0xFAB0,
0xFB55,
0xFC30,
0xFD14,
0xFE12,
0xD312,
0xD450,
0xD560,
0xD601,
0xD700,
0xD801,
0xD990,
0xDA05,
0xDB00,
0xFF83,
0x030F,
0x040E,
0x050C,
0x0608,
0x070C,
0x080B,
0x090A,
0x0A06,
0x0B08,
0x0C11,
0x1107,
0x12BB,

0xFF85,
0xC803,
0xC921,

0xFF82,
0x925D,


/*==================================*/
/* AWB                              */
/*==================================*/
0xFF83,
0x7983,
0x8200,
0x8601,
0x8780,
0x9005,
0x9140,
0x9404,
0x95C0,
0x98D4,
0xA228,
0xA300,
0xA40F,
0xAD65,
0xAE80,
0xAF20,
0xB410,
0xB554,
0xB6BD,
0xB774,
0xB89D,
0xBA4F,
0xBF0C,
0xC080,

0xFF84, /* Page mode			*/
0x3D00, /* AWB_LuxConst1_0	      */
0x3E00, /* AWB_LuxConst1_1	      */
0x3F06, /* AWB_LuxConst1_2	      */
0x4020, /* AWB_LuxConst1_3	      */
0x4107, /* AWB_LuxConst2_0	      */
0x4253, /* AWB_LuxConst2_1	      */
0x4300, /* AWB_LuxConst2_2	      */
0x4400, /* AWB_LuxConst2_3	      */
0x4901, /* Threshold_indoor	      */
0x4A00,
0x4B01, /* Threshold_outdoor	    */
0x4C80,
0x5503, /* AWB_Weight_Genernal_0	*/
0x5610, /* AWB_Weight_Genernal_1	*/
0x5714, /* AWB_Weight_Genernal_2	*/
0x5807, /* AWB_Weight_Genernal_3	*/
0x5904, /* AWB_Weight_Genernal_4	*/
0x5A00, /* AWB_Weight_Genernal_5	*/
0x5B01, /* AWB_Weight_Genernal_6	*/
0x5C01, /* AWB_Weight_Genernal_7	*/
0x5D01, /* AWB_Weight_Indoor_0	  */
0x5E08, /* AWB_Weight_Indoor_1	  */
0x5F12, /* AWB_Weight_Indoor_2	  */
0x6008,
0x6120,
0x6200,
0x6318,
0x6414,
0x6503,
0x6601,
0x6701,
0x6840,
0x6901,
0x6A02,
0x6B01,
0x6C01,
0xFF85,
0xE20C,

0xFF83,
0xCB04,
0xCC58,
0xCD07,
0xCE20,
0xCF04,
0xD020,
0xD106,
0xd268,

/*==================================*/
/* AWB STE                          */
/*==================================  */
0xFFA1, /* Page mode                  */
0x9C18, /* AWBLuThL                 */
0x9DF0, /* AWBLuThH				      */
0xA063, /* AWBZone0LTx - Flash	    */
0xA17A, /* AWBZone0LTy	            */
0xA269, /* AWBZone0RBx	            */
0xA36F, /* AWBZone0RBy	            */
0xA47C, /* AWBZone1LTx - Cloudy	    */
0xA54C, /* AWBZone1LTy	            */
0xA68C, /* AWBZone1RBx	            */
0xA73C, /* AWBZone1RBy	            */
0xA86C, /* AWBZone2LTx - D65	      */
0xA95E, /* AWBZone2LTy	            */
0xAA84, /* AWBZone2RBx	            */
0xAB44, /* AWBZone2RBy	            */
0xAC60, /* AWBZone3LTx - Fluorecent	*/
0xAD64, /* AWBZone3LTy	            */
0xAE78, /* AWBZone3RBx	            */
0xAF4B, /* AWBZone3RBy              */
0xB048, /* AWBZone4LTx - CWF	      */
0xB168, /* AWBZone4LTy	            */
0xB264, /* AWBZone4RBx	            */
0xB34D, /* AWBZone4RBy	            */
0xB400, /* AWBZone5LTx - TL84	      */
0xB500, /* AWBZone5LTy	            */
0xB600, /* AWBZone5RBx	            */
0xB700, /* AWBZone5RBy	            */
0xB841, /* AWBZone6LTx - A	        */
0xB97B, /* AWBZone6LTy	            */
0xBA54, /* AWBZone6RBx	            */
0xBB6A, /* AWBZone6RBy	            */
0xBC3C, /* AWBZone7LTx - Horizon	  */
0xBD90, /* AWBZone7LTy	            */
0xBE4C, /* AWBZone7RBx	            */
0xBF73, /* AWBZone7RBy	            */
0xC05B, /* AWBZone8LTx - Skin	      */
0xC185, /* AWBZone8LTy	            */
0xC260, /* AWBZone8RBx	            */
0xC37F, /* AWBZone8RBy              */

/*==================================*/
/* UR                               */
/*==================================*/
0xFF87, /*Page mode                */
0xC922, /* AWBTrim                 */
0xFF86, /*Page mode                */
0x143E, /* CCM sum 1               */
0xFF85, /*Page mode                */
0x0605, /* Saturation CCM 1        */
0x8640, /* saturation level        */
0x0700, /* sup hysteresis          */

/*==================================*/
/* CCM D65                          */
/*==================================*/
0xFF83, /* Page Mode                */
0xEA00, /* CrcMtx11_Addr1       */
0xEB6A, /* CrcMtx11_Addr0       */
0xECFF, /* CrcMtx12_Addr1       */
0xEDD7, /* CrcMtx12_Addr0       */
0xEEFF, /* CrcMtx13_Addr1       */
0xEFFF, /* CrcMtx13_Addr0       */
0xF0FF, /* CrcMtx21_Addr1       */
0xF1F7, /* CrcMtx21_Addr0       */
0xF200, /* CrcMtx22_Addr1       */
0xF353, /* CrcMtx22_Addr0         */
0xF4FF, /* CrcMtx23_Addr1       */
0xF5F6, /* CrcMtx23_Addr0       */
0xF6FF, /* CrcMtx31_Addr1       */
0xF7FF, /* CrcMtx31_Addr0       */
0xF8FF, /* CrcMtx32_Addr1       */
0xF9B7, /* CrcMtx32_Addr0         */
0xFA00, /* CrcMtx33_Addr1       */
0xFB81, /* CrcMtx33_Addr0       */

/*==================================*/
/* CCM Coolwhite                    */
/*==================================*/
0xFF83, /* Page Mode                */
0xFC00, /* CrcMtx11_Addr1       */
0xFD6B, /* CrcMtx11_Addr0       */
0xFF85, /* Page Mode                */
0xE0FF, /* CrcMtx12_Addr1       */
0xE1DB, /* CrcMtx12_Addr0       */
0xFF84, /* Page Mode                */
0x00FF, /* CrcMtx13_Addr1       */
0x01FA, /* CrcMtx13_Addr0       */
0x02FF, /* CrcMtx21_Addr1       */
0x03F3, /* CrcMtx21_Addr0       */
0x0400, /* CrcMtx22_Addr1       */
0x0554, /* CrcMtx22_Addr0       */
0x06FF, /* CrcMtx23_Addr1       */
0x07F9, /* CrcMtx23_Addr0       */
0x08FF, /* CrcMtx31_Addr1       */
0x09FA, /* CrcMtx31_Addr0       */
0x0AFF, /* CrcMtx32_Addr1       */
0x0BC4, /* CrcMtx32_Addr0       */
0x0C00, /* CrcMtx33_Addr1       */
0x0D82, /* CrcMtx33_Addr0       */


/*==================================*/
/* CCM A                            */
/*==================================*/
0xFF84, /* Page Mode                */
0x0E00, /* CrcMtx11_Addr1       */
0x0F6B, /* CrcMtx11_Addr0       */
0x10FF, /* CrcMtx12_Addr1       */
0x11DA, /* CrcMtx12_Addr0       */
0x12FF, /* CrcMtx13_Addr1       */
0x13FC, /* CrcMtx13_Addr0       */
0x14FF, /* CrcMtx21_Addr1       */
0x15F5, /* CrcMtx21_Addr0       */
0x1600, /* CrcMtx22_Addr1       */
0x175B, /* CrcMtx22_Addr0       */
0x18FF, /* CrcMtx23_Addr1       */
0x19F0, /* CrcMtx23_Addr0       */
0x1AFF, /* CrcMtx31_Addr1       */
0x1BFA, /* CrcMtx31_Addr0       */
0x1CFF, /* CrcMtx32_Addr1       */
0x1DC4, /* CrcMtx32_Addr0       */
0x1E00, /* CrcMtx33_Addr1       */
0x1F82, /* CrcMtx33_Addr0       */


/*==================================*/
/* Outdoor D65                      */
/*==================================*/
0xFF86, /* Page mode				*/
0x4500, /* CCM LuxThreshold        */
0x4698, /* CCM LuxThreshold        */
0xFF85, /* Page mode               */
0xFE15, /* Outdoor CCM On          */
0xEC00, /* gAwb_s16AdapCCMTbl_0    */
0xED72, /* gAwb_s16AdapCCMTbl_1    */
0xEEFF, /* gAwb_s16AdapCCMTbl_2    */
0xEFBC, /* gAwb_s16AdapCCMTbl_3    */
0xF000, /* gAwb_s16AdapCCMTbl_4    */
0xF112, /* gAwb_s16AdapCCMTbl_5    */
0xF2FF, /* gAwb_s16AdapCCMTbl_6    */
0xF3F4, /* gAwb_s16AdapCCMTbl_7    */
0xF400, /* gAwb_s16AdapCCMTbl_8    */
0xF54F, /* gAwb_s16AdapCCMTbl_9    */
0xF6FF, /* gAwb_s16AdapCCMTbl_10   */
0xF7FD, /* gAwb_s16AdapCCMTbl_11   */
0xF8FF, /* gAwb_s16AdapCCMTbl_12   */
0xF9F5, /* gAwb_s16AdapCCMTbl_13   */
0xFAFF, /* gAwb_s16AdapCCMTbl_14   */
0xFBCE, /* gAwb_s16AdapCCMTbl_15   */
0xFC00, /* gAwb_s16AdapCCMTbl_16   */
0xFD7D, /* gAwb_s16AdapCCMTbl_17   */


/*==================================*/
/* ADF                              */
/*==================================*/
/* ISP setting   */
0xFFA0, /* Page mode						*/
0x1080, /* BLC Th	                          */
0x1109, /* BLC Separator	                  */
0x6073, /* CDC: Dark CDC ON                 */
0x611F, /* Six CDC_Edge En, Slash EN        */
0x690C, /* Slash direction Line threshold   */
0x6A60, /* Slash direction Pixel threshold  */
0xC204, /* NSF: directional smoothing       */
0xD051, /* DEM: pattern detection           */
0xFFA1, /* Page mode*/
0x3000, /* EDE: Luminane adaptation off*/
0x3240, /* EDE: Adaptation slope       */
0x3400, /* EDE: x1 point               */
0x3516, /* EDE: x1 point               */
0x3600, /* EDE: x2 point               */
0x3730, /* EDE: x2 point               */
0x3A00, /* EDE: Adaptation left margin */
0x3B20, /* EDE: Adaptation right margin*/
0x3CFF, /* EDE: rgb edge threshol      */

/* Adaptive Setting*/
0xFF85, /*Page mode*/
/* BGT*/
0x1721, /* BGT lux level threshold*/
0x260C, /* MinVal               */
0x3c00, /* MaxVal               */

/* BGT2 */
0xFF86, /*Page mode*/
0x680C, /* BGTLux1*/
0x690B, /* BGTLux2*/
0x6a09, /* BGTLux3*/
0x6b00, /* Value1 */
0x6c00, /* Value2 */
0x6d03, /* Value3 */

/* SAT */
0x6F0B, /* SATLux1   */
0x700C, /* SATLux2   */
0x7102, /* SATZone   */
0x7202, /* SATZone   */
0x7302, /* SATValue1 */
0x7400, /* SATValue2 */

/* CNT */
0xFF85, /* Page mode*/
0x1898, /* CNT lux level threshold*/
0x2702, /* MinVal               */
0x3d02, /* MaxVal               */

/* NSF */
0x12A5, /* NSF lux level threshold*/
0x221A, /* u8MinVal_NSF1 28       */
0x2340, /* u8MinVal_NSF2 70       */
0x3810, /* u8MaxVal_NSF1	      */
0x3928, /* u8MaxVal_NSF2	      */
0xFF86, /*Page mode*/
0x1206, /* u8MinVal_NSF3*/
0x1306, /* u8MaxVal_NSF3*/

/* NSF NEW thresold*/
0xFF85, /*Page mode*/
0xE822,
0xE950,
0xEA0E,
0xEB18,
/* GDC*/
0xFF85, /*Page mode*/
0x15F4, /* GDC lux level threshold */
0x2D20, /* u8MinVal_GDC1           */
0x2E30, /* u8MinVal_GDC2           */
0x4320, /* u8MaxVal_GDC1           */
0x4430, /* u8MaxVal_GDC2           */

/* ISP  Edge */
0xFF85, /*Page mode				*/
0x04FB, /* FuncCtlr_ADF         */
0x14A5, /* u8ThrLevel_EDE       */
0x2808, /* u8MinVal_EDE_CP      */
0x2907, /* u8MinVal_EDE1        */
0x2A08, /* u8MinVal_EDE2        */
0x2B05, /* u8MinVal_EDE_OFS     */
0x2C22, /* u8MinVal_SG          */
0x3E02, /* u8MaxVal_EDE_CP      */
0x3F08, /* u8MaxVal_EDE1 Edge   */
0x4009, /* u8MaxVal_EDE2        */
0x4107, /* u8MaxVal_EDE_OFS     */
0x4222, /* u8MaxVal_SG          */

/* Gamma Adaptive*/
0xFF85, /*Page mode*/
0x1670,
0x4700, /* Min_Gamma_0	*/
0x4816, /* Min_Gamma_1    */
0x4925, /* Min_Gamma_2    */
0x4A37, /* Min_Gamma_3    */
0x4B45, /* Min_Gamma_4    */
0x4C51, /* Min_Gamma_5    */
0x4D65, /* Min_Gamma_6    */
0x4E77, /* Min_Gamma_7    */
0x4F87, /* Min_Gamma_8    */
0x5095, /* Min_Gamma_9    */
0x51AF, /* Min_Gamma_10   */
0x52C6, /* Min_Gamma_11   */
0x53DB, /* Min_Gamma_12   */
0x54E5, /* Min_Gamma_13   */
0x55EE, /* Min_Gamma_14   */
0x56F7, /* Min_Gamma_15   */
0x57FF, /* Min_Gamma_16   */

0x5800,
0x5904,
0x5a16,
0x5b30,
0x5c46,
0x5d58,
0x5e6E,
0x5f7F,
0x608F,
0x619F,
0x62B7,
0x63CC,
0x64DE,
0x65E7,
0x66F0,
0x67F8,
0x68FF,


/* CCM Saturation Level	*/
0xFF85, /* Page mode         */
0x1A21, /* 64 SUP Threshold	*/
0x3070, /* MinSUP	        */

/* LSC */
0x0F43, /* LVLSC lux level threshold         */
0x10E3, /* LSLSC Light source , threshold lux*/
0x1BA0, /* MinVal_LVLSC	*/
0x31C0, /* MaxVal_LVLSC */

/* Lens Shading	*/
0xFFA0, /*Page mode   */
0x4380, /* RH7 rrhrhh */
0x4480, /* RV         */
0x4580, /* RH	      */
0x4680, /* RV         */
0x4780, /* GH         */
0x4880, /* GV         */
0x4980, /* GH         */
0x4A80, /* GV	      */
0x4B80, /* BH         */
0x4C80, /* BV         */
0x4D80, /* BH         */
0x4E80, /* BV         */
0x5290, /* GGain      */
0x5320, /* GGain      */
0x5400, /* GGain      */
/*Max Shading */
0xFF86, /* Page mode   */
0x51B8, /* Rgain1      */
0x5230, /* Rgain2      */
0x5300, /* Rgain3      */
0x5490, /* Bgain1      */
0x5510, /* Bgain2      */
0x5600, /* Bgain3      */
0x5790, /* GGain1     */
0x5820, /* GGain2     */
0x5900, /* GGain3     */
/*Min Shading */
0x48B0, /* Rgain1      */
0x4920, /* Rgain2      */
0x4A00, /* Rgain3      */
0x4B90, /* Bgain1      */
0x4C18, /* Bgain2	  */
0x4D00, /* Bgain3      */
0x4E90, /* GGain1     */
0x4F20, /* GGain2     */
0x5000, /* GGain3     */

/*DDC */
0xFF85,
0x6910,
0x6A00,
0x6B00,
0x6C00,
0x6D20,

/* LineLength*/
0xFF87, /*Page mode */
0xDC05,
0xDDB0,
0xd500,


/*==================================*/
/* MIPI                             */
/*==================================*/
0xFFB0, /*Page mode*/
0x5402, /* MIPI PHY_HS_TX_CTRL*/
0x3805, /* MIPI DPHY_CTRL_SET*/
0x3C81,
0x5011, /* MIPI Current 01 -> 11 */

/* SXGA PR*/
0xFF85, /*Page mode */
0xB71E,
0xB80A, /* gPT_u8PR_Active_SXGA_WORD_COUNT_Addr0*/
0xB900, /* gPT_u8PR_Active_SXGA_WORD_COUNT_Addr1*/
0xBC04, /* gPT_u8PR_Active_SXGA_DPHY_CLK_TIME_Addr3*/
0xFF87, /* Page mode*/
0x0C00, /* start Y*/
0x0D20, /* start Y*/
0x1003, /* end Y*/
0x11E0, /* end Y*/

/* Recoding*/
0xFF86, /*Page mode*/
0x371E,
0x3805, /*gPT_u8PR_Active_720P_WORD_COUNT_Addr0*/
0x3900, /*gPT_u8PR_Active_720P_WORD_COUNT_Addr1*/
0x3C04, /*gPT_u8PR_Active_720P_DPHY_CLK_TIME_Addr3*/

0xFF87,
0x2302, /*gPR_Active_720P_u8SensorCtrl_Addr*/
0x2472, /*gPR_Active_720P_u8SensorMode_Addr*/
0x2501, /*gPR_Active_720P_u8PLL_P_Addr*/
0x260F, /*gPR_Active_720P_u8PLL_M_Addr*/
0x2700, /*gPR_Active_720P_u8PLL_S_Addr*/
0x2800, /*gPR_Active_720P_u8PLL_Ctrl_Addr*/
0x2901, /*gPR_Active_720P_u8src_clk_sel_Addr*/
0x2A00, /*gPR_Active_720P_u8output_pad_status_Addr*/
0x2B3F, /*gPR_Active_720P_u8ablk_ctrl_10_Addr*/
0x2CFF, /*gPR_Active_720P_u8BayerFunc_Addr*/
0x2DFF, /*gPR_Active_720P_u8RgbYcFunc_Addr*/
0x2E00, /*gPR_Active_720P_u8ISPMode_Addr*/
0x2F02, /*gPR_Active_720P_u8SCLCtrl_Addr*/
0x3001, /*gPR_Active_720P_u8SCLHorScale_Addr0*/
0x31FF, /*gPR_Active_720P_u8SCLHorScale_Addr1*/
0x3203, /*gPR_Active_720P_u8SCLVerScale_Addr0*/
0x33FF, /*gPR_Active_720P_u8SCLVerScale_Addr1*/
0x3400, /*gPR_Active_720P_u8SCLCropStartX_Addr0*/
0x3500, /*gPR_Active_720P_u8SCLCropStartX_Addr1*/
0x3600, /*gPR_Active_720P_u8SCLCropStartY_Addr0*/
0x3710, /*gPR_Active_720P_u8SCLCropStartY_Addr1*/
0x3802, /*gPR_Active_720P_u8SCLCropEndX_Addr0*/
0x3980, /*gPR_Active_720P_u8SCLCropEndX_Addr1*/
0x3A01, /*gPR_Active_720P_u8SCLCropEndY_Addr0*/
0x3BF0, /*gPR_Active_720P_u8SCLCropEndY_Addr1*/
0x3C01, /*gPR_Active_720P_u8OutForm_Addr*/
0x3D0C, /*gPR_Active_720P_u8OutCtrl_Addr*/
0x3E04, /*gPR_Active_720P_u8AEWinStartX_Addr*/
0x3F04, /*gPR_Active_720P_u8AEWinStartY_Addr*/
0x4066, /*gPR_Active_720P_u8MergedWinWidth_Addr*/
0x415E, /*gPR_Active_720P_u8MergedWinHeight_Addr*/
0x4204, /*gPR_Active_720P_u8AEHistWinAx_Addr*/
0x4304, /*gPR_Active_720P_u8AEHistWinAy_Addr*/
0x4498, /*gPR_Active_720P_u8AEHistWinBx_Addr*/
0x4578, /*gPR_Active_720P_u8AEHistWinBy_Addr*/
0x4622, /*gPR_Active_720P_u8AWBTrim_Addr*/
0x4728, /*gPR_Active_720P_u8AWBCTWinAx_Addvr*/
0x4820, /*gPR_Active_720P_u8AWBCTWinAy_Addr*/
0x4978, /*gPR_Active_720P_u8AWBCTWinBx_Addr*/
0x4A60, /*gPR_Active_720P_u8AWBCTWinBy_Addr*/
0x4B03, /*gPR_Active_720P_u16AFCFrameLength_0*/
0x4C00, /*gPR_Active_720P_u16AFCFrameLength_1*/

/*VGA PR*/
0xFF86, /*Page mode*/
0x2E1E,
0x2F05, /* gPT_u8PR_Active_VGA_WORD_COUNT_Addr0*/
0x3000, /* gPT_u8PR_Active_VGA_WORD_COUNT_Addr1*/
0x3304, /* gPT_u8PR_Active_VGA_DPHY_CLK_TIME_Addr3*/

0xFF87, /*Page mode*/
0x4D00, /*gPR_Active_VGA_u8SensorCtrl_Addr*/
0x4E72, /*gPR_Active_VGA_u8SensorMode_Addr*/
0x4F01, /*gPR_Active_VGA_u8PLL_P_Addr*/
0x500F, /*gPR_Active_VGA_u8PLL_M_Addr*/
0x5100, /*gPR_Active_VGA_u8PLL_S_Addr*/
0x5200, /*gPR_Active_VGA_u8PLL_Ctrl_Addr*/
0x5301, /*gPR_Active_VGA_u8src_clk_sel_Addr*/
0x5400, /*gPR_Active_VGA_u8output_pad_status_Addr*/
0x553F, /*gPR_Active_VGA_u8ablk_ctrl_10_Addr*/
0x56FF, /*gPR_Active_VGA_u8BayerFunc_Addr*/
0x57FF, /*gPR_Active_VGA_u8RgbYcFunc_Addr*/
0x5800, /*gPR_Active_VGA_u8ISPMode_Addr*/
0x5902, /*gPR_Active_VGA_u8SCLCtrl_Addr*/
0x5A01, /*gPR_Active_VGA_u8SCLHorScale_Addr0*/
0x5BFF, /*gPR_Active_VGA_u8SCLHorScale_Addr1*/
0x5C01, /*gPR_Active_VGA_u8SCLVerScale_Addr0*/
0x5DFF, /*gPR_Active_VGA_u8SCLVerScale_Addr1*/
0x5E00, /*gPR_Active_VGA_u8SCLCropStartX_Addr0*/
0x5F00, /*gPR_Active_VGA_u8SCLCropStartX_Addr1*/
0x6000, /*gPR_Active_VGA_u8SCLCropStartY_Addr0*/
0x6110, /*gPR_Active_VGA_u8SCLCropStartY_Addr1*/
0x6202, /*gPR_Active_VGA_u8SCLCropEndX_Addr0*/
0x6380, /*gPR_Active_VGA_u8SCLCropEndX_Addr1*/
0x6401, /*gPR_Active_VGA_u8SCLCropEndY_Addr0*/
0x65F0, /*gPR_Active_VGA_u8SCLCropEndY_Addr1*/



0xFFC0, /*Page mode*/
0x1041,
0xE764, /*Wait  100*/

};


/* Set-data based on Samsung Reliabilty Group standard
* ,when using WIFI. 15fps
*/
static const u16 db8131m_vt_wifi_common[] = {
/* Fixed 15fps */
};

/*===========================================
* CAMERA_PREVIEW - 篤   枯  *
============================================*/

static const u16 db8131m_preview[] = {
};

/*===========================================
*	CAMERA_SNAPSHOT  - 篤			  *
============================================*/

static const u16 db8131m_capture[] = {
0xffC0,
0x1003,
0xE796,

};

static const u16 db8131m_720p_common[] = {

};

/*===========================================
*	CAMERA_RECORDING WITH 15fps  *
============================================*/

static const u16 db8131m_recording_60Hz_common[] = {
0xE796, /*Wait  150*/

0xFF82,
0x9102,
0xFF83,
0x0B04,
0x0C4C,
0x1104,
0x1276,

0x0308,
0x0406,
0x0504,
0x0604,
0x070C,
0x0807,
0x0903,
0x0A03,

0xFF82,
0x925D,
};

static const u16 db8131m_recording_50Hz_common[] = {
0xFF82,
0x9102,
0xFF83,
0x0B04,
0x0CB0,
0x1104,
0x1276,

0x0308,
0x0406,
0x0504,
0x0604,
0x070C,
0x0807,
0x0903,
0x0A03,

0xFF82,
0x9259,

};

static const u16 db8131m_stream_stop[] = {


};


/*=================================
*	CAMERA_BRIGHTNESS_1 (1/9) M4   *
==================================*/
static const u16 db8131m_bright_m4[] = {
/* Brightness -4 */
0xFF87, /* Page mode  */
0xAEB0, /* Brightness */
};

/*=================================
*	CAMERA_BRIGHTNESS_2 (2/9) M3  *
==================================*/

static const u16 db8131m_bright_m3[] = {
/* Brightness -3 */
0xFF87, /* Page mode  */
0xAED0, /* Brightness */
};

/*=================================
	CAMERA_BRIGHTNESS_3 (3/9) M2
==================================*/
static const u16 db8131m_bright_m2[] = {
/* Brightness -2 */
0xFF87, /* Page mode  */
0xAEE0, /* Brightness */
};

/*=================================
	CAMERA_BRIGHTNESS_4 (4/9) M1
==================================*/

static const u16 db8131m_bright_m1[] = {
/* Brightness -1 */
0xFF87, /* Page mode  */
0xAEF0, /* Brightness */
};

/*=================================
	CAMERA_BRIGHTNESS_5 (5/9) Default
==================================*/
static const u16 db8131m_bright_default[] = {
/* Brightness 0 */
0xFF87, /* Page mode  */
0xAE00, /* Brightness */
};

/*=================================
	CAMERA_BRIGHTNESS_6 (6/9) P1
==================================*/
static const u16 db8131m_bright_p1[] = {
/* Brightness +1 */
0xFF87, /* Page mode */
0xAE10, /* Brightness*/
};

/*=================================
	CAMERA_BRIGHTNESS_7 (7/9) P2
==================================*/
static const u16 db8131m_bright_p2[] = {
/* Brightness +2 */
0xFF87, /* Page mode  */
0xAE20, /* Brightness */
};

/*=================================
	CAMERA_BRIGHTNESS_8 (8/9) P3
==================================*/
static const u16 db8131m_bright_p3[] = {
/* Brightness +3 */
0xFF87, /* Page mode  */
0xAE30,	/* Brightness */
};

/*=================================
	CAMERA_BRIGHTNESS_9 (9/9) P4
==================================*/
static const u16 db8131m_bright_p4[] = {
/* Brightness +4 */
0xFF87, /* Page mode  */
0xAE50,	/* Brightness */
};

/*******************************************************
* CAMERA_VT_PRETTY_0 Default
* 200s self cam pretty
*******************************************************/
static const u16 db8131m_vt_pretty_default[] = {

};


/*******************************************************
*	CAMERA_VT_PRETTY_1
*******************************************************/
static const u16 db8131m_vt_pretty_1[] = {

};


/*******************************************************
*	CAMERA_VT_PRETTY_2				*
*******************************************************/
static const u16 db8131m_vt_pretty_2[] = {

};


/*******************************************************
*	CAMERA_VT_PRETTY_3
*******************************************************/
static const u16 db8131m_vt_pretty_3[] = {

};

static const u16 db8131m_vt_7fps[] = {
0xFF82,
0x9102,
0xFF83,
0x0B09,
0x0C24,
0x0311,
0x0410,
0x050C,
0x0608,
0xFF82,
0x925D,
};

static const u16 db8131m_vt_10fps[] = {
0xFF82,
0x9102,
0xFF83,
0x0B06,
0x0C75,
0x030C,
0x040B,
0x050A,
0x0608,
0xFF82,
0x925D,
};

static const u16 db8131m_vt_12fps[] = {
0xFF82,
0x9102,
0xFF83,
0x0B05,
0x0C62,
0x030A,
0x0409,
0x0508,
0x0606,
0xFF82,
0x925D,
};

static const u16 db8131m_vt_15fps[] = {
0xFF82,
0x9102,
0xFF83,
0x0B04,
0x0C4C,
0x0308,
0x0406,
0x0504,
0x0604,
0xFF82,
0x925D,
};

/*******************************************************
*	CAMERA_DTP_ON
*******************************************************/
static const u16 db8131m_pattern_on[] = {
0xFF87,
0xAB00,
0xAC28,
0xFFA0,
0x0205,

0xE796, /*Wait  150*/

};

/*******************************************************
*	CAMERA_DTP_OFF
*******************************************************/
static const u16 db8131m_pattern_off[] = {
0xFF87,
0xABFF,
0xACFF,
0xFFA0,
0x0200,

0xE796, /*Wait  150*/
};

static const u16 db8131m_flip_off[] = {
0xFF87,
0xd500,
};

static const u16 db8131m_vflip[] = {
0xFF87,
0xd508,
};

static const u16 db8131m_hflip[] = {
0xFF87,
0xd504,
};

static const u16 db8131m_flip_off_No15fps[] = {
0xFF87,
0xd502,
};

static const u16 db8131m_vflip_No15fps[] = {
0xFF87,
0xd50A,
};

static const u16 db8131m_hflip_No15fps[] = {
0xFF87,
0xd506,
};
#endif /* __SEC_db8131m_REG_H */
