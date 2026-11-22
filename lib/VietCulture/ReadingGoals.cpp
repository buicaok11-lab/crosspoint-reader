#include "ReadingGoals.h"

#include <algorithm>

namespace vietread {

const char* badgeName(Badge b) {
  switch (b) {
    case BADGE_FIRST_HOUR: return "Mầm sách";
    case BADGE_TEN_HOURS: return "Mọt sách";
    case BADGE_FIFTY_HOURS: return "Thư sinh";
    case BADGE_STREAK_3: return "Nhóm lửa";
    case BADGE_STREAK_7: return "Tuần chuyên cần";
    case BADGE_STREAK_30: return "Tre già măng mọc";
    case BADGE_GOAL_WEEK: return "Đạt mục tiêu tuần";
    default: return "";
  }
}

const char* badgeDescription(Badge b) {
  switch (b) {
    case BADGE_FIRST_HOUR: return "Đọc tổng cộng 1 giờ";
    case BADGE_TEN_HOURS: return "Đọc tổng cộng 10 giờ";
    case BADGE_FIFTY_HOURS: return "Đọc tổng cộng 50 giờ";
    case BADGE_STREAK_3: return "Đọc 3 ngày liên tiếp";
    case BADGE_STREAK_7: return "Đọc 7 ngày liên tiếp";
    case BADGE_STREAK_30: return "Đọc 30 ngày liên tiếp";
    case BADGE_GOAL_WEEK: return "Hoàn thành mục tiêu đọc của một tuần";
    default: return "";
  }
}

uint32_t ReadingLog::secondsOn(int32_t day) const {
  for (size_t i = 0; i < count_; ++i) {
    if (entries_[i].day == day) return entries_[i].seconds;
  }
  return 0;
}

uint32_t ReadingLog::weekSeconds(int32_t today) const {
  const int32_t start = today - weekdayMonday0(today);
  const int32_t end = start + 6;
  uint32_t sum = 0;
  for (size_t i = 0; i < count_; ++i) {
    if (entries_[i].day >= start && entries_[i].day <= end) sum += entries_[i].seconds;
  }
  return sum;
}

int ReadingLog::weeklyGoalPercent(int32_t today) const {
  const uint32_t goal = weeklyGoalSeconds();
  if (goal == 0) return 0;
  const uint64_t pct = static_cast<uint64_t>(weekSeconds(today)) * 100u / goal;
  return pct > 100 ? 100 : static_cast<int>(pct);
}

int ReadingLog::currentStreak(int32_t today) const {
  int32_t cursor = today;
  if (secondsOn(cursor) < kStreakMinSeconds) cursor = today - 1;  // ngay hom nay con co the cuu chuoi
  int streak = 0;
  while (secondsOn(cursor) >= kStreakMinSeconds) {
    ++streak;
    --cursor;
    if (streak > static_cast<int>(kHistoryDays)) break;
  }
  return streak;
}

void ReadingLog::addSession(int32_t day, uint32_t seconds) {
  if (seconds == 0) return;
  const uint32_t weekBefore = weekSeconds(day);

  total_ += seconds;

  size_t idx = count_;
  for (size_t i = 0; i < count_; ++i) {
    if (entries_[i].day == day) {
      idx = i;
      break;
    }
  }
  if (idx < count_) {
    entries_[idx].seconds += seconds;
  } else {
    if (count_ == kHistoryDays) {
      if (day < entries_[0].day) {  // qua cu, khong con cho: chi cong vao tong
        goto after_insert;
      }
      for (size_t i = 1; i < count_; ++i) entries_[i - 1] = entries_[i];  // bo ngay cu nhat
      --count_;
    }
    {
      size_t pos = count_;
      while (pos > 0 && entries_[pos - 1].day > day) {
        entries_[pos] = entries_[pos - 1];
        --pos;
      }
      entries_[pos] = {day, seconds};
      ++count_;
    }
  }
after_insert:
  // Cat nhung ngay cu hon cua so lich su.
  if (count_ > 0) {
    const int32_t newest = entries_[count_ - 1].day;
    size_t keepFrom = 0;
    while (keepFrom < count_ && entries_[keepFrom].day <= newest - static_cast<int32_t>(kHistoryDays)) ++keepFrom;
    if (keepFrom > 0) {
      for (size_t i = keepFrom; i < count_; ++i) entries_[i - keepFrom] = entries_[i];
      count_ -= keepFrom;
    }
  }

  const uint32_t goal = weeklyGoalSeconds();
  if (goal > 0 && weekBefore < goal && weekSeconds(day) >= goal && goalWeeksMet_ < 0xFFFF) ++goalWeeksMet_;
  longest_ = std::max(longest_, currentStreak(day));
}

void ReadingLog::setWeeklyGoalMinutes(uint32_t minutes) {
  weeklyGoalMinutes_ = std::min<uint32_t>(std::max<uint32_t>(minutes, 10u), 3000u);
}

uint32_t ReadingLog::earnedBadges(int32_t today) const {
  const int best = std::max(longest_, currentStreak(today));
  uint32_t m = 0;
  if (total_ >= 3600u) m |= 1u << BADGE_FIRST_HOUR;
  if (total_ >= 36000u) m |= 1u << BADGE_TEN_HOURS;
  if (total_ >= 180000u) m |= 1u << BADGE_FIFTY_HOURS;
  if (best >= 3) m |= 1u << BADGE_STREAK_3;
  if (best >= 7) m |= 1u << BADGE_STREAK_7;
  if (best >= 30) m |= 1u << BADGE_STREAK_30;
  if (goalWeeksMet_ > 0) m |= 1u << BADGE_GOAL_WEEK;
  return m;
}

void ReadingLog::restore(const DayEntry* entries, size_t count, uint32_t total, int longest, uint16_t goalWeeksMet,
                         uint32_t weeklyGoalMinutes) {
  count_ = 0;
  for (size_t i = 0; i < count && count_ < kHistoryDays; ++i) {
    if (entries[i].seconds == 0) continue;
    entries_[count_++] = entries[i];
  }
  std::sort(entries_, entries_ + count_, [](const DayEntry& a, const DayEntry& b) { return a.day < b.day; });
  // Gop ngay trung lap (du lieu hong).
  size_t w = 0;
  for (size_t i = 0; i < count_; ++i) {
    if (w > 0 && entries_[w - 1].day == entries_[i].day) {
      entries_[w - 1].seconds += entries_[i].seconds;
    } else {
      entries_[w++] = entries_[i];
    }
  }
  count_ = w;
  total_ = total;
  longest_ = longest < 0 ? 0 : longest;
  goalWeeksMet_ = goalWeeksMet;
  setWeeklyGoalMinutes(weeklyGoalMinutes == 0 ? kDefaultWeeklyGoalMinutes : weeklyGoalMinutes);
}

}  // namespace vietread
