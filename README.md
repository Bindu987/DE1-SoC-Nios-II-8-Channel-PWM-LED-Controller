DE1-SoC Nios II + 8-Channel PWM

Custom **8-ch PWM** IP on **DE1-SoC (Cyclone V)**. A **Nios II** C app sets LED brightness using push-buttons.

---

## Overview
- Verilog PWM IP (shared **DIV**, per-channel **DUTY**).
- Top connects PWM → `LEDR[7:0]`.
- Nios II app (polling) maps `KEY[3:1]` to brightness; `KEY[0]` resets.

---

## Repo

docs/ ECE_278_homework5.pdf
ip/ pwm.v
sys/ 
    bindhw5.qsys
    bindhw5.qip 
    bindhw5.sopcinfo
quartus/
      homework5.qpf 
      homework5.qsf 
      homework5top.v
software/app/ main.c

---

## Tools
- Quartus Prime **18.1 Lite**
- Nios II **SBT for Eclipse**
- DE1-SoC board + USB-Blaster II

---

## Build & Run (Quick)
1. Open `quartus/homework5.qpf` in Quartus → **Processing → Start Compilation**.
2. Program FPGA: **Tools → Programmer** → load generated `.sof` → **Start**.
3. In Nios II SBT: **File → New → Nios II Application and BSP from Template**  
   - SOPC Info: `sys/bindhw5.sopcinfo` → Template: *Blank Project*  
   - Replace app `main.c` with `software/app/main.c`.
4. **Build** the app → **Run As → Nios II Hardware**.

> If the PWM base symbol differs, use the exact macro from BSP `system.h` (e.g., `PWM_IP_0_BASE`).

---

## Controls
- `KEY3` = High brightness  
- `KEY2` = Medium brightness  
- `KEY1` = Low brightness  
- `KEY0` = Reset

*If `pwm.v` has `assign pwm_out = ~pwm_out_reg;`, brightness is inverted → **smaller DUTY = brighter LED**.*

---

## Register Map (word offsets)
- Channel `i` (0..7): `2*i + 0` → `DIV` (period), `2*i + 1` → `DUTY` (duty cycle)  
- Byte offset = word offset × 4

---

## Troubleshooting
- **ELF download error:** Clean/rebuild BSP; check JTAG and board power.
- **No LED change:** Use reasonable `DIV` (≈ 1k–100k); verify LED pins in `.qsf`.
- **Brightness reversed:** Confirm PWM inversion in `pwm.v`.

---

This is a part of course work for Embedded system coursework.
