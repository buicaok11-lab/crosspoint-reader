#pragma once
// Thong ke doc sach: chuoi ngay doc, muc tieu tuan, huy hieu. Thuan logic, khong I/O, khong cap phat heap.
#include <cstddef>
#include <cstdint>

namespace vietread {

constexpr size_t kHistoryDays = 60;
constexpr uint32_t kStreakMinSeconds = 600;  // doc >= 10 phut moi tinh la "da doc" trong ngay
constexpr uint32_t kDefaultWeeklyGoalMinutes = 150;

struct DayEntry {
  int32_t day;  // chi so ngay theo gio Viet Nam (vietcal::localDayIndex)
  uint32_t seconds;
};

enum Badge : uint8_t {
  BADGE_FIRST_HOUR = 0,  // Mam sach: tong 1 gio
  BADGE_TEN_HOURS,       // Mot sach: tong 10 gio
  BADGE_FIFTY_HOURS,     // Thu sinh: tong 50 gio
  BADGE_STREAK_3,        // Nho lua: 3 ngay lien tiep
  BADGE_STREAK_7,        // Tuan chuyen can: 7 ngay lien tiep
  BADGE_STREAK_30,       // Tre gia mang moc: 30 ngay lien tiep
  BADGE_GOAL_WEEK,       // Vuot muc tieu tuan it nhat mot lan
  BADGE_COUNT
};

const char* badgeName(Badge b);         // ten huy hieu, UTF-8
const char* badgeDescription(Badge b);  // mo ta ngan, UTF-8

class ReadingLog {
 public:
  // Ghi them 'seconds' vao ngay 'day'. Tu dong cat lich su cu hon kHistoryDays ngay.
  void addSession(int32_t day, uint32_t seconds);

  uint32_t secondsOn(int32_t day) const;
  // Chuoi ngay doc lien tiep ket thuc o 'today'. Neu hom nay chua du thoi gian, chuoi tinh den hom qua
  // (khong bi dut cho den het hom nay).
  int currentStreak(int32_t today) const;
  int longestStreak() const { return longest_; }
  // Tong giay doc trong tuan (thu Hai - Chu nhat) chua 'today'.
  uint32_t weekSeconds(int32_t today) const;
  uint32_t weeklyGoalSeconds() const { return weeklyGoalMinutes_ * 60u; }
  int weeklyGoalPercent(int32_t today) const;  // 0..100 (cat tran)
  uint32_t totalSeconds() const { return total_; }
  uint16_t goalWeeksMet() const { return goalWeeksMet_; }

  uint32_t earnedBadges(int32_t today) const;  // bitmask theo enum Badge
  static bool has(uint32_t mask, Badge b) { return (mask >> b) & 1u; }

  void setWeeklyGoalMinutes(uint32_t minutes);
  uint32_t weeklyGoalMinutes() const { return weeklyGoalMinutes_; }

  // Phuc hoi tu bo nho ben (JSON). Du lieu khong hop le bi bo qua; danh sach duoc sap xep lai.
  void restore(const DayEntry* entries, size_t count, uint32_t total, int longest, uint16_t goalWeeksMet,
               uint32_t weeklyGoalMinutes);
  const DayEntry* entries() const { return entries_; }
  size_t size() const { return count_; }

  // Chi so ngay trong tuan: Thu Hai = 0 ... Chu nhat = 6 (ngay 0 la Thu Nam 1/1/1970).
  static int weekdayMonday0(int32_t day) { return static_cast<int>(((day % 7) + 7 + 3) % 7); }

 private:
  DayEntry entries_[kHistoryDays] = {};
  size_t count_ = 0;
  uint32_t total_ = 0;
  int longest_ = 0;
  uint16_t goalWeeksMet_ = 0;
  uint32_t weeklyGoalMinutes_ = kDefaultWeeklyGoalMinutes;
};

}  // namespace vietread
