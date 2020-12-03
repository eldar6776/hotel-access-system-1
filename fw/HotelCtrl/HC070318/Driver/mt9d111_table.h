/**
  * @file    mt9d111.h
  * @author  WB R&D Team - openmcu666
  * @version V0.1
  * @date    2016.01.09
  * @brief   mt9d111 Camera Driver
  */

#ifndef __MT9D111_TABLE_H__
#define __MT9D111_TABLE_H__


const int mt9d111_QVGA[160][3]=//154
{
          {0, 0x33, 0x0343        }, // RESERVED_CORE_33
          {1, 0xC6, 0xA115        }, //SEQ_LLMODE
          {1, 0xC8, 0x0020        }, //SEQ_LLMODE
          {0, 0x38, 0x0866        }, // RESERVED_CORE_38
          {256, 0x00, 50 }, // Delay =100ms
  
          {2, 0x80, 0x0168        }, // LENS_CORRECTION_CONTROL
          {2, 0x81, 0x6432        }, // ZONE_BOUNDS_X1_X2
          {2, 0x82, 0x3296        }, // ZONE_BOUNDS_X0_X3
          {2, 0x83, 0x9664        }, // ZONE_BOUNDS_X4_X5
          {2, 0x84, 0x5028        }, // ZONE_BOUNDS_Y1_Y2
          {2, 0x85, 0x2878        }, // ZONE_BOUNDS_Y0_Y3
          {2, 0x86, 0x7850        }, // ZONE_BOUNDS_Y4_Y5
          {2, 0x87, 0x0000        }, // CENTER_OFFSET
          {2, 0x88, 0x0152        }, // FX_RED
          {2, 0x89, 0x015C        }, // FX_GREEN
          {2, 0x8A, 0x00F4        }, // FX_BLUE
          {2, 0x8B, 0x0108        }, // FY_RED
          {2, 0x8C, 0x00FA        }, // FY_GREEN
          {2, 0x8D, 0x00CF        }, // FY_BLUE
          {2, 0x8E, 0x09AD        }, // DF_DX_RED
          {2, 0x8F, 0x091E        }, // DF_DX_GREEN
          {2, 0x90, 0x0B3F        }, // DF_DX_BLUE
          {2, 0x91, 0x0C85        }, // DF_DY_RED
          {2, 0x92, 0x0CFF        }, // DF_DY_GREEN
          {2, 0x93, 0x0D86        }, // DF_DY_BLUE
          {2, 0x94, 0x163A        }, // SECOND_DERIV_ZONE_0_RED
          {2, 0x95, 0x0E47        }, // SECOND_DERIV_ZONE_0_GREEN
          {2, 0x96, 0x103C        }, // SECOND_DERIV_ZONE_0_BLUE
          {2, 0x97, 0x1D35        }, // SECOND_DERIV_ZONE_1_RED
          {2, 0x98, 0x173E        }, // SECOND_DERIV_ZONE_1_GREEN
          {2, 0x99, 0x1119        }, // SECOND_DERIV_ZONE_1_BLUE
          {2, 0x9A, 0x1663        }, // SECOND_DERIV_ZONE_2_RED
          {2, 0x9B, 0x1569        }, // SECOND_DERIV_ZONE_2_GREEN
          {2, 0x9C, 0x104C        }, // SECOND_DERIV_ZONE_2_BLUE
          {2, 0x9D, 0x1015        }, // SECOND_DERIV_ZONE_3_RED
          {2, 0x9E, 0x1010        }, // SECOND_DERIV_ZONE_3_GREEN
          {2, 0x9F, 0x0B0A        }, // SECOND_DERIV_ZONE_3_BLUE
          {2, 0xA0, 0x0D53        }, // SECOND_DERIV_ZONE_4_RED
          {2, 0xA1, 0x0D51        }, // SECOND_DERIV_ZONE_4_GREEN
          {2, 0xA2, 0x0A44        }, // SECOND_DERIV_ZONE_4_BLUE
          {2, 0xA3, 0x1545        }, // SECOND_DERIV_ZONE_5_RED
          {2, 0xA4, 0x1643        }, // SECOND_DERIV_ZONE_5_GREEN
          {2, 0xA5, 0x1231        }, // SECOND_DERIV_ZONE_5_BLUE
          {2, 0xA6, 0x0047        }, // SECOND_DERIV_ZONE_6_RED
          {2, 0xA7, 0x035C        }, // SECOND_DERIV_ZONE_6_GREEN
          {2, 0xA8, 0xFE30        }, // SECOND_DERIV_ZONE_6_BLUE
          {2, 0xA9, 0x4625        }, // SECOND_DERIV_ZONE_7_RED
          {2, 0xAA, 0x47F3        }, // SECOND_DERIV_ZONE_7_GREEN
          {2, 0xAB, 0x5859        }, // SECOND_DERIV_ZONE_7_BLUE
          {2, 0xAC, 0xFFFF        }, // X2_FACTORS	
          {2, 0xAD, 0x0000        }, // GLOBAL_OFFSET_FXY_FUNCTION
          {2, 0xAE, 0x0000        }, // K_FACTOR_IN_K_FX_FY
          {1, 0x08, 0x01FC        }, // COLOR_PIPELINE_CONTROL
          {256, 0x00, 50 }, // Delay =100ms
          {1, 0xBE, 0x0004        }, // YUV_YCBCR_CONTROL
          {0, 0x65, 0xA000        }, // CLOCK_ENABLING
          {256, 0x00, 50 }, // Delay =100ms

          {1, 0xC6, 0xA102        }, //SEQ_MODE
          {1, 0xC8, 0x001F        }, //SEQ_MODE
          {1, 0x08, 0x01FC        }, // COLOR_PIPELINE_CONTROL
          {1, 0x08, 0x01EC        }, // COLOR_PIPELINE_CONTROL
          {1, 0x08, 0x01FC        }, // COLOR_PIPELINE_CONTROL
          {1, 0x36, 0x0F08         }, // APERTURE_PARAMETERS
          {1, 0xC6, 0x270B        }, //MODE_CONFIG
          {1, 0xC8, 0x0030        }, //MODE_CONFIG, JPEG disabled for A and B
          {1, 0xC6, 0xA121        }, //SEQ_CAP_MODE
          {1, 0xC8, 0x007F        }, //7F SEQ_CAP_MODE (127 frames before switching to Preview)
          {0, 0x05, 0x011E        }, // HORZ_BLANK_B
          {0, 0x06, 0x006F        }, // VERT_BLANK_B
          {0, 0x07, 0xFE          }, // HORZ_BLANK_A	FE
          {0, 0x08, 19            }, // VERT_BLANK_A	19
          {0, 0x20, 0x0300        }, // READ_MODE_B (Image flip settings)	01ÕýÊÓ£¬00¾µ×Ó
          {0, 0x21, 0x8400        }, // READ_MODE_A (1ADC)	8400



         {1, 0xC6, 0x2717        }, //MODE_SENSOR_X_DELAY_A
         {1, 0xC8, 792           }, //MODE_SENSOR_X_DELAY_A
         {1, 0xC6, 0x270F        }, //MODE_SENSOR_ROW_START_A
         {1, 0xC8, 0x001C        }, //MODE_SENSOR_ROW_START_A
         {1, 0xC6, 0x2711        }, //MODE_SENSOR_COL_START_A
         {1, 0xC8, 0x003C        }, //MODE_SENSOR_COL_START_A
         {1, 0xC6, 0x2713        }, //MODE_SENSOR_ROW_HEIGHT_A
         {1, 0xC8, 0x04B0        }, //MODE_SENSOR_ROW_HEIGHT_A
         {1, 0xC6, 0x2715        }, //MODE_SENSOR_COL_WIDTH_A
         {1, 0xC8, 0x0640        }, //MODE_SENSOR_COL_WIDTH_A
         {1, 0xC6, 0x2719        }, //MODE_SENSOR_ROW_SPEED_A
         {1, 0xC8, 0x0011        }, //MODE_SENSOR_ROW_SPEED_A
         {1, 0xC6, 0x2707        }, //MODE_OUTPUT_WIDTH_B
         {1, 0xC8, 0x0640        }, //MODE_OUTPUT_WIDTH_B
         {1, 0xC6, 0x2709        }, //MODE_OUTPUT_HEIGHT_B
         {1, 0xC8, 0x04B0        }, //MODE_OUTPUT_HEIGHT_B
         {1, 0xC6, 0x271B        }, //MODE_SENSOR_ROW_START_B
         {1, 0xC8, 0x001C        }, //MODE_SENSOR_ROW_START_B
         {1, 0xC6, 0x271D        }, //MODE_SENSOR_COL_START_B
         {1, 0xC8, 0x003C        }, //MODE_SENSOR_COL_START_B
         {1, 0xC6, 0x271F        }, //MODE_SENSOR_ROW_HEIGHT_B
         {1, 0xC8, 0x04B0        }, //MODE_SENSOR_ROW_HEIGHT_B
         {1, 0xC6, 0x2721        }, //MODE_SENSOR_COL_WIDTH_B
         {1, 0xC8, 0x0640        }, //MODE_SENSOR_COL_WIDTH_B
         {1, 0xC6, 0x2723        }, //MODE_SENSOR_X_DELAY_B
         {1, 0xC8, 0x0716        }, //MODE_SENSOR_X_DELAY_B
         {1, 0xC6, 0x2725        }, //MODE_SENSOR_ROW_SPEED_B
         {1, 0xC8, 0x0011        }, //MODE_SENSOR_ROW_SPEED_B


         {1, 0xC6, 0x2703    }, //MODE_OUTPUT_WIDTH_A
         {1, 0xC8, 0x0140   }, //MODE_OUTPUT_WIDTH_A
         {1, 0xC6, 0x2705    }, //MODE_OUTPUT_HEIGHT_A
         {1, 0xC8, 0x00f0    }, //MODE_OUTPUT_HEIGHT_A
         {1, 0xC6, 0x2707    }, //MODE_OUTPUT_WIDTH_B
         {1, 0xC8, 0x0140    }, //MODE_OUTPUT_WIDTH_B
         {1, 0xC6, 0x2709    }, //MODE_OUTPUT_HEIGHT_B
         {1, 0xC8, 0x00f0    }, //MODE_OUTPUT_HEIGHT_B
         {1, 0xC6, 0x2779    }, //Spoof Frame Width
         {1, 0xC8, 0x0240    }, //Spoof Frame Width
         {1, 0xC6, 0x277B    }, //Spoof Frame Height
         {1, 0xC8, 0x00f0    }, //Spoof Frame Height
         {1, 0xC6, 0xA103    }, //SEQ_CMD
         {1, 0xC8, 0x0005    }, //SEQ_CMD
         {256, 0x00, 50 }, // Delay =500ms 
//         //Maximum Slew-Rate on IO-Pads (for Mode A)
         {1, 0xC6, 0x276B        }, //MODE_FIFO_CONF0_A
         {1, 0xC8, 0x0027        }, //MODE_FIFO_CONF0_A
         {1, 0xC6, 0x276D        }, //MODE_FIFO_CONF1_A
         {1, 0xC8, 0xE1E1        }, //MODE_FIFO_CONF1_A
         {1, 0xC6, 0xA76F        }, //MODE_FIFO_CONF2_A
         {1, 0xC8, 0x00E1        }, //MODE_FIFO_CONF2_A

         //Maximum Slew-Rate on IO-Pads (for Mode B)
         {1, 0xC6, 0x2772        }, //MODE_FIFO_CONF0_B
         {1, 0xC8, 0x0027        }, //MODE_FIFO_CONF0_B
         {1, 0xC6, 0x2774        }, //MODE_FIFO_CONF1_B
         {1, 0xC8, 0xE1E1        }, //MODE_FIFO_CONF1_B
        {1, 0xC6, 0xA776        }, //MODE_FIFO_CONF2_B
        {1, 0xC8, 0x00E1        }, //MODE_FIFO_CONF2_B

         //Set maximum integration time to get a minimum of 15 fps at 45MHz
         {1, 0xC6, 0xA20E        }, //AE_MAX_INDEX
         {1, 0xC8, 0x0004        }, //AE_MAX_INDEX
         //Set minimum integration time to get a maximum of 15 fps at 45MHz
         {1, 0xC6, 0xA20D        }, //AE_MAX_INDEX
         {1, 0xC8, 0x0004        }, //AE_MAX_INDEX
         // Configue all GPIO for output and set low to save power
         {1, 0xC6, 0x9078        },
         {1, 0xC8, 0x0000        },
         {1, 0xC6, 0x9079        },
         {1, 0xC8, 0x0000        },
         {1, 0xC6, 0x9070        },
         {1, 0xC8, 0x0000        },
         {1, 0xC6, 0x9071        },
         {1, 0xC8, 0x0000        },
         // gamma and contrast
         {1, 0xC6, 0xA743        }, // MODE_GAM_CONT_A
         {1, 0xC8, 0x0003        }, // MODE_GAM_CONT_A
         {1, 0xC6, 0xA744        }, // MODE_GAM_CONT_B
         {1, 0xC8, 0x0003        }, // MODE_GAM_CONT_B
         {256, 0x00, 80 }, // Delay =500m
							 
         {1, 0x97, 0x0022        },	//0020	shine
         {1, 0x98, 0x0000        },	//0000

         //{0, 0xF2, 0x0000        },      
         {0, 0x65, 0xe000        },      
         {0, 0x66, 0x1000       },      // PLL_REG	3809
         {0, 0x67, 0x0500       },      // PLL2_REG	0001
         {0, 0x65, 0xE000        },      // CLOCK_ENABLING A000
         {256, 0x00, 50 }, // Delay =500ms
				 
				 {2,0x16,1600},
				  {2,0x17,1200},
         {2, 0x0A, 0x0010        },   //jpeg B mode
         {2, 0x0D, 0x0e97        },      // CLOCK_ENABLING
	
					


};

#endif
