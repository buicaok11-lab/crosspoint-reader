#include "VietReadingStore.h"

#include <Logging.h>
#include <LunarCalendar.h>

#include "CrossPointSettings.h"

void VietReadingStore::toJson(JsonDocument& doc) const {
  doc["total"] = log_.totalSeconds();
  doc["longest"] = log_.longestStreak();
  doc["goalWeeks"] = log_.goalWeeksMet();
  doc["goalMin"] = log_.weeklyGoalMinutes();
  JsonArray days = doc["days"].to<JsonArray>();
  for (size_t i = 0; i < log_.size(); ++i) {
    JsonObject o = days.add<JsonObject>();
    o["d"] = log_.entries()[i].day;
    o["s"] = log_.entries()[i].seconds;
  }
}

bool VietReadingStore::fromJson(JsonVariantConst doc) {
  vietread::DayEntry tmp[vietread::kHistoryDays];
  size_t n = 0;
  JsonArrayConst days = doc["days"].as<JsonArrayConst>();
  for (JsonObjectConst o : days) {
    if (n >= vietread::kHistoryDays) break;
    tmp[n].day = o["d"] | 0;
    tmp[n].seconds = o["s"] | 0u;
    ++n;
  }
  log_.restore(tmp, n, doc["total"] | 0u, doc["longest"] | 0, static_cast<uint16_t>(doc["goalWeeks"] | 0),
               doc["goalMin"] | 0u);
  return true;
}

void VietReadingStore::ensureLoaded() {
  if (loaded_) return;
  loaded_ = true;
  loadFromFile();  // file chua co la binh thuong o lan dau
}

void VietReadingStore::applyGoalFromSettings() {
  static constexpr uint32_t kGoalMinutes[] = {60, 150, 300, 600};
  const uint8_t idx = SETTINGS.vietWeeklyGoal < 4 ? SETTINGS.vietWeeklyGoal : 1;
  log_.setWeeklyGoalMinutes(kGoalMinutes[idx]);
}

void VietReadingStore::addSession(int64_t endEpoch, uint32_t seconds) {
  if (endEpoch <= 0 || seconds == 0) return;
  ensureLoaded();
  applyGoalFromSettings();
  log_.addSession(vietcal::localDayIndex(endEpoch), seconds);
  if (!saveToFile()) {
    LOG_ERR("VIET", "Khong luu duoc thong ke doc sach");
  }
}
