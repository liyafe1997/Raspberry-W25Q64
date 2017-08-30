//
// SPI シリアルフラッシュメモリ W25Q64 操作検証プログラム
// W25Q64のメモリ領域構造
//   総バイト数 8388608
//   メモリ空間 24ビットアドレス指定 0x00000 - 0x7FFFFF 
//   ブロック数 128 (64KB/ブロック)
//   セクタ数 2048  ( 4KB/セクタ)
//   総セクタ数 2048

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "W25Q64.h"
#define WRITEDATA 128
#define SPI_CHANNEL 0 // /dev/spidev0.0

//#define SPI_CHANNEL 1 // /dev/spidev0.1

//
// 書込みデータのダンプリスト
// dt(in) : データ格納先頭アドレス
// n(in)  : 表示データ数
//
void dump(uint8_t *dt, uint32_t n) {
  uint32_t sz;
  char buf[64];
  uint16_t clm = 0;
  uint8_t data;
  uint8_t sum;
  uint8_t vsum[16];
  uint8_t total =0;
  uint32_t saddr =0;
  uint32_t eaddr =n-1;
  sz = eaddr -saddr;
  
  printf("----------------------------------------------------------\n");
  uint16_t i;
  for (i=0;i<16;i++) vsum[i]=0;  
  uint32_t addr;
  for (addr = saddr; addr <= eaddr; addr++) {
    data = dt[addr];
    if (clm == 0) {
      sum =0;
      printf("%05x: ",addr);
    }

    sum+=data;
    vsum[addr % 16]+=data;
    
    printf("%02x ",data);
    clm++;
    if (clm == 16) {
      printf("|%02x \n",sum);
      clm = 0;
    }
  }
  printf("----------------------------------------------------------\n");
  printf("       ");
  for (i=0; i<16;i++) {
    total+=vsum[i];
    printf("%02x ",vsum[i]);
  }
  printf("|%02x \n\n",total);
}

void main() {
    uint8_t buf[256];     // 取得データ
    uint8_t wdata[128];    // 書込みデータ
    uint8_t i;
    
    uint16_t n;           // 取得データ数

    // SPI channel 0 を 2MHz で開始。
    // Start SPI channel 0 with 2MHz
    if (wiringPiSPISetup(SPI_CHANNEL, 2000000) < 0) {
      printf("SPISetup failed:\n");
    }
    
    // フラッシュメモリ利用開始
    // Start Flush Memory
    W25Q64_begin(SPI_CHANNEL);
    
    // JEDEC IDの取得テスト
    // JEDEC ID Get
    W25Q64_readManufacturer(buf);
    printf("JEDEC ID : ");
    for (i=0; i< 3; i++) {
      printf("%x ",buf[i]);
    }
    printf("\n");
    
    // Unique IDの取得テスト
    // Unique ID Get
    W25Q64_readUniqieID(buf);
    printf("Unique ID : ");
    for (i=0; i< 7; i++) {
      printf("%x ",buf[i]);
    }
    printf("\n");
    
    // データの読み込み(アドレス0から256バイト取得)
    // Read 256 byte data from Address=0
    memset(buf,0,256);
    n =  W25Q64_read(0, buf, 256);
    printf("Read Data: n=%d\n",n);
    dump(buf,256);

    // 高速データの読み込み(アドレス0から256バイト取得)
    // First read 256 byte data from Address=0
    memset(buf,0,256);
    n =  W25Q64_fastread(0, buf, 256);
    printf("Fast Read Data: n=%d\n",n);
    dump(buf,256);

    // セクタ単位の削除
    // Erase data by Sector
    n = W25Q64_eraseSector(0,true);
    printf("Erase Sector(0): n=%d\n",n);
    memset(buf,0,256);
    n =  W25Q64_read (0, buf, 256);
    dump(buf,256);
 
// READ a 8MB BINARY FILE AND WRITE
	FILE *fp;
	int  nVal;
	fp = fopen("/home/pi/K56CM.BIN", "r"); //FILE PATH
int shanqui = 1;
int alli = 1;
for(alli = 1;alli<=2048;alli++){
    n = W25Q64_eraseSector(alli-1,true);
    printf("Erase Sector(%d): n=%d\n",alli-1,n);




    for(shanqui = 1;shanqui<=32;shanqui++){
	int seciii = 1;
	for(seciii = 1;seciii <= 128;seciii++){
	    nVal = 0;
	    fread(&nVal, 1, 1, fp);
            fseek(fp,(((alli*4096)+(shanqui*128)-128)-4096)+seciii,SEEK_SET);
            wdata[seciii-1]=nVal;
	    printf("%02X", wdata[seciii-1]);
        }
        n =  W25Q64_pageWrite(alli-1, (shanqui*128)-128, wdata, 128);
        printf("page_write(%d,%d,d,128): n=%d\n",alli-1,(shanqui*128)-128,n);
// --READ a 8MB BINARY FILE AND WRITE	


    // データの読み込み(アドレス0から256バイト取得)
    // Read 256 byte data from Address=0
    memset(buf,0,256);
    n =  W25Q64_read( (((alli*4096)+(shanqui*128)-128)-4096), buf, 128);
    printf("Read Data From %d: n=%d\n",(((alli*4096)+(shanqui*128)-128)-4096),n);
    dump(buf,256);


    }


}
close(fp);

















    // 高速データの読み込み(アドレス0から256バイト取得)
    // First read 256 byte data from Address=0
    //memset(buf,0,256);
    //n =  W25Q64_fastread(0,buf, 256);
    //printf("Fast Read Data: n=%d\n",n);
    //dump(buf,256);

    // ステータスレジスタ1の取得
    // Get fron Status Register1
    buf[0] = W25Q64_readStatusReg1();
    printf("Status Register-1: %x\n",buf[0]);

    // ステータスレジスタ2の取得
    // Get fron Status Register2
    buf[0] = W25Q64_readStatusReg2();
    printf("Status Register-2: %x\n",buf[0]);
}


