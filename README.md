# RPI3 OS Kernel
### 規格
* CPU: Cortex-A53 (ARMv8) 64-bit SoC @ 1.4GHz
* Memory: 1GB LPDDR2 SDRAM
* Chip: Broadcom BCM2837B0
### 已實作的功能
* 基本的輸入輸出
* Simple shell
* Bootloader & Bootloader Self Relocation (由於沒有檔案系統的支援，因此kernel image是透過mini uart從本地端傳送至rpi3的記憶體中)
* 實現系統呼叫和硬體中斷處理(e.g.CPU Core Timer Interrupt Handler)
* 實現Process Context Switch & Scheduler並以round-robin作為排程演算法
### 參考資料
1. [https://grasslab.github.io/osdi/en/index.html](https://grasslab.github.io/osdi/en/index.html)
2. [https://oscapstone.github.io/](https://oscapstone.github.io/)
3. [https://blog.ykzheng.com/notes/nctu/osdi/](https://blog.ykzheng.com/notes/nctu/osdi/)
4. [https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf](https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf)
