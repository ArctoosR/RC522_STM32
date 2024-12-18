/*
 * dfplayer.c
 *
 *  Created on: 2022. 2. 23.
 *      Author: sungt
 *
 *
 */


#include "dfplayer.h"
#include "usart.h"
#include "stdio.h"


 uint8_t    rx_buf[UART_MAX_BUF_SIZE];
 uint8_t    tx_buf[UART_MAX_BUF_SIZE];

/* UART 관련 HandleTypeDef 선언   */




void apDFPlayerTest(char rx_data)
{



	dfplayer_SetSource(PLAYBACK_SOURCE_TF); /* 저장장치 SD카드로 설정 */
	dfplayer_SetVolume(10); /* 초기 볼륨 10으로 설정 */
	dfplayer_SetEQ(EQ_CLASSIC); /* EQ를 클래식으로 설정 */


			//rx_data = uartRead(_DEF_UART2);
			printf( "To_DFPlAYER\n");

			switch (rx_data)
			{
				case 'S':
					dfplayer_Play(); /* 음악 재생  */
					printf("Playing...\r\n");
					break;

				case 'D':
					dfplayer_Next(); /* 다음곡 재생  */
					printf( "Playing Next Song...\r\n");
					break;

				case 'A':
					dfplayer_Previous(); /* 이전곡 재생  */
					printf("Playing Previous Song...\r\n");
					break;

				case 'Q':
					dfplayer_Pause(); /* 일시정지  */
					printf( "Pause\r\n");
					break;

				case 'W':
					dfplayer_IncreaseVolume(); /* 볼륨 증가  */
					printf( "Volume Up\r\n");
					break;

				case 'X':
					dfplayer_DecreaseVolume(); /* 볼륨 감소 */
					printf("Volume Down\n");
					break;

				case 'V':
					dfplayer_Stop(); /* 정지 */
					printf( "Stop\n");
					break;

				case 'E':
					dfplayer_InsertAdvertisement(1); /* ADVERT폴더에 있는 음악 인터럽트 실행  */
					printf( "Ad Start\n");
					break;

				case 'T':
					dfplayer_StopAdvertisement(); /* ADVERT 폴더에 있는 음악 인터럽트 정지 후 원래 실행하던 곡으로 복귀  */
					printf( "AD Stop & Go Back\n");
					break;

				default:
					break;
			}




}



void delay_ms(uint32_t ms)
{
	HAL_Delay(ms);
}


void delay_us(uint32_t us)
{
     if(us>1)
     {
       uint32_t count=us*8-6;
       while(count--);
     }
     else
     {
      uint32_t count=2;
      while(count--);
     }
}

uint32_t millis(void)
{

	return HAL_GetTick();
}



uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  uint32_t pre_time;


  	/* UART1 */

        pre_time = millis();
        while(millis() - pre_time < 100)
        {
          if (huart2.gState == HAL_UART_STATE_READY)
          {

            for (int i = 0; i < length; i++)
            {
              tx_buf[i] = p_data[i];
            }

            if(HAL_UART_Transmit_DMA(&huart2, &tx_buf[0], length) == HAL_OK)
            {
              ret = length;
            }

            break;
          }

        }

return ret ;
}




//static uint8_t rx_buf[DFPLAYER_COMMUNICATION_SIZE];



/* dfplayer.c 내 에서만 사용하는 함수이므로 static으로 선언 */
static void 	dfplayer_SendControlMessage(uint8_t cmd, uint8_t para_msb, uint8_t para_lsb);
static uint16_t dfplayer_Checksum(void);

#ifndef _USE_HW_UART

#endif /* _USE_HW_UART */



/**
 * @brief	CMD 명령어 전송 함수
 * @note	내부에서만  사용하는  함수
 * @param  	cmd  				Control Commands define set,  dfplayer.h 참고
 * @param  	para_msb  			Commands Parameters define set, dfplayer.h 참고, 필요없을 경우  0
 * @param  	para_lsb  			Commands Parameters define set, dfplayer.h 참고 , 필요없을 경우 0
 * @warning UART1이 아닌 다른 UART를 사용하는 경우  _DEF_UARTx 또는 huartx 변경
 */
void dfplayer_SendControlMessage( uint8_t cmd, uint8_t para_msb, uint8_t para_lsb)
{
	tx_buf[START_INDEX] 		= START;
	tx_buf[VERSION_INDEX]	 	= VERSION;
	tx_buf[DATA_LENGTH_INDEX] 	= DATA_LENGTH;
	tx_buf[CMD_INDEX] 			= cmd;
	tx_buf[FEEDBACK_INDEX] 		= NO_FEED_BACK;
	tx_buf[PARAMETER_MSB_INDEX] = para_msb;
	tx_buf[PARAMETER_LSB_INDEX] = para_lsb;

	uint16_t checksum = dfplayer_Checksum();

	tx_buf[CHECKSUM_MSB_INDEX]  = (uint8_t)(checksum >> 8);
	tx_buf[CHECKSUM_LSB_INDEX]  = (uint8_t)(checksum & 0xFF);
	tx_buf[END_INDEX] 			= END;

	for (int i = 0; i < 1; i++)
	{

		uartWrite(0, (uint8_t *)&tx_buf[i], DFPLAYER_COMMUNICATION_SIZE);
#ifndef _USE_HW_UART
		HAL_UART_Transmit_DMA(&huart2, (uint8_t *)&tx_buf[i], DFPLAYER_COMMUNICATION_SIZE);	/* HAL Lib의  UART를 사용하는 경우 */
#endif /* _USE_HW_UART */
	}

}



/**
 * @brief	DFplayer 직렬 포멧에  필요한 체크썹 바이트를 구하기 위한 계산 수행
 * @note	내부에서만  사용하는  함수
 *
 */
uint16_t dfplayer_Checksum(void)
{
	uint16_t checksum = 0;

	for (int i = VERSION_INDEX; i <= PARAMETER_LSB_INDEX; i ++)
	{
		checksum += tx_buf[i];
	}

	checksum = 0xFFFF - checksum + 1; // 2의 보수를 취함, checksum 공식

	return checksum;
}



/**
 * @brief	DFplayer 초기화 함수
 * @note 	DFplayer 모듈이  전원이 켜진 후 초기화되기까지 1.5~ 3sec 정도 필요
 *
 */
void dfplayer_Init(void)
{
	delay_ms(1500);
}



/**
 * @brief   다음 mp3 파일 재생
 *
 */
void dfplayer_Next(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_NEXT, 0, 0);
}



/**
 * @brief 	이전 mp3 파일 재생
 *
 */
void dfplayer_Previous(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_PREVIOUS, 0, 0);
}


/**
 * @brief 	트랙번호 지정하기
 * @note    SD카드 또는 U 디스크 등의 저장장치의  최상위 폴더에 0001.mp3/wmv ~ 3000.mp3/wmv 의 넘버링을한  파일이 존재하면 재생하고싶은 특정 track의 넘버를 입력.
 *          만약 1000.mp3파일을 지정한다면 1000은 hex로 0x03E8 이므로  para_msb에 0x03, para_lsb에 0xE8이 직렬 포멧으로 지정되어 uart로 전송됨
 * @param  	track  			1 ~ 30000 (or 0 ~ 2999) 파일명 넘버링에 따라 둘 중 하나의 범위를 가짐
 * @warning ADVERT 특수 폴더 사용 안할 시에는  0 ~ 255의 기본 트랙 범위를 가짐
 */
void dfplayer_SetTrakNumber(int16_t track)// Test complete
{
	dfplayer_SendControlMessage(CMD_SET_TRAK_NUMBER, (uint8_t)(track >> 8), (uint8_t)track & 0xFF);
}


/**
 * @brief 	볼륨 증가
 *
 */
void dfplayer_IncreaseVolume(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_INC_VOLUME, 0, 0);
}



/**
 * @brief 	볼륨 감소
 *
 */
void dfplayer_DecreaseVolume(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_DEC_VOLUME, 0, 0);
}



/*
 * @brief  	초기 볼륨값 설정, default는 30
 * @param  	volume  		1 ~ 30까지 선택
 *
 */
void dfplayer_SetVolume(uint8_t volume)// Test complete
{
	if (volume > 30)
	{
		volume = 30;
	}

	dfplayer_SendControlMessage(CMD_SET_VOLUME, 0, volume);

}



/*
 * @brief   음향효과 설정
 * @param  	source 			EQ_NORMAL 	: 기본
 *			  			    EQ_POP  	: 팝
 *			  			    EQ_ROCK		: 락
 *			  			    EQ_JAZZ 	: 재즈
 *			  			    EQ_CLASSIC	: 클래식
 *			  			    EQ_BASE   	: 베이스
 */
void dfplayer_SetEQ(int8_t eq)// Test complete
{
	if (eq > EQ_BASE)
	{
		eq = EQ_NORMAL ;
	}

	dfplayer_SendControlMessage(CMD_SET_EQ, 0, eq);
}



/*
 * @brief  	재생 모드 설정
 * @param  	track  			1 ~ 30000 (or 0 ~ 2999) 파일명 넘버링에 따라 둘 중 하나의 범위를 가짐
 */
void dfplayer_RepeatTrack(uint8_t track)// Test complete
{

	dfplayer_SendControlMessage(CMD_REPEAT_TRACK, (uint8_t)(track & 0xFF00) >> 8, (uint8_t)track & 0xFF);
}



/*
 * @brief  	사용된 저장장치 설정
 * @param  	source 			PLAYBACK_SOURCE_U 		: U 디스크
 *			  			    PLAYBACK_SOURCE_TF  	: TF 카드 (SD카드 )
 *			  			    PLAYBACK_SOURCE_AUX 	: AUX 케이블
 *			  			    PLAYBACK_SOURCE_SLEEP
 *			  			    PLAYBACK_SOURCE_FLASH	: Flash
 */
void dfplayer_SetSource(uint8_t source)// Test complete
{

	if (source > PLAYBACK_SOURCE_FLASH)
	{
		return ;
	}

	dfplayer_SendControlMessage(CMD_SET_PLAYBACK_SOURCE, 0, source);

}



/**
 * @brief	스탠바이 모드 진입
 * @note 	low power loss, 저전력
 *
 */
void dfplayer_Standby(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_ENTER_INTO_STANDBY, 0, 0);
}



/**
 * @brief	DFplayer 리셋
 *
 */
void dfplayer_Reset(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_RESET, 0, 0);
}



/**
 * @brief 	mp3 파일 재생
 *
 */
void dfplayer_Play(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_PLAYBACK, 0, 0);

}



/**
 * @brief 	mp3 파일 일시정지
 *
 */
void dfplayer_Pause(void)// Test complete
{
	dfplayer_SendControlMessage(CMD_PAUSE, 0, 0);
}



/**
 * @brief	특수 폴더를 제외한 01~99번 폴더 안에 재생하고 싶은 mp3 파일 재생 설정
 * @note	트랙의 범위는 255로 제한
 * @param  	folder  				01 ~ 99 까지
 * @param  	track  					001 ~ 255 까지
 */
void dfplayer_PlayTrackInFolder(uint8_t folder, uint8_t track) //TODO ongoing - 4
{
	if (folder > 99)
	{
		folder = 99;
	}

	if (track > 255)
	{
		track = 255;
	}

	dfplayer_SendControlMessage(CMD_SET_PLAY_TRACK_FOLDER, folder, track);
}



/**
 * @brief   볼륨 조정
 * @note	볼륨 증폭 값 설정, Parameter_MSB에 1을 대입하여 볼륨 조정기능을 활성화 시킴
 * @param  	turn_onoff     		    True (1) : On /  False (0) : Off
 * @param  	gain  				    1 ~ 31까지 선택
 */
void dfplayer_VolumeAdjust(bool turn_onoff, uint8_t gain)// Test complete
{

	if (gain > 31)
	{
		gain = 31;
	}

	dfplayer_SendControlMessage(CMD_VOLUME_ADJUST, turn_onoff, gain);

}



/**
 * @brief   전체 반복 재생
 * @param  	repeat  			     0 : stop play / 1 : start repeat play, 0과 1 둘중 하나 선택
 */
void dfplayer_RepeatAll(bool repeat)// Test complete
{
	dfplayer_SendControlMessage(CMD_REPEAT, 0, repeat);

}


/**
 * @brief   MP3폴더에 있는 트랙 재생
 * @note    'MP3'는 폴더이름
 * @param  	track  			1 ~ 30000 (or 0 ~ 2999) 파일명 넘버링에 따라 둘 중 하나의 범위를 가짐
 */
void dfplayer_PlayMP3Folder(uint16_t track)// Test complete
{
	dfplayer_SendControlMessage(CMD_SET_MP3_FOLDER, (uint8_t)(track >> 8), (uint8_t)(track & 0xFF));
}



/**
 * @brief   음악파일 재생중에 ADVERT폴더 안에 저장된 파일로 끼어들기(인터럽트)
 * @note    ADVERT 폴더는 3000개 트랙 지원
 * @param  	track  			1 ~ 30000 (or 0 ~ 2999) 파일명 넘버링에 따라 둘 중 하나의 범위를 가짐
 */
void dfplayer_InsertAdvertisement(uint16_t track)// Test complete
{
	dfplayer_SendControlMessage(CMD_INSERT_ADVERT,(uint8_t)(track >> 8), (uint8_t)(track & 0xFF));
}



/**
 * @brief   3000개 트랙 지원하는 01~15번 폴더에서 트랙을 선택하여 재생
 * @param  	folder  		1~ 15
 * @param  	track  			1 ~ 30000 (or 0 ~ 2999) 파일명 넘버링에 따라 둘 중 하나의 범위를 가짐
 */
void dfplayer_Play3KFolder(uint8_t folder, uint16_t track) //TODO ongoing - 3
{
	if (folder > 15)
	{
		folder = 15;
	}

	if (track > 3000)
	{
		track = 3000;
	}

	dfplayer_SendControlMessage(CMD_SET_3K_FOLDER, (folder << 4) | (uint8_t)(track & 0xF00) >> 8, (uint8_t)(track & 0xFF));
}



/**
 * @brief   ADVERT폴더 안에 인터럽트된 재생 파일을 정지 시키고 원래 재생파일로 복귀
 *
 */
void dfplayer_StopAdvertisement(void)// Test complete
{

	dfplayer_SendControlMessage(CMD_STOP_ADVERT_GOBACK, 0, 0);
}




/**
 * @brief   정지
 * @note    다시 재생시키면 처음 부터 시작함
 */
void dfplayer_Stop(void)// Test complete
{

	dfplayer_SendControlMessage(CMD_STOP, 0, 0);
}



/**
 * @brief   지정한 폴대 내 반복 재생
 * @note 	stop 커맨드 명렁어가 올 때 까지 멈추지 않고 반복
 */
void dfplayer_RepeatTrackInFolder(uint8_t folder) //TODO ongoing - 2
{
	if (folder > 99)
	{
		folder = 99;
	}

	dfplayer_SendControlMessage(CMD_REPEAT_FOLDER_TRACK, 0, folder);
}



/**
 * @brief   랜덤으로 트랙 재생
 *
 */
void dfplayer_RandomTrack(void) // Test complete
{
	dfplayer_SendControlMessage(CMD_RANDOM_PLAY, 0, 0);
}



/**
 * @brief   현재 트랙 반복 재생
 * @param  	turn_onoff      True (1) : Turn On,  False (0) : Turn Off
 * @warning stop이나 pause인 상태에서는 응답하지 않음. turn_onoff를 사용하여 정지시킬 수 있음
 */
void dfplayer_RepeatCurrentTrack(bool turn_onoff) // Test complete
{
	dfplayer_SendControlMessage(CMD_RANDOM_PLAY, 0, !turn_onoff);
}



/**
 * @brief   DAC 설정
 * @NOTE    DFplayer 모듈이 전원이 켜지면 default 값으로 DAC는 켜져있음
 * @param  	turn_onoff      True (1) : Turn On,  False (0) : Turn Off
 */
void dfplayer_SetDAC(bool turn_onoff)// Test complete
{
	dfplayer_SendControlMessage(CMD_SET_DAC, 0, !turn_onoff);
}
