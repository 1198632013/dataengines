#include "Random.h"
#include <QDebug>
#include <limits>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegExp>

Random::Random()
{

}

Random::~Random()
{

}

// // 帧结构：任意字符串 : CSV结构的数据 \n
// void Random::ProcessingDatas(char *data, int count)
// {
//     frame_list_.clear();
//     int begin = 0, end = 0;

//     char frame_tail_sequence[] = {0x0A, 0x3B, 0x0A};
//     int sequence_index = 0;  // 用于追踪帧尾序列中的当前位置

//     for (int i = 0; i < count; i++) {
//         if (data[i] == frame_tail_sequence[sequence_index]) {
//             sequence_index++;  // 匹配到帧尾序列中的一个字符
//             if (sequence_index == 3) {
//                 // 所有字符都匹配，表示帧尾已找到
//                 end = i;  // 记录帧的结束位置
//             }
//         } else {
//             // 未匹配到帧尾的字符，重置状态
//             sequence_index = 0;
//         }
//         if (sequence_index != 3)
//             continue;

//         // 处理帧数据...
//         // 重置状态，以查找下一个帧尾
//         sequence_index = 0;

//         bool frame_is_valid = false;
//         char *frame_head_ptr = data + begin;
//         int frame_count = i - begin + 1;
//         int image_size = 0;
//         Frame frame;

//         // 将分别位于':'左右两侧的任意字符串和CSV结构的数据分割
//         // 如果符合firewater的帧结构，分割出的段数为2
//         QString frame_str = QString::fromLocal8Bit(frame_head_ptr , frame_count);

//         // 定义正则表达式以匹配数字
//         QRegularExpression numberRegex("\\d+");
//         // 使用正则表达式查找匹配项
//         QRegularExpressionMatch match = numberRegex.match(frame_str);
//         frame.datas_.clear();
//         if (match.hasMatch()) {
//             QString numberStr = match.captured(0);
//             frame.datas_.append(numberStr.toFloat());

//             if (frame.datas_.length() > 0)
//                 frame_is_valid = true;      // 至此，可以确定这是一个合法的采样数据包
//         }

//         // 记录帧 是否合法，开始位置，结束位置，图片尺寸（如果为0，标识其不是图片数据包）
//         frame.is_valid_ = frame_is_valid;
//         frame.start_index_ = begin;
//         frame.end_index_ = end;
//         frame.image_size_ = image_size;
//         frame_list_.append(frame);
//         // !记录帧

//         begin = i+1;
//     }
// }



// QRegExp Separator = QRegExp("\t|\n|\r\n|\r| |,|;");
QRegExp Separator = QRegExp(" |,"); // 空格 or 逗号

// 帧结构：任意字符串 : CSV结构的数据 \n
void FireWater::ProcessingDatas(char *data, int count)
{
    frame_list_.clear();
    int begin = 0, end = 0;
    for (int i = 0; i < count; i++) {
        if (data[i] != '\n')
            continue;

        // 已找到帧尾 —— '\n'
        end = i;
        bool frame_is_valid = false;
        char *frame_head_ptr = data + begin;
        int frame_count = i - begin + 1;
        int image_size = 0;
        Frame frame;

        // 将分别位于':'左右两侧的任意字符串和CSV结构的数据分割
        // 如果符合firewater的帧结构，分割出的段数为2
        QString frame_str = QString::fromLocal8Bit(frame_head_ptr , frame_count);
        QList<QString> name_and_datas = frame_str.split(':');

        if (name_and_datas.size() >= 2) {
            // 为什么分割段数>2，也进行帧解析？
            // 答：没有关系，d:d:1,2,3,4\n，这样分割段数是3，不影响解析。

            if (name_and_datas.size() > 2) {
                // 采用最后一个':'后面的数据作为有效CSV数据
                begin += frame_count - name_and_datas.last().length() - 2;
            }
            QVector<float> dd;
            QString name =  name_and_datas[name_and_datas.size()-2].trimmed();
            QList<QString> datas = name_and_datas[name_and_datas.size()-1].trimmed().split(',');
            if (name == "image") {
                // 图片前导帧
                if (datas.length() != 5) {
                    // 图片前导帧异常
                    break;
                }

                // 获取图片信息
                int image_id        = datas[0].toInt();
                image_size          = datas[1].toInt();
                int image_width     = datas[2].toInt();
                int image_height    = datas[3].toInt();
                RawImage::Format image_format = static_cast<RawImage::Format>(
                                      datas[4].toInt());
                // !获取图片信息


                if ((count - (i + 1)) < image_size) {
                    // 图片长度超过缓冲区长度，可能还没接收完，直接返回，下次再来
                    return;
                }

                if (image_id > (image_channels_.length() - 1)) {
                    // 图片id > 图片通道数量，扩充图片通道
                    // 在扩充图片通道之前，为过滤异常情况，保证发送了6帧大id的图片之后，再进行扩充

                    image_count_mutation_count_++;
                    if (image_id < 6 || image_count_mutation_count_ >= 6) {
                        image_count_mutation_count_ = 0;
                        while (image_channels_.length() < image_id + 1) {
                            image_channels_.append(new RawImage());
                        }
                    }
                }
                if (image_id < image_channels_.length()) {
                    // 图片id合法，把图片数据放到图片通道中
                    image_channels_[image_id]->set((uchar*)data + i + 1, image_size,
                                                   image_width, image_height,
                                                   image_format);
                }

                // 把图片数据结尾记录为帧尾，图片前导帧+图片数据，构成了一个图片数据包
                end = i + image_size;
                i = end;
                frame_is_valid = true;  // 至此，可以确定这是一个合法的图片数据包
            } else {
                // 解析CSV数据，将其转换为采样数据
                for (int i = 0; i < datas.length(); i++) {
                    float value = datas[i].trimmed().toFloat();
                    frame.datas_.append(value);
                }
            }
            frame_is_valid = true;      // 至此，可以确定这是一个合法的采样数据包
        } else {
            // 以下代码对不以冒号开头的纯CSV数据添加支持
            // QList<QString> datas = frame_str.trimmed().split(',');
            QList<QString> datas = frame_str.trimmed().split(Separator, QString::SkipEmptyParts);

            bool ok;
            for (int i = 0; i < datas.length(); i++) {
                float value = datas[i].trimmed().toFloat(&ok);
                if (!ok) {
                    frame.datas_.clear();
                    break;
                }
                frame.datas_.append(value);
            }
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
