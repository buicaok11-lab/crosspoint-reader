#pragma once
#include <ArduinoJson.h>
#include <PersistableStore.h>
#include <ReadingGoals.h>

#include <cstdint>

// Luu thong ke doc sach (chuoi ngay, muc tieu tuan, huy hieu) tren the SD: /.crosspoint/viet_reading.json
class VietReadingStore : public PersistableStore<VietReadingStore> {
  vietread::ReadingLog log_;
  bool loaded_ = false;

  VietReadingStore() = default;
  ~VietReadingStore() = default;
  friend class PersistableStore<VietReadingStore>;

 public:
  static const char* getFilePath() { return "/.crosspoint/viet_reading.json"; }
  void toJson(JsonDocument& doc) const;
  bool fromJson(JsonVariantConst doc);

  // Doc tu SD mot lan (lan goi dau). An toan khi goi nhieu lan.
  void ensureLoaded();
  // Cong mot phien doc vua ket thuc luc 'endEpoch' (giay UTC). Bo qua neu dong ho chua dang tin (<= 0).
  void addSession(int64_t endEpoch, uint32_t seconds);
  // Ap muc tieu tuan tu cai dat (vietWeeklyGoal).
  void applyGoalFromSettings();
  const vietread::ReadingLog& log() const { return log_; }
};

#define VIET_READING VietReadingStore::getInstance()
