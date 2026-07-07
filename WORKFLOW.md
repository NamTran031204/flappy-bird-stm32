# Quy trình phát triển TouchGFX + STM32CubeIDE (từ đầu)

> Áp dụng cho board dạng **STM32F429I-DISCO** (LTDC + SDRAM ngoài + FreeRTOS/CMSIS-OS2 + TouchGFX).
> Dùng khi **đã chốt bài toán** và bắt đầu dựng dự án từ con số không.
> Các ràng buộc an toàn tham chiếu tới `.claude/rules/R1`–`R7`.

## Mục lục
- [Nguyên tắc xuyên suốt](#nguyên-tắc-xuyên-suốt)
- [Giai đoạn 0 — Thiết kế trên giấy](#giai-đoạn-0--thiết-kế-trên-giấy)
- [Giai đoạn 1 — Cấu hình phần cứng (.ioc)](#giai-đoạn-1--cấu-hình-phần-cứng-ioc)
- [Giai đoạn 2 — Thiết lập TouchGFX](#giai-đoạn-2--thiết-lập-touchgfx)
- [Giai đoạn 3 — Thiết kế UI trong Designer](#giai-đoạn-3--thiết-kế-ui-trong-designer)
- [Giai đoạn 4 — Viết logic (MVP)](#giai-đoạn-4--viết-logic-mvp)
- [Giai đoạn 5 — Nối phần cứng ↔ GUI](#giai-đoạn-5--nối-phần-cứng--gui)
- [Giai đoạn 6 — Build / Flash / Test / Lặp](#giai-đoạn-6--build--flash--test--lặp)
- [Vòng lặp phát triển hằng ngày](#vòng-lặp-phát-triển-hằng-ngày)
- [Checklist nhanh](#checklist-nhanh)

---

## Nguyên tắc xuyên suốt

3 thứ chi phối toàn bộ quy trình:

1. **Ba "nguồn sự thật" — luôn sửa qua công cụ, không sửa file sinh:**
   | Nguồn sự thật | Nội dung | Sửa bằng |
   |---|---|---|
   | `.ioc` | Phần cứng: chân/clock/ngoại vi/NVIC/DMA/thứ tự init | **Device Configuration Tool** trong CubeIDE |
   | `.touchgfx` | Giao diện UI (screen/widget/asset) | **TouchGFX Designer** |
   | `TouchGFX/gui/` | Logic game (MVP) | **Code tay** |

2. **Handshake cấu hình (R7):** thay đổi cấu hình là việc **làm trong GUI** rồi Generate. Code tay chỉ viết **logic** dùng lại cấu hình đã sinh. **Không "vá tay" file generated.**

3. **Git là lưới an toàn (R1/R5):** commit ở trạng thái chạy tốt **trước mỗi lần Generate**. **Một lần generate = một thay đổi.**

---

## Giai đoạn 0 — Thiết kế trên giấy

Chốt trước khi mở IDE để tránh cấu hình đi cấu hình lại:

- **Ngoại vi cần dùng:** input (nút/EXTI/cảm biến), output (LED/buzzer/LCD), truyền thông (UART/SPI/I2C).
- **Sơ đồ chân:** đối chiếu **vùng cấm chân (R3)** — LTDC/FMC/SDRAM/SWD đã chiếm rất nhiều chân.
  - ⚠️ **Bẫy số 1:** KHÔNG bật **USB_OTG_FS** — cướp PA11/PA12 của LTDC → sai màu / màn trắng.
- **Kiến trúc UI:** liệt kê các Screen (menu, gameplay, game-over…), widget mỗi màn, dữ liệu chia sẻ giữa màn.
- **Ngân sách RAM:** framebuffer 240×320×2 ≈ 150 KB → **phải nằm ở SDRAM ngoài** (`0xD0000000`), không vừa SRAM nội (R4).

---

## Giai đoạn 1 — Cấu hình phần cứng (.ioc)

Làm hoàn toàn trong GUI CubeIDE.

1. **New STM32 Project** → chọn đúng board → hỏi *"Initialize peripherals with default mode?"* chọn **Yes** (có sẵn LTDC/FMC/clock của board).
2. **Clock (tab Clock Configuration):** đảm bảo **PLLSAI** cấp pixel clock cho LTDC. Sai clock → mất VSYNC → TouchGFX đứng.
3. **Bật ngoại vi cần dùng** (Pinout & Configuration): GPIO/EXTI cho input, DMA cho ngoại vi cần DMA, NVIC cho ngắt.
   - Ngắt gọi API RTOS phải có **priority ≥ `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`**.
4. **FreeRTOS:** chọn **CMSIS-OS2**; tạo task/queue/semaphore ở đây nếu muốn CubeMX quản lý.
5. **Project Manager → Code Generator (BẮT BUỘC — R1):**
   - ✅ **Keep User Code when re-generating**
   - ✅ **Backup previously generated files**
   - ❌ **KHÔNG** bật *"Generate peripheral initialization as a pair of .c/.h files"* (`CoupleFile=false`) — báo cáo cộng đồng: gây hỏng đồ họa TouchGFX.
6. **Kiểm tra thứ tự init (R4)** — bất biến phải giữ qua mọi lần generate:
   ```
   HAL_Init → SystemClock_Config(gồm PLLSAI) → GPIO → DMA
     → FMC(SDRAM) → DMA2D → LTDC → SPI5(init ILI9341) → MX_TouchGFX_Init → osThreadNew(TouchGFX_Task)
   ```
   **FMC phải đứng trước LTDC và TouchGFX** (SDRAM sẵn sàng trước khi ai đọc framebuffer).
7. **Generate Code** → **commit git ngay** ("baseline phần cứng chạy được").

---

## Giai đoạn 2 — Thiết lập TouchGFX

1. Trong `.ioc`, bật **X-CUBE-TOUCHGFX** (Software Packs) → sinh cầu nối `ApplicationTemplate.touchgfx.part` (kích thước/bpp/rotation) và `App/app_touchgfx.c`.
2. Generate Code → commit.
3. **Xác minh chuỗi hook (R4):** `MX_TouchGFX_Init()` trong `main()` → `osThreadNew(TouchGFX_Task)` → `app_touchgfx.c` forward → `touchgfx_taskEntry` (vòng lặp không return). Đứt 1 mắt = màn trắng.
4. **Kiểm tra linker (R4):** section framebuffer phải nằm trong dải SDRAM. CubeMX **không** tự sửa `.ld` — kiểm tra tay.

> Trên board này: LTDC quét RGB parallel làm đường pixel; **SPI5 chỉ gửi lệnh init ILI9341 sang chế độ RGB một lần lúc boot** rồi Display-ON. Chuỗi lệnh init đặt trong `/* USER CODE */`.

---

## Giai đoạn 3 — Thiết kế UI trong Designer

1. Mở `.touchgfx` bằng **TouchGFX Designer**.
2. Tạo **Screen** và kéo thả **widget** (Image, TextArea, Container…). Đặt tên widget rõ ràng — chúng thành member của `*ViewBase` mà code tay tham chiếu.
3. Thêm **assets** (ảnh/font/text) qua Designer, không copy file tay.
4. **Generate Code trong Designer** → sinh `TouchGFX/generated/` + các lớp `*ViewBase`.
5. **Build simulator PC** ngay trong Designer để xem UI (vòng lặp nhanh, không cần nạp mạch).

> **Luật vàng generate 2 tầng (R1):** mỗi lần Generate trong CubeIDE xong → **phải mở Designer Generate lại**. Bỏ bước → `gui_generated` lệch `.part` → widget rác.

---

## Giai đoạn 4 — Viết logic (MVP)

Vùng **an toàn nhất, code tay thoải mái**: `TouchGFX/gui/`.

- Mỗi Screen có 3 lớp viết tay: **View / Presenter** + **Model** dùng chung.
- **Game loop** đặt trong `View::handleTickEvent()` (chạy ~60 Hz theo VSYNC): physics, di chuyển đối tượng, collision, tính điểm.
- Sau khi đổi thuộc tính widget trong code, gọi `.invalidate()` (và `moveTo`/`setXY` cho vị trí) để TouchGFX vẽ lại vùng dirty.
- Trạng thái chia sẻ giữa màn (high score…) đi qua **Presenter → Model**.

---

## Giai đoạn 5 — Nối phần cứng ↔ GUI

Ranh giới HAL/RTOS ↔ TouchGFX. **Chỉ viết trong `/* USER CODE */` (R2):**

- **Input:** đọc nút (polling trong task, hoặc — tốt hơn — **EXTI callback** trong `stm32f4xx_it.c`).
- **Cầu nối nên là một message queue:** ISR/task đẩy sự kiện vào `queueHandle` → `View::handleTickEvent` drain queue mỗi frame. GUI khai báo handle bằng `extern "C"`.
- Tránh gọi trực tiếp từ ISR vào code TouchGFX — luôn đi qua queue để tách tầng.

> **R7:** muốn chuyển input polling → EXTI thì **bạn** bật EXTI/NVIC trong `.ioc` → Generate → đọc lại `stm32f4xx_it.c` xác nhận `HAL_GPIO_EXTI_Callback` đã sinh → **rồi mới** viết thân callback.

---

## Giai đoạn 6 — Build / Flash / Test / Lặp

1. Build trong CubeIDE (artifact ở `STM32CubeIDE/Debug/`).
2. Flash bằng **STM32CubeProgrammer** (hoặc từ Designer với toolchain GCC).
3. **Reset cứng board sau nạp** (một số case chỉ trắng tới khi reset).
4. Smoke test → OK thì **commit** ("baseline chạy được" mới).
5. **Nếu màn trắng/rác — không đoán mò (R5):**
   - `git diff GOOD HEAD` trên **6 file rủi ro**: `main.c`, `*.ld`, `stm32f4xx_hal_msp.c`, `stm32f4xx_it.c`, `FreeRTOSConfig.h`, `*.ioc`.
   - Revert chọn lọc để cô lập thủ phạm → vá lại **qua CubeMX** (không sửa tay ngoài USER CODE).
   - Phân biệt: **trắng hoàn toàn** → mất ngắt LTDC/VSYNC, PLLSAI, init ILI9341, hook TouchGFX đứt. **Rác/nhiễu** → FMC timing/AF/speed sai, framebuffer lệch SDRAM, bpp vênh.

---

## Vòng lặp phát triển hằng ngày

```
Cần đổi HW/UI?  ──yes──► Sửa trong .ioc / Designer ► Generate (2 tầng) ► đọc lại xác minh ► commit
     │no
     ▼
Chỉ đổi logic  ──────► Code trong TouchGFX/gui/ (hoặc USER CODE) ► test simulator/board ► commit
```

**Ưu tiên:** thử ở **simulator PC** trước (nhanh, không rủi ro) → khi logic ổn mới nạp mạch để test phần liên quan HAL/timing thật.

---

## Checklist nhanh

**Trước mỗi lần Generate Code:**
- [ ] Cây làm việc git sạch (đã commit trạng thái chạy được)
- [ ] Chỉ làm **một** thay đổi cấu hình

**Sau mỗi lần Generate trong CubeIDE:**
- [ ] Mở TouchGFX Designer **Generate lại** (nếu đụng UI/hiển thị)
- [ ] `git diff --stat` + soi 6 file rủi ro
- [ ] Init order còn đúng (FMC trước LTDC/TouchGFX)
- [ ] Không bật nhầm OTG_FS / không tụt GPIO speed/AF chân FMC-LTDC
- [ ] `MX_TouchGFX_Init()` vẫn được gọi trong `main()`
- [ ] Build sạch (0 error) + smoke test **rồi mới** commit

**Chỉ viết code tay ở 2 nơi:**
- [ ] `TouchGFX/gui/` (logic)
- [ ] Khối `/* USER CODE BEGIN/END */` trong `Core/` (wiring, ngắt, DMA)
