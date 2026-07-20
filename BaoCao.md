# BÁO CÁO BÀI TẬP LỚN

**Môn học:** IT4210 – Hệ nhúng

**ĐỀ TÀI:** XÂY DỰNG TRÒ CHƠI FLAPPY BIRD TRÊN KIT STM32F429

**Sinh viên thực hiện:**

| STT | Họ và tên | Mã số sinh viên (MSSV) |
| :---: | :--- | :--- |
| 1 | Duy Quang | [Nhập MSSV 1] |
| 2 | [Tên thành viên 2] | [Nhập MSSV 2] |
| 3 | [Tên thành viên 3] | [Nhập MSSV 3] |
| 4 | [Tên thành viên 4] | [Nhập MSSV 4] |
| 5 | [Tên thành viên 5] | [Nhập MSSV 5] |

**Hà Nội, tháng 7 năm 2026**

---

## Mục lục
1. [GIỚI THIỆU ĐỀ TÀI](#1-giới-thiệu-đề-tài)
2. [KIẾN TRÚC PHẦN CỨNG](#2-kiến-trúc-phần-cứng)
3. [THIẾT KẾ PHẦN MỀM](#3-thiết-kế-phần-mềm)
4. [TỔNG KẾT VÀ ĐÁNH GIÁ](#4-tổng-kết-và-đánh-giá)

---

## 1. GIỚI THIỆU ĐỀ TÀI

### 1.1 Mô tả đề tài
Trong lĩnh vực hệ thống nhúng, việc kết hợp giữa phần cứng và phần mềm để xây dựng các ứng dụng hoạt động theo thời gian thực là một nội dung quan trọng. Đề tài này nhằm xây dựng một bản clone của trò chơi Flappy Bird trên nền tảng vi điều khiển STM32F429, sử dụng màn hình LCD tích hợp trên kit để hiển thị đồ họa.

Giao diện trò chơi được thiết kế theo mô hình Model-View-Presenter (MVP) bằng thư viện TouchGFX (phiên bản 4.26.1). Hệ thống sử dụng FreeRTOS (CMSIS-OS2) để quản lý đa nhiệm, cơ chế ngắt phần cứng (EXTI) để tiếp nhận thao tác người chơi mà không cần polling (dò hỏi liên tục), và bộ nhớ RAM để lưu trữ trạng thái game.

### 1.2 Mục tiêu dự án
*   Áp dụng các kiến thức về vi điều khiển STM32 và lập trình nhúng C/C++.
*   Xây dựng giao diện đồ họa mượt mà (60 FPS) bằng TouchGFX.
*   Thiết kế hệ thống xử lý sự kiện dựa trên ngắt (Interrupt-driven) và cờ trạng thái (flags).
*   Triển khai thuật toán Object Pooling và tối ưu hóa bộ nhớ cho thiết bị nhúng.

---

## 2. KIẾN TRÚC PHẦN CỨNG

### 2.1 Thành phần hệ thống
Hệ thống được phát triển trên bộ Kit STM32F429I-DISC1.

| Thành phần | Chức năng |
| :--- | :--- |
| **Kit STM32F429I-DISC1** | Bo mạch chính, chứa vi điều khiển STM32F429ZI (ARM Cortex-M4, 180 MHz). |
| **Màn hình LCD TFT (320x240)** | Hiển thị giao diện đồ họa TouchGFX. Sử dụng giao tiếp SPI/LTDC. |
| **Nút nhấn User (PA0)** | Đầu vào duy nhất của người chơi, được cấu hình ngắt EXTI0 cạnh lên. |

### 2.2 Sơ đồ kết nối và Cấu hình chân (Pinout)
*   **PA0:** Cấu hình `GPIO_EXTI0`, chế độ `Rising edge`, có điện trở kéo xuống (`Pull-down`). Nút này được sử dụng để điều hướng menu và điều khiển chim vỗ cánh.
*   **LTDC & DMA2D:** Các ngoại vi phần cứng được cấu hình qua STM32CubeMX để hỗ trợ render đồ họa xuất ra màn hình.

---

## 3. THIẾT KẾ PHẦN MỀM

### 3.1 Cấu trúc thư mục và Mô hình MVP
Dự án tuân thủ mô hình Model-View-Presenter của TouchGFX, tách biệt giao diện (`generated`), hiển thị (`View`), trung gian (`Presenter`), và dữ liệu (`Model`).
Mọi code logic game được viết tay nằm trong thư mục `TouchGFX/gui/` và các khối `USER CODE` của `Core/Src/`.

### 3.2 Quản lý Đa nhiệm (FreeRTOS)


### 3.3 Thiết kế logic game

#### 3.3.1 Luồng hoạt động của trò chơi
Logic của trò chơi được đóng gói chủ yếu trong lớp `GameScreenView` và được điều khiển bởi hệ thống sự kiện thời gian thực. Cốt lõi của luồng hoạt động nằm ở hàm `handleTickEvent()`, được TouchGFX tự động gọi liên tục theo tần số quét của màn hình (khoảng 60 khung hình/giây). 

Mỗi khi hàm này được kích hoạt, chương trình sẽ thực hiện tuần tự các công việc: đọc trạng thái tín hiệu đầu vào từ ngắt, tính toán hệ thực vật lý cho nhân vật (chim), cập nhật tọa độ các chướng ngại vật (ống nước), kiểm tra va chạm, và cuối cùng là yêu cầu TouchGFX vẽ lại các vùng đồ họa có sự thay đổi. Trò chơi được chia làm ba trạng thái chính: Chờ bắt đầu (`!isGameStarted`), Đang diễn ra, và Kết thúc (`isGameOver`).

#### 3.3.2 Điều khiển nhân vật và hệ thực vật lý
Thay vì đọc trực tiếp chân GPIO bằng vòng lặp, chương trình tiếp nhận thao tác điều khiển thông qua biến cờ `birdPressedFlag`. Biến này được cập nhật từ trình phục vụ ngắt EXTI0. Ở mỗi chu kỳ tick, chương trình kiểm tra cờ này, nếu có tín hiệu nhấn, cờ sẽ được xóa về `0` và hàm `requestJump()` được gọi để bắt đầu chu kỳ bay của chim.

Chuyển động của chim được mô phỏng dựa trên cơ chế vật lý cơ bản với hai hằng số: trọng lực (`gravity = 0.2f`) và lực nhảy (`jumpForce = -4.0f`).
*   **Hiệu ứng rơi tự nhiên:** Trong mỗi khung hình, vận tốc hiện tại (`birdVelocity`) sẽ được cộng dồn một lượng bằng trọng lực, sau đó tọa độ Y của chim (`birdY`) được cộng thêm vận tốc này. Quá trình này mô phỏng gia tốc rơi tự do.
*   **Hiệu ứng vỗ cánh:** Khi người chơi nhấn nút, vận tốc của chim bị ghi đè trực tiếp bằng `jumpForce` (giá trị âm do trục Y hướng xuống dưới). Điều này triệt tiêu ngay lập tức lực rơi hiện tại và đẩy chim bay lên trên.
*   **Giới hạn không gian:** Chương trình luôn kiểm tra tọa độ `birdY`. Nếu chim bay vượt quá mép trên màn hình (`birdY < 0`), tọa độ sẽ bị chặn lại ở mức 0 để tránh chim bay mất khỏi khung hình. Nếu chim chạm vào mặt đất (`birdY >= ground`), trò chơi ngay lập tức chuyển sang trạng thái kết thúc.

#### 3.3.3 Di chuyển và tái sử dụng chướng ngại vật
Nhằm tối ưu hóa tài nguyên RAM hữu hạn trên vi điều khiển, chương trình không liên tục khởi tạo và hủy các đối tượng ống nước. Thay vào đó, hệ thống ứng dụng kỹ thuật *Object Pooling* bằng cách chỉ sử dụng một mảng gồm 3 vùng chứa đồ họa (`Container`).

Ba cặp ống này liên tục được cuộn từ phải sang trái bằng cách trừ tọa độ X một lượng bằng tốc độ hiện tại (`currentPipeSpeed`). Khi một ống di chuyển hoàn toàn ra khỏi cạnh trái màn hình, thay vì bị xóa bỏ, chương trình sẽ tìm kiếm ống đang ở vị trí xa nhất bên phải màn hình, tính toán một khoảng cách an toàn (`PIPE_DISTANCE = 205`), và dịch chuyển ống vừa khuất ra phía sau ống đó. 

Đồng thời, chiều cao khe hở của ống được thiết lập lại thông qua hàm `randomizePipeHeight()`. Để tránh lỗi hệ thống (HardFault) do hàm `rand()` của thư viện chuẩn C gây ra trên các vi điều khiển, thuật toán Linear Congruential Generator (LCG) đã được tự xây dựng. Thuật toán này sử dụng một biến trạng thái `rngState` (được gieo mầm bằng `HAL_GetTick()`) để sinh ra các giá trị bù trừ (`offset`) ngẫu nhiên, tạo sự đa dạng cho từng ván chơi.

#### 3.3.4 Kiểm tra va chạm và tính điểm
Việc phát hiện va chạm được xử lý thông qua hàm `checkCollision()`, áp dụng thuật toán kiểm tra giao nhau giữa hai hình chữ nhật (Axis-Aligned Bounding Box - AABB). Trong mỗi chu kỳ, tọa độ và kích thước bao quanh chim được so sánh trực tiếp với tọa độ và kích thước của các phần tử con trong mỗi ống (gồm `BottomPipe` và `TopPipe`). Chỉ cần một điều kiện giao nhau xảy ra, hàm trả về `true` và trò chơi kết thúc.

Để tính điểm, hệ thống thiết kế một vùng ẩn có tên là `ScoreZone` nằm ở giữa hai ống. Để khắc phục hiện tượng điểm số bị tăng liên tục nhiều lần khi chim bay ngang qua vùng này trong nhiều khung hình, một mảng cờ `scored[3]` được triển khai. Khi chim chạm vào `ScoreZone` và cờ tương ứng đang là `false`, điểm số sẽ được cộng thêm 1, đồng thời cờ được lật thành `true`. Cờ này chỉ được đặt lại về `false` khi ống nước đó được tái sử dụng (chuyển ra sau cùng), đảm bảo mỗi ống chỉ được tính điểm duy nhất một lần.

#### 3.3.5 Tăng độ khó và chuyển đổi giao diện
Để tăng tính thử thách cho người chơi, hệ thống tích hợp cơ chế tăng độ khó theo chu kỳ. Mỗi khi điểm số chia hết cho 12 (`score % 12 == 0`), tốc độ di chuyển của ống (`currentPipeSpeed`) sẽ được tăng thêm 0.5 đơn vị. Giá trị này được giới hạn bởi hằng số `SPEED_MAX` để đảm bảo trò chơi không rơi vào trạng thái bất khả thi.

Đi kèm với việc tăng độ khó, chương trình sẽ gọi hàm `toggleBackground()` để lật trạng thái ngày/đêm (`isNightMode`). Ở chu kỳ cập nhật tiếp theo, hệ thống sẽ thay đổi thuộc tính hiển thị (`setVisible`) giữa hai bức ảnh nền Ngày và Đêm, mang lại sự thay đổi về mặt thị giác mà không làm gián đoạn luồng vật lý của trò chơi.

#### 3.3.6 Quản lý điểm cao nhất (High Score)
Để duy trì tính cạnh tranh, điểm kỷ lục được lưu trữ xuyên suốt vòng đời của ứng dụng. Dữ liệu này không nằm trong màn hình trò chơi mà được quản lý bởi đối tượng `Model`, đảm bảo nó không bị xóa đi khi bộ nhớ của `GameScreenView` được giải phóng. Thông qua lớp trung gian `GameScreenPresenter`, View có thể lấy được điểm kỷ lục hiện tại để hiển thị, đồng thời gửi yêu cầu lưu điểm mới (`saveHighScore`) nếu thành tích hiện tại vượt qua kỷ lục cũ khi trò chơi kết thúc.

#### 3.3.7 Xử lý khi kết thúc trò chơi
Khi phát hiện va chạm với ống hoặc chạm mặt đất, chương trình gọi hàm `setGameOver()`. Cờ `isGameOver` được thiết lập thành `true`, khiến luồng xử lý vật lý nhánh `PLAYING` ngừng hoạt động. Khối giao diện `GameOverContainer` sẽ được kích hoạt hiển thị, cập nhật bộ đệm ký tự để in ra điểm số vừa đạt được và điểm kỷ lục mới nhất. 

Đặc biệt, nếu trò chơi kết thúc do chim va chạm với ống nước (chưa chạm đất), một đoạn mã vật lý nhỏ trong nhánh `GAME_OVER` của `handleTickEvent()` vẫn tiếp tục hoạt động. Đoạn mã này mô phỏng lực rơi tự do, kéo con chim rơi thẳng xuống mặt đất rồi mới dừng lại hoàn toàn, tạo ra hiệu ứng kết thúc trò chơi tự nhiên và trực quan.
---

## 4. TỔNG KẾT VÀ ĐÁNH GIÁ

### 4.1 Kết quả đạt được
*   Xây dựng thành công tựa game Flappy Bird hoạt động mượt mà trên bộ Kit STM32F429I-DISC1 mà không cần màn hình cảm ứng.
*   Tối ưu hóa hiệu năng hiển thị và bộ nhớ: Xóa bỏ dữ liệu ảnh dư thừa (tiết kiệm hàng triệu dòng mảng pixel), sử dụng Object Pooling thay vì tạo mới liên tục.
*   Xử lý thành công các lỗi nền tảng như: Xung đột hàm `rand()` của newlib, tắc nghẽn I2C (I/O blocking) do TouchGFX mặc định gọi chip cảm ứng.

### 4.2 Ưu điểm
*   Thiết kế kiến trúc phần mềm sạch sẽ, tuân thủ mô hình MVP.
*   Quản lý input phần cứng cực tốt: Dùng cờ Volatile kết hợp với ngắt để đảm bảo an toàn cho RTOS và độ trễ thấp.
*   Khả năng mở rộng tốt nhờ cấu trúc Runtime Theming (thay đổi Theme lúc chạy) không cần nhân bản code.

### 4.3 Nhược điểm & Hướng phát triển
*   Hệ thống lưu trữ High Score (Điểm cao nhất) hiện tại mới chỉ lưu trên RAM (trong đối tượng `Model`), sẽ bị xóa khi khởi động lại kit. Hướng phát triển tiếp theo là lưu vào EEPROM hoặc Flash.
*   Chưa có âm thanh (do Board chưa tích hợp module loa hoặc Buzzer).