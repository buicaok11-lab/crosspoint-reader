#pragma once
// Ca dao, tuc ngu, thanh ngu dan gian Viet Nam (tai san van hoa chung).
#include <cstddef>
#include <cstdint>

namespace vietproverb {

inline constexpr const char* kProverbs[] = {
    "Có công mài sắt, có ngày nên kim.",
    "Ăn quả nhớ kẻ trồng cây.",
    "Uống nước nhớ nguồn.",
    "Học ăn, học nói, học gói, học mở.",
    "Một cây làm chẳng nên non, ba cây chụm lại nên hòn núi cao.",
    "Đi một ngày đàng, học một sàng khôn.",
    "Có chí thì nên.",
    "Không thầy đố mày làm nên.",
    "Học thầy không tày học bạn.",
    "Lá lành đùm lá rách.",
    "Gần mực thì đen, gần đèn thì rạng.",
    "Thất bại là mẹ thành công.",
    "Nước chảy đá mòn.",
    "Tốt gỗ hơn tốt nước sơn.",
    "Chớ thấy sóng cả mà ngã tay chèo.",
    "Ở hiền gặp lành.",
    "Bầu ơi thương lấy bí cùng, tuy rằng khác giống nhưng chung một giàn.",
    "Công cha như núi Thái Sơn, nghĩa mẹ như nước trong nguồn chảy ra.",
    "Thương người như thể thương thân.",
    "Lời nói chẳng mất tiền mua, lựa lời mà nói cho vừa lòng nhau.",
    "Đói cho sạch, rách cho thơm.",
    "Tre già măng mọc.",
    "Một nghề cho chín còn hơn chín nghề.",
    "Biết thì thưa thốt, không biết thì dựa cột mà nghe.",
    "Tiên học lễ, hậu học văn.",
    "Kiến tha lâu cũng đầy tổ.",
    "Có học mới biết, có đi mới đến.",
    "Lửa thử vàng, gian nan thử sức.",
    "Chuông có đánh mới kêu, đèn có khêu mới tỏ.",
    "Nói có sách, mách có chứng.",
    "Sông có khúc, người có lúc.",
    "Giấy rách phải giữ lấy lề.",
    "Mưa dầm thấm lâu.",
    "Học một biết mười.",
    "Ăn có nhai, nói có nghĩ.",
    "Trăm hay không bằng tay quen.",
    "Cái khó ló cái khôn.",
    "Đường xa mới biết ngựa hay, ở lâu mới biết lòng người.",
    "Có thực mới vực được đạo.",
    "Người không học như ngọc không mài.",
};
inline constexpr size_t kCount = sizeof(kProverbs) / sizeof(kProverbs[0]);

// Chon cau theo ngay (on dinh trong ngay, doi moi ngay). 'day' co the am.
inline const char* forDay(int32_t day) {
  const int64_t m = static_cast<int64_t>(kCount);
  return kProverbs[((day % m) + m) % m];
}

}  // namespace vietproverb
