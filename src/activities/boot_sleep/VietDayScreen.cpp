#include "VietDayScreen.h"

#include <GfxRenderer.h>
#include <TrustedTime.h>
#include <VietCulture/LunarCalendar.h>
#include <VietCulture/Proverbs.h>
#include <VietCulture/ReadingGoals.h>

#include <cstdio>
#include <string>

#include "VietReadingStore.h"
#include "fontIds.h"

namespace {

const char* const kWeekdays[7] = {"Thứ hai", "Thứ ba", "Thứ tư", "Thứ năm", "Thứ sáu", "Thứ bảy", "Chủ nhật"};

void rule(GfxRenderer& r, int y, int margin) { r.drawLine(margin, y, r.getScreenWidth() - margin, y, true); }

// Ve cac dong da ngat va tra ve y tiep theo.
int drawWrapped(GfxRenderer& r, int fontId, int y, int width, const char* text, int maxLines, bool bold = false) {
  const auto style = bold ? EpdFontFamily::BOLD : EpdFontFamily::REGULAR;
  const auto lines = r.wrappedText(fontId, text, width, maxLines, style);
  const int lh = r.getLineHeight(fontId);
  for (const auto& line : lines) {
    r.drawCenteredText(fontId, y, line.c_str(), true, style);
    y += lh;
  }
  return y;
}

}  // namespace

void renderVietDayScreen(GfxRenderer& r) {
  const int w = r.getScreenWidth();
  const int h = r.getScreenHeight();
  const int margin = 36;
  const int textW = w - 2 * margin;

  r.clearScreen();
  VIET_READING.ensureLoaded();
  VIET_READING.applyGoalFromSettings();
  const auto& log = VIET_READING.log();

  const int64_t now = trustedtime::trustedNow();
  const bool haveTime = now > 0;

  int y = margin + 10;
  int32_t day = 0;
  vietcal::SolarDate solar{0, 0, 0};

  if (haveTime) {
    solar = vietcal::solarFromEpoch(now);
    day = vietcal::localDayIndex(now);
    const int jd = vietcal::jdFromDate(solar.day, solar.month, solar.year);
    const auto lunar = vietcal::solarToLunar(solar.day, solar.month, solar.year);

    char buf[96];
    snprintf(buf, sizeof(buf), "%s, %d/%d/%d", kWeekdays[vietread::ReadingLog::weekdayMonday0(day)], solar.day,
             solar.month, solar.year);
    r.drawCenteredText(UI_12_FONT_ID, y, buf, true, EpdFontFamily::BOLD);
    y += r.getLineHeight(UI_12_FONT_ID) + 6;

    snprintf(buf, sizeof(buf), "Âm lịch: ngày %d tháng %d%s năm %s %s", lunar.day, lunar.month,
             lunar.leap ? " nhuận" : "", vietcal::canOfYear(lunar.year), vietcal::chiOfYear(lunar.year));
    y = drawWrapped(r, UI_10_FONT_ID, y, textW, buf, 2);
    snprintf(buf, sizeof(buf), "Ngày %s %s", vietcal::canOfDay(jd), vietcal::chiOfDay(jd));
    y = drawWrapped(r, SMALL_FONT_ID, y, textW, buf, 1);

    if (const char* holiday = vietcal::holidayName(solar.day, solar.month, solar.year)) {
      y += 10;
      const int bh = r.getLineHeight(UI_10_FONT_ID) * 2 + 12;
      r.drawRect(margin, y, textW, bh, 2, true);
      drawWrapped(r, UI_10_FONT_ID, y + 8, textW - 20, holiday, 2, true);
      y += bh;
    }
  } else {
    r.drawCenteredText(UI_12_FONT_ID, y, "Lịch Việt", true, EpdFontFamily::BOLD);
    y += r.getLineHeight(UI_12_FONT_ID) + 6;
    y = drawWrapped(r, SMALL_FONT_ID, y, textW, "Chưa có giờ chính xác. Kết nối Wi-Fi một lần để hiện ngày âm lịch.", 3);
    day = static_cast<int32_t>(log.totalSeconds() / 600);  // doi cau theo muc do doc khi chua co gio
  }

  y += 18;
  rule(r, y, margin);
  y += 22;

  // Ca dao / tuc ngu (khung giua man hinh)
  // 21/4: Ngay Sach va Van hoa doc Viet Nam -> loi chuc thay cho ca dao.
  const bool bookDay = haveTime && solar.month == 4 && solar.day == 21;
  const char* quote = bookDay ? "Chúc mừng Ngày Sách và Văn hóa đọc Việt Nam!" : vietproverb::forDay(day);
  y = drawWrapped(r, UI_12_FONT_ID, y, textW, quote, 6, false);
  y += 18;
  rule(r, y, margin);
  y += 20;

  // Thong ke doc sach
  char buf[96];
  const int streak = haveTime ? log.currentStreak(day) : 0;
  snprintf(buf, sizeof(buf), "Chuỗi đọc: %d ngày  (dài nhất: %d)", streak, log.longestStreak());
  r.drawCenteredText(UI_10_FONT_ID, y, buf, true, EpdFontFamily::BOLD);
  y += r.getLineHeight(UI_10_FONT_ID) + 8;

  const uint32_t weekMin = haveTime ? log.weekSeconds(day) / 60 : 0;
  snprintf(buf, sizeof(buf), "Tuần này: %lu / %lu phút", static_cast<unsigned long>(weekMin),
           static_cast<unsigned long>(log.weeklyGoalMinutes()));
  r.drawCenteredText(SMALL_FONT_ID, y, buf);
  y += r.getLineHeight(SMALL_FONT_ID) + 6;

  const int barW = textW;
  const int barH = 14;
  r.drawRect(margin, y, barW, barH, 2, true);
  const int pct = haveTime ? log.weeklyGoalPercent(day) : 0;
  if (pct > 0) r.fillRect(margin + 3, y + 3, (barW - 6) * pct / 100, barH - 6, true);
  y += barH + 16;

  const uint32_t badges = log.earnedBadges(day);
  std::string earned;
  int count = 0;
  for (int b = 0; b < vietread::BADGE_COUNT; ++b) {
    if (vietread::ReadingLog::has(badges, static_cast<vietread::Badge>(b))) {
      ++count;
      if (!earned.empty()) earned += " · ";
      earned += vietread::badgeName(static_cast<vietread::Badge>(b));
    }
  }
  snprintf(buf, sizeof(buf), "Huy hiệu: %d/%d", count, static_cast<int>(vietread::BADGE_COUNT));
  r.drawCenteredText(SMALL_FONT_ID, y, buf, true, EpdFontFamily::BOLD);
  y += r.getLineHeight(SMALL_FONT_ID) + 4;
  if (!earned.empty() && y < h - margin) {
    drawWrapped(r, SMALL_FONT_ID, y, textW, earned.c_str(), 3);
  }
}
