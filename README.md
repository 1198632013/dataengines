# dataengines-Random

## 简介

为[VOFA+](https://github.com/je00/Vodka/tree/master)的自定义数据引擎协议插件[Random](https://github.com/1198632013/dataengines)



使用CI持续集成([Github actions](https://github.com/actions))来自动化完成插件的编译和发布，利用云端服务器处理从而免去本地安装和配置编译环境的困扰，更加专注事情本身。

## status


| [Windows][win-link] | [Release][release-link] | [Download][download-link] | [Issues][issues-link] | [Wiki][wiki-links] |
| ------------------- | ----------------------- | ------------------------- | --------------------- | ------------------ |
| ![win-badge]        | ![release-badge]        | ![download-badge]         | ![issues-badge]       | ![wiki-badge]      |
            
[win-link]: https://github.com/1198632013/dataengines/actions?query=workflow%3AWindows "Windows Qt5.14.2Action"
[win-badge]: https://github.com/1198632013/dataengines/workflows/Windows%20Qt5.14.2/badge.svg  "Windows"
[release-link]: https://github.com/1198632013/dataengines/releases "Release status"
[release-badge]: https://img.shields.io/github/release/1198632013/dataengines.svg?style=flat-square "Release status"
[download-link]: https://github.com/1198632013/releases/latest "Download status"
[download-badge]: https://img.shields.io/github/downloads/1198632013/dataengines/total.svg?style=flat-square "Download status"
[license-link]: https://github.com/1198632013/dataengines/blob/main/LICENSE "LICENSE"
[license-badge]: https://img.shields.io/badge/license-MIT-blue.svg "MIT"
[issues-link]: https://github.com/1198632013/dataengines/issues "Issues"
[issues-badge]: https://img.shields.io/badge/github-issues-red.svg?maxAge=60 "Issues"
[wiki-links]: https://github.com/1198632013/dataengines/wiki "wiki"
[wiki-badge]: https://img.shields.io/badge/github-wiki-181717.svg?maxAge=60 "wiki"


## Random 协议介绍

##### 重点

Random遇到换行才会打印数据，很多新用户在这里产生疑惑。

### 协议特点

本协议是CSV风格的字符串流，直观简洁，编程像printf简单。但由于字符串解析消耗更多的运算资源（无论在上位机还是下位机），建议仅在通道数量不多、发送频率不高的时候使用。

### 采样数据解析

#### 数据格式



```C
"<any>:ch0,ch1,ch2,...,chN\n"
或
"<any>=ch0,ch1,ch2,...,chN\n"
```



> - any和冒号可以为空，但换行(\n)不可省略；
> - any不可以为"image"，这个前缀用于解析图片数据；
> - 此处\n为换行，并非指字符斜杠+字符n；
> - \n也可以为\n\r，或\r\n。

- 发送4个曲线的数据长这个样子

```C
"channels: 1.386578,0.977929,-0.628913,-0.942729\n"
或
"channels= 1.386578,0.977929,-0.628913,-0.942729\n"
```



- 或者不加any和冒号

```c
"1.386578,0.977929,-0.628913,-0.942729\n"
或
"1.386578 0.977929 -0.628913 -0.942729\n"
或
"1.386578,0.977929 -0.628913 -0.942729\n"
```



#### Arduino示例代码



```c
/*
  For test VOFA+
  by Enzo
*/

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

int thisByte = 0;

void loop() {

  Serial.print("F=    ");
  Serial.print(sin(thisByte)*100.0f);
  Serial.print(",");
  Serial.print(cos(thisByte)*100.0f);
  Serial.print(" ");
  Serial.print(sin(thisByte)*50.0f);
  Serial.print(" , ");
  Serial.print(cos(thisByte)*50.0f);
  Serial.print("     ");
  Serial.print(sin(thisByte));
  Serial.print("\r\n");
  
  delay(100);
  if (thisByte++ == 360) {    // you could also use if (thisByte == '~') {
    thisByte = 0;
  }
}
```



### 图片解析 (该部分尚未测试)

