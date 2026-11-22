#include <cstdio>

#include "VietCulture/ReadingGoals.h"

using namespace vietread;

static int fails = 0;
#define EXPECT(c)                                              \
  do {                                                         \
    if (!(c)) {                                                \
      std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #c); \
      ++fails;                                                 \
    }                                                          \
  } while (0)

int vietread_selftest() {
  {  // chuoi ngay
    ReadingLog l;
    l.addSession(100, 600);
    l.addSession(101, 700);
    l.addSession(102, 900);
    EXPECT(l.currentStreak(102) == 3);
    EXPECT(l.currentStreak(103) == 3);  // hom nay chua doc: van con chuoi
    EXPECT(l.currentStreak(104) == 0);  // bo lo 1 ngay
    EXPECT(l.longestStreak() == 3);
  }
  {  // ngay chua du 10 phut lam dut chuoi
    ReadingLog l;
    l.addSession(100, 600);
    l.addSession(101, 300);
    l.addSession(102, 600);
    EXPECT(l.currentStreak(102) == 1);
  }
  {  // tuan bat dau thu Hai; ngay 0 la thu Nam
    EXPECT(ReadingLog::weekdayMonday0(0) == 3);
    EXPECT(ReadingLog::weekdayMonday0(4) == 0);   // thu Hai
    EXPECT(ReadingLog::weekdayMonday0(10) == 6);  // chu nhat
    ReadingLog l;
    l.addSession(3, 5000);   // thu Tu tuan truoc: khong tinh
    l.addSession(4, 1800);   // thu Hai
    l.addSession(6, 1200);   // thu Tu
    EXPECT(l.weekSeconds(7) == 3000);
    EXPECT(l.weeklyGoalPercent(7) == 33);  // 3000 / 9000
    EXPECT(l.goalWeeksMet() == 0);
    l.addSession(7, 6000);  // vuot 9000
    EXPECT(l.goalWeeksMet() == 1);
    l.addSession(8, 100);   // da vuot, khong dem lai
    EXPECT(l.goalWeeksMet() == 1);
    EXPECT(l.weeklyGoalPercent(8) == 100);
  }
  {  // huy hieu
    ReadingLog l;
    EXPECT(l.earnedBadges(1) == 0);
    l.addSession(1, 3600);
    EXPECT(ReadingLog::has(l.earnedBadges(1), BADGE_FIRST_HOUR));
    EXPECT(!ReadingLog::has(l.earnedBadges(1), BADGE_TEN_HOURS));
    for (int d = 2; d <= 8; ++d) l.addSession(d, 600);
    EXPECT(ReadingLog::has(l.earnedBadges(8), BADGE_STREAK_7));
    EXPECT(ReadingLog::has(l.earnedBadges(8), BADGE_STREAK_3));
    EXPECT(!ReadingLog::has(l.earnedBadges(8), BADGE_STREAK_30));
  }
  {  // cat lich su 60 ngay, tong van dung
    ReadingLog l;
    uint32_t sum = 0;
    for (int d = 1000; d < 1070; ++d) {
      l.addSession(d, 600);
      sum += 600;
    }
    EXPECT(l.size() <= kHistoryDays);
    EXPECT(l.secondsOn(1000) == 0);
    EXPECT(l.secondsOn(1069) == 600);
    EXPECT(l.totalSeconds() == sum);
    EXPECT(l.longestStreak() >= 60);
    // ngay rat cu khi bo nho day: chi cong vao tong
    const uint32_t before = l.totalSeconds();
    l.addSession(5, 600);
    EXPECT(l.totalSeconds() == before + 600);
    EXPECT(l.secondsOn(5) == 0);
  }
  {  // restore sap xep + gop trung lap
    DayEntry e[] = {{12, 100}, {10, 50}, {12, 20}, {11, 0}};
    ReadingLog l;
    l.restore(e, 4, 170, 2, 1, 0);
    EXPECT(l.size() == 2);
    EXPECT(l.entries()[0].day == 10);
    EXPECT(l.secondsOn(12) == 120);
    EXPECT(l.weeklyGoalMinutes() == kDefaultWeeklyGoalMinutes);
    l.setWeeklyGoalMinutes(1);
    EXPECT(l.weeklyGoalMinutes() == 10);
  }
  return fails;
}
