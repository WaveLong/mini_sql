/*
+--------------------------------------------------------------------------------------+
| 文件名称: configuration.h
| 功    能:	定义了一些系统参数
| 作    者: wang xin
| 联系方式: wangxinn@zju.edu.cn
| 生成日期: 2021/05/07 17:34
| 版 本 号: V1
| 修改日志: 
| 版权说明: 2021, All right reserved.
+--------------------------------------------------------------------------------------+
*/

#pragma once

#define PAGE_SIZE 4096
#define INT_TYPE 0
#define FLOAT_TYPE 1
#define STRING_TYPE 2

#define INT_LEN sizeof(int)
#define FLOAT_LEN sizeof(float)
#define ADRR_LEN sizeof(int)
#define RECORD_COUNT_LEN sizeof(int)

#define ADRR_BASE (2*sizeof(int) + sizeof(int))

#define BUFFER_SIZE 300

// condition
#define EQ 0
#define NEQ 1
#define GT 2
#define GE 3
#define LS 4
#define LE 5

