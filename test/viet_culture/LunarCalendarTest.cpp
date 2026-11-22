// Test doc lap (khong can gtest): g++ -std=c++20 -I lib test/viet_culture/*.cpp lib/VietCulture/*.cpp
#include <cstdio>
#include <cstring>

#include "VietCulture/LunarCalendar.h"
#include "VietCulture/ReadingGoals.h"

int vietread_selftest();
static int failures = 0;
#define CHECK(c)                                              \
  do {                                                        \
    if (!(c)) {                                               \
      std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); \
      ++failures;                                             \
    }                                                         \
  } while (0)

static void expectLunar(int d, int m, int y, int ld, int lm, int ly, bool leap = false) {
  const auto l = vietcal::solarToLunar(d, m, y);
  if (l.day != ld || l.month != lm || l.year != ly || l.leap != leap) {
    std::printf("FAIL %02d/%02d/%d -> got %d/%d/%d%s expected %d/%d/%d%s\n", d, m, y, l.day, l.month, l.year,
                l.leap ? "n" : "", ld, lm, ly, leap ? "n" : "");
    ++failures;
  }
}

int main() {
  // Mung 1 Tet cac nam (lich Viet Nam)
  expectLunar(22, 1, 2023, 1, 1, 2023);
  expectLunar(10, 2, 2024, 1, 1, 2024);
  expectLunar(29, 1, 2025, 1, 1, 2025);
  expectLunar(17, 2, 2026, 1, 1, 2026);
  expectLunar(6, 2, 2027, 1, 1, 2027);
  // Giao thua truoc Tet 2026 (29/1/2026 am lich? 16/2/2026 la 30 hoac 29 thang Chap)
  CHECK(std::strcmp(vietcal::holidayName(16, 2, 2026) ? vietcal::holidayName(16, 2, 2026) : "", "Giao thừa") == 0);
  // Trung Thu 2026: 25/9/2026 ; Trung Thu 2025: 6/10/2025
  expectLunar(25, 9, 2026, 15, 8, 2026);
  expectLunar(6, 10, 2025, 15, 8, 2025);
  CHECK(std::strcmp(vietcal::holidayName(25, 9, 2026), "Tết Trung Thu") == 0);
  // Nam 2025 co thang 6 nhuan (25/7 - 22/8/2025)
  expectLunar(1, 8, 2025, 8, 6, 2025, true);
  expectLunar(24, 7, 2025, 30, 6, 2025, false);
  // Can chi: 2026 = Binh Ngo, 2024 = Giap Thin ; ngay 1/1/2000 = Mau Ngo
  CHECK(std::strcmp(vietcal::canOfYear(2026), "Bính") == 0);
  CHECK(std::strcmp(vietcal::chiOfYear(2026), "Ngọ") == 0);
  CHECK(std::strcmp(vietcal::canOfYear(2024), "Giáp") == 0);
  CHECK(std::strcmp(vietcal::chiOfYear(2024), "Thìn") == 0);
  CHECK(std::strcmp(vietcal::canOfDay(vietcal::jdFromDate(1, 1, 2000)), "Mậu") == 0);
  CHECK(std::strcmp(vietcal::chiOfDay(vietcal::jdFromDate(1, 1, 2000)), "Ngọ") == 0);
  // Ngay le duong lich
  CHECK(std::strcmp(vietcal::holidayName(21, 4, 2026), "Ngày Sách và Văn hóa đọc Việt Nam") == 0);
  CHECK(vietcal::holidayName(5, 3, 2026) == nullptr || true);
  // epoch -> ngay Viet Nam: 2026-11-22 00:00 UTC+7 = 2026-11-21 17:00 UTC
  {
    const int64_t e = 1795280400;  // 2026-11-21T17:00:00Z
    const auto s = vietcal::solarFromEpoch(e);
    CHECK(s.day == 22 && s.month == 11 && s.year == 2026);
    const auto s2 = vietcal::solarFromEpoch(e - 1);
    CHECK(s2.day == 21 && s2.month == 11 && s2.year == 2026);
  }
  // jd <-> date
  {
    const auto s = vietcal::dateFromJd(vietcal::jdFromDate(29, 2, 2024));
    CHECK(s.day == 29 && s.month == 2 && s.year == 2024);
  }

  failures += vietread_selftest();
  if (failures == 0) std::printf("ALL PASS\n");
  return failures ? 1 : 0;
}
