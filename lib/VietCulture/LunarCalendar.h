#pragma once
// Am lich Viet Nam (mui gio UTC+7), thuat toan Ho Ngoc Duc. Khong phu thuoc Arduino.
#include <cstdint>

namespace vietcal {

struct SolarDate {
  int day;
  int month;
  int year;
};

struct LunarDate {
  int day;
  int month;
  int year;
  bool leap;  // thang nhuan
};

constexpr double kVietnamTz = 7.0;

// Ngay Julian (so nguyen) cua ngay duong lich Gregorian.
int jdFromDate(int dd, int mm, int yy);
SolarDate dateFromJd(int jd);

// Epoch (giay UTC) -> ngay duong lich theo gio Viet Nam.
SolarDate solarFromEpoch(int64_t epoch);
// So ngay ke tu 1970-01-01 theo gio dia phuong Viet Nam (dung lam khoa "ngay").
int32_t localDayIndex(int64_t epoch);

LunarDate solarToLunar(int dd, int mm, int yy);

// Can Chi. Ket qua la chuoi UTF-8 tinh, vi du "Binh Ngo" (co dau).
const char* canOfYear(int lunarYear);
const char* chiOfYear(int lunarYear);
const char* canOfDay(int jd);
const char* chiOfDay(int jd);

// Ten ngay le / dip dac biet (UTF-8) hoac nullptr neu khong co.
// Kiem tra ca ngay le duong lich lan am lich. Giao thua duoc nhan biet o ngay cuoi thang Chap.
const char* holidayName(int dd, int mm, int yy);

// 1 neu la 1/1 am lich, 0 neu khong (tien ich cho giao dien).
bool isTet(const LunarDate& l);

}  // namespace vietcal
