# CrossPoint Việt (bản thử nghiệm)

Bản mở rộng của CrossPoint Reader 2.0.0 cho Xteink X3/X4, tập trung vào bản sắc Việt và thói quen đọc.

## Tính năng
- **Màn hình ngủ "Lịch Việt"** (Cài đặt → Màn hình → Màn hình ngủ → *Lịch Việt*):
  ngày dương, ngày âm, can chi năm/ngày, ngày lễ Việt (Tết, Rằm, Giỗ Tổ, Trung Thu, 21/4 Ngày Sách và Văn hóa đọc…),
  một câu ca dao/tục ngữ mỗi ngày.
- **Chuỗi ngày đọc**: đọc từ 10 phút/ngày tính là một ngày; hôm nay chưa đọc thì chuỗi vẫn giữ đến hết ngày.
- **Mục tiêu tuần** (Cài đặt → Đọc → *Mục tiêu đọc mỗi tuần*): 1 / 2,5 / 5 / 10 giờ, có thanh tiến độ.
- **7 huy hiệu**: Mầm sách, Mọt sách, Thư sinh, Nhóm lửa, Tuần chuyên cần, Tre già măng mọc, Đạt mục tiêu tuần.
- Dữ liệu lưu tại `/.crosspoint/viet_reading.json` trên thẻ SD.
- Cần giờ chính xác: kết nối Wi-Fi một lần (NTP). Chưa có giờ thì màn hình ngủ chỉ hiện ca dao và thống kê tổng.

## Mã nguồn mới
- `lib/VietCulture/` – lịch âm (thuật toán Hồ Ngọc Đức, UTC+7), ngày lễ, ca dao, thống kê/huy hiệu. Không phụ thuộc Arduino.
- `src/VietReadingStore.*` – lưu thống kê vào SD.
- `src/activities/boot_sleep/VietDayScreen.*` – vẽ màn hình ngủ.
- Sửa nhỏ: `CrossPointSettings.h`, `SettingsList.cpp`, `SleepActivity.cpp`, `ReaderActivity.cpp`, `english.yaml`, `vietnamese.yaml`.

## Kiểm thử (chạy được không cần phần cứng)
```
g++ -std=c++20 -Wall -Wextra -I lib test/viet_culture/*.cpp lib/VietCulture/*.cpp -o viet_test && ./viet_test
```
Test kiểm tra Tết 2023–2027, tháng 6 nhuận 2025, Giao thừa, Trung Thu, can chi, chuỗi ngày, mục tiêu tuần, huy hiệu.

## Biên dịch firmware
Chưa được biên dịch hay nạp thử trên thiết bị. Cách làm: fork repo, áp patch, chạy workflow build (env `default` cho X3/X4):
```
git apply crosspoint-viet.patch
pio run -e default
```
Khi nạp thử, giữ sẵn `update.bin` gốc để khôi phục (xem docs/fix-bricked-xteink.md).
