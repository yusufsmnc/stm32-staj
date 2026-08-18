# STM32F407 Staj Projeleri

Bu depo, Erciyes Üniversitesi Bilgisayar Mühendisliği Bölümü staj sürecinde STM32F407G-DISC1 geliştirme kartı üzerinde gerçekleştirilen gömülü yazılım projelerini içermektedir. Projeler, temel GPIO kontrolünden gerçek zamanlı işletim sistemi uygulamalarına ve güç ölçüm sistemine kadar geniş bir konu yelpazesini kapsamaktadır.

## Donanım

| Bileşen | Detay |
|---------|-------|
| Geliştirme Kartı | STM32F407G-DISC1 |
| İşlemci | ARM Cortex-M4 @ 168 MHz |
| Flash | 1 MB |
| RAM | 192 KB |
| Geliştirme Ortamı | STM32CubeIDE |
| Kütüphane | HAL (Hardware Abstraction Layer) |

---

## Projeler

### 01 — Yürüyen Işık, Kesme ve Zamanlayıcı
Kart üzerindeki dört LED'in (PD12–PD15) sırayla ileri ve geri yönde yakılması. HAL_Delay ile başlayan uygulama, TIM2 zamanlayıcı kesmesi tabanlı mimariye dönüştürüldü. Kullanıcı butonu (PA0) EXTI kesmesiyle LED hızını değiştirmekte, HAL_GetTick tabanlı debounce mekanizması kontak titremesini gidermektedir.

**Kullanılan Çevre Birimleri:** GPIO, TIM2, EXTI, NVIC

---

## 02 - SPI + LIS3DSH İvmeölçer
 
LIS3DSH ivmeölçer sensörünün SPI protokolü üzerinden okunması. Kimlik doğrulama, ham veri okuma, kalibrasyon ve alçak geçiren filtre uygulaması. DRDY kesmesiyle veri hazır tespiti. Roll ve pitch açısı hesabı.
 
**Kullanılan Çevre Birimleri:** SPI1, EXTI, LIS3DSH sürücüsü
 
---
 
## 03 - PWM + Servo
 
TIM4 ile LED parlaklık kontrolü ve TIM3 ile SG90 servo motor kontrolü. PWM duty cycle ayarı ile 0-180° servo açı kontrolü.
 
**Kullanılan Çevre Birimleri:** TIM3, TIM4, PWM
 
---
 
## 04 - MEMS + PID + FreeRTOS + State Machine
 
İvmeölçerden okunan roll açısını PID kontrolcüsü ile dengeleyip servo motoru süren tam sistem. FreeRTOS ile üç görev (sensör okuma, PID hesaplama, UART gönderimi) ve aralarında mesaj kuyruğu. Anti-windup, değişken dt, integral sıfırlama mekanizmaları. Dört durumlu state machine (KALİBRASYON, ÇALIŞIYOR, HATA, DURDURULDU). Python ile gerçek zamanlı grafik görselleştirme.
 
**Kullanılan Çevre Birimleri:** SPI1, TIM3, FreeRTOS, USART2, Queue, State Machine
 
---
 
## 05 - FreeRTOS Temel
 
FreeRTOS kavramlarının temel uygulaması. Dört LED görevi ile çoklu görev (multitasking) demosu. Mutex ile paylaşılan sayaca karşılıklı erişim koruması. Mesaj kuyruğu (Queue) ile buton görevi üzerinden LED hız kontrolü.
 
**Kullanılan Çevre Birimleri:** , Mutex, Queue, GPIO

---

### 06 — DAC/ADC Sinüs Üretimi (Zamanlayıcı Kesmesi)
Bellekte önceden hesaplanmış sinüs tablosu (Look-up Table) kullanılarak DAC'tan 50 Hz sinüs üretimi. TIM6 kesmesiyle DAC güncellenmekte, TIM3 kesmesiyle ADC örneklenmektedir. UART üzerinden Python ile gerçek zamanlı görselleştirme yapılmaktadır.

**Kullanılan Çevre Birimleri:** DAC, ADC, TIM6, TIM3, USART2

---

### 07 — DMA Tabanlı DAC/ADC Sinüs Üretimi
DAC ve ADC'nin DMA ile CPU müdahalesi olmaksızın otomatik olarak çalıştırıldığı proje. TIM6 TRGO sinyaliyle DAC DMA tetiklenmekte, TIM3 TRGO sinyaliyle ADC DMA örnekleme yapmaktadır. Vrms hesabı: 1.1597V (teorik: 1.1667V, hata: %0.60).

**Kullanılan Çevre Birimleri:** DAC, ADC, DMA, TIM6, TIM3, USART2

---

### 08 — PowerMeter — Güç Ölçüm ve Analiz Sistemi

Projenin en kapsamlı uygulaması. STM32F407 üzerinde elektrik sayacı protopipi.

#### Özellikler
- **Vrms, Irms** — Etkin değer hesabı
- **P_act** — Aktif güç (W)
- **P_react** — Reaktif güç (VAr), 90° faz kaydırma yöntemi
- **P_app** — Görünür güç (VA)
- **PF** — Güç faktörü
- **Frekans** — Sıfır geçişi sayımı ile 50 Hz doğrulama
- **Enerji** — Birikimli enerji (Wh), Flash'a kalıcı kayıt
- **LCD** — 16x2 HD44780 paralel sürücü, 3 farklı ekran modu
- **Python** — Gerçek zamanlı 6 panelli güç analizörü grafiği

#### Mimari

```
08_PowerMeter/
├── power_meter.h/.c      → Güç hesaplama (TI SLAA577)
├── lcd_2x16_driver.h/.c  → HD44780 4-bit paralel LCD sürücüsü
├── load_types.h          → Yük tipi tanımları (circular dependency çözümü)
├── display_sm.h/.c       → LCD ekran durum makinesi (nested SM)
├── system_sm.h/.c        → Ana sistem durum makinesi
└── flash_storage.h/.c    → STM32 iç Flash kalıcı bellek
```

#### Sistem Durum Makinesi

```
INIT → MEASURE ⇄ TRANSMIT
              ↕
         LOAD_CHANGE
```

| Durum | Açıklama |
|-------|----------|
| INIT | LCD hoşgeldin, başlangıç yük tipi |
| MEASURE | ADC ölçüm, LCD güncelleme (TIM2 500ms), buton takibi |
| TRANSMIT | UART CSV gönderimi, Flash kayıt (her 10s) |
| LOAD_CHANGE | Yük tipi değiştirme, DAC sinüs yeniden üretimi |

#### Yük Tipleri (Buton ile Seçim)

| Yük | Faz Açısı | PF |
|-----|-----------|-----|
| Rezistif | 0° | 1.00 |
| Endüktif | 30° | 0.87 |
| Motor | 60° | 0.50 |
| Kapasitif | -30° | 0.87 |

#### Buton Kontrolü
- **Kısa basış** (< 500ms) → LCD ekran modu değiştir
- **Uzun basış** (≥ 500ms) → Yük tipi değiştir

#### LCD Ekran Modları
```
Mod 1:               Mod 2:               Mod 3:
┌────────────────┐   ┌────────────────┐   ┌────────────────┐
│ P:1.166W       │   │ V:1.160 I:1.160│   │ PF:0.87 f:50Hz │
│ E:2.339 Wh     │   │ Yuk:Enduktif   │   │ Q:-0.673 VAr   │
└────────────────┘   └────────────────┘   └────────────────┘
```

#### Interrupt Mimarisi
```
TIM2 (500ms)  → lcdUpdateFlag
TIM4 (1000ms) → uartTransmitFlag
ADC DMA       → adcHazir
EXTI (PA0)    → kısa/uzun basış ayrımı
while(1)      → SystemSM_Run() + __WFI()
```

#### Flash Depolama
- **Adres:** Sektör 11 (0x080E0000)
- **Magic Number:** 0xDEADBEEF (geçerlilik kontrolü)
- **Checksum:** XOR tabanlı bütünlük doğrulama
- **Kayıt:** Her 10 saniyede bir

**Kullanılan Çevre Birimleri:** DAC CH1/CH2, ADC, DMA, TIM2/TIM3/TIM4/TIM6, USART3, GPIO, Flash

---

## Kullanılan Teknolojiler

| Teknoloji | Kullanım |
|-----------|---------|
| STM32CubeIDE | Geliştirme ortamı |
| HAL | Donanım soyutlama katmanı |
| FreeRTOS (CMSIS_V2) | Gerçek zamanlı işletim sistemi |
| DMA | CPU'suz bellek transferi |
| Python (pyserial, matplotlib) | Gerçek zamanlı veri görselleştirme |
| Git / GitHub | Versiyon kontrolü |

---

## Kaynaklar

- [STM32F407 Reference Manual (RM0090)](https://dfimg.dfrobot.com/enshop/image/data/DFR0161/NEW/DM00031020RM.pdf)
- [STM32F407 Datasheet](https://www.st.com/resource/en/datasheet/stm32f405zg.pdf)
- [STM32F4DISCOVERY Schematic](https://www.st.com/resource/en/schematic_pack/mb997-f407vgt6-d01_schematic.pdf)
- [LIS3DSH Datasheet](https://www.st.com/content/st_com/en/products/mems-and-sensors/accelerometers/lis3dsh.html)
- [TI SLAA577 — Metering Library](https://www.ti.com/lit/an/slaa577/slaa577.pdf)
- [Vishay LCD-016N002B-CFH-ET Datasheet](https://www.vishay.com/docs/37484/lcd016n002bcfhet.pdf)
- [Samek, M. — State Machines for Event-Driven Systems](https://barrgroup.com/embedded-systems/how-to/state-machines-event-driven-systems)

---

## Lisans

Bu proje MIT lisansı ile lisanslanmıştır.
