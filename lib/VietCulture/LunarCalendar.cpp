#include "LunarCalendar.h"

#include <cmath>

namespace vietcal {
namespace {

constexpr double kPi = 3.14159265358979323846;

double newMoon(int k) {
  const double T = k / 1236.85;
  const double T2 = T * T;
  const double T3 = T2 * T;
  const double dr = kPi / 180.0;
  double Jd1 = 2415020.75933 + 29.53058868 * k + 0.0001178 * T2 - 0.000000155 * T3;
  Jd1 += 0.00033 * std::sin((166.56 + 132.87 * T - 0.009173 * T2) * dr);
  const double M = 359.2242 + 29.10535608 * k - 0.0000333 * T2 - 0.00000347 * T3;
  const double Mpr = 306.0253 + 385.81691806 * k + 0.0107306 * T2 + 0.00001236 * T3;
  const double F = 21.2964 + 390.67050646 * k - 0.0016528 * T2 - 0.00000239 * T3;
  double C1 = (0.1734 - 0.000393 * T) * std::sin(M * dr) + 0.0021 * std::sin(2 * dr * M);
  C1 -= 0.4068 * std::sin(Mpr * dr) + 0.0161 * std::sin(dr * 2 * Mpr);
  C1 -= 0.0004 * std::sin(dr * 3 * Mpr);
  C1 += 0.0104 * std::sin(dr * 2 * F) - 0.0051 * std::sin(dr * (M + Mpr));
  C1 -= 0.0074 * std::sin(dr * (M - Mpr)) + 0.0004 * std::sin(dr * (2 * F + M));
  C1 -= 0.0004 * std::sin(dr * (2 * F - M)) - 0.0006 * std::sin(dr * (2 * F + Mpr));
  C1 += 0.0010 * std::sin(dr * (2 * F - Mpr)) + 0.0005 * std::sin(dr * (2 * Mpr + M));
  double deltat;
  if (T < -11) {
    deltat = 0.001 + 0.000839 * T + 0.0002261 * T2 - 0.00000845 * T3 - 0.000000081 * T * T3;
  } else {
    deltat = -0.000278 + 0.000265 * T + 0.000262 * T2;
  }
  return Jd1 + C1 - deltat;
}

double sunLongitude(double jdn) {
  const double T = (jdn - 2451545.0) / 36525.0;
  const double T2 = T * T;
  const double dr = kPi / 180.0;
  const double M = 357.52910 + 35999.05030 * T - 0.0001559 * T2 - 0.00000048 * T * T2;
  const double L0 = 280.46645 + 36000.76983 * T + 0.0003032 * T2;
  double DL = (1.914600 - 0.004817 * T - 0.000014 * T2) * std::sin(dr * M);
  DL += (0.019993 - 0.000101 * T) * std::sin(dr * 2 * M) + 0.000290 * std::sin(dr * 3 * M);
  double L = L0 + DL;
  const double omega = 125.04 - 1934.136 * T;
  L = L - 0.00569 - 0.00478 * std::sin(omega * dr);
  L *= dr;
  L -= kPi * 2 * std::floor(L / (kPi * 2));
  return L;
}

int sunLongitudeSector(int dayNumber, double tz) {
  return static_cast<int>(std::floor(sunLongitude(dayNumber - 0.5 - tz / 24.0) / kPi * 6));
}

int newMoonDay(int k, double tz) { return static_cast<int>(std::floor(newMoon(k) + 0.5 + tz / 24.0)); }

int lunarMonth11(int yy, double tz) {
  const int off = jdFromDate(31, 12, yy) - 2415021;
  const int k = static_cast<int>(std::floor(off / 29.530588853));
  int nm = newMoonDay(k, tz);
  if (sunLongitudeSector(nm, tz) >= 9) nm = newMoonDay(k - 1, tz);
  return nm;
}

int leapMonthOffset(int a11, double tz) {
  const int k = static_cast<int>(std::floor((a11 - 2415021.076998695) / 29.530588853 + 0.5));
  int i = 1;
  int arc = sunLongitudeSector(newMoonDay(k + i, tz), tz);
  int last;
  do {
    last = arc;
    ++i;
    arc = sunLongitudeSector(newMoonDay(k + i, tz), tz);
  } while (arc != last && i < 14);
  return i - 1;
}

const char* const kCan[10] = {"Giáp", "Ất", "Bính", "Đinh", "Mậu", "Kỷ", "Canh", "Tân", "Nhâm", "Quý"};
const char* const kChi[12] = {"Tý", "Sửu", "Dần", "Mão", "Thìn", "Tỵ", "Ngọ", "Mùi", "Thân", "Dậu", "Tuất", "Hợi"};

int mod(int a, int m) { return ((a % m) + m) % m; }

}  // namespace

int jdFromDate(int dd, int mm, int yy) {
  const int a = (14 - mm) / 12;
  const int y = yy + 4800 - a;
  const int m = mm + 12 * a - 3;
  return dd + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}

SolarDate dateFromJd(int jd) {
  const int a = jd + 32044;
  const int b = (4 * a + 3) / 146097;
  const int c = a - (b * 146097) / 4;
  const int d = (4 * c + 3) / 1461;
  const int e = c - (1461 * d) / 4;
  const int m = (5 * e + 2) / 153;
  SolarDate out;
  out.day = e - (153 * m + 2) / 5 + 1;
  out.month = m + 3 - 12 * (m / 10);
  out.year = b * 100 + d - 4800 + m / 10;
  return out;
}

int32_t localDayIndex(int64_t epoch) {
  const int64_t local = epoch + static_cast<int64_t>(kVietnamTz * 3600);
  int64_t days = local / 86400;
  if (local % 86400 < 0) --days;
  return static_cast<int32_t>(days);
}

SolarDate solarFromEpoch(int64_t epoch) {
  // 2440588 = JD cua 1970-01-01
  return dateFromJd(2440588 + localDayIndex(epoch));
}

LunarDate solarToLunar(int dd, int mm, int yy) {
  const double tz = kVietnamTz;
  const int dayNumber = jdFromDate(dd, mm, yy);
  const int k = static_cast<int>(std::floor((dayNumber - 2415021.076998695) / 29.530588853));
  int monthStart = newMoonDay(k + 1, tz);
  if (monthStart > dayNumber) monthStart = newMoonDay(k, tz);
  int a11 = lunarMonth11(yy, tz);
  int b11 = a11;
  int lunarYear;
  if (a11 >= monthStart) {
    lunarYear = yy;
    a11 = lunarMonth11(yy - 1, tz);
  } else {
    lunarYear = yy + 1;
    b11 = lunarMonth11(yy + 1, tz);
  }
  LunarDate out;
  out.day = dayNumber - monthStart + 1;
  const int diff = (monthStart - a11) / 29;
  out.leap = false;
  int lunarMonth = diff + 11;
  if (b11 - a11 > 365) {
    const int leapDiff = leapMonthOffset(a11, tz);
    if (diff >= leapDiff) {
      lunarMonth = diff + 10;
      if (diff == leapDiff) out.leap = true;
    }
  }
  if (lunarMonth > 12) lunarMonth -= 12;
  if (lunarMonth >= 11 && diff < 4) lunarYear -= 1;
  out.month = lunarMonth;
  out.year = lunarYear;
  return out;
}

const char* canOfYear(int lunarYear) { return kCan[mod(lunarYear + 6, 10)]; }
const char* chiOfYear(int lunarYear) { return kChi[mod(lunarYear + 8, 12)]; }
const char* canOfDay(int jd) { return kCan[mod(jd + 9, 10)]; }
const char* chiOfDay(int jd) { return kChi[mod(jd + 1, 12)]; }

bool isTet(const LunarDate& l) { return l.month == 1 && l.day == 1 && !l.leap; }

const char* holidayName(int dd, int mm, int yy) {
  // Ngay le duong lich
  if (dd == 1 && mm == 1) return "Tết Dương lịch";
  if (dd == 3 && mm == 2) return "Ngày thành lập Đảng";
  if (dd == 8 && mm == 3) return "Quốc tế Phụ nữ";
  if (dd == 21 && mm == 4) return "Ngày Sách và Văn hóa đọc Việt Nam";
  if (dd == 23 && mm == 4) return "Ngày Sách Thế giới";
  if (dd == 30 && mm == 4) return "Ngày Giải phóng miền Nam";
  if (dd == 1 && mm == 5) return "Quốc tế Lao động";
  if (dd == 19 && mm == 5) return "Ngày sinh Chủ tịch Hồ Chí Minh";
  if (dd == 1 && mm == 6) return "Quốc tế Thiếu nhi";
  if (dd == 27 && mm == 7) return "Ngày Thương binh Liệt sĩ";
  if (dd == 2 && mm == 9) return "Quốc khánh";
  if (dd == 20 && mm == 10) return "Ngày Phụ nữ Việt Nam";
  if (dd == 20 && mm == 11) return "Ngày Nhà giáo Việt Nam";
  if (dd == 22 && mm == 12) return "Ngày Quân đội Nhân dân Việt Nam";

  // Ngay le am lich
  const LunarDate l = solarToLunar(dd, mm, yy);
  if (!l.leap) {
    if (l.month == 1 && l.day == 1) return "Mùng 1 Tết Nguyên đán";
    if (l.month == 1 && l.day == 2) return "Mùng 2 Tết";
    if (l.month == 1 && l.day == 3) return "Mùng 3 Tết";
    if (l.month == 1 && l.day == 15) return "Rằm tháng Giêng";
    if (l.month == 3 && l.day == 10) return "Giỗ Tổ Hùng Vương";
    if (l.month == 4 && l.day == 15) return "Phật Đản";
    if (l.month == 5 && l.day == 5) return "Tết Đoan Ngọ";
    if (l.month == 7 && l.day == 15) return "Vu Lan";
    if (l.month == 8 && l.day == 15) return "Tết Trung Thu";
    if (l.month == 12 && l.day == 23) return "Ông Táo chầu trời";
    if (l.month == 12 && l.day >= 29) {
      // Giao thua: ngay hom sau la mung 1 thang Gieng
      const SolarDate next = dateFromJd(jdFromDate(dd, mm, yy) + 1);
      const LunarDate ln = solarToLunar(next.day, next.month, next.year);
      if (isTet(ln)) return "Giao thừa";
    }
  }
  return nullptr;
}

}  // namespace vietcal
