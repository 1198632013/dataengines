#include "Random.h"
#include <QDebug>
#include <limits>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

Random::Random()
{

}

Random::~Random()
{

}

// 帧结构：任意字符串 : CSV结构的数据 \n
void Random::ProcessingDatas(char *data, int count)
{
    frame_list_.clear();
    int begin = 0, end = 0;

    char frame_tail_sequence[] = {0x0A, 0x3B, 0x0A};
    int sequence_index = 0;  // 用于追踪帧尾序列中的当前位置

    for (int i = 0; i < count; i++) {
        if (data[i] == frame_tail_sequence[sequence_index]) {
            sequence_index++;  // 匹配到帧尾序列中的一个字符
            if (sequence_index == 3) {
                // 所有字符都匹配，表示帧尾已找到
                end = i;  // 记录帧的结束位置
            }
        } else {
            // 未匹配到帧尾的字符，重置状态
            sequence_index = 0;
        }
        if (sequence_index != 3)
            continue;

        // 处理帧数据...
        // 重置状态，以查找下一个帧尾
        sequence_index = 0;

        bool frame_is_valid = false;
        char *frame_head_ptr = data + begin;
        int frame_count = i - begin + 1;
        int image_size = 0;
        Frame frame;

        // 将分别位于':'左右两侧的任意字符串和CSV结构的数据分割
        // 如果符合firewater的帧结构，分割出的段数为2
        QString frame_str = QString::fromLocal8Bit(frame_head_ptr , frame_count);

        // 定义正则表达式以匹配数字
        QRegularExpression numberRegex("\\d+");
        // 使用正则表达式查找匹配项
        QRegularExpressionMatch match = numberRegex.match(frame_str);
        frame.datas_.clear();
        if (match.hasMatch()) {
            QString numberStr = match.captured(0);
            frame.datas_.append(numberStr.toFloat());

            if (frame.datas_.length() > 0)
                frame_is_valid = true;      // 至此，可以确定这是一个合法的采样数据包
        }

        // 记录帧 是否合法，开始位置，结束位置，图片尺寸（如果为0，标识其不是图片数据包）
        frame.is_valid_ = frame_is_valid;
        frame.start_index_ = begin;
        frame.end_index_ = end;
        frame.image_size_ = image_size;
        frame_list_.append(frame);
        // !记录帧

        begin = i+1;
    }
}
