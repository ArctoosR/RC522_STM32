/*
 * dfplayer.h
 *
 *  Created on: 2022. 2. 23.
 *      Author: sungt
 *
 *
 ******************************************************************************
 * @attention
 *
 * #0. mp3/wmv 인덱스 번호는 파일명이 아닌 파일의 물리적인 순서
 * #1. 루프 디렉토리에 있는 트랙은 3000곡 까지 선택 재생 가능 (CMD_SET_TRAK_NUMBER : CMD 0x03)
 * #2. 폴더는 01 ~ 99개 까지 지원
 * #3. 01 ~ 15번 폴더는 3000트랙 까지 지원, 16 ~ 99번 폴더는 255트랙 까지 지원 (CMD_SET_PLAY_TRACK_FOLDER : 0x0F)
 * #4. MP3 특수 폴더 생성 시 MP3 폴더는 65536 트랙까지 지원하나 속도가 느려지므로 3000 트랙까지 권장  (CMD_SET_MP3_FOLDER : 0x12)
 * #5. ADVERT 특수 폴더 생성 시 ADVERT 폴더는 3000 트랙 지원
 * #6. ADVERT 특수 폴더를 이용하여 인터럽트 제어 가능  (CMD_INSERT_ADVERT : 0x13, CMD_STOP_ADVERT_GOBACK : 0x15)
 *
 *
 *
 */

#ifndef SRC_COMMON_HW_INCLUDE_DFPLAYER_H_
#define SRC_COMMON_HW_INCLUDE_DFPLAYER_H_

#include "main.h"
//#include "hw_def.h"

#include "stdbool.h"
#define   _USE_HW_DFPLAYER

#ifdef _USE_HW_DFPLAYER


/*
 *
 *           Define                 |  CMD  |        Description          |        Parameters (16bit)
 *------------------------------------------------------------------------------------------------------------------------------------
 */

/*
 * @brief	Directly send commands, no parameters returned
 * @note 	DFplayer의 동작 명령어
 *
 */

/* Control Commands */
#define CMD_NEXT                      0x01    //   다음곡 재생
#define CMD_PREVIOUS                  0x02    //   이전곡 재생
#define CMD_SET_TRAK_NUMBER           0x03    //   트랙 번호 지정						1 ~ 3000 (or 0 ~ 2999)
#define CMD_INC_VOLUME                0x04    //   볼륨 증가
#define CMD_DEC_VOLUME                0x05    //   볼륨 감소
#define CMD_SET_VOLUME                0x06    //   특정 볼륨 값 지정					0 ~ 30
#define CMD_SET_EQ                    0x07    //   EQ (음향효과) 지정				0 : Normal / 1 : Pop / 2 : Rock / 3 : Jazz / 4 : Classic / 5 : Base
#define CMD_REPEAT_TRACK              0x08    //   트랙 반복 재생
#define CMD_SET_PLAYBACK_SOURCE       0x09    //   재생 시킬 저장 장치 지정				0 : U / 1 : TF / 2 : AUX / 3 : SLEEP / 4 : FLASH
#define CMD_ENTER_INTO_STANDBY        0x0A    //   저전력 모드 진입
//#define CMD_NORMAL_WORK               0x0B		   N/A
#define CMD_RESET                     0x0C    //   모듈 초기화
#define CMD_PLAYBACK                  0x0D    //   재생
#define CMD_PAUSE                     0x0E    //   일시 정지
#define CMD_SET_PLAY_TRACK_FOLDER     0x0F    //   재생 폴더 및 트랙 지정				folder : 01 ~ 99 / folder 01 ~ 15 -> track : 001 ~ 3000 / folder 16 ~ 99 -> track : 001 ~ 255
#define CMD_VOLUME_ADJUST             0x10    //   볼륨 조정						Parameter_MSB= 1: Open volume adjust / Parameter_LSB = set volume gain 0 ~ 31
#define CMD_REPEAT					  0x11    //   반복 재생						0 : stop play / 1 : start repeat play
#define CMD_SET_MP3_FOLDER            0x12    //   MP3라는 폴더가 있을 시 재생폴더로 지정
#define CMD_INSERT_ADVERT			  0x13    //   인터럽트 재생
#define CMD_SET_3K_FOLDER			  0x14    //   3K 지원하는 폴더 내 트랙 재생
#define CMD_STOP_ADVERT_GOBACK        0x15    //   인터럽트 재생 멈추고 복귀
#define CMD_STOP                      0x16    //   정지
#define CMD_REPEAT_FOLDER_TRACK       0x17    //   지정한 폴더내 반복재생
#define CMD_RANDOM_PLAY               0x18    //   랜덤 재생
#define CMD_REPEAT_CURRENT_TRACK      0x19    //   현재 트랙 반복재생
#define CMD_SET_DAC                   0x1A    //   DAC 설정

/*
 * @brief	Query the System Parameters
 * @note	DFplayer에서의 어떠한 정보를 얻고 싶을 때 아래와 같은 명령어를 보내주면 그에 따른 응답이 회신됨
 *
 */

/* Query Commands */
//#define CMD_Q_STAY1                    0x3C	//  N/A
//#define CMD_Q_STAY2                    0x3D   //  N/A
//#define CMD_Q_STAY3                    0x3E   //  N/A
#define CMD_Q_SEND_INIT_PARA           0x3F   //  초기화 매개 변수 전송				0 - 0x0F (each bit represent one device of the low-four bits)
#define CMD_Q_RETURN_ERROR             0x40	  //  오류를 반환,재전송 요청
#define CMD_Q_REPLY                    0x41	  //  응답
#define CMD_Q_CURRENT_STATUS           0x42	  //  현재 상태 요청
#define CMD_Q_CURRENT_VOLUME           0x43   //  현재 볼륨 요청
#define CMD_Q_CURRENT_EQ               0x44   //  현재 EQ 요청
#define CMD_Q_CURRENT_PLAYBACK_MODE    0x45   //  현재 재생 모드 요청
#define CMD_Q_CURRENT_SOFTWARE_VER     0x46   //  현재 소프트웨어 버전 요청
#define CMD_Q_TOTAL_NUM_TF_FILES       0x47   //  TF Card 내 전체 파일 수 요청
#define CMD_Q_TOTAL_NUM_U_FILES        0x48	  //  U Disk  내 전체 파일 수 요청
#define CMD_Q_TOTAL_NUM_FLASH_FILES    0x49	  //  Flash   내 전체 파일 수 요청
//#define CMD_Q_KEEP_ON                  0x4A	  //  N/A
#define CMD_Q_CURRENT_TRACK_TF         0x4B	  //  TF Card 내  현재 재생중인 트랙번호  요청
#define MP3_Q_CURRENT_TRACK_U_DISK     0x4C	  //  U Dish  내 현재 재생중인 트랙번호 요청
#define MP3_Q_CURRENT_TRACK_FLASH      0x4D	  //  Flash   내 현재 재생중인 트랙번호 요청


/* @brief
 * @note	enum 으로 처리로 해서도 사용 가능
 *
 */

/* Commands Parameters */

//EQ Parameter
#define EQ_NORMAL                      0
#define EQ_POP                         1
#define EQ_ROCK                        2
#define EQ_JAZZ                        3
#define EQ_CLASSIC                     4
#define EQ_BASE                        5

//Playback Source Parameter
#define PLAYBACK_SOURCE_U              0
#define PLAYBACK_SOURCE_TF             1
#define PLAYBACK_SOURCE_AUX            2
#define PLAYBACK_SOURCE_SLEEP          3
#define PLAYBACK_SOURCE_FLASH          4

//Repeat Parameter
#define REPEAT_STOP					   0
#define REPEAT_START				   1



/*
 * @brief	DFPlayer 내 직렬  프로토콜 형식
 * @note	DFPlayer와 MCU 간 UART로  통신하되, DFPlayer 직렬 포멧을 지켜야함
 *
 *     0        1            2          3          4             5               6              7              8           9
 * |  Start | Version | Data_Length |  CMD  |  Feedback  | Parameter_MSB | Parameter_LSB | Checksum_MSB | Checksum_LSB |  End  |
 * -----------------------------------------------------------------------------------------------------------------------------
 * |  0x7E  |  0xFF   |     0x06    |       |  0x01/0x00 |               |               |              |              |       |                                                   0xEF
 * -----------------------------------------------------------------------------------------------------------------------------
 * |  고정값    |   고정값     |    고정값           |       |  사용/사용x   |    DH라고도 함       |   DL이라고도 함      |              |              |  고정값  |
 *
 *
 * @ Start 	 	   : 시작 바이트
 * @ Version 	   : 버전 정보
 * @ Data_Length   : Version 부터 Parameter_LSB 까지 자신을 포한함 바이트 수  즉, 6 Byte
 * @ Feedback	   : 명령 피드백, 피드백 필요 : 0x01, 피드백 필요없음 : 0x00
 * @ Parameter_MSB : 파라미터 최상위 비트
 * @ Parameter_LSB : 파라미터 최하위 비트
 * @ Checksum_MSB  : 체크썸 최상위 비트
 * @ Checksum_LSB  : 체크썸 최하위 비트
 * @ End		   : 종료 바이트
 *
 */

/* DFplayer Serial Format SIZE & INDEX */
#define DFPLAYER_COMMUNICATION_SIZE    10

#define START_INDEX					   0
#define VERSION_INDEX				   1
#define DATA_LENGTH_INDEX			   2
#define CMD_INDEX					   3
#define FEEDBACK_INDEX				   4
#define PARAMETER_MSB_INDEX			   5
#define PARAMETER_LSB_INDEX			   6
#define CHECKSUM_MSB_INDEX			   7
#define CHECKSUM_LSB_INDEX			   8
#define END_INDEX					   9

/* DFplayer Serial Communication Format Default Value */
#define START						   0x7E
#define VERSION						   0xFF
#define DATA_LENGTH					   0x06
#define FEED_BACK					   0x01
#define NO_FEED_BACK				   0x00
#define	END							   0xEF


#define UART_MAX_BUF_SIZE       256

void delay_ms(uint32_t ms);


void delay_us(uint32_t us);

uint32_t millis(void);


uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length);

void apDFPlayerTest(char rx_data);
void dfplayer_Init(void);

void dfplayer_Next(void);
void dfplayer_Previous(void);
void dfplayer_SetTrakNumber(int16_t track);
void dfplayer_IncreaseVolume(void);
void dfplayer_DecreaseVolume(void);
void dfplayer_SetVolume(uint8_t volume);
void dfplayer_SetEQ(int8_t eq);
void dfplayer_RepeatTrack(uint8_t track);
void dfplayer_SetSource(uint8_t source);
void dfplayer_Standby(void);
void dfplayer_Reset(void);
void dfplayer_Play(void);
void dfplayer_Pause(void);
void dfplayer_PlayTrackInFolder(uint8_t folder, uint8_t track);
void dfplayer_VolumeAdjust(bool turn_onoff, uint8_t gain);
void dfplayer_RepeatAll(bool repeat);
void dfplayer_PlayMP3Folder(uint16_t track);
void dfplayer_InsertAdvertisement(uint16_t track);
void dfplayer_Play3KFolder(uint8_t folder, uint16_t track);
void dfplayer_StopAdvertisement(void);
void dfplayer_Stop(void);
void dfplayer_RepeatTrackInFolder(uint8_t folder);
void dfplayer_RandomTrack(void);
void dfplayer_RepeatCurrentTrack(bool turn_onoff);
void dfplayer_SetDAC(bool turn_onoff);

#endif /* _USE_HW_DFPLAYER */

#endif /* SRC_COMMON_HW_INCLUDE_DFPLAYER_H_ */
